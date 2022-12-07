#ifndef __LOONGSON_DAEMON_H__
#define __LOONGSON_DAEMON_H__  1

#include <glib-object.h>

G_BEGIN_DECLS

#define LOONGSON_TYPE_DAEMON           (loongson_daemon_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonDaemon, loongson_daemon, LOONGSON, DAEMON, GObject);

LoongsonDaemon*     loongson_daemon_new    (GMainLoop *loop, gboolean replace);

G_END_DECLS

#endif /* __LOONGSON_DAEMON_H__ */
