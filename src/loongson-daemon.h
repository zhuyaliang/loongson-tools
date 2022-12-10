#ifndef __LOONGSON_DAEMON_H__
#define __LOONGSON_DAEMON_H__  1

#include <glib-object.h>

G_BEGIN_DECLS

#define LS7A_MISC_BASE                  (0x0)
#define LS7A_PWM_BASE                   (LS7A_MISC_BASE + 0x20000)

/*PWM*/
#define LS7A_PWM0_REG_BASE              (LS7A_MISC_BASE + 0x0)
#define LS7A_PWM0_LOW                   (LS7A_PWM0_REG_BASE + 0x4)
#define LS7A_PWM0_FULL                  (LS7A_PWM0_REG_BASE + 0x8)
#define LS7A_PWM0_CTRL                  (LS7A_PWM0_REG_BASE + 0xc)
#define CPU_TEMP_REG                    (0x1fe00198 + 0x4)

#define Writel(addr, data)              (*(volatile uint*)(addr) = (data))
#define Readl(addr)                     (*(volatile uint*)(addr))

#define LOONGSON_TYPE_DAEMON            (loongson_daemon_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonDaemon, loongson_daemon, LOONGSON, DAEMON, GObject);

LoongsonDaemon*     loongson_daemon_new    (GMainLoop *loop, gboolean replace);

G_END_DECLS

#endif /* __LOONGSON_DAEMON_H__ */
