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

#include "InfoBoxes/Content/Time.hpp"

#include "InfoBoxes/InfoBoxWindow.hpp"
#include "Interface.hpp"
#include "LocalTime.hpp"

#include <tchar.h>
#include <stdio.h>

void
InfoBoxContentTimeLocal::Update(InfoBoxWindow &infobox)
{
  TCHAR sTmp[32];

  // Set Value
  int dd = abs((int)DetectCurrentTime(&XCSoarInterface::Basic())) % (3600 * 24);
  int hours = (dd / 3600);
  int mins = (dd / 60 - hours * 60);
  int seconds = (dd - mins * 60 - hours * 3600);
  hours = hours % 24;

  // Set Value
  _stprintf(sTmp, _T("%02d:%02d"), hours, mins);
  infobox.SetValue(sTmp);

  // Set Comment
  _stprintf(sTmp, _T("%02d"), seconds);
  infobox.SetComment(sTmp);
}

void
InfoBoxContentTimeUTC::Update(InfoBoxWindow &infobox)
{
  TCHAR sTmp[32];

  // Set Value
  const BrokenDateTime t = XCSoarInterface::Basic().DateTime;
  _stprintf(sTmp, _T("%02d:%02d"), t.hour, t.minute);
  infobox.SetValue(sTmp);

  // Set Comment
  _stprintf(sTmp, _T("%02d"), t.second);
  infobox.SetComment(sTmp);
}

void
InfoBoxContentTimeFlight::Update(InfoBoxWindow &infobox)
{
  TCHAR sTmp[32];

  if (!positive(XCSoarInterface::Basic().flight.FlightTime)) {
    infobox.SetInvalid();
    return;
  }

  // Set Value
  int dd = abs((int)XCSoarInterface::Basic().flight.FlightTime) % (3600 * 24);
  int hours = (dd / 3600);
  int mins = (dd / 60 - hours * 60);
  int seconds = (dd - mins * 60 - hours * 3600);
  hours = hours % 24;

  if (hours > 0) { // hh:mm, ss
    // Set Value
    _stprintf(sTmp, _T("%02d:%02d"), hours, mins);
    infobox.SetValue(sTmp);

    // Set Comment
    _stprintf(sTmp, _T("%02d"), seconds);
    infobox.SetComment(sTmp);
  } else { // mm:ss
    // Set Value
    _stprintf(sTmp, _T("%02d:%02d"), mins, seconds);
    infobox.SetValue(sTmp);

    // Set Comment
    infobox.SetComment(_T(""));
  }
}
