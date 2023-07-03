//
//  screen.c
//  alis
//
//  Created by Vadim Kindl on 02.07.2023.
//  Copyright © 2023 Zlot. All rights reserved.
//

#include "image.h"
#include "screen.h"

sScreen screen = { .ptscreen = 0 };

s8  get_scr_state(u32 scridx)                    { return xread8(alis.basemain + scridx + 0x0); }
void set_scr_state(u32 scridx, s8 val)           { xwrite8(alis.basemain + scridx + 0x0, val); }

s8  get_scr_numelem(u32 scridx)                  { return xread8(alis.basemain + scridx + 0x1); }
void set_scr_numelem(u32 scridx, s8 val)         { xwrite8(alis.basemain + scridx + 0x1, val); }

s16 get_scr_screen_id(u32 scridx)                { return xread16(alis.basemain + scridx + 0x2); }
void set_scr_screen_id(u32 scridx, s16 val)      { xwrite16(alis.basemain + scridx + 0x2, val); }

s16 get_scr_to_next(u32 scridx)                  { return xread16(alis.basemain + scridx + 0x4); }
void set_scr_to_next(u32 scridx, s16 val)        { xwrite16(alis.basemain + scridx + 0x4, val); }

s16 get_scr_link(u32 scridx)                     { return xread16(alis.basemain + scridx + 0x6); }
void set_scr_link(u32 scridx, s16 val)           { xwrite16(alis.basemain + scridx + 0x6, val); }

s16 get_scr_unknown0x08(u32 scridx)              { return xread16(alis.basemain + scridx + 0x8); }
void set_scr_unknown0x08(u32 scridx, s16 val)    { xwrite16(alis.basemain + scridx + 0x8, val); }

s16 get_scr_unknown0x0a(u32 scridx)              { return xread16(alis.basemain + scridx + 0xa); }
void set_scr_unknown0x0a(u32 scridx, s16 val)    { xwrite16(alis.basemain + scridx + 0xa, val); }

s16 get_scr_unknown0x0c(u32 scridx)              { return xread16(alis.basemain + scridx + 0xc); }
void set_scr_unknown0x0c(u32 scridx, s16 val)    { xwrite16(alis.basemain + scridx + 0xc, val); }

s16 get_scr_newx(u32 scridx)                     { return xread16(alis.basemain + scridx + 0xe); }
void set_scr_newx(u32 scridx, s16 val)           { xwrite16(alis.basemain + scridx + 0xe, val); }

s16 get_scr_newy(u32 scridx)                     { return xread16(alis.basemain + scridx + 0x10); }
void set_scr_newy(u32 scridx, s16 val)           { xwrite16(alis.basemain + scridx + 0x10, val); }

s16 get_scr_width(u32 scridx)                    { return xread16(alis.basemain + scridx + 0x12); }
void set_scr_width(u32 scridx, s16 val)          { xwrite16(alis.basemain + scridx + 0x12, val); }

s16 get_scr_height(u32 scridx)                   { return xread16(alis.basemain + scridx + 0x14); }
void set_scr_height(u32 scridx, s16 val)         { xwrite16(alis.basemain + scridx + 0x14, val); }

s16 get_scr_depx(u32 scridx)                     { return xread16(alis.basemain + scridx + 0x16); }
void set_scr_depx(u32 scridx, s16 val)           { xwrite16(alis.basemain + scridx + 0x16, val); }

s16 get_scr_depy(u32 scridx)                     { return xread16(alis.basemain + scridx + 0x18); }
void set_scr_depy(u32 scridx, s16 val)           { xwrite16(alis.basemain + scridx + 0x18, val); }

s16 get_scr_depz(u32 scridx)                     { return xread16(alis.basemain + scridx + 0x1a); }
void set_scr_depz(u32 scridx, s16 val)           { xwrite16(alis.basemain + scridx + 0x1a, val); }

u8  get_scr_credon_off(u32 scridx)               { return xread8(alis.basemain + scridx + 0x1c); }
void set_scr_credon_off(u32 scridx, u8 val)      { xwrite8(alis.basemain + scridx + 0x1c, val); }

u8  get_scr_creducing(u32 scridx)                { return xread8(alis.basemain + scridx + 0x1d); }
void set_scr_creducing(u32 scridx, u8 val)       { xwrite8(alis.basemain + scridx + 0x1d, val); }

s16 get_scr_clinking(u32 scridx)                 { return xread16(alis.basemain + scridx + 0x1e); }
void set_scr_clinking(u32 scridx, s16 val)       { xwrite16(alis.basemain + scridx + 0x1e, val); }

s8  get_scr_unknown0x20(u32 scridx)              { return xread8(alis.basemain + scridx + 0x20); }
void set_scr_unknown0x20(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x20, val); }

