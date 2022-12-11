#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "spi.h"


//UINT64 SPI_REG_BASE;
#define readw(addr)  (*(volatile unsigned int *)(addr))

//#define  SPI_ADDR 0x1fe001f0
#define SPI_ADDR  0  //Get at runtime
#define FLASH_SIZE 4128768

#define GPIO_0        (0x1<<0)
#define GPIO_1        (0x1<<1)
#define GPIO_2        (0x1<<2)
#define GPIO_3        (0x1<<3)
#define GPIO_12       (0x1<<12)
#define GPIO_CS_BIT           GPIO_0
#define GPIO_CPU_CS_ENABLE    GPIO_3

#define SPE_OFFSET     6
#define MSTR_OFFSET    4
#define CPOL_OFFSET    3
#define CPHA_OFFSET    2
#define SPR_OFFSET     0
#define SPE            (1<<SPE_OFFSET)
#define MSTR           (1<<MSTR_OFFSET)

#define BP0    (1 << 2)
#define BP1    (1 << 3)
#define BP2    (1 << 4)
#define BP3    0

#define REG_SPCR   0x00
#define REG_SPSR   0x01
#define REG_SPDR   0x02
#define REG_SPER   0x03
#define REG_SOFTCS 0x05
#define REG_TIME   0x06
#define REG_PARAM  0x04
#define WREN       0x06
#define WRDI       0x04
#define RDID       0x90
#define RDSR       0x05
#define WRSR       0x01
#define READ       0x03
#define AAI        0x02
#define EBSY       0x70
#define DBSY       0x80
#define EWSR       0x50
#define FAST_READ  0x0B
#define BYTE_WRITE 0x02     /* Byte Program Enable */
#define AAI_WRITE  0xad     /* Byte Program Enable */
#define BE4K       0x20     /* 4K Byte block Rrase, Sector Erase */
#define BE4KSIZE   0x1000   /* 4K Byte block Rrase, Sector Erase */
#define BE32K      0x52     /* 32K Byte block Rrase, Sector Erase */
#define BE32KSIZE  0x8000   /* 32K Byte block Rrase, Sector Erase */
#define BE64K      0xD8     /* 64K Byte block Rrase, Sector Erase */
#define BE64KSIZE  0x10000  /* 64K Byte block Rrase, Sector Erase */
#define CHIPERASE  0xC7     /* Chip Erase */
#define BLKSIZE    0x1000
#define BLKERASE   BE4K
#define MACID_ADDR 0x00
#define DEVID_ADDR 0x01
#define VOID_ADDR  0x00
#define VOID_CMD   0x00
#define CHIPERASESET               0x4
#define CSCCHIPERASET              0x5
#define PROGRAM_AAI_MODE           0x1
#define SPI_FREE                   0x00
#define SPI_BUSY                   0x01

#define STORE8(Addr, Value)    *(volatile UINT8 *)((Addr)) = ((UINT8) (Value & 0xff))
#define LOAD8(Addr)            *(volatile UINT8 *)((Addr))
#define REGSET(Id,Value)       STORE8(SPI_REG_BASE + Id, Value)
#define REGGET(Id)             LOAD8(SPI_REG_BASE + Id)

#define LS3ASPIFLASHADDR     0x900000001fe001f0
#define LS7ASPIFLASHADDR     GetLs7ASpiRegBaseAddr()
#define SpiFlashDelayUs(x) 

#define LS3APROC_PLL_REG     0x900000001fe001d0
#define LS3APROC_MISC_REG    0x900000001fe00420

#define INVALID_OFFSET(x)     ((x > 0x400000)||(x < 0)) ? TRUE:FALSE
#define TCM_INVALID_OFFSET(x)     ((x > 0xffffffff)||(x < 0x0)) ? TRUE:FALSE
#define INVALID_NUM(x)        ((x > 0x400000)||( x <= 0x0)) ? TRUE:FALSE
#define IS_SST25VF032B(M,D,C) ((M == 0xBF)&&(D == 0x25)&&(C == 0x4A)) ? TRUE:FALSE

