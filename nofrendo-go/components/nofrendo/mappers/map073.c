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
** map073.c
**
** Mapper #73 (Konami VRC3)
** Implementation by Firebug
** $Id: map073.c,v 1.2 2001/04/27 14:37:11 neil Exp $
**
*/

#include <nofrendo.h>
#include <nes_mmc.h>
#include <nes.h>

static struct
{
    uint32 counter;
    bool enabled;
} irq;


static void map_hblank(int scanline)
{
    /* Increment the counter if it is enabled and check for strike */
    if (irq.enabled)
    {
        irq.counter += nes_getptr()->cycles_per_line;

        /* Counter triggered on overflow into Q16 */
        if (irq.counter & 0x10000)
        {
                /* Clip to sixteen-bit word */
                irq.counter &= 0xFFFF;

                /* Trigger the IRQ */
                nes6502_irq();

                /* Shut off IRQ counter */
                irq.enabled = false;
        }
    }
}

static void map_write(uint32 address, uint8 value)
{
    switch (address & 0xF000)
    {
        case 0x8000: irq.counter &= 0xFFF0;
                    irq.counter |= (uint32) (value);
                    break;
        case 0x9000: irq.counter &= 0xFF0F;
                    irq.counter |= (uint32) (value << 4);
                    break;
        case 0xA000: irq.counter &= 0xF0FF;
                    irq.counter |= (uint32) (value << 8);
                    break;
        case 0xB000: irq.counter &= 0x0FFF;
                    irq.counter |= (uint32) (value << 12);
                    break;
        case 0xC000: irq.enabled = (value & 0x02);
                    break;
        case 0xF000: mmc_bankrom (16, 0x8000, value);
        default:     break;
    }
}


/**************************/
/* Mapper #73: Salamander */
/**************************/
static void map_init (void)
{
    irq.enabled = false;
    irq.counter = 0x0000;
}

static mem_write_handler_t map_memwrite [] =
{
   { 0x8000, 0xFFFF, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map73_intf =
{
   73,                               /* Mapper number */
   "Konami VRC3",                    /* Mapper name */
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
** $Log: map073.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1  2001/04/27 12:54:40  neil
** blah
**
** Revision 1.1  2001/04/27 10:57:41  neil
** wheee
**
** Revision 1.1  2000/12/30 06:35:05  firebug
** Initial revision
**
**
*/

#endif