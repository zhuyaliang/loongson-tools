/*************************************************************************
 File Name: src/loongson-utils.c
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

 Created Time: 2022年12月06日 星期五 21时17分47秒
************************************************************************/
#include "loongson-utils.h"

#define MSGFORMAT           "<span foreground='red'font_desc='13'>%s </span>"

typedef enum
{
    ERROR = 0,
    WARING,
    INFOR,
    QUESTION
}MsgType;

void set_lable_style (GtkWidget  *lable,
                      const char *color,
                      int         font_szie,
                      const char *text,
                      gboolean    blod)
{
    char *lable_text = NULL;

    if (color == NULL)
    {
        lable_text = g_strdup_printf ("<span weight=\'light\'font_desc=\'%d\'><b>%s</b></span>", font_szie, text);
    }
    else
    {
        if(blod)
        {
            lable_text = g_strdup_printf ("<span foreground=\'%s\'weight=\'light\'font_desc=\'%d\'><b>%s</b></span>",
                             color,
                             font_szie,
                             text);
        }
        else
        {
            lable_text = g_strdup_printf("<span foreground=\'%s\'weight=\'light\'font_desc=\'%d\'>%s</span>",
                            color,
                            font_szie,
                            text);
        }
    }

    gtk_label_set_markup (GTK_LABEL(lable), lable_text);

    g_free (lable_text);
}

int loongson_message_dialog (const char *title, MsgType type, const char *msg,...)
{
    GtkWidget *dialog = NULL;
    va_list    args;
    char      *message;
    int        ret;

    switch(type)
    {
        case ERROR:
        {
            dialog = gtk_message_dialog_new (NULL,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_OK,
                                             "%s",title);
            break;
        }
        case WARING:
        {
            dialog = gtk_message_dialog_new (NULL,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_WARNING,
                                             GTK_BUTTONS_OK,
                                             "%s",title);
            break;
        }
        case INFOR:
        {
            dialog = gtk_message_dialog_new (NULL,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK,
                                             "%s",title);
            break;
        }
        case QUESTION:
        {
            dialog = gtk_message_dialog_new (NULL,
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO,
                                             "%s",title);
            break;
        }
        default :
            break;

    }

    va_start (args, msg);
    message = g_strdup_vprintf (msg, args);
    va_end (args);

    gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
                                                MSGFORMAT,
                                                message);

    gtk_window_set_title (GTK_WINDOW (dialog), _("Message"));
    ret = gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    g_free (message);

    return ret;
}

GtkWidget *create_tab_label (const char *image_name, const char *label_name)
{
    g_autoptr(GdkPixbuf) pb = NULL;
    g_autoptr(GdkPixbuf) pb2 = NULL;
    GtkWidget *image;
    GtkWidget *box;
    GtkWidget *label;

    pb = gdk_pixbuf_new_from_file (image_name, NULL);
    pb2 = gdk_pixbuf_scale_simple (pb, 45, 45, GDK_INTERP_BILINEAR);
    image = gtk_image_new_from_pixbuf (pb2);
    
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
    label = gtk_label_new (NULL);
    set_lable_style (label, "gray", 16, label_name, TRUE);
    
    gtk_box_pack_start (GTK_BOX (box), image, TRUE, TRUE, 6);
    gtk_box_pack_start (GTK_BOX (box), label, TRUE, TRUE, 6);

    gtk_widget_show_all (box);
  
    return box;
}   