static UINT8 ValueRegSpcr  = 0xFF;
static UINT8 ValueRegSpsr  = 0xFF;
static UINT8 ValueRegSper  = 0xFF;
static UINT8 ValueRegParam = 0xFF;

static void ResetSfcParamReg (void)
{
    REGSET(REG_PARAM,0x47);
}

static void
SpiFlashSetCs (INTN Bit)
{
    UINT8          Val;

    if (Bit)
        Val = 0x11;
    else
        Val = 0x01;
    REGSET(CSCCHIPERASET,Val);

    SpiFlashDelayUs (3);
}

static UINT8
SpiFlashWriteByteCmd (UINT8 Value)
{
    UINT8 Ret;
    INT32 TimeOut = 100000;

    REGSET (REG_SPDR, Value);
    while ((REGGET(REG_SPSR) & 0x01) && TimeOut--);
    Ret = REGGET(REG_SPDR);
    if (TimeOut < 0)
        while(1);

    return Ret;
}

static UINT8
SpiFlashReadByteCmd (void)
{
    return SpiFlashWriteByteCmd(0x00);
}

static void
SpiFlashInit (void)
{
    if (ValueRegSpcr == 0xFF) {
        ValueRegSpcr = REGGET(REG_SPCR);
    }
    if (ValueRegSpsr == 0xFF) {
        ValueRegSpsr = REGGET(REG_SPSR);
    }
    if (ValueRegSper == 0xFF) {
        ValueRegSper = REGGET(REG_SPER);
    }
    if (ValueRegParam == 0xFF) {
        ValueRegParam = REGGET(REG_PARAM);
    }

    //[spre:spr] [01:00] means clk_div=8
    REGSET(REG_SPCR, 0x52);//[1:0] [0:0]spr
    REGSET(REG_SPSR, 0xc0);
    REGSET(REG_SPER, 0x04);//[1:0] [0:1]spre
    REGSET(REG_PARAM, 0x20);
    REGSET(REG_TIME, 0x01);
}

static void
SpiFlashReset (void)
{

    REGSET(REG_SPCR, ValueRegSpcr);
    REGSET(REG_SPSR, ValueRegSpsr);
    REGSET(REG_SPER, ValueRegSper);
    REGSET(REG_PARAM, ValueRegParam);

    ValueRegSpcr  = 0xFF;
    ValueRegSpsr  = 0xFF;
    ValueRegSper  = 0xFF;
    ValueRegParam = 0xFF;
}

static UINT8 
SpiFlashReadStatus (void)
{
    UINT8 Val;

    REGSET(REG_SOFTCS,0x01);
    SpiFlashWriteByteCmd(RDSR);
    Val = SpiFlashReadByteCmd();
    REGSET(REG_SOFTCS,0x11);

    return Val;
}

static UINTN
SpiFlashWait (void)
{
    UINTN Ret;
    INT32 TimeOut = 100000;
    INT32 Count = 5;
    do {
        Ret = SpiFlashReadStatus();
        if (TimeOut == 0)
        {
            Count--;
            if (Count < 0 )
                while(1);
            TimeOut = 100000;
            SpiFlashDelayUs(2);
        }
    } while ((Ret & 1) && TimeOut--);

    return Ret;
}

static void 
SpiFlashEWRS (void)
{
    SpiFlashWait();
    REGSET(REG_SOFTCS,0x01);
    SpiFlashWriteByteCmd(EWSR);
    REGSET(REG_SOFTCS,0x11);
}

static void
SpiFlashWriteDisable (void)
{
    SpiFlashWait();
    REGSET(REG_SOFTCS,0x01);
    SpiFlashWriteByteCmd(WRDI);
    REGSET(REG_SOFTCS,0x11);
}

static void
SpiFlashWriteStatus (UINT8 Val)
{
    SpiFlashEWRS();
    REGSET(REG_SOFTCS,0x01);
    SpiFlashWriteByteCmd(WRSR);
    SpiFlashWriteByteCmd(Val);
    REGSET(REG_SOFTCS,0x11);

    SpiFlashWriteDisable();
}

