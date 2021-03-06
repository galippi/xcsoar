/* Copyright_License {

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

#ifndef TASK_BEHAVIOUR_HPP
#define TASK_BEHAVIOUR_HPP

#include "Tasks/PathSolvers/Contests.hpp"
#include "OrderedTaskBehaviour.hpp"

struct AIRCRAFT_STATE;

/**
 *  Class defining options for task system.
 *  Typical uses might be default values, and simple aspects of task behaviour.
 */
class TaskBehaviour 
{
public:
  /**
   * Constructor, sets default task behaviour
   */
  TaskBehaviour();

  /**
   * Option to enable positionining of AAT targets to achieve
   * desired AAT minimum task time
   */
  bool optimise_targets_range;
  /** Option to enable positioning of AAT targets at optimal point on isoline */
  bool optimise_targets_bearing;
  /** Option to enable calculation and setting of auto MacCready */
  bool auto_mc;

  /** Enumeration of auto MC modes */
  enum AutoMCMode_t {
    /** Final glide only */
    AUTOMC_FINALGLIDE = 0,
    /** Climb average */
    AUTOMC_CLIMBAVERAGE,
    /** Final glide if above FG, else climb average */
    AUTOMC_BOTH
  };

  /** Options for auto MC calculations */
  AutoMCMode_t auto_mc_mode;
  
  /** Option to enable calculation of cruise efficiency */
  bool calc_cruise_efficiency;
  /** Option to enable calculation of effective mc */
  bool calc_effective_mc;
  /** Option to enable calculation of required sink rate for final glide */
  bool calc_glide_required;
  /** Option to enable Goto tasks for non-landable waypoints */
  bool goto_nonlandable;

  /** Compensation factor for risk at low altitude */
  fixed risk_gamma;

  /** Whether to do online OLC optimisation */
  bool enable_olc;

  /** Rule set to scan for in OLC */
  Contests contest;
  /** Handicap factor */
  unsigned contest_handicap;

  /** Safety MacCready value (m/s) used by abort task */
  fixed safety_mc;
  /** Whether to use safety mc value or current task polar MC */
  bool safety_mc_use_current;

  /** Minimum height above terrain for arrival height at non-landable waypoint (m) */
  fixed safety_height_terrain;
  /** Minimum height above terrain for arrival height at landable waypoint (m) */
  fixed safety_height_arrival;

  /** Margin in maximum ground speed (m/s) allowed in start sector */
  fixed start_max_speed_margin;
  /** Margin in maximum height (m) allowed in start sector */
  unsigned start_max_height_margin;

  /** Defaults for ordered task */
  OrderedTaskBehaviour ordered_defaults;

  /** Whether to maintain a thinned trace of the flight.  This is overridden by enable_olc. */
  bool enable_trace;

  /**
   * Convenience function (used primarily for testing) to disable
   * all expensive task behaviour functions.
   */
  void all_off();

  /**
   * Return safety MC value (based on options)
   *
   * @param fallback_mc Current glide polar mc value (m/s)
   *
   * @return Safety MC value (m/s)
   */
  fixed get_safety_mc(const fixed fallback_mc) const;
};

#endif
