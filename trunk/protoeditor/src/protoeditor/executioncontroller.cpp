/***************************************************************************
 *   Copyright (C) 2004-2006 by Thiago Silva                               *
 *   thiago.silva@kdemail.net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "executioncontroller.h"
#include "protoeditor.h"
#include "mainwindow.h"
#include "editorui.h"

#include <kurl.h>

ExecutionController::ExecutionController()
{
}


ExecutionController::~ExecutionController()
{
}

void ExecutionController::executeScript()
{
  //1: get the URL of the file to execute
//   KURL url = Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL();

  //2: figure what interpreter to use (mime, settings, etc)
  //
}


void ExecutionController::debugStart()
{
}

void ExecutionController::debugStop()
{
}

void ExecutionController::debugRunToCursor()
{
}


void ExecutionController::debugStepOver()
{
}

void ExecutionController::debugStepInto()
{
}

void ExecutionController::debugStepOut()
{
}

void ExecutionController::profile()
{
}
