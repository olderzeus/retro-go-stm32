#include "build/config.h"

#ifdef ENABLE_EMULATOR_NES
/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** map046.c
**
** Mapper #46 (Pelican Game Station)
** Implementation by Firebug
** Mapper information courtesy of Kevin Horton
** $Id: map046.c,v 1.2 2001/04/27 14:37:11 neil Exp $
**
*/

#include <nofrendo.h>
#include <nes_mmc.h>
#include <nes.h>

static uint32 prg_bank;
static uint32 chr_bank;


static void map_set_banks(void)
{
    mmc_bankrom(32, 0x8000, prg_bank);
    mmc_bankvrom(8, 0x0000, chr_bank);
}

static void map_write(uint32 address, uint8 value)
{
    /* $8000-$FFFF: D6-D4 = lower three bits of CHR bank */
    /*              D0    = low bit of PRG bank          */
    /* $6000-$7FFF: D7-D4 = high four bits of CHR bank   */
    /*              D3-D0 = high four bits of PRG bank   */
    if (address & 0x8000)
    {
        prg_bank &= ~0x01; // Keep only the high part
        prg_bank |= value & 0x01;

        chr_bank &= ~0x07; // Keep only the high part
        chr_bank |= (value >> 4) & 0x07;
    }
    else
    {
        prg_bank &= 0x01; // Keep only the low part
        prg_bank |= (value & 0x0F) << 1;

        chr_bank &= 0x07; // Keep only the low part
        chr_bank |= ((value >> 4) & 0x0F) << 3;
    }
    map_set_banks();
}

/*********************************************************/
/* Mapper #46: Pelican Game Station (aka Rumble Station) */
/*********************************************************/
static void map_init (void)
{
    prg_bank = 0x00;
    chr_bank = 0x00;
    map_set_banks();
}

static mem_write_handler_t map_memwrite [] =
{
   { 0x6000, 0xFFFF, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map46_intf =
{
   46,                               /* Mapper number */
   "Pelican Game Station",           /* Mapper name */
   map_init,                       /* Initialization routine */
   NULL,                             /* VBlank callback */
   NULL,                             /* HBlank callback */
   NULL,                             /* Get state (SNSS) */
   NULL,                             /* Set state (SNSS) */
   NULL,                             /* Memory read structure */
   map_memwrite,                   /* Memory write structure */
   NULL                              /* External sound device */
};

/*
** $Log: map046.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1  2001/04/27 10:57:41  neil
** wheee
**
** Revision 1.1  2000/12/27 19:23:05  firebug
** initial revision
**
**
*/

#endif