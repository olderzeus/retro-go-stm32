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
** map75.c
**
** mapper 75 interface
** $Id: map075.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <nofrendo.h>
#include <nes_mmc.h>
#include <nes_ppu.h>


static uint8 latch[2];
static uint8 hibits;


static void map_write(uint32 address, uint8 value)
{
    switch ((address & 0xF000) >> 12)
    {
    case 0x8:
        mmc_bankrom(8, 0x8000, value);
        break;

    case 0x9:
        hibits = (value & 0x06);

        mmc_bankvrom(4, 0x0000, ((hibits & 0x02) << 3) | latch[0]);
        mmc_bankvrom(4, 0x1000, ((hibits & 0x04) << 2) | latch[1]);

        if (value & 1)
            ppu_setmirroring(PPU_MIRROR_VERT);
        else
            ppu_setmirroring(PPU_MIRROR_HORI);

        break;

    case 0xA:
        mmc_bankrom(8, 0xA000, value);
        break;

    case 0xC:
        mmc_bankrom(8, 0xC000, value);
        break;

    case 0xE:
        latch[0] = (value & 0x0F);
        mmc_bankvrom(4, 0x0000, ((hibits & 0x02) << 3) | latch[0]);
        break;

    case 0xF:
        latch[1] = (value & 0x0F);
        mmc_bankvrom(4, 0x1000, ((hibits & 0x04) << 2) | latch[1]);
        break;

    default:
        break;
    }
}

static mem_write_handler_t map_memwrite[] =
{
   { 0x8000, 0xFFFF, map_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map75_intf =
{
   75, /* mapper number */
   "Konami VRC1", /* mapper name */
   NULL, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   NULL, /* get state (snss) */
   NULL, /* set state (snss) */
   NULL, /* memory read structure */
   map_memwrite, /* memory write structure */
   NULL /* external sound device */
};

/*
** $Log: map075.c,v $
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
** Revision 1.6  2000/10/22 19:17:46  matt
** mapper cleanups galore
**
** Revision 1.5  2000/10/22 15:03:14  matt
** simplified mirroring
**
** Revision 1.4  2000/10/21 19:33:38  matt
** many more cleanups
**
** Revision 1.3  2000/07/10 05:29:03  matt
** cleaned up some mirroring issues
**
** Revision 1.2  2000/07/06 02:48:43  matt
** clearly labelled structure members
**
** Revision 1.1  2000/07/06 01:01:56  matt
** initial revision
**
*/

#endif