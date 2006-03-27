/***************************************************************************
 *   Copyright (C) 2004-2006 by Thiago Silva                               *
 *   thiago.silva@kdemail.net                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                      *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include "document.h"

#include <ktexteditor/document.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/view.h>
#include <ktexteditor/markinterfaceextension.h>
#include <ktexteditor/viewcursorinterface.h>

#include <kiconloader.h>
#include <kdeversion.h>
#include <kactioncollection.h>

//Tied to Kate :|
#include <kate/document.h>
#include <kate/view.h>

#include <klocale.h>

#include <qwidget.h>
#include <qlayout.h>

Document::Document(QWidget *parent, const KURL& url, const char *name)
  : QObject(parent, name), m_terminating(false), m_userMarkedGuard(false), 
    m_tab(0), m_view(0), m_execLine(0), m_preExecLine(0), m_url(url)
{ 
  init();
}


Document::~Document()
{
  m_terminating = true;
  
  if(m_view)
  {
    KTextEditor::Document* doc = m_view->document();
    doc->closeURL();
    //delete m_view; //KWrite only deletes the doc
    delete doc;
  }
  delete m_tab;
}

void Document::init() {  

  m_tab = new QWidget(dynamic_cast<QWidget*>(parent()));

  KTextEditor::Document *doc =
  KTextEditor::EditorChooser::createDocument(
    m_tab, "KTextEditor::Document");

  if(!doc) {
    //error!
    return;
  }
  
  QVBoxLayout *lay = new QVBoxLayout(m_tab, 1, 1);

  KTextEditor::View * view = doc->createView(m_tab);
  
  lay->addWidget(view);

  m_view = view;

  setupMarks();

  connect(m_view, SIGNAL(newStatus()), this, SIGNAL(sigTextChanged()));
  connect(m_view, SIGNAL(viewStatusMsg(const QString&)), this, SIGNAL(sigStatusMsg(const QString&)));

  //remove file_save* actons. Default goes directly to katepart
  //wich doesn't use our directory location, but the 
  //application binary directory as default to KFileDialog.
  //Those should be configured by the MainWindow now.
  KAction* ac = m_view->actionCollection()->action("file_save_as");
  if(ac) m_view->actionCollection()->take(ac);
  
  ac = m_view->actionCollection()->action("file_save");
  if(ac) m_view->actionCollection()->take(ac);

  //TabEditor process the settings.
  ac = m_view->actionCollection()->action("set_confdlg");
  if(ac) m_view->actionCollection()->take(ac);
}

KTextEditor::View* Document::view()
{
  return m_view;
}

KURL Document::url() {
  if(m_view->document()->url().isEmpty())
  {
    return m_url;
  }
  else
  {
    return m_view->document()->url();
  }
}

QWidget* Document::tab()
{
  return m_tab;
}

bool Document::save()
{
  bool ret;
  if(m_view)
  {
    ret = m_view->document()->save();
  }
  else
  {
    ret = false;
  }

  if(ret)
  {
    emit sigDocumentSaved();
  }

  return ret;
}

bool Document::saveAs(const KURL& url)
{
  bool ret;
  if(m_view)
  {
    ret = m_view->document()->saveAs(url);
  }
  else
  {
    ret = false;
  }

  if(ret)
  {
    emit sigDocumentSaved();
  }

  return ret;
}


bool Document::open(const KURL& url)
{
  if(!m_view)
  {
    return false;
  }
  else if(!m_view->document()->openURL(url))
  {
    return false;
  }

  //activate the reload dialog when external changes happen to the text
  Kate::Document* kdoc = dynamic_cast<Kate::Document*>(m_view->document());
  if(kdoc)
    kdoc->setFileChangedDialogsActivated(true);

  m_url = url;

  return true;
}

bool Document::close()
{
  //don't signal the remotion of makrs if it close
  //(persistent breakpoints)
  
  m_terminating = true;
  bool ret = m_view->document()->closeURL();
  m_terminating = false;
  
  return ret;
}

bool Document::existsOnDisk()
{
  return m_view->document()->url().isValid();
}

bool Document::isModified()
{
  return m_view->document()->isModified();
}

void Document::setupMarks()
{
  KTextEditor::MarkInterfaceExtension* imarkex =
    dynamic_cast<KTextEditor::MarkInterfaceExtension*>(m_view->document());

  if(!imarkex)
  {
    return;
  }

  KIconLoader *loader = KGlobal::iconLoader();

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setPixmap(KTextEditor::MarkInterface::Execution, loader->loadIcon(
                       "executionpoint", KIcon::Small));
#else
  imarkex->setPixmap(KTextEditor::MarkInterface::markType05, loader->loadIcon(
                       "executionpoint", KIcon::Small));
#endif

  imarkex->setPixmap(KTextEditor::MarkInterface::markType08, loader->loadIcon(
                       "preexecutionpoint", KIcon::Small));

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setPixmap(KTextEditor::MarkInterface::BreakpointActive, loader->loadIcon(
                       "activebreakpoint",KIcon::Small));
#else
  imarkex->setPixmap(KTextEditor::MarkInterface::markType02, loader->loadIcon(
                       "activebreakpoint", KIcon::Small));
#endif

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setPixmap(KTextEditor::MarkInterface::BreakpointDisabled, loader->loadIcon(
                       "disabledbreakpoint",KIcon::Small));
#else
  imarkex->setPixmap(KTextEditor::MarkInterface::markType04, loader->loadIcon(
                       "disabledbreakpoint", KIcon::Small));
#endif

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setDescription(KTextEditor::MarkInterface::BreakpointActive, i18n("Breakpoint"));
#else
  imarkex->setDescription(KTextEditor::MarkInterface::markType02, i18n("Breakpoint"));
#endif


#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setMarksUserChangable(KTextEditor::MarkInterface::Bookmark + KTextEditor::MarkInterface::BreakpointActive);
#else
  imarkex->setMarksUserChangable(KTextEditor::MarkInterface::markType01 + KTextEditor::MarkInterface::markType02);
#endif

  connect(m_view->document(), SIGNAL(marksChanged()), this, SLOT(slotMarkChanged()));
}



void Document::gotoLine(int line)
{
  if(!m_view) return;

  KTextEditor::ViewCursorInterface *vci;

  vci = dynamic_cast<KTextEditor::ViewCursorInterface*>(m_view);
  vci->setCursorPosition(line, 0);
}

int Document::currentLine()
{
  if(!m_view) return 0;

  unsigned int line, col;
  KTextEditor::ViewCursorInterface *vci;

  vci = dynamic_cast<KTextEditor::ViewCursorInterface*>(m_view);
  vci->cursorPosition(&line, &col);

  //kate is 0 based line
  return line+1;
}

void Document::addMark(int line, unsigned int type)
{
  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_view->document());

  if(!imark) return;

  m_userMarkedGuard = true;
  imark->addMark(line-1, type);
  m_userMarkedGuard = false;
}

void Document::removeMark(int line, unsigned int type)
{
  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_view->document());

  if(!imark) return;

  m_userMarkedGuard = true;
  imark->removeMark(line-1, type);
  m_userMarkedGuard = false;
}

void Document::markActiveBreakpoint(int line)
{
  if(!m_view) return;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  addMark(line, KTextEditor::MarkInterface::BreakpointActive);
#else
  addMark(line, KTextEditor::MarkInterface::markType02);
#endif
}

void Document::unmarkActiveBreakpoint(int line)
{
  if(!m_view) return;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  removeMark(line, KTextEditor::MarkInterface::BreakpointActive);
#else
  removeMark(line, KTextEditor::MarkInterface::markType02);
#endif
}

void Document::markDisabledBreakpoint(int line)
{
  if(!m_view) return;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  addMark(line, KTextEditor::MarkInterface::BreakpointDisabled);
#else
  addMark(line, KTextEditor::MarkInterface::markType04);
#endif
}

void Document::unmarkDisabledBreakpoint(int line)
{
  if(!m_view) return;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  removeMark(line, KTextEditor::MarkInterface::BreakpointDisabled);
#else
  removeMark(line, KTextEditor::MarkInterface::markType04);
#endif
}

void Document::markExecutionPoint(int line)
{
  if(!m_view) return;

  m_execLine = line;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  addMark(m_execLine, KTextEditor::MarkInterface::Execution);
#else
  addMark(m_execLine, KTextEditor::MarkInterface::markType05);
#endif
}

void Document::unmarkExecutionPoint()
{
  if(!m_view) return;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  removeMark(m_execLine, KTextEditor::MarkInterface::Execution);
#else
  removeMark(m_execLine, KTextEditor::MarkInterface::markType05);
#endif
}

void Document::markPreExecutionPoint(int line)
{
  if(!m_view) return;

  m_preExecLine = line;
  addMark(m_preExecLine, KTextEditor::MarkInterface::markType08);
}

void Document::unmarkPreExecutionPoint()
{
  if(!m_view) return;
  removeMark(m_preExecLine, KTextEditor::MarkInterface::markType08);
}

bool Document::hasBreakpointAt(int line)
{
  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_view->document());

  if(!imark) return false;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  return
    imark->mark(line-1) &
    (KTextEditor::MarkInterface::BreakpointDisabled |
     KTextEditor::MarkInterface::BreakpointActive);
#else
  return
    imark->mark(line-1) &
    (KTextEditor::MarkInterface::markType04 |
     KTextEditor::MarkInterface::markType02);
#endif
}

QString Document::wordUnderCursor()
{

  Kate::View* v = dynamic_cast<Kate::View*>(m_view);
  if(v)
  {
    return v->currentWord();
  }
  else
  {
    return QString::null;
  }
}

void Document::slotMarkChanged()
{
  if(m_terminating) return;

  //   if(m_processingMark) return;

  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_view->document());

  if(!imark) return;

  QPtrList<KTextEditor::Mark> marks = imark->marks();

  QValueList<KTextEditor::Mark>::iterator it;
  KTextEditor::Mark* mark;

  //delete all modified/removed marks
  bool found = false;
  for (it = m_breakpointMarks.begin(); it != m_breakpointMarks.end(); ++it)
  {
    //see if this mark was removed
    for (mark = marks.first(); mark; mark = marks.next())
    {
      //skip if is not a breakpoint mark
      if(!((mark->type & KTextEditor::MarkInterface::markType02) ||
           (mark->type & KTextEditor::MarkInterface::markType04)))
      {
        continue;
      }

      if (mark->line == (*it).line)
      {
        if((mark->type & KTextEditor::MarkInterface::markType02) &&
          (mark->type & KTextEditor::MarkInterface::markType04))
        {
          //activeBreakpoint and DisabledBreakpoint on the same line:
          //We should remove both (slotMarkChanged will be recursed, so, return after removing
          //and forget about it, since all marks on this context will be invalidated and
          //the recursion will take care of the update.
          int line = mark->line;
          removeMark(mark->line+1, KTextEditor::MarkInterface::markType02 | KTextEditor::MarkInterface::markType04);
          emit sigBreakpointUnmarked(this, line+1);
          return;
        }

        found = true;
        break;
      }
    }

    //mark doesn't exists anymore, inform everyone
    if (!found &&  !m_userMarkedGuard)
    {
      emit sigBreakpointUnmarked(this, (*it).line+1);
    }

    found = false;
  }

  //inform everyone about new/modified breakpoints
  found = false;
  for (mark = marks.first(); mark; mark = marks.next())
  {
    //skip if is not a breakpoint mark
    if(!((mark->type & KTextEditor::MarkInterface::markType02) ||
         (mark->type & KTextEditor::MarkInterface::markType04)))    
    {
      continue;
    }

    for (it = m_breakpointMarks.begin(); it != m_breakpointMarks.end(); ++it)
    {
      if ((*it).line == mark->line)
      {
        found = true;
        break;
      }
    }

    if (!found && !m_userMarkedGuard)
    {      
      emit sigBreakpointMarked(this, mark->line+1, 
        mark->type & KTextEditor::MarkInterface::markType02?true:false);
    }
    found = false;
  }

  //load the new marks
  m_breakpointMarks.clear();
  m_execLine = 0;
  m_preExecLine = 0;
  //   marks = imark->marks(); //
  for (mark = marks.first(); mark; mark = marks.next())
  {
    if((mark->type & KTextEditor::MarkInterface::markType02) ||
        (mark->type & KTextEditor::MarkInterface::markType04))
    {
      m_breakpointMarks.append(*mark);
    }

    if(mark->type &  KTextEditor::MarkInterface::markType05)
    {
      m_execLine = mark->line+1;
    }

    if(mark->type &  KTextEditor::MarkInterface::markType08)
    {
      m_preExecLine = mark->line+1;
    }
  }
}


void Document::configureEditor(KTextEditor::View* view)
{
  KTextEditor::ConfigInterface * conf = configInterface (view->document());
  if (!conf)
  {
    return;
  }

  conf->configDialog();
  conf->writeConfig();
}

#include "document.moc"
