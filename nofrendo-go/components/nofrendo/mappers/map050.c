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
** map050.c
**
** Mapper #50 (SMB2j - 3rd discovered variation)
** Implementation by Firebug
** Mapper information courtesy of Kevin Horton
** $Id: map050.c,v 1.2 2001/04/27 14:37:11 neil Exp $
**
*/

#include <nofrendo.h>
#include <nes_mmc.h>
#include <nes.h>

static struct
{
    uint16 enabled;
    uint16 counter;
} irq;


static void map_irq_reset(void)
{
    /* Turn off IRQs */
    irq.enabled = false;
    irq.counter = 0x0000;
}

static void map_hblank(int scanline)
{
    /* Increment the counter if it is enabled and check for strike */
    if (irq.enabled)
    {
        irq.counter += nes_getptr()->cycles_per_line;

        /* IRQ line is hooked to Q12 of the counter */
        if (irq.counter & 0x1000)
        {
            nes6502_irq();
            map_irq_reset();
        }
    }
}

static void map_write(uint32 address, uint8 value)
{
    uint8 selectable_bank;

    /* For address to be decoded, A5 must be high and A6 low */
    if ((address & 0x60) != 0x20) return;

    /* A8 low  = $C000-$DFFF page selection */
    /* A8 high = IRQ timer toggle */
    if (address & 0x100)
    {
        /* IRQ settings */
        if (value & 0x01) irq.enabled = true;
        else              map_irq_reset();
    }
    else
    {
        /* Stupid data line swapping */
        selectable_bank = 0x00;
        if (value & 0x08) selectable_bank |= 0x08;
        if (value & 0x04) selectable_bank |= 0x02;
        if (value & 0x02) selectable_bank |= 0x01;
        if (value & 0x01) selectable_bank |= 0x04;
        mmc_bankrom (8, 0xC000, selectable_bank);
    }
}


/**************************************************************/
/* Mapper #50: 3rd discovered variation of SMB2j cart bootleg */
/**************************************************************/
static void map_init (void)
{
    mmc_bankrom(8, 0x6000, 0x0F);
    mmc_bankrom(8, 0x8000, 0x08);
    mmc_bankrom(8, 0xA000, 0x09);
    mmc_bankrom(8, 0xE000, 0x0B);
    map_irq_reset();
}

static mem_write_handler_t map_memwrite [] =
{
   { 0x4000, 0x5FFF, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map50_intf =
{
   50,                               /* Mapper number */
   "SMB2j (3rd discovered variant)", /* Mapper name */
   map_init,                       /* Initialization routine */
   NULL,                             /* VBlank callback */
   map_hblank,                     /* HBlank callback */
   NULL,                             /* Get state (SNSS) */
   NULL,                             /* Set state (SNSS) */
   NULL,                             /* Memory read structure */
   map_memwrite,                   /* Memory write structure */
   NULL                              /* External sound device */
};

/*
** $Log: map050.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1  2001/04/27 10:57:41  neil
** wheee
**
** Revision 1.1  2000/12/27 19:22:13  firebug
** initial revision
**
**
*/

#endif