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

#ifndef MAINWINDOWBASE_H
#define MAINWINDOWBASE_H

#include <kparts/mainwindow.h>
#include <qstring.h>

class EditorUI;
class DebuggerUI;
class StatusBarWidget;

class KRecentFilesAction;
class KURL;
class KSelectAction;
class KToggleAction;
class KHistoryCombo;

class DebuggerManager;
class DebuggerSettingsWidget;
class BrowserSettingsWidget;

class MainWindow : public KParts::MainWindow
{
  Q_OBJECT

public:

  MainWindow(QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel);
  ~MainWindow();

  EditorUI*           editorUI();
  DebuggerUI*         debuggerUI();
  StatusBarWidget*    statusBar();

  KHistoryCombo*      cbArguments();

  void showError(const QString&) const;
  void showSorry(const QString&) const;

  void openFile();
  void openFile(const KURL& url);
  void actionStateChanged(const QString&);
  
  bool useCurrentScript();

  bool saveCurrentFileAs();
  bool saveCurrentFile();
  
private slots:
  void slotSaveCurrentFile();
  void slotSaveCurrentFileAs();
  void slotNewFile();
  void slotOpenFile();
  void slotCloseFile();
  void slotCloseAllFiles();
  void slotQuit();
  void slotEditKeys();
  void slotEditToolbars();

  void slotShowSettings();
  void slotFileRecent(const KURL&);

  void slotClose();

  void slotSettingsChanged();

  void slotScriptRun();
  void slotDebugStart();

  void slotFocusArgumentBar();  

protected:
  virtual void closeEvent(QCloseEvent * e);
private:
  void setupStatusBar();
  void setupActions();
  void createWidgets();
  void loadSites();

  bool checkOverwrite(KURL u);

  //main widgets
  EditorUI        *m_editorUI;
  DebuggerUI      *m_debuggerUI;
  StatusBarWidget *m_statusBar;

  //toolbar
  KHistoryCombo* m_cbArguments;

  KSelectAction      *m_siteAction;
  KToggleAction      *m_activeScriptAction;
  KRecentFilesAction *m_actionRecent;

  //settings
  DebuggerSettingsWidget *m_debuggerSettings;
  BrowserSettingsWidget  *m_browserSettings;

  //controllers
  DebuggerManager* m_debugger_manager;
};

#endif
