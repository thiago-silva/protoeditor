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

#include "mainwindow.h"

#include "dbugrconfigform.h"
#include "abstractdebugger.h"
#include "editortabwidget.h"
#include "protoeditor.h"
#include "debuggerstack.h"
#include "debuggercombostack.h"

#include <qfiledialog.h>
#include <qmessagebox.h>

#include <qpushbutton.h>
#include <qaction.h>
#include <kdebug.h>

#include <qhttp.h>


MainWindow::MainWindow(/*ProtoEditor* protoEditor, */QWidget* parent, const char* name, WFlags fl)
    : MainWindowBase(parent, name, fl)//, m_protoEditor(protoEditor)
{
}

MainWindow::~MainWindow()
{

}

QString MainWindow::openFile() {
  QString filepath = QFileDialog::getOpenFileName( "/home", "PHP (*.php)",
    this, "open file dialog", "Choose a file" );

  return filepath;
}

void MainWindow::closeEvent(QCloseEvent * e)
{
  //sigfalt when closing the application with documents opened.
  //closing them before works
  tabEditor()->terminate();
  MainWindowBase::closeEvent(e);
}

/*
void MainWindow::addDocumentToEditor(const QUrl& url) {
  tabEditor()->addDocument(url);
}

void MainWindow::removeDocumentFromEditor(QString filepath) {
  tabEditor()->removeDocument(filepath);
}
*/

void MainWindow::showError(QString error) {
  QMessageBox::critical(this, "Error", error);
}


#include "mainwindow.moc"
