/***************************************************************************
 *   Copyright (C) 2004 by Thiago Silva                                    *
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

#include "protoeditor.h"

#include "debuggerconfigurations.h"
#include "mainwindow.h"
#include "abstractdebugger.h"
#include "debuggerfactory.h"
#include "dbugrconfigform.h"
#include "debuggermanager.h"
#include "debuggerstack.h"
#include "debuggerconfigurations.h"
#include "editortabwidget.h"
#include "variableslistview.h"
#include "debuggercombostack.h"

#include <qwidget.h>
//#include <qvariant.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <ktextedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qheader.h>
#include <klistview.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qurl.h>
#include <kapplication.h>
#include <kconfig.h>

ProtoEditor::ProtoEditor(QObject *parent, const char* name)
  : QObject(parent, name)
{
  setupResource();

  m_mainWindow       = new MainWindow();
  m_debugger_manager = new DebuggerManager(this);

    //connect(m_mainWindow->edAddWatch(), SIGNAL(()) , this, SLOT(slotAddWatch()));
    connect(m_mainWindow->fileOpenAction(), SIGNAL(activated()), this, SLOT(slotOpenFile()));
    connect(m_mainWindow->fileSaveAction(), SIGNAL(activated()), this, SLOT(slotSaveFile()));
    connect(m_mainWindow->fileCloseAction(), SIGNAL(activated()) , this, SLOT(slotCloseFile()));
    connect(m_mainWindow->fileExitAction(), SIGNAL(activated()) , this, SLOT(slotExitApplication()));


}

ProtoEditor::~ProtoEditor()
{
}

void ProtoEditor::setupResource() {
  KConfig* config = KApplication::kApplication()->config();

  config->setGroup("Kate View Default");
  config->writeEntry("Default Mark Type", "1");
  config->writeEntry("Icon Bar", "true");
  config->writeEntry("Line Numbers", "true");
  config->sync();
}

MainWindow* ProtoEditor::mainWindow()
{
  return m_mainWindow;
}

void ProtoEditor::slotOpenFile() {
  QString filepath = m_mainWindow->openFile();
  if(!filepath.isEmpty()) {
    openDocument(filepath);
  }
}

void ProtoEditor::slotSaveFile() {
  if(!m_mainWindow->tabEditor()->saveCurrentFile()) {
    showError("Unable to save file");
  }
}

void ProtoEditor::slotCloseFile() {
  m_mainWindow->tabEditor()->closeCurrentDocument();
}

void ProtoEditor::slotExitApplication() {
  m_mainWindow->close();
}


void ProtoEditor::openDocument(const QString& filepath)
{
  m_mainWindow->tabEditor()->addDocument((QUrl(filepath)));
  //-populate breakpoints
  //-update the current line execution (if is this file that is currently being debuged)
}

/*
DebuggerConfigurations* ProtoEditor::debuggerConfigurations()
{
  return m_debuggerConfigurations;
}
*/

void ProtoEditor::showError(QString error)
{
  m_mainWindow->showError(error);
}

#include "protoeditor.moc"
