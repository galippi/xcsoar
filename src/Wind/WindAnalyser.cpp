/***********************************************************************
**
**   WindAnalyser.cpp
**
**   This file is part of Cumulus
**
************************************************************************
**
**   Copyright (c):  2002 by Andr� Somers
**
**   This file is distributed under the terms of the General Public
**   Licence. See the file COPYING for more information.
**
**   $Id$
**
***********************************************************************/
/*
NOTE: Some portions copyright as above

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

#include "Wind/WindAnalyser.hpp"
#include "Math/Constants.h"
#include "Math/FastMath.h"
#include "LogFile.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/Derived.hpp"

#include <stdlib.h>
#include <algorithm>

using std::min;

/*
About Windanalysation

Currently, the wind is being analyzed by finding the minimum and the maximum
groundspeeds measured while flying a circle. The direction of the wind is taken
to be the direction in wich the speed reaches it's maximum value, the speed
is half the difference between the maximum and the minimum speeds measured.
A quality parameter, based on the number of circles allready flown (the first
circles are taken to be less accurate) and the angle between the headings at
minimum and maximum speeds, is calculated in order to be able to weigh the
resulting measurement.

There are other options for determining the windspeed. You could for instance
add all the vectors in a circle, and take the resuling vector as the windspeed.
This is a more complex method, but because it is based on more heading/speed
measurements by the GPS, it is probably more accurate. If equiped with
instruments that pass along airspeed, the calculations can be compensated for
changes in airspeed, resulting in better measurements. We are now assuming
the pilot flies in perfect circles with constant airspeed, wich is of course
not a safe assumption.
The quality indication we are calculation can also be approched differently,
by calculating how constant the speed in the circle would be if corrected for
the windspeed we just derived. The more constant, the better. This is again
more CPU intensive, but may produce better results.

Some of the errors made here will be averaged-out by the WindStore, wich keeps
a number of windmeasurements and calculates a weighted average based on quality.
*/

#ifndef NDEBUG
  #define DEBUG_WIND
#endif

WindAnalyser::WindAnalyser()
  :circleCount(0), circleLeft(false),
   active(false),
   startmarker(0),
   circleDeg(0),
   lastHeading(Angle::native(fixed_zero)),
   pastHalfway(false),
   minSatCnt(1), // JMW conf->getWindMinSatCount();
   curModeOK(false)
{
}

WindAnalyser::~WindAnalyser()
{
}

static fixed
Magnitude(Vector v)
{
  return hypot(v.x, v.y);
}

/**
 * Called if a new sample is available in the samplelist.
 */
void
WindAnalyser::slot_newSample(const NMEA_INFO &info, DERIVED_INFO &derived)
{
  if (!active)
    // only work if we are in active mode
    return;

  Vector curVector;

  bool fullCircle = false;

  // Circle detection
  int diff = (int)(info.TrackBearing - lastHeading).as_delta().magnitude_degrees();
  circleDeg += diff;
  lastHeading = info.TrackBearing;

  if (circleDeg >= 360) {
    //full circle made!

    fullCircle = true;
    circleDeg = 0;
    circleCount++; //increase the number of circles flown (used
    //to determine the quality)
  }

  curVector = Vector(SpeedVector(info.TrackBearing, info.GroundSpeed));

  windsamples[numwindsamples].v = curVector;
  windsamples[numwindsamples].t = info.Time;
  windsamples[numwindsamples].mag = Magnitude(curVector);

  if (numwindsamples < MAXWINDSAMPLES - 1) {
    numwindsamples++;
  } else {
    // TODO code: give error, too many wind samples
    // or use circular buffer
  }

  if ((info.GroundSpeed < Magnitude(minVector)) || first)
    minVector = curVector;

  if ((info.GroundSpeed > Magnitude(maxVector)) || first)
    maxVector = curVector;

  if (fullCircle) { //we have completed a full circle!
    if (numwindsamples < MAXWINDSAMPLES - 1)
      // calculate the wind for this circle, only if it is valid
      _calcWind(info, derived);

    fullCircle = false;

    // should set each vector to average

    minVector = maxVector = Vector((maxVector.x - minVector.x) / 2,
                                   (maxVector.y - minVector.y) / 2);

    first = true;
    numwindsamples = 0;

    if (startcircle > 1)
      startcircle--;

    if (startcircle == 1) {
      climbstartpos = GeoPoint(info.Location.Longitude,
                               info.Location.Latitude);
      climbstarttime = info.Time;
      startcircle = 0;
    }

    climbendpos = GeoPoint(info.Location.Longitude,
                           info.Location.Latitude);
    climbendtime = info.Time;

    //no need to reset fullCircle, it will automaticly be reset in the next itteration.
  }

  first = false;
  windstore.SlotAltitude(info, derived);
}

