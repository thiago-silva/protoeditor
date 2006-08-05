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

#ifndef PERLLOCALCOM_H
#define PERLLOCALCOM_H

#include "perlcom.h"

class KProcess;
class QString;

class PerlLocalCom : public PerlCom 
{
  Q_OBJECT
public:
  PerlLocalCom();
  ~PerlLocalCom();

  virtual void startDebugging(const QString& filePath, const QString& args);
  virtual void requestContinue();
  virtual void requestStop();
  virtual void requestRunToCursor(const QString& filePath, int line);
  virtual void requestStepInto();
  virtual void requestBreakpoint(DebuggerBreakpoint*);
  virtual void requestBreakpointRemoval(int);
  virtual void requestWatch(const QString&);
  virtual void executeCmd(const QString&);

private slots:
  void slotProcessExited(KProcess*);
  void slotReceivedOutput(KProcess*, char*, int);
private:

  KProcess *m_process;
  bool      m_processRunning;  
};

#endif
