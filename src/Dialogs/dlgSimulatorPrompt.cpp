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

#include "Screen/Bitmap.hpp"
#include "Gauge/LogoView.hpp"
#include "Dialogs/Internal.hpp"
#include "resource.h"
#include "Screen/Layout.hpp"
#include "MainWindow.hpp"
#include "Simulator.hpp"

#include <stdio.h>

#ifdef SIMULATOR_AVAILABLE

static WndForm *wf = NULL;

static void
OnLogoPaint(WndOwnerDrawFrame *Sender, Canvas &canvas)
{
  canvas.clear_white();
  DrawLogo(canvas, Sender->get_client_rect());
}

static void
OnSimulatorClicked(gcc_unused WndButton &button)
{
  wf->SetModalResult(mrOK);
}

static void
OnFlyClicked(gcc_unused WndButton &button)
{
  wf->SetModalResult(mrCancel);
}

static CallBackTableEntry CallBackTable[] = {
  DeclareCallBackEntry(OnLogoPaint),
  DeclareCallBackEntry(NULL)
};

#endif

bool
dlgSimulatorPromptShowModal()
{
#ifdef SIMULATOR_AVAILABLE
  wf = LoadDialog(CallBackTable, XCSoarInterface::main_window,
                  Layout::landscape ? _T("IDR_XML_SIMULATORPROMPT_L") :
                                      _T("IDR_XML_SIMULATORPROMPT"));
  assert(wf != NULL);

  WndButton* wb;
  wb = ((WndButton *)wf->FindByName(_T("cmdSimulator")));
  assert(wb != NULL);
  wb->SetOnClickNotify(OnSimulatorClicked);

  wb = ((WndButton *)wf->FindByName(_T("cmdFly")));
  assert(wb != NULL);
  wb->SetOnClickNotify(OnFlyClicked);

  bool retval = (wf->ShowModal() == mrOK);

  delete wf;

  return retval;
#else
  return false;
#endif
}

