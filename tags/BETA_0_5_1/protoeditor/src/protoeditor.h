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

#ifndef PROTOEDITOR_H
#define PROTOEDITOR_H

#include <qobject.h>
#include <qstring.h>

class QWidget;
class MainWindow;
class AbstractDebugger;
class DebuggerManager;
class DebuggerExecutionLine;
class DebuggerStack;
class Variable;

class ProtoEditor : public QObject {
Q_OBJECT
public:
  ProtoEditor(QObject *parent = 0, const char* name = 0);
  ~ProtoEditor();

  MainWindow* mainWindow();

  void showError(QString);

public slots:
  virtual void slotOpenFile();
  virtual void slotSaveFile();
  virtual void slotCloseFile();
  virtual void slotExitApplication();


private:
  void openDocument(const QString& filepath);
  void saveState();
  //void setupResource();

  MainWindow* m_mainWindow;
  DebuggerManager* m_debugger_manager;
  QString m_currentOpenPath;
};

#endif
