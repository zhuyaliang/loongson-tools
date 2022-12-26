#ifndef __SPI_H__
#define __SPI_H__ 

#include <glib-object.h>
#include "daemon-dbus-generated.h"

#define FLASH_SIZE             4128768

typedef char                INT8;
typedef unsigned char       UINT8;
typedef short               INT16;
typedef unsigned short      UINT16;
typedef int                 INT32;
typedef unsigned int        UINT32;
typedef long long           INT64;
typedef unsigned long long  UINT64;
typedef long long           INTN;
typedef unsigned long long  UINTN;

extern UINT64 SPI_REG_BASE;

gboolean
UpdateBiosInSpiFlash             (UINTN        Offset,
                                  char        *Buffer,
                                  UINTN        Num,
                                  LoongDaemon *object);
#endif
