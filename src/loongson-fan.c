/*************************************************************************
 File Name: loongson-fan.c
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

 Created Time: 2022年12月06日 星期四 22时34分47秒
************************************************************************/
#include "loongson-fan.h"
#include "loongson-utils.h"

struct _LoongsonFan
{
    GtkBox        box;
    LoongDaemon  *proxy;

    char      *name;
    char      *image;
    GtkWidget *scale;
    GtkWidget *temp_label;
    guint      time_id;
};

G_DEFINE_TYPE (LoongsonFan, loongson_fan, GTK_TYPE_BOX)

static void
set_scale_data (GtkWidget *scale)
{
    gtk_scale_set_draw_value (GTK_SCALE (scale), TRUE);

    gtk_scale_add_mark (GTK_SCALE (scale), 20.0, GTK_POS_BOTTOM, "<span color='red'>20</span>");
    gtk_scale_add_mark (GTK_SCALE (scale), 30.0, GTK_POS_BOTTOM, "<span color='red'>30</span>");
    gtk_scale_add_mark (GTK_SCALE (scale), 40.0, GTK_POS_BOTTOM, "<span color='red'>40</span>");
    gtk_scale_add_mark (GTK_SCALE (scale), 50.0, GTK_POS_BOTTOM, "<span color='red'>50</span>");
    gtk_scale_add_mark (GTK_SCALE (scale), 60.0, GTK_POS_BOTTOM, "<span color='red'>60</span>");
    gtk_scale_add_mark (GTK_SCALE (scale), 70.0, GTK_POS_BOTTOM, "<span color='red'>70</span>");
    gtk_scale_add_mark (GTK_SCALE (scale), 80.0, GTK_POS_BOTTOM, "<span color='red'>80</span>");
    gtk_scale_add_mark (GTK_SCALE (scale), 90.0, GTK_POS_BOTTOM, "<span color='red'>90</span>");
    gtk_scale_add_mark (GTK_SCALE (scale), 100.0, GTK_POS_BOTTOM, "<span color='red'>100</span>");
}

static void
loongson_fan_fill (LoongsonFan *fan)
{
    GtkWidget *fixed;
    GtkWidget *box;
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *label;

    fixed = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (fan), fixed);

    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    gtk_fixed_put (GTK_FIXED (fixed), box, 18, 10);

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start (GTK_BOX (box), vbox, TRUE, TRUE, 6);
    gtk_widget_set_size_request (vbox, 480, -1);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 6);

    label = gtk_label_new (NULL);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 6);
    set_lable_style (label, "gray", 16, "风扇转速", TRUE);

    fan->scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 20.0, 100.0, 1.0);
    set_scale_data (fan->scale);
    gtk_box_pack_end (GTK_BOX (hbox), fan->scale, TRUE, TRUE, 6);

    fan->temp_label = gtk_label_new (NULL);
    gtk_box_pack_start (GTK_BOX (vbox), fan->temp_label, TRUE, TRUE, 18);
}

static GObject *
loongson_fan_constructor (GType                  type,
                           guint                  n_construct_properties,
                           GObjectConstructParam *construct_properties)
{
    GObject        *obj;
    LoongsonFan   *fan;

    obj = G_OBJECT_CLASS (loongson_fan_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    fan = LOONGSON_FAN (obj);
    loongson_fan_fill (fan);

    return obj;
}

static void
loongson_fan_finalize (GObject *object)
{
    LoongsonFan *fan;

    fan = LOONGSON_FAN (object);
    
    g_free (fan->image);
    g_free (fan->name);
    G_OBJECT_CLASS (loongson_fan_parent_class)->finalize (object);
}

static void
loongson_fan_class_init (LoongsonFanClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->constructor = loongson_fan_constructor;
    gobject_class->finalize = loongson_fan_finalize;
}

static void
loongson_fan_init (LoongsonFan *fan)
{
    gtk_orientable_set_orientation (GTK_ORIENTABLE (fan), GTK_ORIENTATION_VERTICAL);
    
    fan->image = g_strdup ("/usr/share/loongson-tools/icons/loongson-fan.png");
    fan->name = g_strdup ("fan");
}

static gboolean
update_loongson_temp (LoongsonFan *fan)
{
    guint   temperature;
    char   *text;
    GError *error = NULL;

    loong_daemon_call_cpu_temperature_sync (fan->proxy,
                                           &temperature,
                                            NULL,
                                           &error);

    text = g_strdup_printf (_("CPU temperature: %u"), temperature);
    set_lable_style (fan->temp_label, "red", 14, text, TRUE);

    g_free (text);
    
    return TRUE;
}

GtkWidget *
loongson_fan_new (void)
{
    LoongsonFan *fan;

    fan = g_object_new (LOONGSON_TYPE_FAN, NULL);

    return GTK_WIDGET (fan);
}

GtkWidget *
loongson_fan_get_label (LoongsonFan *fan)
{
    GtkWidget *label;

    label = create_tab_label (fan->image, fan->name);

    return label;
}

void
loongson_fan_update (LoongsonFan *fan, LoongDaemon  *proxy)
{
    gint fan_speed;

    if (fan->time_id > 0)
        g_source_remove (fan->time_id);
    
    fan->proxy  = proxy;
    fan->time_id = g_timeout_add (1000, (GSourceFunc) update_loongson_temp, fan);
    fan_speed  = loong_daemon_get_fan_speed (proxy);

    g_print ("fsn_speed = %d\r\n",fan_speed);
    gtk_range_set_value (GTK_RANGE (fan->scale),  80.0);
}
