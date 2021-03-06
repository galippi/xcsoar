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

#include "TaskDijkstra.hpp"
#include "Task/Tasks/OrderedTask.hpp"

TaskDijkstra::TaskDijkstra(OrderedTask& _task) :
  NavDijkstra<SearchPoint> (_task.task_size()),
  task(_task),
  active_stage(_task.getActiveTaskPointIndex())
{
  sp_sizes.reserve(num_stages);
}

void
TaskDijkstra::get_sizes()
{
  for (unsigned stage = 0; stage != num_stages; ++stage)
    sp_sizes[stage] = task.get_tp_search_points(stage).size();
}

unsigned
TaskDijkstra::get_size(const unsigned stage) const
{
  return sp_sizes[stage];
}

const SearchPoint &
TaskDijkstra::get_point(const ScanTaskPoint &sp) const
{
  return task.get_tp_search_points(sp.first)[sp.second];
}

bool
TaskDijkstra::distance_max()
{
  if (num_stages < 2)
    return 0;

  get_sizes();

  const ScanTaskPoint start(0, 0);
  DijkstraTaskPoint dijkstra(start, false);

  const bool retval = distance_general(dijkstra);
  if (retval)
    save_max();

  return retval;
}

bool
TaskDijkstra::distance_min(const SearchPoint &currentLocation)
{
  if (num_stages < 2)
    return 0;

  get_sizes();

  const ScanTaskPoint start(max(1, (int)active_stage) - 1, 0);
  DijkstraTaskPoint dijkstra(start, true);
  if (active_stage)
    add_start_edges(dijkstra, currentLocation);

  const bool retval = distance_general(dijkstra);
  if (retval)
    save_min();

  return retval;
}

void
TaskDijkstra::save_min()
{
  for (unsigned j = active_stage; j != num_stages; ++j)
    task.set_tp_search_min(j, solution[j]);
}

void
TaskDijkstra::save_max()
{
  for (unsigned j = 0; j != num_stages; ++j) {
    task.set_tp_search_max(j, solution[j]);
    if (j <= active_stage)
      task.set_tp_search_achieved(j, solution[j]);
  }
}

void
TaskDijkstra::add_edges(DijkstraTaskPoint &dijkstra,
                        const ScanTaskPoint& curNode)
{
  ScanTaskPoint destination(curNode.first + 1, 0);
  const unsigned dsize = get_size(destination.first);

  for (; destination.second != dsize; ++destination.second)
    dijkstra.link(destination, curNode, distance(curNode, destination));
}

void 
TaskDijkstra::add_start_edges(DijkstraTaskPoint &dijkstra,
                              const SearchPoint &currentLocation)
{
  // need to remove dummy first point
  dijkstra.pop();

  ScanTaskPoint destination(active_stage, 0);
  const unsigned dsize = get_size(destination.first);

  for (; destination.second != dsize; ++destination.second)
    dijkstra.link(destination, destination,
                  distance(destination, currentLocation));
}

/**
 * @todo
 * - only scan parts that are required, and prune out points
 *   that become irrelevant (strictly under-performing) 
 * - if in sector, prune out all default points that result in
 *   lower distance than current achieved max
 *
 * - if searching min 
 *   first search max actual up to current
 *     (taking into account aircraft location?)
 *   then search min after that from aircraft location
 *
 * - also update saved rank for potential pruning operations
 */
