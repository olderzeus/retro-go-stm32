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
** map024.c: Konami VRC6 mapper interface
**
*/

#include <nofrendo.h>
#include <nes_mmc.h>
#include <nes.h>

static struct
{
    bool enabled, wait_state;
    int counter, latch;
} irq;


static void map_hblank(int scanline)
{
    if (irq.enabled)
    {
        if (256 == ++irq.counter)
        {
            irq.counter = irq.latch;
            nes6502_irq();
            //irq.enabled = false;
            irq.enabled = irq.wait_state;
        }
    }
}

static void map_write(uint32 address, uint8 value)
{
    switch (address & 0xF003)
    {
    case 0x8000:
        mmc_bankrom(16, 0x8000, value);
        break;

    case 0x9003:
        /* ??? */
        break;

    case 0xB003:
        switch (value & 0x0C)
        {
            case 0x0: ppu_setmirroring(PPU_MIRROR_VERT); break;
            case 0x4: ppu_setmirroring(PPU_MIRROR_HORI); break;
            case 0x8: ppu_setmirroring(PPU_MIRROR_SCR0); break;
            case 0xC: ppu_setmirroring(PPU_MIRROR_SCR1); break;
        }
        break;

    case 0xC000:
        mmc_bankrom(8, 0xC000, value);
        break;

    case 0xD000:
        mmc_bankvrom(1, 0x0000, value);
        break;

    case 0xD001:
        mmc_bankvrom(1, 0x0400, value);
        break;

    case 0xD002:
        mmc_bankvrom(1, 0x0800, value);
        break;

    case 0xD003:
        mmc_bankvrom(1, 0x0C00, value);
        break;

    case 0xE000:
        mmc_bankvrom(1, 0x1000, value);
        break;

    case 0xE001:
        mmc_bankvrom(1, 0x1400, value);
        break;

    case 0xE002:
        mmc_bankvrom(1, 0x1800, value);
        break;

    case 0xE003:
        mmc_bankvrom(1, 0x1C00, value);
        break;

    case 0xF000:
        irq.latch = value;
        break;

    case 0xF001:
        irq.enabled = (value >> 1) & 0x01;
        irq.wait_state = value & 0x01;
        if (irq.enabled)
            irq.counter = irq.latch;
        break;

    case 0xF002:
        irq.enabled = irq.wait_state;
        break;

    default:
        MESSAGE_DEBUG("invalid VRC6 write: $%02X to $%04X", value, address);
        break;
    }
}

static void map_getstate(uint8 *state)
{
    state[0] = irq.counter;
    state[1] = irq.enabled;
}

static void map_setstate(uint8 *state)
{
    irq.counter = state[0];
    irq.enabled = state[1];
}


static void map_init(void)
{
    irq.enabled = irq.wait_state = 0;
    irq.counter = irq.latch = 0;
}

static mem_write_handler_t map_memwrite[] =
{
   { 0x8000, 0xF002, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map24_intf =
{
   24, /* mapper number */
   "Konami VRC6", /* mapper name */
   map_init, /* init routine */
   NULL, /* vblank callback */
   map_hblank, /* hblank callback */
   map_getstate, /* get state (snss) */
   map_setstate, /* set state (snss) */
   NULL, /* memory read structure */
   map_memwrite, /* memory write structure */
   NULL /* external sound device */
};

/*
** $Log: map024.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.1  2000/10/24 12:19:33  matt
** changed directory structure
**
** Revision 1.11  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.10  2000/10/22 15:03:13  matt
** simplified mirroring
**
** Revision 1.9  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.8  2000/10/10 13:58:17  matt
** stroustrup squeezing his way in the door
**
** Revision 1.7  2000/10/09 12:00:53  matt
** removed old code
**
** Revision 1.6  2000/08/16 02:50:11  matt
** random mapper cleanups
**
** Revision 1.5  2000/07/15 23:52:19  matt
** rounded out a bunch more mapper interfaces
**
** Revision 1.4  2000/07/10 13:51:25  matt
** using generic nes6502_irq() routine now
**
** Revision 1.3  2000/07/10 05:29:03  matt
** cleaned up some mirroring issues
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/04 23:11:45  matt
** initial revision
**
*/

#endif