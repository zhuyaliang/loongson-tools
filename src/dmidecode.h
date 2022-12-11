#ifndef __DMIDECODE_H__
#define __DMIDECODE_H__  1

#include <glib-object.h>

G_BEGIN_DECLS

#define LOONGSON_TYPE_DMIDECODE           (loongson_dmidecode_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonDmidecode, loongson_dmidecode, LOONGSON, DMIDECODE, GObject);

LoongsonDmidecode*   loongson_dmidecode_new       (void);

const char          *dmi_get_firmware_date        (LoongsonDmidecode *dmi);

const char          *dmi_get_firmware_vendor      (LoongsonDmidecode *dmi);

const char          *dmi_get_firmware_name        (LoongsonDmidecode *dmi);

const char          *dmi_get_bios_version         (LoongsonDmidecode *dmi);

G_END_DECLS

#endif /* __DMIDECODE_H__ */
