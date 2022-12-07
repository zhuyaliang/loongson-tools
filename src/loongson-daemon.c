#include <stdio.h>

#include <gio/gio.h>
#include <glib/gi18n.h>
#include "daemon-dbus-generated.h"
#include "loongson-daemon.h"

#define LOONGSON_DBUS_NAME "cn.loongson.daemon"
#define LOONGSON_DBUS_PATH "/cn/loongson/daemon"


struct _LoongsonDaemon
{
    GObject            parent;
    LoongDaemon       *skeleton;
    guint              bus_name_id;
    GMainLoop         *loop;
    gboolean           replace;
};

enum {
    PROP_0,
    PROP_LOOP,
    PROP_REPLACE,
    LAST_PROP
};

typedef GDBusMethodInvocation GDBusMI;

static GParamSpec *properties[LAST_PROP] = { NULL };

G_DEFINE_TYPE (LoongsonDaemon, loongson_daemon, G_TYPE_OBJECT)

static gboolean loongson_daemon_get_biso_version (LoongDaemon *object,
                                                  GDBusMI     *invocation,
                                                  gpointer     user_data)
{
    gchar *bios_version = "V2.0";

    loong_daemon_complete_bios_version (object, invocation, g_strdup (bios_version));
    
    return TRUE;
}

static gboolean loongson_daemon_get_cpu_temperature (LoongDaemon *object,
                                                     GDBusMI     *invocation,
                                                     gpointer     user_data)
{
    gchar *temp = "60";

    loong_daemon_complete_cpu_temperature (object, invocation, g_strdup (temp));
    
    return TRUE;
}

static gboolean loongson_daemon_get_fan_speed (LoongDaemon *object,
                                               GDBusMI     *invocation,
                                               gpointer     user_data)
{
    gchar *fan_speed = "80";

    loong_daemon_complete_fan_speed (object, invocation, g_strdup (fan_speed));
    
    return TRUE;
}

static gboolean loongson_daemon_get_firmware_date (LoongDaemon *object,
                                                   GDBusMI     *invocation,
                                                   gpointer     user_data)
{
    gchar *date = "2022/12/07";

    loong_daemon_complete_firmware_date (object, invocation, g_strdup (date));
    
    return TRUE;
}

static gboolean loongson_daemon_get_firmware_name (LoongDaemon *object,
                                                   GDBusMI     *invocation,
                                                   gpointer     user_data)
{
    gchar *name = "Loongson-UDK2018-V1.49";

    loong_daemon_complete_firmware_name (object, invocation, g_strdup (name));
    
    return TRUE;
}

static gboolean loongson_daemon_firmware_update (LoongDaemon *object,
                                                     GDBusMI     *invocation,
                                                     const char  *file,
                                                     gpointer     user_data)
{

    loong_daemon_complete_firmware_update (object, invocation);
    
    return TRUE;
}

static gboolean loongson_daemon_get_firmware_vendor (LoongDaemon *object,
                                                     GDBusMI     *invocation,
                                                     gpointer     user_data)
{
    gchar *vendor = "Loongson";

    loong_daemon_complete_firmware_vendor (object, invocation, g_strdup (vendor));
    
    return TRUE;
}

static void set_dbus_signal_method (LoongsonDaemon *daemon)
{
    g_signal_connect (daemon->skeleton, "handle_bios_version", G_CALLBACK (loongson_daemon_get_biso_version), daemon);
    g_signal_connect (daemon->skeleton, "handle_cpu_temperature", G_CALLBACK (loongson_daemon_get_cpu_temperature), daemon);
    g_signal_connect (daemon->skeleton, "handle_fan_speed", G_CALLBACK (loongson_daemon_get_fan_speed), daemon);
    g_signal_connect (daemon->skeleton, "handle_firmware_date", G_CALLBACK (loongson_daemon_get_firmware_date), daemon);
    g_signal_connect (daemon->skeleton, "handle_firmware_name", G_CALLBACK (loongson_daemon_get_firmware_name), daemon);
    g_signal_connect (daemon->skeleton, "handle_firmware_update", G_CALLBACK (loongson_daemon_firmware_update), daemon);
    g_signal_connect (daemon->skeleton, "handle_firmware_vendor", G_CALLBACK (loongson_daemon_get_firmware_vendor), daemon);

}

