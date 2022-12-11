/*************************************************************************
 File Name: dmidecode.c
 Author: zhuyaliang

 Copyright (C) 2022  zhuyaliang https://github.com/zhuyaliang/
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 Created Time: 2022年08月16日 星期二 20时31分33秒
************************************************************************/
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "dmidecode.h"

typedef struct {
    guint l;
    guint h;
} u64;

struct _LoongsonDmidecode
{
    GObject   parent;
    char     *dmi_data;
    char     *dmi_table_data;
    gint64    dmi_base;
    guint     dmi_len;
    guint16   dmi_num;
    guint     dmi_flags;
    gboolean  found;
};

#define FLAG_NO_FILE_OFFSET     (1 << 0)
#define FLAG_STOP_AT_EOT        (1 << 1)

#define SYS_FIRMWARE_DIR        "/sys/firmware/dmi/tables"
#define SYS_ENTRY_FILE SYS_FIRMWARE_DIR "/smbios_entry_point"
#define SYS_TABLE_FILE SYS_FIRMWARE_DIR "/DMI"
#define DWORD(x)                (char)(*(const char *)(x))
#define QWORD(x)                (*(const u64 *)(x))
#define WORD(x)                 (guint16)(*(const guint16 *)(x))

G_DEFINE_TYPE (LoongsonDmidecode, loongson_dmidecode, G_TYPE_OBJECT)

static int myread (int fd, char *buf, size_t count, const char *prefix)
{
    ssize_t r = 1;
    size_t r2 = 0;

    while (r2 != count && r != 0)
    {
        r = read (fd, buf + r2, count - r2);
        if (r == -1)
            return -1;
        else
            r2 += r;
    }

    if (r2 != count)
        return -1;

    return 0;
}

static char *dmi_read_file (size_t *max_len, const char *filename)
{
    struct stat statbuf;
    int    fd;
    char  *p;

    if ((fd = open (filename, O_RDONLY)) == -1)
        return NULL;

    if (fstat (fd, &statbuf) == 0)
    {
        if (statbuf.st_size <= 0)
        {
            close (fd);
            return NULL;
        }
        *max_len = statbuf.st_size;
    }

    if ((p = malloc (*max_len)) == NULL)
        goto out;

    if (lseek (fd, 0, SEEK_SET) == -1)
        goto err_free;

    if (myread (fd, p, *max_len, filename) == 0)
        goto out;

err_free:
    free(p);
    p = NULL;

out:
    close (fd);
    return p;
}

static int checksum (const char *data, size_t len)
{
    char   sum = 0;
    size_t a;

    for (a = 0; a < len; a++)
        sum += data[a];

    return (sum == 0);
}

static void ascii_filter (char *bp, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++)
        if (bp[i] < 32 || bp[i] >= 127)
            bp[i] = '.';
}

static char *_dmi_string (char *data, int filter, char s)
{
    char *bp = data;

    bp += data[1];
    while (s > 1 && *bp)
    {
        bp += strlen (bp);
        bp++;
        s--;
    }

    if (!*bp)
        return g_strdup ("Unknown");

    if (filter)
        ascii_filter (bp, strlen(bp));

    return g_strdup (bp);
}

static const char *dmi_string (char *data, char s)
{
    char *bp;

    if (s == 0)
        return g_strdup ("Not Specified");

    bp = _dmi_string (data, 1, s);
    if (bp == NULL)
        return g_strdup ("Unknown");

    return bp;
}

static const char *dmi_decode (char *data, guint sub_index)
{
    const char *string = NULL;

    switch (sub_index)
    {
        case 0:
            string = dmi_string (data, data[0x04]);
            break;
        case 1:
            string = dmi_string (data, data[0x05]);
            break;
        case 2:
            string = dmi_string (data, data[0x08]);
            break;
        case 3:
            string = g_strdup_printf ("%d.%d", data[0x14], data[0x15]);
            break;
        default:
            string  = g_strdup ("Unknown");
            break;
    }

    return string;
}

