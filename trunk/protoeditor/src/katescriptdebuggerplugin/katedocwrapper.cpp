#include "katedocwrapper.h"


#include <ktexteditor/document.h>
#include <ktexteditor/editorchooser.h>
#include <ktexteditor/view.h>
#include <ktexteditor/markinterfaceextension.h>
#include <ktexteditor/markinterface.h>
#include <ktexteditor/viewcursorinterface.h>

#include <kiconloader.h>
#include <kdeversion.h>
//#include <kactioncollection.h>

#include <kate/document.h>
#include <kate/view.h>

// #include <qdir.h>
#include <klocale.h>

#include "kateeditorui.h"

KateDocWrapper::KateDocWrapper(Kate::Document* doc, KateEditorUI* editor)
  : m_editor(editor), m_doc(doc), m_terminating(false), m_userMarkedGuard(false), 
    m_execLine(0), m_preExecLine(0)
{  

  connect(this, SIGNAL(sigBreakpointMarked( const KURL&, int, bool )),
    m_editor, SIGNAL(sigBreakpointMarked(const KURL&, int, bool)));

  connect(this, SIGNAL(sigBreakpointUnmarked( const KURL&, int)),
    m_editor, SIGNAL(sigBreakpointUnmarked(const KURL&, int)));

  connect(m_doc, SIGNAL(completed()), this, SLOT(slotDocumentOpened()));

  connect(this, SIGNAL(sigDocumentOpened(const KURL&)), 
    m_editor, SLOT(slotDocumentOpened(const KURL&)));

  if(m_doc->url().isValid())
  {
    slotDocumentOpened();
  }
}

void KateDocWrapper::slotDocumentOpened()
{  
  //breakpoints should be active only for HD-existing documents
  setupMarks();
  emit sigDocumentOpened(m_doc->url());
}

KateDocWrapper::~KateDocWrapper()
{
  if(m_editor->terminating()) return;

  m_terminating = true;

  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_doc);

  if(!imark) return;

  QPtrList<KTextEditor::Mark> marks = imark->marks();

  KTextEditor::Mark* mark;

  for (mark = marks.first(); mark; mark = marks.next())
  {
    if( (mark->type == KTextEditor::MarkInterface::markType05) ||
        (mark->type == KTextEditor::MarkInterface::markType08) ||
        (mark->type == KTextEditor::MarkInterface::markType02) ||
        (mark->type == KTextEditor::MarkInterface::markType04)) 
    {
      removeMark(mark->line, mark->type);
    }
  }

  KTextEditor::MarkInterfaceExtension* imarkex =
    dynamic_cast<KTextEditor::MarkInterfaceExtension*>(m_doc);

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  imarkex->setMarksUserChangable(KTextEditor::MarkInterface::Bookmark);
#else
  imarkex->setMarksUserChangable(KTextEditor::MarkInterface::markType01);
#endif
}


void KateDocWrapper::setupMarks()
{
  KTextEditor::MarkInterfaceExtension* imarkex =
    dynamic_cast<KTextEditor::MarkInterfaceExtension*>(m_doc);

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

  connect(m_doc, SIGNAL(marksChanged()), this, SLOT(slotMarkChanged()));
}

void KateDocWrapper::addMark(int line, unsigned int type)
{
  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_doc);

  if(!imark) return;

  m_userMarkedGuard = true;
  imark->addMark(line-1, type);
  m_userMarkedGuard = false;
}

void KateDocWrapper::removeMark(int line, unsigned int type)
{
  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_doc);

  if(!imark) return;

  m_userMarkedGuard = true;
  imark->removeMark(line-1, type);
  m_userMarkedGuard = false;
}

void KateDocWrapper::markActiveBreakpoint(int line)
{
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  addMark(line, KTextEditor::MarkInterface::BreakpointActive);
#else
  addMark(line, KTextEditor::MarkInterface::markType02);
#endif
}

void KateDocWrapper::unmarkActiveBreakpoint(int line)
{
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  removeMark(line, KTextEditor::MarkInterface::BreakpointActive);
#else
  removeMark(line, KTextEditor::MarkInterface::markType02);
#endif
}

void KateDocWrapper::markDisabledBreakpoint(int line)
{
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  addMark(line, KTextEditor::MarkInterface::BreakpointDisabled);
#else
  addMark(line, KTextEditor::MarkInterface::markType04);
#endif
}

void KateDocWrapper::unmarkDisabledBreakpoint(int line)
{
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  removeMark(line, KTextEditor::MarkInterface::BreakpointDisabled);
#else
  removeMark(line, KTextEditor::MarkInterface::markType04);
#endif
}

void KateDocWrapper::markExecutionPoint(int line)
{
  m_execLine = line;

#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  addMark(m_execLine, KTextEditor::MarkInterface::Execution);
#else
  addMark(m_execLine, KTextEditor::MarkInterface::markType05);
#endif
}

void KateDocWrapper::unmarkExecutionPoint()
{
#if (KDE_VERSION_MAJOR >= 3) &&  (KDE_VERSION_MINOR >= 3)
  removeMark(m_execLine, KTextEditor::MarkInterface::Execution);
#else
  removeMark(m_execLine, KTextEditor::MarkInterface::markType05);
#endif
}

void KateDocWrapper::markPreExecutionPoint(int line)
{
  m_preExecLine = line;
  addMark(m_preExecLine, KTextEditor::MarkInterface::markType08);
}

void KateDocWrapper::unmarkPreExecutionPoint()
{
  removeMark(m_preExecLine, KTextEditor::MarkInterface::markType08);
}

bool KateDocWrapper::hasBreakpointAt(int line)
{
  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_doc);

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

void KateDocWrapper::slotMarkChanged()
{
  KURL url = m_doc->url();

  if(m_terminating || (m_editor->documentIndex(url) == -1))
  {
    return; 
  }  

//   Kate::Document* dd = m_editor->documentFromID(m_doc->documentNumber());
//   QString name = m_doc->docName();


  KTextEditor::MarkInterface* imark =
    dynamic_cast<KTextEditor::MarkInterface*>(m_doc);

  if(!imark) return;

  KTextEditor::Mark* mark;

  QPtrList<KTextEditor::Mark> marks = imark->marks();

  QValueList<KTextEditor::Mark>::iterator it;
  

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

          if(!url.isValid())
          {
            url.setFileName(m_doc->docName());
          }
          emit sigBreakpointUnmarked(url, line+1);
          return;
        }

        found = true;
        break;
      }
    }

    //mark doesn't exists anymore, inform everyone
    if (!found &&  !m_userMarkedGuard)
    {
      if(!url.isValid())
      {
        url.setFileName(m_doc->docName());
      }
      emit sigBreakpointUnmarked(url, (*it).line+1);
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
      if(!url.isValid())
      {
          url.setFileName(m_doc->docName());
      }
      
      emit sigBreakpointMarked(url, mark->line+1, 
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

QString KateDocWrapper::docName()
{
  return m_doc->docName();
}

#include "katedocwrapper.moc"
