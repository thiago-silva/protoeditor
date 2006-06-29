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

#include "session.h"

#include "protoeditorsettings.h"
#include "languagesettings.h"

#include <kurl.h>
#include <klocale.h>


ExecutionController::ExecutionController()
{
}


ExecutionController::~ExecutionController()
{
}

bool ExecutionController::executionPrologue()
{
  //saves the argument in the history
  Protoeditor::self()->mainWindow()->saveArgumentList();

  //save all opened+existing files
  Protoeditor::self()->mainWindow()->editorUI()->saveExistingFiles();

  //check for opened+existing+saved file
  if(Protoeditor::self()->mainWindow()->editorUI()->count() == 0)
  {
    Protoeditor::self()->openFile();
    if(Protoeditor::self()->mainWindow()->editorUI()->count() == 0)
    {
      //couldn't open the file for some reason
      return false;
    }
  } else if(!Protoeditor::self()->mainWindow()->editorUI()->currentDocumentExistsOnDisk()) {
    if(!Protoeditor::self()->saveCurrentFileAs()) {
      //user didn't want to save the current file
      return false;
    }
  }

  //check for any session running
  //if(hasActiveSession()) { kill session? ask to kill before kill?; }

  //check if file is in the local system
  if(!Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL().isLocalFile())
  {
    Protoeditor::self()->showSorry(i18n("Unable to run non-local file"));
  }

  return true;
}

void ExecutionController::executeScript(const QString& langName, const QString& args)
{
  if(!executionPrologue()) return;

  KURL url = Protoeditor::self()->mainWindow()->editorUI()->currentDocumentURL();

  QString cmd = ProtoeditorSettings::self()->languageSettings(langName)->interpreterCommand();

  Session::self()->startLocal(langName, url, args);  
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