static void
SpiFlashDisableWriteProtection (void)
{

    UINT8 Val;

    Val = SpiFlashWait ();
    Val &= ~(BP0 | BP1 | BP2);
    SpiFlashWriteStatus(Val);
    SpiFlashWait ();

#if ENABLE_WRITE_PROTECTION
    /*some flash not support 0x50 need the follow code*/
    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (WREN);
    SpiFlashSetCs (1);
    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (WRSR);
    SpiFlashWriteByteCmd (Val);
    SpiFlashSetCs (1);
    SpiFlashWait ();
    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (WREN);
    SpiFlashSetCs (1);
#endif
}

static void
SpiFlashEnableWriteProtection (void)
{
    UINT8 Val;

    Val = (BP0 | BP1 | BP2);
    SpiFlashWriteStatus(Val);
    SpiFlashWait ();
#if ENABLE_WRITE_PROTECTION
    /*some flash not support 0x50 need the follow code*/
    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (WREN);
    SpiFlashSetCs (1);
    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (WRSR);
    SpiFlashWriteByteCmd (Val);
    SpiFlashSetCs (1);
    SpiFlashWait ();

    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (WRDI);
    SpiFlashSetCs (1);
    /*wait command executed done*/
    SpiFlashWait();
#endif
}

static void
SpiFlashWriteByte (UINTN  Addr0,
                   UINTN  Addr1,
                   UINTN  Addr2,
                   UINT8  Buf)
{
    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (WREN);
    SpiFlashSetCs (1);

    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (BYTE_WRITE);
    SpiFlashWriteByteCmd (Addr2);
    SpiFlashWriteByteCmd (Addr1);
    SpiFlashWriteByteCmd (Addr0);
    SpiFlashWriteByteCmd (Buf);
    SpiFlashSetCs (1);
    SpiFlashWait ();
}

static void
SpiFlashEraseBlock (
        UINTN  Addr0,
        UINTN  Addr1,
        UINTN  Addr2)
{
    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (WREN);
    SpiFlashSetCs (1);
    SpiFlashWait();

    SpiFlashSetCs (0);
    SpiFlashWriteByteCmd (BLKERASE);
    SpiFlashWriteByteCmd (Addr2);
    SpiFlashWriteByteCmd (Addr1);
    SpiFlashWriteByteCmd (Addr0);
    SpiFlashSetCs (1);
}
static void
SpiFlashEraseAndWriteBlocks (UINTN      Offset,
                             char      *Buffer,
                             UINTN      Num)
{
    UINTN  Pos = Offset;
    UINT8  *Buf = (UINT8 *) Buffer;
    UINTN  Index = 0;
    UINTN  Addr0;
    UINTN  Addr1;
    UINTN  Addr2;

    /* Erase blocks step */
    printf("Erase   : ");
    do {
        if ((Pos % (4 * BLKSIZE)) == 0) {
            printf("*");
            fflush(stdout);
        }
        Addr0 =  Pos & 0xff;
        Addr1 =  (Pos >> 8) & 0xff;
        Addr2 =  (Pos >> 16) & 0xff;

        SpiFlashEraseBlock (Addr0, Addr1, Addr2);
        SpiFlashDelayUs(1);
        Pos += BLKSIZE;
        SpiFlashWait();
    } while (Pos < Num + Offset);
    printf("   Erase OK.\n");

    /* Write blocks step */
    Pos = Offset;
    printf("Program : ");
    while (Index < Num) {
        if ((Index % 0x4000) == 0) {
            printf("*");
            fflush(stdout);
        }
        Addr0 = (Pos + Index) & 0xff;
        Addr1 = ((Pos + Index) >> 8) & 0xff;
        Addr2 = ((Pos + Index) >> 16) & 0xff;

        SpiFlashWriteByte (Addr0, Addr1, Addr2, Buf[Index]);
        Index++;
    }
    printf("   Program OK.\n");
}

gboolean
UpdateBiosInSpiFlash (UINTN      Offset,
                      char      *Buffer,
                      UINTN      Num)
{
    SpiFlashInit ();
    SpiFlashDisableWriteProtection ();
    SpiFlashEraseAndWriteBlocks (Offset, Buffer, Num);
    SpiFlashEnableWriteProtection ();
    SpiFlashReset ();
    ResetSfcParamReg();

    return TRUE;
}
