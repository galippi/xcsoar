/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2010 The XCSoar Project
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

#include "Screen/Pen.hpp"

#ifdef ENABLE_SDL

void
Pen::set(enum style style, unsigned _width, const Color c)
{
  width = _width;
  color = c;
}

void
Pen::set(unsigned width, const Color c)
{
  set(SOLID, width, c);
}

void
Pen::reset()
{
}

#else /* !ENABLE_SDL */

void
Pen::set(enum style style, unsigned width, const Color c)
{
  reset();
  pen = ::CreatePen(style, width, c);
}

void
Pen::set(unsigned width, const Color c)
{
  set(SOLID, width, c);
}

void
Pen::reset()
{
  if (pen != NULL) {
    ::DeleteObject(pen);
    pen = NULL;
  }
}

#endif /* !ENABLE_SDL */
