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


// ToDo

// adding baro alt sentance parser to support baro source priority  if (d == pDevPrimaryBaroSource){...}

#include "Device/Driver/Volkslogger.hpp"
#include "Device/Parser.hpp"
#include "Device/Driver.hpp"
#include "ProgressGlue.hpp"
#include "Language.hpp"
#include "UtilsText.hpp"
#include "Device/Volkslogger/vlapi2.h"
#include "Device/Volkslogger/vlapihlp.h"
#include "Components.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/InputLine.hpp"
#include "Waypoint/Waypoint.hpp"

#ifdef _UNICODE
#include <windows.h>
#endif

class VolksloggerDevice : public AbstractDevice {
private:
  Port *port;

public:
  VolksloggerDevice(Port *_port):port(_port) {}

public:
  virtual bool ParseNMEA(const char *line, struct NMEA_INFO *info,
                         bool enable_baro);
  virtual bool Declare(const Declaration *declaration);
};

// RMN: Volkslogger
// Source data:
// $PGCS,1,0EC0,FFF9,0C6E,02*61
// $PGCS,1,0EC0,FFFA,0C6E,03*18
static bool
vl_PGCS1(NMEAInputLine &line, NMEA_INFO *GPS_INFO, bool enable_baro)
{
  GPS_STATE &gps = GPS_INFO->gps;

  if (line.read(1) != 1)
    return false;

  /* pressure sensor */
  line.skip();

  // four characers, hex, barometric altitude
  long altitude = line.read_hex(0L);

  if (enable_baro) {
    if (altitude > 60000)
      /* Assuming that altitude has wrapped around.  60 000 m occurs
         at QNH ~2000 hPa */
      altitude -= 65535;

    GPS_INFO->BaroAltitude =
      GPS_INFO->pressure.AltitudeToQNHAltitude(fixed(altitude));

    GPS_INFO->BaroAltitudeAvailable = true;
  }

  // ExtractParameter(String,ctemp,3);
  // four characters, hex, constant.  Value 1371 (dec)

  // nSatellites = (int)(min(12,HexStrToDouble(ctemp, NULL)));

  if (gps.SatellitesUsed <= 0) {
    gps.SatellitesUsed = 4;
    // just to make XCSoar quit complaining.  VL doesn't tell how many
    // satellites it uses.  Without this XCSoar won't do wind
    // measurements.
  }

  return false;
}

bool
VolksloggerDevice::ParseNMEA(const char *String, NMEA_INFO *GPS_INFO,
                             bool enable_baro)
{
  if (!NMEAParser::NMEAChecksum(String))
    return false;

  NMEAInputLine line(String);
  char type[16];
  line.read(type, 16);

  if (strcmp(type, "$PGCS") == 0)
    return vl_PGCS1(line, GPS_INFO, enable_baro);
  else
    return false;
}

static VLAPI vl;

static unsigned nturnpoints = 0;

static bool
VLDeclAddWayPoint(const Waypoint &way_point);

static void
CopyToNarrowBuffer(char *dest, size_t max_size, const TCHAR *src)
{
#ifdef _UNICODE
    if (WideCharToMultiByte(CP_ACP, 0, src, -1,
                            dest, max_size,
                            NULL, NULL) <= 0)
      dest[0] = 0;
#else
    strncpy(dest, src, max_size - 1);
    dest[max_size - 1] = 0;
#endif
}

