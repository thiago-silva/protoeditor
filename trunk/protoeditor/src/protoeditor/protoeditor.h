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

#ifndef PROTOEDITOR_H
#define PROTOEDITOR_H

#include <qobject.h>

class MainWindow;
class KURL;

class ExecutionController;

class Protoeditor : public QObject
{
  Q_OBJECT

public:
  ~Protoeditor();

  static Protoeditor* self();

  void init();
  void dispose();

  MainWindow* mainWindow();

  void openFile();
  void openFile(const KURL& url);
  
  bool saveCurrentFile();
  bool saveCurrentFileAs();  
  
  bool useCurrentScript();

public slots:
  //menu "file"
  void slotAcNewFile();
  void slotAcOpenFile();
  void slotAcFileRecent(const KURL& url);

  void slotAcSaveCurrentFile();
  void slotAcSaveCurrentFileAs();

  
  void slotAcCloseFile();
  void slotAcCloseAllFiles();
  void slotAcQuit();
  
  //menu "script"
  void slotAcExecuteScript(const QString&);
  void slotAcDebugStart(const QString&);  

  void slotAcDebugStop();
  void slotAcDebugRunToCursor();
  void slotAcDebugStepOver();
  void slotAcDebugStepInto();
  void slotAcDebugStepOut();
  void slotProfile();
  void slotAcDebugToggleBp();

  void slotSettingsChanged();

private slots:
  void slotNoDocumentOpened();
  void slotFirstDocumentOpened();


private:
  Protoeditor();

  void loadSites();
  void registerLanguages();
  void loadLanguages();

  bool checkOverwrite(const KURL&);

  void executionPrologue();

  static Protoeditor *m_self;

  MainWindow          *m_window;

  ExecutionController *m_executionController;
};

#endif
