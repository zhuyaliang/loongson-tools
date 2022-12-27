/*************************************************************************
 File Name: loongson-firmware.c
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

 Created Time: 2022年12月06日 星期四 18时06分47秒
************************************************************************/
#include "loongson-firmware.h"
#include "loongson-utils.h"

struct _LoongsonFirmware
{
    GtkBox     box;
    LoongDaemon *proxy;
    char      *name;
    char      *image;
    char      *filename;
    GtkWidget *name_label;
    GtkWidget *time_label;
    GtkWidget *vendor_label;
    GtkWidget *bios_label;
    GtkWidget *update_button;
};

G_DEFINE_TYPE (LoongsonFirmware, loongson_firmware, GTK_TYPE_BOX)

static void
chooser_selection_changed_cb (GtkFileChooser *chooser,
                              gpointer        user_data)
{
    LoongsonFirmware *firmware;

    firmware = LOONGSON_FIRMWARE (user_data);

    firmware->filename = gtk_file_chooser_get_filename (chooser);
    gtk_widget_set_sensitive (firmware->update_button, TRUE);
  
}

static void
start_update_firmware (GtkButton        *button,
                       LoongsonFirmware *firmware)
{
    GError  *error = NULL;
    gboolean ret; 

    ret = loong_daemon_call_firmware_update_sync (firmware->proxy,
                                                  firmware->filename,
                                                  NULL,
                                                 &error);

    if (ret != TRUE)
    {
       loongson_message_dialog (_("update filename"), WARING, error->message);
       g_error_free (error);
    }
}

static void
loongson_firmware_fill (LoongsonFirmware *firmware)
{
    GtkWidget *label;
    GtkWidget *hbox;
    GtkWidget *vbox;
    GtkWidget *vbox1; 
    GtkWidget *fixed;
    GtkWidget *separator;
    GtkWidget *picker;

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (firmware), fixed);
  
    hbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    gtk_fixed_put (GTK_FIXED (fixed), hbox, 18, 10);

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 6);
    gtk_widget_set_size_request (vbox, 480, -1);

    vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start (GTK_BOX (hbox), vbox1, TRUE, TRUE, 18);
    gtk_widget_set_size_request (vbox1, 450, -1);
    
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 6);
 
    label = gtk_label_new (NULL);
    set_lable_style (label, "gray", 14, "当前固件", TRUE);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 12);

    firmware->name_label = gtk_label_new (NULL);
    separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_end (GTK_BOX (hbox), firmware->name_label, FALSE, FALSE, 12);
    gtk_box_pack_start (GTK_BOX (vbox), separator, TRUE, TRUE, 6);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 6);

    label = gtk_label_new (NULL);
    set_lable_style (label, "gray", 14, "更新时间", TRUE);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 12);

    firmware->time_label = gtk_label_new (NULL);
    separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_end (GTK_BOX (hbox), firmware->time_label, FALSE, FALSE, 12);
    gtk_box_pack_start (GTK_BOX (vbox), separator, TRUE, TRUE, 6);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 6);

    label = gtk_label_new (NULL);
    set_lable_style (label, "gray", 14, "供应商", TRUE);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 12);

    firmware->vendor_label = gtk_label_new (NULL);
    separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_end (GTK_BOX (hbox), firmware->vendor_label, FALSE, FALSE, 12);
    gtk_box_pack_start (GTK_BOX (vbox), separator, TRUE, TRUE, 6);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 6);

    label = gtk_label_new (NULL);
    set_lable_style (label, "gray", 14, "BIOS", TRUE);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 12);

    firmware->bios_label = gtk_label_new (NULL);
    separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_end (GTK_BOX (hbox), firmware->bios_label, FALSE, FALSE, 12);
    gtk_box_pack_start (GTK_BOX (vbox), separator, TRUE, TRUE, 6);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox, TRUE, TRUE, 6);

    label = gtk_label_new (NULL);
    set_lable_style (label, "gray", 14, "选择固件", TRUE);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 12);
    picker = gtk_file_chooser_button_new ("Pick a File",
                                          GTK_FILE_CHOOSER_ACTION_OPEN);
    g_signal_connect (picker,
                     "selection-changed",
                      G_CALLBACK (chooser_selection_changed_cb),
                      firmware);

    gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (picker), FALSE);
    gtk_box_pack_start (GTK_BOX (hbox), picker, FALSE, FALSE, 6);

    firmware->update_button = gtk_button_new_with_label (_("Start Update"));
    g_signal_connect (firmware->update_button,
                     "clicked",
                      G_CALLBACK (start_update_firmware),
                      firmware);

    gtk_widget_set_sensitive (firmware->update_button, FALSE);
    gtk_box_pack_end (GTK_BOX (hbox), firmware->update_button, FALSE, FALSE, 12);

}