bool
VolksloggerDevice::Declare(const Declaration *decl)
{
  ProgressGlue::Create(_("Comms with Volkslogger"));

  vl.set_port(port);
  nturnpoints = 0;

  int err;
  if((err = vl.open(1, 20, 1, 38400L)) != VLA_ERR_NOERR) {
    //    _isConnected = false;
  }
  else {
    //    _isConnected = true;
  }

  if (err == VLA_ERR_NOERR) {
    if ((err = vl.read_info()) == VLA_ERR_NOERR) {
      //      vl.read_db_and_declaration();
    }

    CopyToNarrowBuffer(vl.declaration.flightinfo.pilot,
                       sizeof(vl.declaration.flightinfo.pilot),
                       decl->PilotName);

    CopyToNarrowBuffer(vl.declaration.flightinfo.competitionid,
                       sizeof(vl.declaration.flightinfo.competitionid),
                       decl->AircraftRego);

    CopyToNarrowBuffer(vl.declaration.flightinfo.competitionclass,
                       sizeof(vl.declaration.flightinfo.competitionclass),
                       decl->AircraftType);

#ifdef OLD_TASK
    if (way_points.verify_index(XCSoarInterface::SettingsComputer().HomeWaypoint)) {
      const WAYPOINT &way_point = way_points.get(XCSoarInterface::SettingsComputer().HomeWaypoint);

      sprintf(temp, "%S", way_point.Name);

      strncpy(vl.declaration.flightinfo.homepoint.name, temp, 6);
      vl.declaration.flightinfo.homepoint.lon = way_point.Location.Longitude;
      vl.declaration.flightinfo.homepoint.lat = way_point.Location.Latitude;
    }
#endif

  }

  if (err != VLA_ERR_NOERR)
    return false;

  unsigned i;
  for (i = 0; i < decl->size(); i++)
    VLDeclAddWayPoint(decl->waypoints[i]);

  vl.declaration.task.nturnpoints = (nturnpoints<=2)? 0: std::min(nturnpoints-2, (unsigned)12);

#ifdef OLD_TASK
  const SETTINGS_TASK settings = task.getSettings();

  // start..
  switch(settings.StartType) {
  case START_CIRCLE:
    vl.declaration.task.startpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
    vl.declaration.task.startpoint.lw = min(1500,settings.StartRadius);
    vl.declaration.task.startpoint.rz = min(1500,settings.StartRadius);
    vl.declaration.task.startpoint.rs = 0;
    break;
  case START_LINE:
    vl.declaration.task.startpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_LINE;
    vl.declaration.task.startpoint.lw = min(1500,settings.StartRadius*2);
    vl.declaration.task.startpoint.rs = 0;
    vl.declaration.task.startpoint.rz = 0;
    break;
  case START_SECTOR:
    vl.declaration.task.startpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
    vl.declaration.task.startpoint.lw = min(1500,settings.StartRadius);
    vl.declaration.task.startpoint.rz = 0;
    vl.declaration.task.startpoint.rs = min(1500,settings.StartRadius);
    break;
  }
  vl.declaration.task.startpoint.ws = 360;

  // rest of task...
  for (i=0; i<nturnpoints; i++) {
    // note this is for non-aat only!
    switch (settings.SectorType) {
    case 0: // cylinder
      vl.declaration.task.turnpoints[i].oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
      vl.declaration.task.turnpoints[i].rz = settings.SectorRadius;
      vl.declaration.task.turnpoints[i].rs = 0;
      vl.declaration.task.turnpoints[i].lw = 0;
      break;
    case 1: // sector
      vl.declaration.task.turnpoints[i].oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
      vl.declaration.task.turnpoints[i].rz = 0;
      vl.declaration.task.turnpoints[i].rs = 15000;
      vl.declaration.task.turnpoints[i].lw = 0;
      break;
    case 2: // German DAe 0.5/10
      vl.declaration.task.turnpoints[i].oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
      vl.declaration.task.turnpoints[i].rz = 500;
      vl.declaration.task.turnpoints[i].rs = 10000;
      vl.declaration.task.turnpoints[i].lw = 0;
      break;
    };
    vl.declaration.task.turnpoints[i].ws = 360; // auto direction

  }

  // Finish
  switch(settings.FinishType) {
  case FINISH_CIRCLE:
    vl.declaration.task.finishpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
    vl.declaration.task.finishpoint.lw = min(1500,settings.FinishRadius);
    vl.declaration.task.finishpoint.rz = min(1500,settings.FinishRadius);
    vl.declaration.task.finishpoint.rs = 0;
    break;
  case FINISH_LINE:
    vl.declaration.task.finishpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_LINE;
    vl.declaration.task.finishpoint.lw = settings.FinishRadius*2;
    vl.declaration.task.finishpoint.rz = 0;
    vl.declaration.task.finishpoint.rs = 0;
    break;
  case FINISH_SECTOR:
    vl.declaration.task.finishpoint.oztyp = VLAPI_DATA::DCLWPT::OZTYP_CYLSKT;
    vl.declaration.task.finishpoint.lw = min(1500,settings.FinishRadius);
    vl.declaration.task.finishpoint.rz = 0;
    vl.declaration.task.finishpoint.rs = min(1500,settings.FinishRadius);
    break;
  }
  vl.declaration.task.finishpoint.ws = 360;
#endif

  bool ok = (vl.write_db_and_declaration() == VLA_ERR_NOERR);

  vl.close(1);

  ProgressGlue::Close();

  return ok;
}

static void
CopyWaypoint(VLAPI_DATA::WPT &dest, const Waypoint &src)
{
  CopyToNarrowBuffer(dest.name, sizeof(dest.name), src.Name.c_str());
  dest.lon = src.Location.Longitude.value_degrees();
  dest.lat = src.Location.Latitude.value_degrees();
}

static bool
VLDeclAddWayPoint(const Waypoint &way_point)
{
  if (nturnpoints == 0) {
    CopyWaypoint(vl.declaration.task.startpoint, way_point);
    nturnpoints++;
  } else {
    CopyWaypoint(vl.declaration.task.turnpoints[nturnpoints-1], way_point);
    nturnpoints++;
  }

  CopyWaypoint(vl.declaration.task.finishpoint, way_point);

  return true;

}


static Device *
VolksloggerCreateOnPort(Port *com_port)
{
  return new VolksloggerDevice(com_port);
}

const struct DeviceRegister vlDevice = {
  _T("Volkslogger"),
  drfGPS | drfLogger | drfBaroAlt,
  VolksloggerCreateOnPort,
};