static const char *dmi_table_decode (LoongsonDmidecode *dmi, guint index, guint sub_index)
{
    char *data;
    int   i = 0;

    data = dmi->dmi_table_data;
    while ((i < dmi->dmi_num || !dmi->dmi_num) && data + 4 <= dmi->dmi_table_data + dmi->dmi_len)
    {
        char *next;

        if (data[1] < 4 || (data[0] == 127))
            break;
        i++;

        next = data + data[1];
        while ((guint64)(next - dmi->dmi_table_data + 1) < dmi->dmi_len
            && (next[0] != 0 || next[1] != 0))
            next++;
        next += 2;

        if ((guint64)(next - dmi->dmi_table_data) > dmi->dmi_len)
            break;
        data = next;
    }
    i = 0;
    data = dmi->dmi_table_data;

    while ((i < dmi->dmi_num || !dmi->dmi_num) && data + 4 <= dmi->dmi_table_data + dmi->dmi_len)
    {
        char *next;

        if (data[1] < 4 || data[0] == 127)
            break;
        i++;

        next = data + data[1];
        while ((guint64)(next - dmi->dmi_table_data + 1) < dmi->dmi_len
            && (next[0] != 0 || next[1] != 0))
            next++;
        next += 2;

        if ((guint)data[0] == index)
            return dmi_decode (data, sub_index);

        data = next;
        if (data[0] == 127 && (dmi->dmi_flags & FLAG_STOP_AT_EOT))
            break;
    }

    return g_strdup ("Unknown");
}

static gboolean dmi_table (LoongsonDmidecode *dmi)
{
    size_t size = dmi->dmi_len;

    dmi->dmi_table_data = dmi_read_file (&size, SYS_TABLE_FILE);
    dmi->dmi_len = size;

    if (dmi->dmi_table_data == NULL)
        return FALSE;

    return TRUE;
}

static gboolean smbios3_decode (LoongsonDmidecode *dmi)
{
    u64  offset;

    if (dmi->dmi_data[0x06] > 0x20)
        return FALSE;

    if (!checksum (dmi->dmi_data, dmi->dmi_data[0x06]))
        return FALSE;

    offset = QWORD (dmi->dmi_data + 0x10);
    if (offset.h && sizeof(off_t) < 8)
        return FALSE;

    dmi->dmi_base = ((off_t)offset.h << 32) | offset.l;
    dmi->dmi_len = DWORD (dmi->dmi_data + 0x0C);
    dmi->dmi_num = 0;
    dmi->dmi_flags = FLAG_NO_FILE_OFFSET | FLAG_STOP_AT_EOT;

    if (!dmi_table (dmi))
        return FALSE;

    return TRUE;
}

static gboolean smbios_decode (LoongsonDmidecode *dmi)
{
    if (dmi->dmi_data[0x05] > 0x20)
        return FALSE;

    if (!checksum (dmi->dmi_data, dmi->dmi_data[0x05])
     || memcmp (dmi->dmi_data + 0x10, "_DMI_", 5) != 0
     || !checksum (dmi->dmi_data + 0x10, 0x0F))
        return FALSE;

    dmi->dmi_base = DWORD (dmi->dmi_data + 0x18);
    dmi->dmi_len = WORD (dmi->dmi_data + 0x16);
    dmi->dmi_num = WORD (dmi->dmi_data + 0x1C);
    dmi->dmi_flags = FLAG_NO_FILE_OFFSET;

    if (!dmi_table (dmi))
        return FALSE;

    return TRUE;
}

static void loongson_dmidecode_dispose (GObject *object)
{
    LoongsonDmidecode *dmi;

    dmi = LOONGSON_DMIDECODE (object);
    if (dmi->dmi_data != NULL)
        g_free (dmi->dmi_data);

    if (dmi->dmi_table_data != NULL)
        g_free (dmi->dmi_table_data);

    G_OBJECT_CLASS (loongson_dmidecode_parent_class)->dispose (object);
}

static void loongson_dmidecode_class_init (LoongsonDmidecodeClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->dispose = loongson_dmidecode_dispose;
}

static void loongson_dmidecode_init (LoongsonDmidecode *dmi)
{
    size_t size = 0;

    dmi->found = FALSE;

    if ((dmi->dmi_data = dmi_read_file (&size, SYS_ENTRY_FILE)) != NULL)
    {
        if (size >= 24 && memcmp (dmi->dmi_data, "_SM3_", 5) == 0)
        {
            if (smbios3_decode (dmi))
                dmi->found = TRUE;
        }
        else if (size >= 31 && memcmp (dmi->dmi_data, "_SM_", 4) == 0)
        {
            if (smbios_decode (dmi))
                dmi->found = TRUE;
        }
    }
}

const char *dmi_get_firmware_date (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 0, 2);

    return data;
}

const char *dmi_get_firmware_vendor (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 0, 0);

    return data;
}

const char *dmi_get_firmware_name (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 0, 1);

    return data;
}

const char *dmi_get_bios_version (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 0, 3);

    return data;
}

LoongsonDmidecode *loongson_dmidecode_new (void)
{
    LoongsonDmidecode *dmi;

    dmi = g_object_new (LOONGSON_TYPE_DMIDECODE, NULL);

    if (!dmi->found)
        return NULL;

    return dmi;
}
