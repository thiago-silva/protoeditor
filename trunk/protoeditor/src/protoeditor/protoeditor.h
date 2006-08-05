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

class ExecutionController;
class DataController;

class DebuggerStack;

class UInterface;
class EditorInterface;
class DebuggerInterface;
class ProtoeditorSettings;
class Session;

class KURL;

class Protoeditor : public QObject
{
  Q_OBJECT

public: 

  ~Protoeditor();

  static Protoeditor* self();
  static void dispose();
  
  void init(UInterface*, const QString& config);
  

  UInterface* uinterface();

  EditorInterface*   editorUI();
  DebuggerInterface* debuggerUI();

  ProtoeditorSettings* settings();
  Session*             session();

  ExecutionController* executionController();
  DataController*      dataController();

 
  bool useCurrentScript();

  void showError(const QString&) const;
  void showSorry(const QString&) const;

  QString fileFilter();
public slots:
  //menu "script"
  void slotAcExecuteScript(const QString&);  
  void slotAcDebugStart(const QString&);  
  void slotAcDebugStart();  
  void slotAcProfileScript(const QString&);
  void slotAcProfileScript();

  void slotAcDebugStop();
  void slotAcDebugRunToCursor();
  void slotAcDebugStepOver();
  void slotAcDebugStepInto();
  void slotAcDebugStepOut();  
  void slotAcDebugToggleBp();  

  //UI slots
  void slotGotoLineAtFile(const KURL&, int);

private slots:
  //UI slots
  void slotFirstDocumentOpened();
  void slotNoDocumentOpened();  

  //Debug state slots
  void slotDebugStarted();
  void slotDebugEnded();

  void slotError(const QString& message);

  void slotSettingsChanged();
private:
  Protoeditor();

  void registerLanguages();
  void loadLanguages();
  void loadSites();
  

  static Protoeditor *m_self;

  UInterface          *m_uinterface;
  ProtoeditorSettings *m_settings;  
  Session             *m_session;
  
  ExecutionController *m_executionController;
  DataController      *m_dataController;  
  QString              m_fileFilter;
};

#endif
