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

#include "configdlg.h"

#include <qframe.h>
#include <qvbox.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kwin.h>
#include <kapplication.h>

#include "sitesettingswidget.h"
#include "extappsettingswidget.h"
#include "protoeditor.h"
#include "protoeditorsettings.h"
#include "languagesettings.h"

ConfigDlg* ConfigDlg::m_self = 0;

ConfigDlg::ConfigDlg(QWidget *parent, const char *name)
 : KDialogBase(KDialogBase::TreeList, WStyle_DialogBorder, parent, name, true, "Settings", KDialogBase::Ok|KDialogBase::Cancel)
{
  setShowIconsInTreeList(true);
  setMinimumSize(600,400);

  enableButtonSeparator(true);
  unfoldTreeList();
  
  QVBox *frame;

  QStringList path;

  path << i18n("Languages");
  setFolderIcon(path, BarIcon("protoeditor", KIcon::SizeSmall));

  QValueList<LanguageSettings*> llist = Protoeditor::self()->settings()->languageSettingsList();
  QValueList<LanguageSettings*>::iterator it;
  for(it = llist.begin(); it != llist.end(); ++it)
  {
    path.clear();
    path << i18n("Languages") << (*it)->languageName();
    frame = addVBoxPage(path, (*it)->languageName(), BarIcon((*it)->iconName(), KIcon::SizeSmall));
    frame->setSpacing(0);
    frame->setMargin(0);
    m_langSettingsWidgets.append((*it)->createSettingsWidget(frame));
  }

  path.clear();
  path << i18n("Sites");
  frame= addVBoxPage(path, i18n("Sites"), BarIcon("package_network", KIcon::SizeSmall));
  frame->setSpacing(0);
  frame->setMargin(0);
  m_siteSettingsWidget = new SiteSettingsWidget(frame);//->reparent(((QWidget*)frame), 0, QPoint());

  path.clear();
  path << i18n("External Applications");
  frame = addVBoxPage(path, i18n("External Applications"), BarIcon("gear", KIcon::SizeSmall));
  frame->setSpacing(0);
  frame->setMargin(0);
  m_extAppSettingsWidget = new ExtAppSettingsWidget(frame, i18n("External Applications"));//->reparent(((QWidget*)frame), 0, QPoint());

  resize(QSize(590, 300).expandedTo(minimumSizeHint()));
}

ConfigDlg::~ConfigDlg()
{
}

void ConfigDlg::showDialog()
{
  populateWidgets();
  exec();
}

void ConfigDlg::populateWidgets()
{
  m_extAppSettingsWidget->populate();
  m_siteSettingsWidget->populate();

  for(QValueList<LanguageSettingsWidget*>::iterator it = m_langSettingsWidgets.begin(); it != m_langSettingsWidgets.end(); ++it)
  {
    (*it)->populate();
  }  
}

void ConfigDlg::slotOk()
{
  KDialogBase::slotOk();

  m_siteSettingsWidget->updateSettings();
  m_extAppSettingsWidget->updateSettings();

  for(QValueList<LanguageSettingsWidget*>::iterator it = m_langSettingsWidgets.begin(); it != m_langSettingsWidgets.end(); ++it)
  {
    (*it)->updateSettings();
  }  

  Protoeditor::self()->settings()->writeConfig();
}


#include "configdlg.moc"
