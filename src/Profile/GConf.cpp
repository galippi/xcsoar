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

#include "Profile/GConf.hpp"
#include "Config/GConf.hpp"

bool
ProfileGConf::Get(const TCHAR *szRegValue, int &pPos)
{
  int value;
  if (!GConf().get(szRegValue, value))
    return false;

  pPos = value;
  return true;
}

bool
ProfileGConf::Set(const TCHAR *szRegValue, int Pos)
{
  return GConf().set(szRegValue, Pos);
}

/**
 * Reads a value from the registry file
 * @param szRegValue Name of the value that should be read
 * @param pPos Pointer to the output buffer
 * @param dwSize Maximum size of the output buffer
 */
bool
ProfileGConf::Get(const TCHAR *szRegValue, TCHAR *pPos, size_t dwSize)
{
  if (GConf().get(szRegValue, pPos, dwSize))
    return true;

  pPos[0] = _T('\0');
  return false;
}

/**
 * Writes a value to the registry
 * @param szRegValue Name of the value that should be written
 * @param Pos Value that should be written
 */
bool
ProfileGConf::Set(const TCHAR *szRegValue, const TCHAR *Pos)
{
  return GConf().set(szRegValue, Pos);
}

void
ProfileGConf::Export(ProfileWriter &writer)
{
  // XXX implement
}
