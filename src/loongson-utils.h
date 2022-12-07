#ifndef __LOONGSON_UTILS__
#define __LOONGSON_UTILS__

#include <gtk/gtk.h>
#include <fcntl.h>
#include <sys/types.h>
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>

typedef enum
{
    ERROR = 0,
    WARING,
    INFOR,
    QUESTION
}MsgType;

void        set_lable_style         (GtkWidget   *lable ,
                                     const char  *color,
                                     int          font_szie,
                                     const char  *text,
                                     gboolean     blod);

GtkWidget  *create_tab_label         (const char *image_name,
                                      const char *label_name);

int         loongson_message_dialog  (const char *title,
                                      MsgType     type,
                                      const char *msg,...);

G_END_DECLS

#endif