static GObject *
loongson_firmware_constructor (GType                  type,
                           guint                  n_construct_properties,
                           GObjectConstructParam *construct_properties)
{
    GObject        *obj;
    LoongsonFirmware   *firmware;

    obj = G_OBJECT_CLASS (loongson_firmware_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    firmware = LOONGSON_FIRMWARE (obj);
    loongson_firmware_fill (firmware);

    return obj;
}

static void
loongson_firmware_finalize (GObject *object)
{
    LoongsonFirmware *firmware;

    firmware = LOONGSON_FIRMWARE (object);
    
    g_free (firmware->image);
    g_free (firmware->name);
    if (firmware->filename != NULL)
        g_free (firmware->filename);
    G_OBJECT_CLASS (loongson_firmware_parent_class)->finalize (object);
}

static void
loongson_firmware_class_init (LoongsonFirmwareClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->constructor = loongson_firmware_constructor;
    gobject_class->finalize = loongson_firmware_finalize;
}

static void
loongson_firmware_init (LoongsonFirmware *firmware)
{
    gtk_orientable_set_orientation (GTK_ORIENTABLE (firmware), GTK_ORIENTATION_VERTICAL);
    
    firmware->image = g_strdup ("/usr/share/loongson-tools/icons/loongson-firmware.png");
    firmware->name = g_strdup ("firmware");
}

GtkWidget *
loongson_firmware_new (void)
{
    LoongsonFirmware *firmware;

    firmware = g_object_new (LOONGSON_TYPE_FIRMWARE, NULL);

    return GTK_WIDGET (firmware);
}

GtkWidget *
loongson_firmware_get_label (LoongsonFirmware *firmware)
{
    GtkWidget *label;

    label = create_tab_label (firmware->image, firmware->name);

    return label;
}

static void firmware_update_progress (LoongsonFirmware *firmware,
                                      double            percent)
{
    g_print ("percent = %lf\r\n",percent);
}


void loongson_firmware_set_proxy (LoongsonFirmware *firmware,
                                  LoongDaemon      *proxy)
{
    firmware->proxy = proxy;

    g_signal_connect_object (proxy,
                            "firmware-progress",
                             G_CALLBACK(firmware_update_progress),
                             firmware,
                             G_CONNECT_SWAPPED);

}

void
loongson_firmware_update (LoongsonFirmware *firmware)
{
    char *name;
    char *date;
    char *vendor;
    char *bios;
    GError *error = NULL;

    loong_daemon_call_firmware_name_sync (firmware->proxy,
                                          &name,
                                          NULL,
                                          &error);

    loong_daemon_call_firmware_date_sync (firmware->proxy,
                                          &date,
                                          NULL,
                                          &error);

    loong_daemon_call_firmware_vendor_sync (firmware->proxy,
                                            &vendor,
                                            NULL,
                                            &error);

    loong_daemon_call_bios_version_sync (firmware->proxy,
                                         &bios,
                                         NULL,
                                         &error);

    set_lable_style (firmware->name_label, "gray", 14, name, TRUE);
    set_lable_style (firmware->time_label, "gray", 14, date, TRUE);
    set_lable_style (firmware->vendor_label, "gray", 14, vendor, TRUE);
    set_lable_style (firmware->bios_label, "gray", 14, bios, TRUE);
}