s8  get_scr_unknown0x21(u32 scridx)              { return xread8(alis.basemain + scridx + 0x21); }
void set_scr_unknown0x21(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x21, val); }

s8  get_scr_unknown0x22(u32 scridx)              { return xread8(alis.basemain + scridx + 0x22); }
void set_scr_unknown0x22(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x22, val); }

s8  get_scr_unknown0x23(u32 scridx)              { return xread8(alis.basemain + scridx + 0x23); }
void set_scr_unknown0x23(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x23, val); }

s8  get_scr_unknown0x24(u32 scridx)              { return xread8(alis.basemain + scridx + 0x24); }
void set_scr_unknown0x24(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x24, val); }

s8  get_scr_unknown0x25(u32 scridx)              { return xread8(alis.basemain + scridx + 0x25); }
void set_scr_unknown0x25(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x25, val); }

s8  get_scr_unknown0x26(u32 scridx)              { return xread8(alis.basemain + scridx + 0x26); }
void set_scr_unknown0x26(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x26, val); }

s8  get_scr_unknown0x27(u32 scridx)              { return xread8(alis.basemain + scridx + 0x27); }
void set_scr_unknown0x27(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x27, val); }

s8  get_scr_unknown0x28(u32 scridx)              { return xread8(alis.basemain + scridx + 0x28); }
void set_scr_unknown0x28(u32 scridx, s8 val)     { xwrite8(alis.basemain + scridx + 0x28, val); }

u8  get_scr_unknown0x29(u32 scridx)              { return xread8(alis.basemain + scridx + 0x29); }
void set_scr_unknown0x29(u32 scridx, u8 val)     { xwrite8(alis.basemain + scridx + 0x29, val); }

s16 get_scr_unknown0x2a(u32 scridx)              { return xread16(alis.basemain + scridx + 0x2a); }
void set_scr_unknown0x2a(u32 scridx, s16 val)    { xwrite16(alis.basemain + scridx + 0x2a, val); }

s16 get_scr_unknown0x2c(u32 scridx)              { return xread16(alis.basemain + scridx + 0x2c); }
void set_scr_unknown0x2c(u32 scridx, s16 val)    { xwrite16(alis.basemain + scridx + 0x2c, val); }

s16 get_scr_unknown0x2e(u32 scridx)              { return xread16(alis.basemain + scridx + 0x2e); }
void set_scr_unknown0x2e(u32 scridx, s16 val)    { xwrite16(alis.basemain + scridx + 0x2e, val); }

void scadd(s16 scridx)
{
    set_scr_to_next(scridx, 0);
    
    u16 nextscreen = screen.ptscreen;
    if (screen.ptscreen == 0)
    {
        screen.ptscreen = scridx;
        return;
    }
    
    s16 curscreen;

    do
    {
        curscreen = nextscreen;
        nextscreen = get_scr_to_next(curscreen);
    }
    while (nextscreen != 0);
    
    set_scr_to_next(curscreen, scridx);
}

void scbreak(s16 scridx)
{
    if (screen.ptscreen == 0)
        return;
    
    s16 prevptscreen = screen.ptscreen;
    if (scridx == screen.ptscreen)
    {
        screen.ptscreen = get_scr_to_next(scridx);
        return;
    }
    
    s16 nextptscreen;

    do
    {
        nextptscreen = get_scr_to_next(prevptscreen);
        if (nextptscreen == scridx)
        {
            set_scr_to_next(prevptscreen, get_scr_to_next(scridx));
            return;
        }
        
        prevptscreen = nextptscreen;
    }
    while (nextptscreen != 0);
}

void scdosprite(s16 scridx)
{
    s16 spritidx = get_scr_screen_id(scridx);

    sSprite *sprite = SPRITE_VAR(spritidx);
    sprite->numelem = get_scr_numelem(scridx);
    sprite->newx = get_scr_newx(scridx);
    sprite->newy = get_scr_newy(scridx);
    sprite->newd = 0x7fff;
    sprite->depx = get_scr_newx(scridx) + get_scr_width(scridx);
    sprite->depy = get_scr_newy(scridx) + get_scr_height(scridx);
}

void vectoriel(s16 scridx)
{
    set_scr_unknown0x26(scridx, get_scr_unknown0x21(scridx) * get_scr_unknown0x25(scridx) - get_scr_unknown0x24(scridx) * get_scr_unknown0x22(scridx));
    set_scr_unknown0x27(scridx, get_scr_unknown0x22(scridx) * get_scr_unknown0x23(scridx) - get_scr_unknown0x25(scridx) * get_scr_unknown0x20(scridx));
    set_scr_unknown0x28(scridx, get_scr_unknown0x20(scridx) * get_scr_unknown0x24(scridx) - get_scr_unknown0x23(scridx) * get_scr_unknown0x21(scridx));
}
