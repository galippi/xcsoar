/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Transcode.hpp"
#include "Key.h"
#include "Asset.hpp"

struct KeyMap {
  unsigned from, to;
};

static gcc_constexpr_data KeyMap altair_key_map[] = {
  { VK_F1, VK_APP1 },
  { VK_F2, VK_APP2 },
  { VK_F3, VK_APP3 },
  { VK_F4, VK_APP4 },
  { VK_F5, '6' },
  { VK_F6, '7' },
  { VK_F7, '8' },
  { VK_F8, '9' },
  { VK_F9, '0' },
  { 0 }
};

static gcc_constexpr_data KeyMap hp31x_key_map[] = {
  { VK_F12, VK_ESCAPE },
  { 0 }
};

static gcc_constexpr_data KeyMap medion_p5_key_map[] = {
  { VK_F6, 0x26 }, // + key = pg up
  { VK_F7, 0x28 }, // - key = pg down
  { VK_F8, VK_RETURN }, // middle key = enter
  { 0 }
};

static gcc_constexpr_data KeyMap nokia_500_key_map[] = {
  { 0xc1, VK_RETURN }, // middle key = enter
  { 0xc5, 0x26 }, // + key = pg down
  { 0xc6, 0x28 }, // - key = pg up
  { 0 }
};

static gcc_constexpr_data KeyMap pn_6000_key_map[] = {
  { VK_F10, VK_APP1 }, // Upper Silver key short press
  { VK_F12, VK_APP2 }, // Lower Silver key short press
  { VK_F3, VK_APP3 }, // Back key plus
  { VK_F2, VK_APP4 }, // Back key minus
  { VK_F11, VK_F1 }, // Upper silver key LONG press
  { VK_F13, VK_F2 }, // Lower silver key LONG press
  { 0 }
};

static gcc_constexpr_data KeyMap lx_mm_key_map[] = {
  { 'n', VK_APP1 }, // NAV
  { 'c', VK_APP3 }, // SET/SYS
  { 'p', VK_APP4 }, // INFO
  { 0 }
};

gcc_const
static unsigned
KeyMapLookup(const KeyMap *map, unsigned key_code)
{
  for (auto i = map; i->from != 0; ++i)
    if (i->from == key_code)
      return i->to;

  return key_code;
}

unsigned
TranscodeKey(unsigned key_code)
{
  if (IsAltair())
    return KeyMapLookup(altair_key_map, key_code);

#ifdef _WIN32_WCE
  /* VK_F23 is the "action" key on some iPaqs */
  if (key_code == VK_F23)
    return VK_RETURN;
#endif

  switch (global_model_type) {
  case ModelType::HP31X:
    return KeyMapLookup(hp31x_key_map, key_code);

  case ModelType::MEDION_P5:
    return KeyMapLookup(medion_p5_key_map, key_code);

  case ModelType::NOKIA_500:
    return KeyMapLookup(nokia_500_key_map, key_code);

  case ModelType::PN6000:
    return KeyMapLookup(pn_6000_key_map, key_code);

  case ModelType::LX_MINI_MAP:
    return KeyMapLookup(lx_mm_key_map, key_code);

  default:
    return key_code;
  }
}