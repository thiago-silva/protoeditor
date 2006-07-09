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

#include "consolewidget.h"

#include <kcombobox.h>
#include <ktextedit.h>
#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>


ConsoleWidget::ConsoleWidget(QWidget* parent, const char* name)
  : QWidget(parent, name)
{ 
  QVBoxLayout* consoleLayout = new QVBoxLayout(this, 1, 1);

  m_edit = new KTextEdit(this);
  m_edit->setReadOnly(true);
  m_edit->setPaper(QBrush(QColor("white")));

  consoleLayout->addWidget(m_edit);
    
  QHBoxLayout* cmdLayout = new QHBoxLayout(this, 1, 1);

  QLabel* cmdLabel = new QLabel(this);
  cmdLabel->setText(i18n("Command:"));
  cmdLabel->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, cmdLabel->sizePolicy().hasHeightForWidth()));
  cmdLayout->addWidget(cmdLabel);

  m_cbCommand = new KHistoryCombo(this);
  cmdLayout->addWidget(m_cbCommand);

  consoleLayout->addLayout(cmdLayout);

  connect(m_cbCommand, SIGNAL(returnPressed()), SLOT(slotUserCmd()) );  
}


ConsoleWidget::~ConsoleWidget()
{
}

void ConsoleWidget::appendDebuggerText(const QString& str)
{
  m_edit->append(str);
}

void ConsoleWidget::appendUserText(const QString& str)
{
  m_edit->append(QString("<font color=\"blue\">").append(str).append("</font>") );
}

void ConsoleWidget::slotUserCmd()
{
  QString cmd = m_cbCommand->currentText();
  if(!cmd.isEmpty())
  {
    m_cbCommand->addToHistory(cmd);
    emit sigExecuteCmd(cmd);
  }
}

#include "consolewidget.moc"
