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

#include "WayPointFileZander.hpp"
#include "Units.hpp"
#include "Waypoint/Waypoints.hpp"

#include <stdio.h>

bool
WayPointFileZander::parseLine(const TCHAR* line, const unsigned linenum,
                              Waypoints &way_points,
                              const RasterTerrain *terrain)
{
  // If (end-of-file or comment)
  if (line[0] == '\0' || line[0] == 0x1a ||
      _tcsstr(line, _T("**")) == line ||
      _tcsstr(line, _T("*")) == line)
    // -> return without error condition
    return true;

  // Determine the length of the line
  size_t len = _tcslen(line);
  // If less then 34 characters -> something is wrong -> cancel
  if (len < 34)
    return false;

  GeoPoint location;

  // Latitude (Characters 13-20 // DDMMSS(N/S))
  if (!parseAngle(line + 13, location.Latitude, true))
    return false;

  // Longitude (Characters 21-29 // DDDMMSS(E/W))
  if (!parseAngle(line + 21, location.Longitude, false))
    return false;

  Waypoint new_waypoint(location);
  new_waypoint.FileNum = file_num;

  // Name (Characters 0-12)
  if (!parseString(line, new_waypoint.Name, 12))
    return false;

  // Altitude (Characters 30-34 // e.g. 1561 (in meters))
  /// @todo configurable behaviour
  bool alt_ok = parseAltitude(line + 30, new_waypoint.Altitude);
  check_altitude(new_waypoint, terrain, alt_ok);

  // Description (Characters 35-44)
  if (len > 35)
    parseString(line + 35, new_waypoint.Comment, 9);

  // Flags (Characters 45-49)
  if (len > 45)
    parseFlags(line + 45, new_waypoint.Flags);
  else
    new_waypoint.Flags.TurnPoint = true;

  add_waypoint(way_points, new_waypoint);
  return true;
}


bool
WayPointFileZander::parseString(const TCHAR* src, tstring& dest, unsigned len)
{
  if (src[0] == 0)
    return true;

  dest.assign(src);

  // Cut the string after the first space, tab or null character
  size_t found = dest.find_first_of(_T("\t\0"));
  if (found != tstring::npos)
    dest = dest.substr(0, found);

  dest = dest.substr(0, len);
  trim_inplace(dest);
  return true;
}

bool
WayPointFileZander::parseAngle(const TCHAR* src, Angle& dest, const bool lat)
{
  TCHAR *endptr;

  long deg = _tcstol(src, &endptr, 10);
  if (endptr == src || deg < 0)
    return false;

  long sec = deg % 100;
  if (sec >= 60)
    return false;

  long min = ((deg - sec) % 10000) / 100;
  if (min >= 60)
    return false;

  deg = (deg - min - sec) / 10000;

  // Limit angle to +/- 90 degrees for Latitude or +/- 180 degrees for Longitude
  deg = std::min(deg, lat ? 90L : 180L);

  fixed value = fixed(deg) + fixed(min) / 60 + fixed(sec) / 3600;

  TCHAR sign = *endptr;
  if (sign == 'W' || sign == 'w' || sign == 'S' || sign == 's')
    value = -value;

  // Save angle
  dest = Angle::degrees(value);
  return true;
}

bool
WayPointFileZander::parseAltitude(const TCHAR* src, fixed& dest)
{
  // Parse string
  TCHAR *endptr;
  double val = _tcstod(src, &endptr);
  if (endptr == src)
    return false;

  // Save altitude
  dest = (fixed)val;
  return true;
}

bool
WayPointFileZander::parseFlags(const TCHAR* src, WaypointFlags& dest)
{
  // WP = Waypoint
  // HA = Home Field
  // WA = WP + Airfield
  // LF = Landing Field
  // WL = WP + Landing Field
  // RA = Restricted

  // If flags field exists (this function isn't called otherwise)
  // -> turnpoint needs to be declared as one
  dest.TurnPoint = false;

  if ((src[0] == 'W' || src[0] == 'w') &&
      (src[1] == 'P' || src[1] == 'p')) {
    dest.TurnPoint = true;
  }
  else if ((src[0] == 'H' || src[0] == 'h') &&
           (src[1] == 'A' || src[1] == 'a')) {
    dest.TurnPoint = true;
    dest.Airport = true;
    dest.Home = true;
  }
  else if ((src[0] == 'W' || src[0] == 'w') &&
           (src[1] == 'A' || src[1] == 'a')) {
    dest.TurnPoint = true;
    dest.Airport = true;
  }
  else if ((src[0] == 'L' || src[0] == 'l') &&
           (src[1] == 'F' || src[1] == 'f')) {
    dest.LandPoint = true;
  }
  else if ((src[0] == 'W' || src[0] == 'w') &&
           (src[1] == 'L' || src[1] == 'l')) {
    dest.TurnPoint = true;
    dest.LandPoint = true;
  }
  else if ((src[0] == 'R' || src[0] == 'r') &&
           (src[1] == 'A' || src[1] == 'a')) {
    dest.Restricted = true;
  }

  return true;
}
