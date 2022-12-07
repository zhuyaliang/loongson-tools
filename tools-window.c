/*************************************************************************
  File Name: tools-window.c

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

  Created Time: 2022年12月06日 星期四 17时01分28秒
 ************************************************************************/

#include "tools-window.h"
#include "loongson-firmware.h"
#include "loongson-fan.h"
#include "loongson-utils.h"

struct _ToolsWindow
{
    GtkWindow     window;
    GtkWidget    *firmware;
    GtkWidget    *fan;
    GtkWidget    *usb;
    GtkWidget    *secure;
    GtkWidget    *other;
};

G_DEFINE_TYPE (ToolsWindow, tools_window, GTK_TYPE_WINDOW)

static void
tools_window_fill (ToolsWindow *loongwin)
{
    GtkWidget *notebook;
    GtkWidget *label;

    notebook = gtk_notebook_new ();
    gtk_notebook_set_scrollable (GTK_NOTEBOOK (notebook), TRUE);
    gtk_container_add (GTK_CONTAINER (loongwin), notebook);
    gtk_widget_realize (notebook);

    loongwin->firmware = loongson_firmware_new ();
    label = loongson_firmware_get_label (LOONGSON_FIRMWARE (loongwin->firmware));
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), loongwin->firmware, label);

    loongwin->fan = loongson_fan_new ();
    label = loongson_fan_get_label (LOONGSON_FAN (loongwin->fan));
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), loongwin->fan, label);
    
}

static GObject *
tools_window_constructor (GType                  type,
                          guint                  n_construct_properties,
                          GObjectConstructParam *construct_properties)
{
    GObject       *obj;
    ToolsWindow   *toolswin;

    obj = G_OBJECT_CLASS (tools_window_parent_class)->constructor (type,
                                      n_construct_properties,
                                      construct_properties);

    toolswin = TOOLS_WINDOW (obj);
    tools_window_fill (toolswin);

    return obj;
}

static void
tools_window_destroy (GtkWidget *widget)
{
    GTK_WIDGET_CLASS (tools_window_parent_class)->destroy (widget);

    gtk_main_quit ();
}

static void
tools_window_class_init (ToolsWindowClass *klass)
{
    GObjectClass   *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *gtk_class = GTK_WIDGET_CLASS (klass);

    gobject_class->constructor = tools_window_constructor;
    gtk_class->destroy = tools_window_destroy;
}

static void
tools_window_init (ToolsWindow *toolswin)
{
    GtkWindow  *window;

    window = GTK_WINDOW (toolswin);
    gtk_window_set_title (GTK_WINDOW (window), _("Loonsgon Tools"));
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

    gtk_window_set_position (window, GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW (window), 550, 500);
}

void tools_window_update (ToolsWindow *win)
{
    loongson_firmware_update (LOONGSON_FIRMWARE (win->firmware));
    loongson_fan_update (LOONGSON_FAN (win->fan));
}


GtkWidget *
tools_window_new (void)
{
    ToolsWindow *toolswin;

    toolswin = g_object_new (TOOLS_TYPE_WINDOW,
                             "type", GTK_WINDOW_TOPLEVEL,
                              NULL);

    
    tools_window_update (toolswin);
    return GTK_WIDGET (toolswin);
}
