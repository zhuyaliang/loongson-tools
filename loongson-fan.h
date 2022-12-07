#ifndef __LOONGSON_FAN__
#define __LOONGSON_FAN__

#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>

G_BEGIN_DECLS

#define LOONGSON_TYPE_FAN         (loongson_fan_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonFan, loongson_fan, LOONGSON, FAN, GtkBox);

GtkWidget      *loongson_fan_new                  (void);

GtkWidget      *loongson_fan_get_label            (LoongsonFan *fan);

void            loongson_fan_update               (LoongsonFan *fan);

G_END_DECLS

#endif
