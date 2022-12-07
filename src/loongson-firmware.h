#ifndef __LOONGSON_FIRMWARE__
#define __LOONGSON_FIRMWARE__

#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <glib/gi18n.h>
#include "daemon-dbus-generated.h"

G_BEGIN_DECLS

#define LOONGSON_TYPE_FIRMWARE         (loongson_firmware_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonFirmware, loongson_firmware, LOONGSON, FIRMWARE, GtkBox);

GtkWidget      *loongson_firmware_new                  (void);

GtkWidget      *loongson_firmware_get_label            (LoongsonFirmware *firmware);

void            loongson_firmware_update               (LoongsonFirmware *firmware,
                                                        LoongDaemon      *proxy);

G_END_DECLS

#endif
