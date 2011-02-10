/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2011 The XCSoar Project
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

#include "InfoBoxes/Content/Factory.hpp"

#include "InfoBoxes/Content/Alternate.hpp"
#include "InfoBoxes/Content/Altitude.hpp"
#include "InfoBoxes/Content/Direction.hpp"
#include "InfoBoxes/Content/Glide.hpp"
#include "InfoBoxes/Content/Other.hpp"
#include "InfoBoxes/Content/Speed.hpp"
#include "InfoBoxes/Content/Task.hpp"
#include "InfoBoxes/Content/Team.hpp"
#include "InfoBoxes/Content/Thermal.hpp"
#include "InfoBoxes/Content/Time.hpp"
#include "InfoBoxes/Content/Weather.hpp"

// Groups:
//   Altitude: e_HeightGPS,e_HeightAGL,e_H_Terrain,e_H_Baro,e_H_QFE
//   Aircraft info: e_Bearing,e_Speed_GPS,e_Track_GPS,e_AirSpeed_Ext,e_Load_G,e_WP_BearingDiff,e_Speed
//   LD: e_LD_Instantaneous,e_LD_Cruise,e_Fin_LD,e_Fin_GR,e_WP_LD,e_LD,e_LD_Avg
//   Vario: e_Thermal_30s,e_TL_Avg,e_TL_Gain,e_TL_Time,e_Thermal_Avg,e_Thermal_Gain,e_Climb_Avg,e_VerticalSpeed_GPS,e_VerticalSpeed_Netto
//   Wind: e_WindSpeed_Est,e_WindBearing_Est,e_Temperature,e_HumidityRel,e_Home_Temperature
//   MacCready: e_MacCready,e_WP_Speed_MC,e_Climb_Perc,e_Act_Speed
//   Nav: e_WP_Distance,e_WP_AltDiff,e_WP_H,e_WP_AltReq,e_Fin_AltDiff,e_Fin_AltReq,e_SpeedTaskAvg,59,61,e_Fin_Distance,e_AA_Time,e_AA_DistanceMax,e_AA_DistanceMin,e_AA_SpeedMax,e_AA_SpeedMin,e_Fin_AA_Distance,e_AA_SpeedAvg,60,73
//   Waypoint: e_WP_Name,e_TimeSinceTakeoff,e_TimeLocal,e_TimeUTC,e_Fin_Time,e_WP_Time,e_Fin_TimeLocal,e_WP_TimeLocal,e_DistanceVario
//   Team: e_Team_Code,e_Team_Bearing,e_Team_BearingDiff,e_Team_Range
//   Gadget: e_Battery,e_CPU_Load
//   Alternates: e_Alternate_1_Name,e_Alternate_2_Name,e_Alternate_1_GR
//   Experimental: e_Experimental1,e_Experimental2
const InfoBoxFactory::InfoBoxMetaData InfoBoxFactory::MetaData[NUM_TYPES] = {
  // 0
  {
    N_("Height GPS"),
    N_("H GPS"),
    N_("This is the height above mean sea level reported by the GPS. Touchscreen/PC only: in simulation mode, this value is adjustable with the up/down arrow keys and the right/left arrow keys also cause the glider to turn."),
    c_InfoBox::e_HeightAGL, // H AGL
    c_InfoBox::e_H_QFE, // QFE MSL
  },

  // 1
  {
    N_("Height AGL"),
    N_("H AGL"),
    N_("This is the navigation altitude minus the terrain height obtained from the terrain file. The value is coloured red when the glider is below the terrain safety clearance height."),
    c_InfoBox::e_H_Terrain, // H Gnd
    c_InfoBox::e_HeightGPS, // H GPS
  },

  // 2
  {
    N_("Thermal last 30 sec"),
    N_("TC 30s"),
    N_("A 30 second rolling average climb rate based of the reported GPS altitude, or vario if available."),
    c_InfoBox::e_TL_Avg, // TL Avg
    c_InfoBox::e_VerticalSpeed_Netto, // Netto
  },

  // 3
  {
    N_("Bearing"),
    N_("Bearing"),
    N_("True bearing of the next waypoint.  For AAT tasks, this is the true bearing to the target within the AAT sector."),
    c_InfoBox::e_Speed_GPS, // V Gnd
    c_InfoBox::e_Speed, // V TAS
  },

  // 4
  {
    N_("L/D instantaneous"),
    N_("L/D Inst"),
    N_("Instantaneous glide ratio, given by the ground speed divided by the vertical speed (GPS speed) over the last 20 seconds. Negative values indicate climbing cruise. If the vertical speed is close to zero, the displayed value is '---'."),
    c_InfoBox::e_LD_Cruise, // LD Cruise
    c_InfoBox::e_LD_Avg, // LD Avg
  },

  // 5
  {
    N_("L/D cruise"),
    N_("L/D Cru"),
    N_("The distance from the top of the last thermal, divided by the altitude lost since the top of the last thermal. Negative values indicate climbing cruise (height gain since leaving the last thermal). If the vertical speed is close to zero, the displayed value is '---'."),
    c_InfoBox::e_Fin_LD, // Final LD
    c_InfoBox::e_LD_Instantaneous, // LD Inst
  },

  // 6
  {
    N_("Speed ground"),
    N_("V Gnd"),
    N_("Ground speed measured by the GPS. If this infobox is active in simulation mode, pressing the up and down arrows adjusts the speed, and left and right turn the glider."),
    c_InfoBox::e_Track_GPS, // Track
    c_InfoBox::e_Bearing, // Bearing
  },

  // 7
  {
    N_("Last Thermal Average"),
    N_("TL Avg"),
    N_("Total altitude gain/loss in the last thermal divided by the time spent circling."),
    c_InfoBox::e_TL_Gain, // TL Gain
    c_InfoBox::e_Thermal_30s, // TC 30s
  },

  // 8
  {
    N_("Last Thermal Gain"),
    N_("TL Gain"),
    N_("Total altitude gain/loss in the last thermal."),
    c_InfoBox::e_TL_Time, // TL Time
    c_InfoBox::e_TL_Avg, // TL Avg
  },

  // 9
  {
    N_("Last Thermal Time"),
    N_("TL Time"),
    N_("Time spent circling in the last thermal."),
    c_InfoBox::e_Thermal_Avg, // TC Avg
    c_InfoBox::e_TL_Gain, // TL Gain
  },

  // 10
  {
    N_("MacCready Setting"),
    N_("MacCready"),
    N_("The current MacCready setting. This infobox also shows whether MacCready is manual or auto. (Touchscreen/PC only) Also used to adjust the MacCready Setting if the infobox is active, by using the up/down cursor keys."),
    c_InfoBox::e_WP_Speed_MC, // V MC
    c_InfoBox::e_Act_Speed, // V Opt
  },

  // 11
  {
    N_("Next Distance"),
    N_("WP Dist"),
    N_("The distance to the currently selected waypoint. For AAT tasks, this is the distance to the target within the AAT sector."),
    c_InfoBox::e_WP_AltDiff, // WP AltD
    c_InfoBox::e_OC_Distance, // OLC
  },

  // 12
  {
    N_("Next Altitude Difference"),
    N_("WP AltD"),
    N_("Arrival altitude at the next waypoint relative to the safety arrival altitude."),
    c_InfoBox::e_WP_H, // WP AltA
    c_InfoBox::e_WP_Distance, // WP Dist
  },

  // 13
  {
    N_("Next Altitude Required"),
    N_("WP AltR"),
    N_("Additional altitude required to reach the next turn point."),
    c_InfoBox::e_Fin_AltDiff, // Fin AltD
    c_InfoBox::e_WP_AltDiff, // WP AltD
  },

  // 14
  {
    N_("Next Waypoint"),
    N_("Next"),
    N_("The name of the currently selected turn point. When this infobox is active, using the up/down cursor keys selects the next/previous waypoint in the task. (Touchscreen/PC only) Pressing the enter cursor key brings up the waypoint details."),
    c_InfoBox::e_TimeSinceTakeoff, // Time flt
    c_InfoBox::e_DistanceVario, // RH Trend
  },

  // 15
  {
    N_("Final Altitude Difference"),
    N_("Fin AltD"),
    N_("Arrival altitude at the final task turn point relative to the safety arrival altitude."),
    c_InfoBox::e_Fin_AltReq, // Fin AltR
    c_InfoBox::e_WP_AltReq, // WP AltR
  },

  // 16
  {
    N_("Final Altitude Required"),
    N_("Fin AltR"),
    N_("Additional altitude required to finish the task."),
    c_InfoBox::e_SpeedTaskAvg, // V Task Av
    c_InfoBox::e_Fin_AltDiff, // Fin AltD
  },

  // 17
  {
    N_("Speed Task Average"),
    N_("V Task Av"),
    N_("Average cross country speed while on current task, compensated for altitude."),
    c_InfoBox::e_CC_SpeedInst, // V Task Inst
    c_InfoBox::e_Fin_AltReq, // Fin AltR
  },

  // 18
  {
    N_("Final Distance"),
    N_("Fin Dis"),
    N_("Distance to finish around remaining turn points."),
    c_InfoBox::e_AA_Time, // AA Time
    c_InfoBox::e_CC_Speed, // V Task Ach
  },

  // 19
  {
    N_("Final LD"),
    N_("Fin LD"),
    N_("The required glide ratio to finish the task, given by the distance to go divided by the height required to arrive at the safety arrival altitude. Negative values indicate a climb is necessary to finish. If the height required is close to zero, the displayed value is '---'. Note that this calculation may be optimistic because it reduces the height required to finish by the excess energy height of the glider if its true airspeed is greater than the MacCready and best LD speeds."),
    c_InfoBox::e_Fin_GR, // Final GR
    c_InfoBox::e_LD_Cruise, // LD Cruise
  },

  // 20
  {
    N_("Terrain Elevation"),
    N_("H GND"),
    N_("This is the elevation of the terrain above mean sea level, obtained from the terrain file at the current GPS location."),
    c_InfoBox::e_H_Baro, // H Baro
    c_InfoBox::e_HeightAGL, // H AGL
  },

  // 21
  {
    N_("Thermal Average"),
    N_("TC Avg"),
    N_("Altitude gained/lost in the current thermal, divided by time spent thermaling."),
    c_InfoBox::e_Thermal_Gain, // TC Gain
    c_InfoBox::e_TL_Time, // TL Time
  },

  // 22
  {
    N_("Thermal Gain"),
    N_("TC Gain"),
    N_("The altitude gained/lost in the current thermal."),
    c_InfoBox::e_Climb_Avg, // TC All
    c_InfoBox::e_Thermal_Avg, // TC Avg
  },

  // 23
  {
    N_("Track"),
    N_("Track"),
    N_("Magnetic track reported by the GPS. (Touchscreen/PC only) If this infobox is active in simulation mode, pressing the up and down  arrows adjusts the track."),
    c_InfoBox::e_AirSpeed_Ext, // V IAS
    c_InfoBox::e_Speed_GPS, // V Gnd
  },

  // 24
  {
    N_("Vario"),
    N_("Vario"),
    N_("Instantaneous vertical speed, as reported by the GPS, or the intelligent vario total energy vario value if connected to one."),
    c_InfoBox::e_VerticalSpeed_Netto, // Netto
    c_InfoBox::e_Climb_Avg, // TC All
  },

  // 25
  {
    N_("Wind Speed"),
    N_("Wind V"),
    N_("Wind speed estimated by XCSoar. (Touchscreen/PC only) Manual adjustment is possible by pressing the up/down cursor keys to adjust magnitude and left/right cursor keys to adjust bearing when the infobox is active. Pressing the enter cursor key saves the wind value as the initial value when XCSoar next starts."),
    c_InfoBox::e_WindBearing_Est, // Wind B
    c_InfoBox::e_Home_Temperature, // Max Temp
  },

  // 26
  {
    N_("Wind Bearing"),
    N_("Wind B"),
    N_("Wind bearing estimated by XCSoar. (Touchscreen/PC only) Manual adjustment is possible by pressing the up/down cursor keys to adjust bearing when the infobox is active."),
    c_InfoBox::e_Temperature, // OAT
    c_InfoBox::e_WindSpeed_Est, // Wind V
  },

  // 27
  {
    N_("AA Time"),
    N_("AA Time"),
    N_("Assigned Area Task time remaining. Goes red when time remaining has expired."),
    c_InfoBox::e_AA_TimeDiff, // AA dTime
    c_InfoBox::e_Fin_Distance, // Fin Dis
  },

  // 28
  {
    N_("AA Distance Max"),
    N_("AA Dmax"),
    N_("Assigned Area Task maximum distance possible for remainder of task."),
    c_InfoBox::e_AA_DistanceMin, // AA Dmin
    c_InfoBox::e_AA_TimeDiff, // AA dTime
  },

  // 29
  {
    N_("AA Distance Min"),
    N_("AA Dmin"),
    N_("Assigned Area Task minimum distance possible for remainder of task."),
    c_InfoBox::e_AA_SpeedMax, // AA Vmax
    c_InfoBox::e_AA_DistanceMax, // AA Dmax
  },

  // 30
  {
    N_("AA Speed Max"),
    N_("AA Vmax"),
    N_("Assigned Area Task average speed achievable if flying maximum possible distance remaining in minimum AAT time."),
    c_InfoBox::e_AA_SpeedMin, // AA Vmin
    c_InfoBox::e_AA_DistanceMin, // AA Dmin
  },

  // 31
  {
    N_("AA Speed Min"),
    N_("AA Vmin"),
    N_("Assigned Area Task average speed achievable if flying minimum possible distance remaining in minimum AAT time."),
    c_InfoBox::e_Fin_AA_Distance, // AA Dtgt
    c_InfoBox::e_AA_SpeedMax, // AA Vmax
  },

  // 32
  {
    N_("Airspeed IAS"),
    N_("V IAS"),
    N_("Indicated Airspeed reported by a supported external intelligent vario."),
    c_InfoBox::e_Load_G, // G load
    c_InfoBox::e_Track_GPS, // Track
  },

  // 33
  {
    N_("Pressure Altitude"),
    N_("H Baro"),
    N_("This is the barometric altitude obtained from a GPS equipped with pressure sensor, or a supported external intelligent vario."),
    c_InfoBox::e_H_QFE, // QFE GPS
    c_InfoBox::e_H_Terrain, // H Gnd
  },

  // 34
  {
    N_("Speed MacCready"),
    N_("V MC"),
    N_("The MacCready speed-to-fly for optimal flight to the next waypoint. In cruise flight mode, this speed-to-fly is calculated for maintaining altitude. In final glide mode, this speed-to-fly is calculated for descent."),
    c_InfoBox::e_Climb_Perc, // % Climb
    c_InfoBox::e_MacCready, // MC
  },

  // 35
  {
    N_("Percentage climb"),
    N_("% Climb"),
    N_("Percentage of time spent in climb mode. These statistics are reset upon starting the task."),
    c_InfoBox::e_Act_Speed, // V Opt
    c_InfoBox::e_WP_Speed_MC, // V MC
  },

  // 36
  {
    N_("Time of flight"),
    N_("Time flt"),
    N_("Time elapsed since takeoff was detected."),
    c_InfoBox::e_TimeLocal, // Time local
    c_InfoBox::e_WP_Name, // Next
  },

  // 37
  {
    N_("G load"),
    N_("G"),
    N_("Magnitude of G loading reported by a supported external intelligent vario. This value is negative for pitch-down manoeuvres."),
    c_InfoBox::e_WP_BearingDiff, // Bearing D
    c_InfoBox::e_AirSpeed_Ext, // Track
  },

  // 38
  {
    N_("Next LD"),
    N_("WP LD"),
    N_("The required glide ratio to reach the next waypoint, given by the distance to next waypoint divided by the height required to arrive at the safety arrival altitude. Negative values indicate a climb is necessary to reach the waypoint. If the height required is close to zero, the displayed value is '---'.   Note that this calculation may be optimistic because it reduces the height required to reach the waypoint by the excess energy height of the glider if its true airspeed is greater than the MacCready and best LD speeds."),
    c_InfoBox::e_LD, // LD Vario
    c_InfoBox::e_Fin_GR, // Final GR
  },

  // 39
  {
    N_("Time local"),
    N_("Time loc"),
    N_("GPS time expressed in local time zone."),
    c_InfoBox::e_TimeUTC, // Time UTC
    c_InfoBox::e_TimeSinceTakeoff, // Time flt
  },

  // 40
  {
    N_("Time UTC"),
    N_("Time UTC"),
    N_("GPS time expressed in UTC."),
    c_InfoBox::e_Fin_Time, // Fin ETE
    c_InfoBox::e_TimeLocal, // Time local
  },

  // 41
  {
    N_("Task Time To Go"),
    N_("Fin ETE"),
    N_("Estimated time required to complete task, assuming performance of ideal MacCready cruise/climb cycle."),
    c_InfoBox::e_WP_Time, // WP ETE
    c_InfoBox::e_TimeUTC, // Time UTC
  },

  // 42
  {
    N_("Next Time To Go"),
    N_("WP ETE"),
    N_("Estimated time required to reach next waypoint, assuming performance of ideal MacCready cruise/climb cycle."),
    c_InfoBox::e_Fin_TimeLocal, // Fin ETA
    c_InfoBox::e_Fin_Time, // Fin ETE
  },

  // 43
  {
    N_("Speed Dolphin"),
    N_("V Opt"),
    N_("The instantaneous MacCready speed-to-fly, making use of Netto vario calculations to determine dolphin cruise speed in the glider's current bearing. In cruise flight mode, this speed-to-fly is calculated for maintaining altitude. In final glide mode, this speed-to-fly is calculated for descent. In climb mode, this switches to the speed for minimum sink at the current load factor (if an accelerometer is connected). When Block mode speed to fly is selected, this infobox displays the MacCready speed."),
    c_InfoBox::e_MacCready, // MC
    c_InfoBox::e_Climb_Perc, // % Climb
  },

  // 44
  {
    N_("Netto Vario"),
    N_("Netto"),
    N_("Instantaneous vertical speed of air-mass, equal to vario value less the glider's estimated sink rate. Best used if airspeed, accelerometers and vario are connected, otherwise calculations are based on GPS measurements and wind estimates."),
    c_InfoBox::e_Thermal_30s, // TC 30s
    c_InfoBox::e_VerticalSpeed_GPS, // Vario
  },

  // 45
  {
    N_("Task Arrival Time"),
    N_("Fin ETA"),
    N_("Estimated arrival local time at task completion, assuming performance of ideal MacCready cruise/climb cycle."),
    c_InfoBox::e_WP_TimeLocal, // WP ETA
    c_InfoBox::e_WP_Time, // WP ETE
  },

  // 46
  {
    N_("Next Arrival Time"),
    N_("WP ETA"),
    N_("Estimated arrival local time at next waypoint, assuming performance of ideal MacCready cruise/climb cycle."),
    c_InfoBox::e_DistanceVario, // RH Trend
    c_InfoBox::e_Fin_TimeLocal, // Fin ETA
  },

  // 47
  {
    N_("Bearing Difference"),
    N_("Brng D"),
    N_("The difference between the glider's track bearing, to the bearing of the next waypoint, or for AAT tasks, to the bearing to the target within the AAT sector. GPS navigation is based on the track bearing across the ground, and this track bearing may differ from the glider's heading when there is wind present. Chevrons point to the direction the glider needs to alter course to correct the bearing difference, that is, so that the glider's course made good is pointing directly at the next waypoint. This bearing takes into account the curvature of the Earth."),
    c_InfoBox::e_Speed, // V TAS
    c_InfoBox::e_Load_G, // G load
  },

  // 48
  {
    N_("Outside Air Temperature"),
    N_("OAT"),
    N_("Outside air temperature measured by a probe if supported by a connected intelligent variometer."),
    c_InfoBox::e_HumidityRel, // RelHum
    c_InfoBox::e_WindBearing_Est, // Wind B
  },

  // 49
  {
    N_("Relative Humidity"),
    N_("RelHum"),
    N_("Relative humidity of the air in percent as measured by a probe if supported by a connected intelligent variometer."),
    c_InfoBox::e_Home_Temperature, // MaxTemp
    c_InfoBox::e_Temperature, // OAT
  },

  // 50
  {
    N_("Forecast Temperature"),
    N_("MaxTemp"),
    N_("Forecast temperature of the ground at the home airfield, used in estimating convection height and cloud base in conjunction with outside air temperature and relative humidity probe. (Touchscreen/PC only) Pressing the up/down cursor keys adjusts this forecast temperature."),
    c_InfoBox::e_WindSpeed_Est, // Wind V
    c_InfoBox::e_HumidityRel, // RelHum
  },

  // 51
  {
    N_("AA Distance Tgt"),
    N_("AA Dtgt"),
    N_("Assigned Area Task distance around target points for remainder of task."),
    c_InfoBox::e_AA_SpeedAvg, // AA Vtgt
    c_InfoBox::e_AA_SpeedMin, // AA Vmin
  },

  // 52
  {
    N_("AA Speed Tgt"),
    N_("AA Vtgt"),
    N_("Assigned Area Task average speed achievable around target points remaining in minimum AAT time."),
    c_InfoBox::e_Home_Distance, // Home Dis
    c_InfoBox::e_Fin_AA_Distance, // AA Dtgt
  },

  // 53
  {
    N_("L/D vario"),
    N_("L/D vario"),
    N_("Instantaneous glide ratio, given by the indicated airspeed divided by the total energy vertical speed, when connected to an intelligent variometer. Negative values indicate climbing cruise. If the total energy vario speed is close to zero, the displayed value is '---'."),
    c_InfoBox::e_LD_Avg, // LD Avg
    c_InfoBox::e_WP_LD, // Next LD
  },

  // 54
  {
    N_("Airspeed TAS"),
    N_("V TAS"),
    N_("True Airspeed reported by a supported external intelligent vario."),
    c_InfoBox::e_Bearing, // Bearing
    c_InfoBox::e_WP_BearingDiff, // Bearing Diff
  },

  // 55
  {
    N_("Team Code"),
    N_("TeamCode"),
    N_("The current Team code for this aircraft. Use this to report to other team members. The last team aircraft code entered is displayed underneath."),
    c_InfoBox::e_Team_Bearing, // Team Bearing
    c_InfoBox::e_Team_Range, // Team Range
  },

  // 56
  {
    N_("Team Bearing"),
    N_("Tm Brng"),
    N_("The bearing to the team aircraft location at the last team code report."),
    c_InfoBox::e_Team_BearingDiff, // Team Bearing Diff
    c_InfoBox::e_Team_Code, // Team Code
  },

  // 57
  {
    N_("Team Bearing Diff"),
    N_("Team Bd"),
    N_("The relative bearing to the team aircraft location at the last reported team code."),
    c_InfoBox::e_Team_Range, // Team Range
    c_InfoBox::e_Team_Bearing, // Team Bearing
  },

  // 58
  {
    N_("Team Range"),
    N_("Team Dis"),
    N_("The range to the team aircraft location at the last reported team code."),
    c_InfoBox::e_Team_Code, // Team Code
    c_InfoBox::e_Team_BearingDiff, // Team Bearing Diff
  },

  // 59
  {
    N_("Speed Task Instantaneous"),
    N_("V Tsk Ins"),
    N_("Instantaneous cross country speed while on current task, compensated for altitude."),
    c_InfoBox::e_CC_Speed, // V Task Ach
    c_InfoBox::e_SpeedTaskAvg, // V Task Av
  },

  // 60
  {
    N_("Distance Home"),
    N_("Home Dis"),
    N_("Distance to home waypoint (if defined)."),
    c_InfoBox::e_OC_Distance, // OLC
    c_InfoBox::e_AA_SpeedAvg, // AA Vtgt
  },

  // 61
  {
    N_("Speed Task Achieved"),
    N_("V Tsk Ach"),
    N_("Achieved cross country speed while on current task, compensated for altitude."),
    c_InfoBox::e_Fin_Distance, // Fin Dis
    c_InfoBox::e_CC_SpeedInst, // V Task Inst
  },

  // 62
  {
    N_("AA Delta Time"),
    N_("AA dT"),
    N_("Difference between estimated task time and AAT minimum time. Colored red if negative (expected arrival too early), or blue if in sector and can turn now with estimated arrival time greater than AAT time plus 5 minutes."),
    c_InfoBox::e_AA_DistanceMax, // AA Dmax
    c_InfoBox::e_AA_Time, // AA Time
  },

  // 63
  {
    N_("Thermal All"),
    N_("TC All"),
    N_("Time-average climb rate in all thermals."),
    c_InfoBox::e_VerticalSpeed_GPS, // Vario
    c_InfoBox::e_Thermal_Gain, // TC Gain
  },

  // 64
  {
    N_("Distance Vario"),
    N_("D Vario"),
    NULL,
    c_InfoBox::e_Thermal_30s, // TC 30s
    c_InfoBox::e_VerticalSpeed_Netto, // Netto
  },

  // 65
  {
#ifndef GNAV
    N_("Battery Percent"),
#else
    N_("Battery Voltage"),
#endif
    N_("Battery"),
    N_("Displays percentage of device battery remaining (where applicable) and status/voltage of external power supply."),
    c_InfoBox::e_CPU_Load, // CPU
    c_InfoBox::e_CPU_Load, // CPU
  },

  // 66
  {
    N_("Final GR"),
    N_("Fin GR"),
    N_("Geometric gradient to the arrival height above the final waypoint. This is not adjusted for total energy."),
    c_InfoBox::e_WP_LD, // Next LD
    c_InfoBox::e_Fin_LD, // Fin LD
  },

  // 67
  {
    N_("Alternate1 name"),
    N_("Altrn 1"),
    N_("Displays name and bearing to the best alternate landing location."),
    c_InfoBox::e_Alternate_2_Name, // Altern2 name
    c_InfoBox::e_Alternate_1_GR, // Altern1 GR
  },

  // 68
  {
    N_("Alternate2 name"),
    N_("Altrn 2"),
    N_("Displays name and bearing to the second alternate landing location."),
    c_InfoBox::e_Alternate_1_GR, // Altern1 GR
    c_InfoBox::e_Alternate_1_Name, // Altern1 name
  },

  // 69 
  {
    N_("Alternate1 GR"),
    N_("Altrn1 GR"),
    N_("Geometric gradient to the arrival height above the best alternate. This is not adjusted for total energy."),
    c_InfoBox::e_Alternate_1_Name, // Altern1 name
    c_InfoBox::e_Alternate_2_Name, // Altern2 name
  },

  // 70
  {
    N_("QFE GPS"),
    N_("QFE GPS"),
    N_("Automatic QFE. This altitude value is constantly reset to 0 on ground BEFORE taking off. After takeoff, it is no more reset automatically even if on ground. During flight you can change QFE with up and down keys. Bottom line shows QNH altitude. Changing QFE does not affect QNH altitude."),
    c_InfoBox::e_HeightGPS, // H MSL
    c_InfoBox::e_H_Baro, // H Baro
  },

  // 71
  {
    N_("L/D Average"),
    N_("L/D Avg"),
    N_("The distance made in the configured period of time , divided by the altitude lost since then. Negative values are shown as ^^^ and indicate climbing cruise (height gain). Over 200 of LD the value is shown as +++ . You can configure the period of averaging in the Special config menu. Suggested values for this configuration are 60, 90 or 120: lower values will be closed to LD INST, and higher values will be closed to LD Cruise. Notice that the distance is NOT the straight line between your old and current position: it's exactly the distance you have made even in a zigzag glide. This value is not calculated while circling. "),
    c_InfoBox::e_LD_Instantaneous, // LD Inst
    c_InfoBox::e_LD, // LD Vario
  },

  // 72
  {
    N_("Experimental1"),
    N_("Exp1"),
    NULL,
    c_InfoBox::e_Experimental2, // Exp2
    c_InfoBox::e_Experimental2, // Exp2
  },

  // 73
  {
    N_("Online Contest Distance"),
    N_("OLC"),
    N_("Instantaneous evaluation of the flown distance according to the configured Online-Contest rule set."),
    c_InfoBox::e_WP_Distance, // WP Dist
    c_InfoBox::e_Home_Distance, // Home Dis
  },

  // 74
  {
    N_("Experimental2"),
    N_("Exp2"),
    NULL,
    c_InfoBox::e_Experimental1, // Exp1
    c_InfoBox::e_Experimental1, // Exp1
  },

  // 75
  {
    N_("CPU Load"),
    N_("CPU"),
    N_("CPU load consumed by XCSoar averaged over 5 seconds."),
    c_InfoBox::e_Battery, // Battery
    c_InfoBox::e_Battery, // Battery
  },

  // 76
  {
    N_("Next Altitude Arrival"),
    N_("WP AltA"),
    N_("Absolute arrival altitude at the next waypoint in final glide."),
    c_InfoBox::e_WP_AltReq, // WP AltR
    c_InfoBox::e_WP_AltDiff, // WP AltD
  },
};

