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
#include <qmap.h>

class EditorUI;
class DebuggerUI;
class StatusBarWidget;
class KHistoryCombo;

class KToolBarPopupAction;

class KSelectAction;
class KToggleAction;
class KRecentFilesAction;

class MainWindow : public KParts::MainWindow
{
  Q_OBJECT

public:

  MainWindow(QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel);
  ~MainWindow();

  EditorUI*           editorUI();
  DebuggerUI*         debuggerUI();
  StatusBarWidget*    statusBar();

  KHistoryCombo*      argumentCombo();

  void actionStateChanged(const QString&);

  void saveArgumentList();  

  QString currentSiteName();
  void setSiteNames(const QStringList&);
  void setCurrentSite(int);

  void addRecentURL(const KURL&);
  void removeRecentURL(const KURL&);
  bool isCurrentScriptActionChecked();

  void saveRecentEntries();

  void clearLanguages();
  void addLanguage(const QString&);

  void showError(const QString&) const;
  void showSorry(const QString&) const;

signals:
  void sigExecuteScript(const QString&);
  void sigDebugScript(const QString&);

private slots:
  void slotAcEditKeys();
  void slotAcEditToolbars();
  void slotAcShowSettings();

  void slotAcFocusArgumentBar();

  void slotAcExecuteScript(int);
  void slotAcExecuteScript();
  void slotAcDebugStart(int);  
  void slotAcDebugStart();
protected:
  virtual void closeEvent(QCloseEvent * e);

private:
  void createWidgets();  
  void setupActions();

  //main widgets
  EditorUI        *m_editorUI;
  DebuggerUI      *m_debuggerUI;
  StatusBarWidget *m_statusBar;

  //toolbar
  KHistoryCombo       *m_cbArguments;

  KSelectAction       *m_siteAction;
  KToggleAction       *m_activeScriptAction;
  KRecentFilesAction  *m_actionRecent;
  KToolBarPopupAction *m_executeAction;
  KToolBarPopupAction *m_debugAction;

  QString             m_lastLang;
  QMap<int, QString>  m_langMap;                
};

#endif