void
WindAnalyser::slot_Altitude(const NMEA_INFO &info, DERIVED_INFO &derived)
{
  windstore.SlotAltitude(info, derived);
}

/**
 * Called if the flightmode changes
 */
void
WindAnalyser::slot_newFlightMode(const NMEA_INFO &info,
                                 const DERIVED_INFO &derived,
                                 bool left, int marker)
{
  // we are inactive by default
  active = false;

  // reset the circlecounter for each flightmode
  // change. The important thing to measure is the
  // number of turns in this thermal only.
  circleCount = 0;

  startcircle = 3; // ignore first two circles in thermal drift calcs

  circleDeg = 0;
  if (derived.Circling) {
    if (left) {
      circleLeft = true;
      curModeOK = true;
    } else {
      circleLeft = false;
      curModeOK = true;
    }
  } else {
    // end circling?
    if (curModeOK) {
      //calcThermalDrift();
    }
    curModeOK = false;

    return; //ok, so we are not circling. Exit function.
  }

  // do we have enough satelites in view?
  //if (satCnt<minSatCnt) return;

  // initialize analyser-parameters
  startmarker = marker;
  active = true;
  first = true;
  numwindsamples = 0;
}

void
WindAnalyser::_calcWind(const NMEA_INFO &info, DERIVED_INFO &derived)
{
  int i;
  fixed av = fixed_zero;

  if (!numwindsamples)
    return;

  // reject if average time step greater than 2.0 seconds
  if ((windsamples[numwindsamples - 1].t - windsamples[0].t)
      / (numwindsamples - 1) > fixed_two)
    return;

  // find average
  for (i = 0; i < numwindsamples; i++) {
    av += windsamples[i].mag;
  }
  av /= numwindsamples;

  // find zero time for times above average
  fixed rthisp;
  int j;
  int ithis = 0;
  fixed rthismax = fixed_zero;
  fixed rthismin = fixed_zero;
  int jmax = -1;
  int jmin = -1;
  fixed rpoint;
  int idiff;

  for (j = 0; j < numwindsamples; j++) {
    rthisp = fixed_zero;
    rpoint = windsamples[j].mag;

    for (i = 0; i < numwindsamples; i++) {
      if (i == j)
        continue;

      ithis = (i + j) % numwindsamples;
      idiff = i;

      if (idiff > numwindsamples / 2)
        idiff = numwindsamples - idiff;

      rthisp += (windsamples[ithis].mag) * idiff;
    }

    if ((rthisp < rthismax) || (jmax == -1)) {
      rthismax = rthisp;
      jmax = j;
    }

    if ((rthisp > rthismin) || (jmin == -1)) {
      rthismin = rthisp;
      jmin = j;
    }
  }

  // jmax is the point where most wind samples are below
  // jmin is the point where most wind samples are above

  maxVector = windsamples[jmax].v;
  minVector = windsamples[jmin].v;

  // attempt to fit cycloid

  fixed phase;
  fixed mag = fixed_half * (windsamples[jmax].mag - windsamples[jmin].mag);
  fixed wx, wy;
  fixed cmag;
  fixed rthis = fixed_zero;

  for (i = 0; i < numwindsamples; i++) {
    phase = ((i + jmax) % numwindsamples) * fixed_two_pi / numwindsamples;
    wx = cos(phase) * av + mag;
    wy = sin(phase) * av;
    cmag = hypot(wx, wy) - windsamples[i].mag;
    rthis += cmag * cmag;
  }

  rthis /= numwindsamples;
  rthis = sqrt(rthis);

  int quality;

  if (mag > fixed_one)
    quality = 5 - iround(rthis / mag * 3);
  else
    quality = 5 - iround(rthis);

  if (circleCount < 3)
    quality--;
  if (circleCount < 2)
    quality--;
  if (circleCount < 1)
    return;

  quality = min(quality, 5); //5 is maximum quality, make sure we honour that.

  Vector a(-mag * maxVector.x / windsamples[jmax].mag,
           -mag * maxVector.y / windsamples[jmax].mag);

  if (quality < 1)
    //measurment quality too low
    return;

  if (a.x * a.x + a.y * a.y < fixed(30 * 30))
    // limit to reasonable values (60 knots), reject otherwise
    slot_newEstimate(info, derived, a, quality);
}

void
WindAnalyser::slot_newEstimate(const NMEA_INFO &info,
                               DERIVED_INFO &derived,
                               Vector a, int quality)
{
  #ifdef DEBUG_WIND
  const char *type;

  if (quality >= 6)
    type = "external wind";
  else
    type = "wind circling";

  LogDebug(_T("%f %f %d # %s"), (double)a.x, (double)a.y, quality, type);
  #endif

  windstore.SlotMeasurement(info, derived, a, quality);
}
