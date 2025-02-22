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
** map4.c
**
** mapper 4 interface
** $Id: map004.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <nofrendo.h>
#include <nes_mmc.h>

static struct
{
   uint8 counter;
   uint8 latch;
   uint8 enabled;
} irq;

static uint8 reg8000;
static uint16 vrombase;


/* mapper 4: MMC3 */
static void mmc3_bankchr(int size, uint32 address, int bank)
{
    if ((bank >= 0x00) && (bank <= 0x03)) {
       mmc_bankchr(size, address, bank & 0x07, CHR_RAM);
    } else {
       mmc_bankchr(size, address, bank, CHR_ROM);
    }
}

static void map_write(uint32 address, uint8 value)
{
    switch (address & 0xE001)
    {
    case 0x8000:
        reg8000 = value;
        vrombase = (value & 0x80) ? 0x1000 : 0x0000;
        mmc_bankrom(8, (value & 0x40) ? 0x8000 : 0xC000, -2);
        break;

    case 0x8001:
        switch (reg8000 & 0x07)
        {
        case 0:
            value &= 0xFE;
            mmc3_bankchr(1, vrombase ^ 0x0000, value);
            mmc3_bankchr(1, vrombase ^ 0x0400, value + 1);
            break;
        case 1:
            value &= 0xFE;
            mmc3_bankchr(1, vrombase ^ 0x0800, value);
            mmc3_bankchr(1, vrombase ^ 0x0C00, value + 1);
            break;
        case 2:
            mmc3_bankchr(1, vrombase ^ 0x1000, value);
            break;

        case 3:
            mmc3_bankchr(1, vrombase ^ 0x1400, value);
            break;

        case 4:
            mmc3_bankchr(1, vrombase ^ 0x1800, value);
            break;

        case 5:
            mmc3_bankchr(1, vrombase ^ 0x1C00, value);
            break;

        case 6:
            mmc_bankrom(8, (reg8000 & 0x40) ? 0xC000 : 0x8000, value);
            break;

        case 7:
            mmc_bankrom(8, 0xA000, value);
            break;
        }
        break;

    case 0xA000:
        if (mmc_getinfo()->flags & ROM_FLAG_FOURSCREEN)
            ppu_setmirroring(PPU_MIRROR_FOUR);
        else if (value & 1)
            ppu_setmirroring(PPU_MIRROR_HORI);
        else
            ppu_setmirroring(PPU_MIRROR_VERT);
        break;

    case 0xA001:
        /* Save RAM enable / disable */
        /* Messes up Startropics I/II if implemented -- bah */
        break;

    case 0xC000:
        irq.latch = value; //  - 1
        break;

    case 0xC001:
        irq.counter = 0; // Trigger reload
        break;

    case 0xE000:
        irq.enabled = false;
        break;

    case 0xE001:
        irq.enabled = true;
        break;

    default:
        MESSAGE_DEBUG("map004: unhandled write: address=%p, value=0x%x\n", (void*)address, value);
        break;
    }
}

static void map_hblank(int scanline)
{
    if (scanline > 240)
        return;

    if (!ppu_enabled())
        return;

    if (irq.counter == 0)
        irq.counter = irq.latch;
    else
        irq.counter--;

    if (irq.enabled && irq.counter == 0)
        nes6502_irq();
}

static void map_getstate(uint8 *state)
{
    state[0] = irq.counter;
    state[1] = irq.latch;
    state[2] = irq.enabled;
    state[3] = reg8000;
}

static void map_setstate(uint8 *state)
{
    irq.counter =state[0];
    irq.latch = state[1];
    irq.enabled = state[2];
    map_write(0x8000, state[3]);
}

static void map_init(void)
{
    irq.counter = irq.latch = 0;
    irq.enabled = false;
    reg8000 = vrombase = 0;
}

static mem_write_handler_t map_memwrite[] =
{
   { 0x8000, 0xFFFF, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map195_intf =
{
   195, /* mapper number */
   "MMC3 Clone", /* mapper name */
   map_init, /* init routine */
   NULL, /* vblank callback */
   map_hblank, /* hblank callback */
   map_getstate, /* get state (snss) */
   map_setstate, /* set state (snss) */
   NULL, /* memory read structure */
   map_memwrite, /* memory write structure */
   NULL /* external sound device */
};

#endif