static void bus_acquired_handler_cb (GDBusConnection *connection,
                                     const gchar     *name,
                                     gpointer         user_data)
{
    LoongsonDaemon *daemon;

    GError *error = NULL;
    gboolean exported;

    daemon = LOONGSON_DAEMON (user_data);

    set_dbus_signal_method (daemon);
    exported = g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (daemon->skeleton),
            connection, LOONGSON_DBUS_PATH, &error);

    if (!exported)
    {
        g_warning ("Failed to export interface: %s", error->message);
        g_error_free (error);

        g_main_loop_quit (daemon->loop);
    }
}

static void name_lost_handler_cb (GDBusConnection *connection,
                                  const gchar     *name,
                                  gpointer         user_data)
{
    LoongsonDaemon *daemon;

    daemon = LOONGSON_DAEMON (user_data);
    g_debug("bus name lost\n");

    g_main_loop_quit (daemon->loop);
}

static void loongson_daemon_constructed (GObject *object)
{
    LoongsonDaemon *daemon;

    GBusNameOwnerFlags flags;

    daemon = LOONGSON_DAEMON (object);

    G_OBJECT_CLASS (loongson_daemon_parent_class)->constructed (object);

    flags = G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT;
    if (daemon->replace)
        flags |= G_BUS_NAME_OWNER_FLAGS_REPLACE;

    daemon->bus_name_id = g_bus_own_name (G_BUS_TYPE_SYSTEM,
                                          LOONGSON_DBUS_NAME, flags,
                                          bus_acquired_handler_cb, NULL,
                                          name_lost_handler_cb, daemon, NULL);
}

static void loongson_daemon_dispose (GObject *object)
{
    LoongsonDaemon *daemon;

    daemon = LOONGSON_DAEMON (object);

    if (daemon->skeleton != NULL)
    {
        GDBusInterfaceSkeleton *skeleton;

        skeleton = G_DBUS_INTERFACE_SKELETON (daemon->skeleton);
        g_dbus_interface_skeleton_unexport (skeleton);

        g_clear_object (&daemon->skeleton);
    }

    if (daemon->bus_name_id > 0)
    {
        g_bus_unown_name (daemon->bus_name_id);
        daemon->bus_name_id = 0;
    }
    G_OBJECT_CLASS (loongson_daemon_parent_class)->dispose (object);
}

static void loongson_daemon_set_property (GObject *object,
                                          guint prop_id,
                                          const GValue *value,
                                          GParamSpec   *pspec)
{
    LoongsonDaemon *daemon;

    daemon = LOONGSON_DAEMON (object);

    switch (prop_id)
    {
        case PROP_LOOP:
            daemon->loop = g_value_get_pointer(value);
            break;
        case PROP_REPLACE:
            daemon->replace = g_value_get_boolean (value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void loongson_daemon_class_init (LoongsonDaemonClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->set_property = loongson_daemon_set_property;

    gobject_class->constructed = loongson_daemon_constructed;
    gobject_class->dispose = loongson_daemon_dispose;

    properties[PROP_LOOP] =
        g_param_spec_pointer("loop", "loop", "loop",
                G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE |
                G_PARAM_STATIC_STRINGS);
    properties[PROP_REPLACE] =
        g_param_spec_boolean ("replace", "replace", "replace", FALSE,
                G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE |
                G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties (gobject_class, LAST_PROP, properties);
}

static void loongson_daemon_init (LoongsonDaemon *daemon)
{
    daemon->skeleton = loong_daemon_skeleton_new();
}

LoongsonDaemon* loongson_daemon_new (GMainLoop *loop, gboolean replace)
{
    return g_object_new (LOONGSON_TYPE_DAEMON, "loop", loop, NULL);
}