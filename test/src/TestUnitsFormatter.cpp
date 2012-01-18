/* Copyright_License {

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

#include "Units/UnitsFormatter.hpp"
#include "Units/System.hpp"
#include "Util/Macros.hpp"
#include "Util/StringUtil.hpp"
#include "TestUtil.hpp"

int main(int argc, char **argv)
{
  plan_tests(12);

  TCHAR buffer[256];

  // Test FormatAltitude()
  Units::FormatAltitude(buffer, ARRAY_SIZE(buffer), fixed(1234), Unit::METER);
  ok1(StringIsEqual(buffer, _T("1234 m")));

  Units::FormatAltitude(buffer, ARRAY_SIZE(buffer),
                        Units::ToSysUnit(fixed(1234), Unit::FEET), Unit::FEET);
  ok1(StringIsEqual(buffer, _T("1234 ft")));

  Units::FormatAltitude(buffer, ARRAY_SIZE(buffer), fixed(-1234), Unit::METER);
  ok1(StringIsEqual(buffer, _T("-1234 m")));

  Units::FormatAltitude(buffer, ARRAY_SIZE(buffer),
                        Units::ToSysUnit(fixed(-1234), Unit::FEET), Unit::FEET);
  ok1(StringIsEqual(buffer, _T("-1234 ft")));

  Units::FormatAltitude(buffer, ARRAY_SIZE(buffer), fixed(1234),
                        Unit::METER, false);
  ok1(StringIsEqual(buffer, _T("1234")));

  Units::FormatAltitude(buffer, ARRAY_SIZE(buffer), fixed(-1234),
                        Unit::METER, false);
  ok1(StringIsEqual(buffer, _T("-1234")));

  // Test FormatRelativeAltitude()
  Units::FormatRelativeAltitude(buffer, ARRAY_SIZE(buffer), fixed(1234),
                                Unit::METER);
  ok1(StringIsEqual(buffer, _T("+1234 m")));

  Units::FormatRelativeAltitude(buffer, ARRAY_SIZE(buffer),
                                Units::ToSysUnit(fixed(1234), Unit::FEET),
                                Unit::FEET);
  ok1(StringIsEqual(buffer, _T("+1234 ft")));

  Units::FormatRelativeAltitude(buffer, ARRAY_SIZE(buffer), fixed(-1234),
                                Unit::METER);
  ok1(StringIsEqual(buffer, _T("-1234 m")));

  Units::FormatRelativeAltitude(buffer, ARRAY_SIZE(buffer),
                                Units::ToSysUnit(fixed(-1234), Unit::FEET),
                                Unit::FEET);
  ok1(StringIsEqual(buffer, _T("-1234 ft")));

  Units::FormatRelativeAltitude(buffer, ARRAY_SIZE(buffer), fixed(1234),
                        Unit::METER, false);
  ok1(StringIsEqual(buffer, _T("+1234")));

  Units::FormatRelativeAltitude(buffer, ARRAY_SIZE(buffer), fixed(-1234),
                        Unit::METER, false);
  ok1(StringIsEqual(buffer, _T("-1234")));

  return exit_status();
}