InfoBoxContent*
InfoBoxFactory::Create(const c_InfoBox &InfoBoxType)
{
  switch (InfoBoxType.type) {
  case c_InfoBox::e_HeightGPS:
    return new InfoBoxContentAltitudeGPS();
  case c_InfoBox::e_HeightAGL:
    return new InfoBoxContentAltitudeAGL();
  case c_InfoBox::e_Thermal_30s:
    return new InfoBoxContentThermal30s();
  case c_InfoBox::e_Bearing:
    return new InfoBoxContentBearing();
  case c_InfoBox::e_LD_Instantaneous:
    return new InfoBoxContentLDInstant();
  case c_InfoBox::e_LD_Cruise:
    return new InfoBoxContentLDCruise();
  case c_InfoBox::e_Speed_GPS:
    return new InfoBoxContentSpeedGround();
  case c_InfoBox::e_TL_Avg:
    return new InfoBoxContentThermalLastAvg();
  case c_InfoBox::e_TL_Gain:
    return new InfoBoxContentThermalLastGain();
  case c_InfoBox::e_TL_Time:
    return new InfoBoxContentThermalLastTime();
  case c_InfoBox::e_MacCready:
    return new InfoBoxContentMacCready();
  case c_InfoBox::e_WP_Distance:
    return new InfoBoxContentNextDistance();
  case c_InfoBox::e_WP_AltDiff:
    return new InfoBoxContentNextAltitudeDiff();
  case c_InfoBox::e_WP_AltReq:
    return new InfoBoxContentNextAltitudeRequire();
  case c_InfoBox::e_WP_Name:
    return new InfoBoxContentNextWaypoint();
  case c_InfoBox::e_Fin_AltDiff:
    return new InfoBoxContentFinalAltitudeDiff();
  case c_InfoBox::e_Fin_AltReq:
    return new InfoBoxContentFinalAltitudeRequire();
  case c_InfoBox::e_SpeedTaskAvg:
    return new InfoBoxContentTaskSpeed();
  case c_InfoBox::e_Fin_Distance:
    return new InfoBoxContentFinalDistance();
  case c_InfoBox::e_Fin_LD:
    return new InfoBoxContentFinalLD();
  case c_InfoBox::e_H_Terrain:
    return new InfoBoxContentTerrainHeight();
  case c_InfoBox::e_Thermal_Avg:
    return new InfoBoxContentThermalAvg();
  case c_InfoBox::e_Thermal_Gain:
    return new InfoBoxContentThermalGain();
  case c_InfoBox::e_Track_GPS:
    return new InfoBoxContentTrack();
  case c_InfoBox::e_VerticalSpeed_GPS:
    return new InfoBoxContentVario();
  case c_InfoBox::e_WindSpeed_Est:
    return new InfoBoxContentWindSpeed();
  case c_InfoBox::e_WindBearing_Est:
    return new InfoBoxContentWindBearing();
  case c_InfoBox::e_AA_Time:
    return new InfoBoxContentTaskAATime();
  case c_InfoBox::e_AA_DistanceMax:
    return new InfoBoxContentTaskAADistanceMax();
  case c_InfoBox::e_AA_DistanceMin:
    return new InfoBoxContentTaskAADistanceMin();
  case c_InfoBox::e_AA_SpeedMax:
    return new InfoBoxContentTaskAASpeedMax();
  case c_InfoBox::e_AA_SpeedMin:
    return new InfoBoxContentTaskAASpeedMin();
  case c_InfoBox::e_AirSpeed_Ext:
    return new InfoBoxContentSpeedIndicated();
  case c_InfoBox::e_H_Baro:
    return new InfoBoxContentAltitudeBaro();
  case c_InfoBox::e_WP_Speed_MC:
    return new InfoBoxContentSpeedMacCready();
  case c_InfoBox::e_Climb_Perc:
    return new InfoBoxContentThermalRatio();
  case c_InfoBox::e_TimeSinceTakeoff:
    return new InfoBoxContentTimeFlight();
  case c_InfoBox::e_Load_G:
    return new InfoBoxContentGLoad();
  case c_InfoBox::e_WP_LD:
    return new InfoBoxContentNextLD();
  case c_InfoBox::e_TimeLocal:
    return new InfoBoxContentTimeLocal();
  case c_InfoBox::e_TimeUTC:
    return new InfoBoxContentTimeUTC();
  case c_InfoBox::e_Fin_Time:
    return new InfoBoxContentFinalETE();
  case c_InfoBox::e_WP_Time:
    return new InfoBoxContentNextETE();
  case c_InfoBox::e_Act_Speed:
    return new InfoBoxContentSpeedDolphin();
  case c_InfoBox::e_VerticalSpeed_Netto:
    return new InfoBoxContentVarioNetto();
  case c_InfoBox::e_Fin_TimeLocal:
    return new InfoBoxContentFinalETA();
  case c_InfoBox::e_WP_TimeLocal:
    return new InfoBoxContentNextETA();
  case c_InfoBox::e_WP_BearingDiff:
    return new InfoBoxContentBearingDiff();
  case c_InfoBox::e_Temperature:
    return new InfoBoxContentTemperature();
  case c_InfoBox::e_HumidityRel:
    return new InfoBoxContentHumidity();
  case c_InfoBox::e_Home_Temperature:
    return new InfoBoxContentTemperatureForecast();
  case c_InfoBox::e_Fin_AA_Distance:
    return new InfoBoxContentTaskAADistance();
  case c_InfoBox::e_AA_SpeedAvg:
    return new InfoBoxContentTaskAASpeed();
  case c_InfoBox::e_LD:
    return new InfoBoxContentLDVario();
  case c_InfoBox::e_Speed:
    return new InfoBoxContentSpeed();
  case c_InfoBox::e_Team_Code:
    return new InfoBoxContentTeamCode();
  case c_InfoBox::e_Team_Bearing:
    return new InfoBoxContentTeamBearing();
  case c_InfoBox::e_Team_BearingDiff:
    return new InfoBoxContentTeamBearingDiff();
  case c_InfoBox::e_Team_Range:
    return new InfoBoxContentTeamDistance();
  case c_InfoBox::e_CC_SpeedInst:
    return new InfoBoxContentTaskSpeedInstant();
  case c_InfoBox::e_Home_Distance:
    return new InfoBoxContentHomeDistance();
  case c_InfoBox::e_CC_Speed:
    return new InfoBoxContentTaskSpeedAchieved();
  case c_InfoBox::e_AA_TimeDiff:
    return new InfoBoxContentTaskAATimeDelta();
  case c_InfoBox::e_Climb_Avg:
    return new InfoBoxContentThermalAllAvg();
  case c_InfoBox::e_DistanceVario:
    return new InfoBoxContentVarioDistance();
  case c_InfoBox::e_Battery:
    return new InfoBoxContentBattery();
  case c_InfoBox::e_Fin_GR:
    return new InfoBoxContentFinalGR();
  case c_InfoBox::e_Alternate_1_Name:
    return new InfoBoxContentAlternateName(0);
  case c_InfoBox::e_Alternate_2_Name:
    return new InfoBoxContentAlternateName(1);
  case c_InfoBox::e_Alternate_1_GR:
    return new InfoBoxContentAlternateGR(0);
  case c_InfoBox::e_H_QFE:
    return new InfoBoxContentAltitudeQFE();
  case c_InfoBox::e_LD_Avg:
    return new InfoBoxContentLDAvg();
  case c_InfoBox::e_Experimental1:
    return new InfoBoxContentExperimental1();
  case c_InfoBox::e_OC_Distance:
    return new InfoBoxContentOLC();
  case c_InfoBox::e_Experimental2:
    return new InfoBoxContentExperimental2();
  case c_InfoBox::e_CPU_Load:
    return new InfoBoxContentCPULoad();
  case c_InfoBox::e_WP_H:
    return new InfoBoxContentNextAltitudeArrival();
  default:
    return NULL;
  }
}
