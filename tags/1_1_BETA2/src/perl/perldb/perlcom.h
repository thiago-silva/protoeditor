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

#ifndef PERLCOM_H
#define PERLCOM_H

#include <qobject.h>

class DebuggerBreakpoint;

class PerlCom : public QObject
{
  Q_OBJECT
public:
  PerlCom() : QObject() {};
  virtual ~PerlCom() {};

  virtual void startDebugging(const QString& filePath, const QString& args) = 0;
  virtual void requestContinue() = 0;
  virtual void requestStop() = 0;
  virtual void requestRunToCursor(const QString& filePath, int line) = 0;
  virtual void requestStepInto() = 0;
  virtual void requestBreakpoint(DebuggerBreakpoint*) = 0;
  virtual void requestBreakpointRemoval(int) = 0;
//   virtual void requestVariables(int, int) = 0;
  virtual void requestWatch(const QString&) = 0;

  virtual void executeCmd(const QString&) = 0;

signals:
  void sigOutput(const QString&);
  void sigError(const QString&);
};

#endif
