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

#include "configdlg.h"

#include <qframe.h>
#include <qvbox.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kwin.h>
#include <kapplication.h>

#include "phpsettingswidget.h"
#include "sitesettingswidget.h"
#include "extoutputsettingswidget.h"

#include "protoeditorsettings.h"

ConfigDlg* ConfigDlg::m_self = 0;

ConfigDlg::ConfigDlg(QWidget *parent, const char *name)
 : KDialogBase(KDialogBase::TreeList, WStyle_DialogBorder, parent, name, true, "Settings", KDialogBase::Ok|KDialogBase::Cancel)
{
  setShowIconsInTreeList(true);
  setMinimumSize(600,400);


  QStringList path;
  path << i18n("Debuggers");
  setFolderIcon(path, BarIcon("debugger", KIcon::SizeSmall));

  QVBox *frame;

  path.clear();
  path << i18n("Debuggers") << i18n("PHP");
  frame = addVBoxPage(path, i18n("PHP"), BarIcon("php", KIcon::SizeSmall));
  frame->setSpacing(0);
  frame->setMargin(0);
  m_phpSettingsWidget = new PHPSettingsWidget(frame, "PHP");//->reparent(((QWidget*)frame), 0, QPoint());

  path.clear();
  path << i18n("Sites");
  frame= addVBoxPage(path, i18n("Sites"), BarIcon("package_network", KIcon::SizeSmall));
  frame->setSpacing(0);
  frame->setMargin(0);
  m_siteSettingsWidget = new SiteSettingsWidget(frame);//->reparent(((QWidget*)frame), 0, QPoint());

  path.clear();
  path << i18n("External output");
  frame = addVBoxPage(path, i18n("Browsers"), BarIcon("konqueror", KIcon::SizeSmall));
  frame->setSpacing(0);
  frame->setMargin(0);
  m_extOutputSettingsWidget = new ExtOutputSettingsWidget(frame, "External output");//->reparent(((QWidget*)frame), 0, QPoint());

  //(void)new DebuggerSettingsWidget(f, "Debugger");
  //(void) new BrowserSettingsWidget(f, "Browser");
  //addPage(i18n("Browser"), i18n("Browser"), "network");

  //QFrame *page = addPage(QString("Layout"));
 //QVBoxLayout *topLayout = new QVBoxLayout( page, 0, KDialog::spacingHint() );
 //QLabel *label = new QLabel( QString("Layout type"), page );
 //topLayout->addWidget( label );

  enableButtonSeparator(true);
  unfoldTreeList();
}


ConfigDlg::~ConfigDlg()
{
}

void ConfigDlg::showDialog()
{
  if(!m_self) {
    m_self = new ConfigDlg();
  }

  m_self->populateWidgets();
  m_self->exec();
}

void ConfigDlg::populateWidgets()
{
  m_extOutputSettingsWidget->populate();
  m_phpSettingsWidget->populate();
  m_siteSettingsWidget->populate();
}

void ConfigDlg::slotOk()
{
  KDialogBase::slotOk();


  m_siteSettingsWidget->updateSettings();
  m_extOutputSettingsWidget->updateSettings();
  m_phpSettingsWidget->updateSettings();

  ProtoeditorSettings::self()->writeConfig();

  emit settingsChanged();
}


#include "configdlg.moc"
