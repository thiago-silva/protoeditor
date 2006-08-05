#include "kateeditorui.h"

#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kate/document.h>
#include <kate/view.h>
#include <kate/viewmanager.h>
#include <kate/mainwindow.h>
#include <kurl.h>
#include <qstring.h>

#include "plugin_katescriptdebugger.h"
#include "kateuinterface.h"

#include "protoeditor.h"
#include "datacontroller.h"

#include <kaction.h>
#include <klocale.h>

#include <kdebug.h>
#include "katedocwrapper.h"
#include <qfile.h>

KateEditorUI::KateEditorUI(KateUInterface* ui)
  : m_ui(ui), m_terminating(false)
{
//   (void)new KAction(i18n("Add Watch"), "math_brace", 0, this,
//                     SLOT(slotAddWatch()), ui->actionCollection(), "editor_add_watch");

//   connect(this, SIGNAL(sigFirstPage()),
//           Protoeditor::self(), SLOT(slotFirstDocumentOpened()));

//   connect(this, SIGNAL(sigEmpty()),
//           Protoeditor::self(), SLOT(slotNoDocumentOpened()));

  connect(this, SIGNAL(sigNewPage()),
          Protoeditor::self()->dataController(), SLOT(slotNewDocument()));

  connect(this, SIGNAL(sigAddWatch(const QString&)),
          Protoeditor::self()->dataController(), SLOT(slotWatchAdded(const QString&)));  

  //we aways have a page
//   emit sigFirstPage();

  connect(m_ui->plugin()->application()->documentManager(),
      SIGNAL(documentCreated (Kate::Document *)),
    this, SLOT(slotDocumentCreated(Kate::Document *)));


  connect(m_ui->plugin()->application()->documentManager(),
      SIGNAL(documentDeleted(uint)),
    this, SLOT(slotDocumentDeleted(uint)));
}


KateEditorUI::~KateEditorUI()
{

}

void KateEditorUI::setup()
{
  //setup all documents
  for(uint i = 0; i < m_ui->plugin()->application()->documentManager()->documents(); i++)
  {
    Kate::Document* d = m_ui->plugin()->application()->documentManager()->document(i);
    slotDocumentCreated(d);
    if(d->url().isValid())
    {
      slotDocumentOpened(d->url());
    }    
  }
}

void KateEditorUI::setTerminating()
{
  m_terminating = true;
}

bool KateEditorUI::terminating()
{
  return m_terminating;
}

bool KateEditorUI::openDocument(const KURL& url)
{
  activateDocument(url, true);
  return documentIndex(url)==-1?false:true;
}

void KateEditorUI::activateDocument(const KURL& url, bool forceOpen)
{
  int c = documentIndex(url);
  if(c != -1)
  {
    m_ui->plugin()->application()->activeMainWindow()->viewManager()->activateView(c);
  }
  else if(forceOpen)
  {
    m_ui->plugin()->application()->activeMainWindow()->viewManager()->openURL(url);
  }

  return;
}

bool KateEditorUI::saveCurrentFile()
{
  Kate::View::saveResult res = m_ui->plugin()->application()->activeMainWindow()->viewManager()->activeView()->save();
  return (res == Kate::View::SAVE_OK)?true:false;

// SAVE_OK, SAVE_CANCEL, SAVE_RETRY, SAVE_ERROR
}

bool KateEditorUI::saveCurrentFileAs(const KURL & url, const QString& encoding)
{
  if(!encoding.isEmpty()) {
    m_ui->plugin()->application()->documentManager()->activeDocument()->setEncoding(encoding);
  }

  return m_ui->plugin()->application()->documentManager()->activeDocument()->saveAs(url);
}

void KateEditorUI::gotoLineAtFile(const KURL& url, int line)
{
  activateDocument(url, true);

  m_ui->plugin()->application()->activeMainWindow()->viewManager()->activeView()->setCursorPosition(line, 0);
}

Kate::Document* KateEditorUI::currentDocument()
{
  return m_ui->plugin()->application()->documentManager()->activeDocument();
}

KURL KateEditorUI::currentDocumentURL()
{
  Kate::Document* doc = currentDocument();
  if(doc)
  {
    return doc->url();
  }
  else
  {
    return KURL();
  }
}

int  KateEditorUI::currentDocumentLine()
{
  unsigned int line, col;
  m_ui->plugin()->application()->activeMainWindow()->viewManager()->activeView()->cursorPosition(&line, &col);
  return line+1;
}


bool KateEditorUI::currentDocumentExistsOnDisk()
{
  return m_ui->plugin()->application()->documentManager()->activeDocument()->url().isValid();
}

void KateEditorUI::markActiveBreakpoint(const KURL& url, int line)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    m_docMap[doc]->markActiveBreakpoint(line);
  }
}

void KateEditorUI::unmarkActiveBreakpoint(const KURL& url, int line)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    m_docMap[doc]->unmarkActiveBreakpoint(line);
  }
}

void KateEditorUI::markDisabledBreakpoint(const KURL& url, int line)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    m_docMap[doc]->markDisabledBreakpoint(line);
  }
}

void KateEditorUI::unmarkDisabledBreakpoint(const KURL& url, int line)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    m_docMap[doc]->unmarkDisabledBreakpoint(line);
  }
}

void KateEditorUI::markExecutionPoint(const KURL& url, int line)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    m_docMap[doc]->markExecutionPoint(line);
  }
}

void KateEditorUI::unmarkExecutionPoint(const KURL& url)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    m_docMap[doc]->unmarkExecutionPoint();
  }
}

void KateEditorUI::markPreExecutionPoint(const KURL& url, int line)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    m_docMap[doc]->markPreExecutionPoint(line);
  }
}

void KateEditorUI::unmarkPreExecutionPoint(const KURL& url)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    m_docMap[doc]->unmarkPreExecutionPoint();
  }
}

bool KateEditorUI::hasBreakpointAt(const KURL& url, int line)
{
  Kate::Document* doc = documentFromURL(url);
  if(doc)
  {
    return m_docMap[doc]->hasBreakpointAt(line);
  }
  else
  {
    return false;
  }
}

void KateEditorUI::saveExistingFiles()
{
  Kate::DocumentManager* manager = m_ui->plugin()->application()->documentManager();
  for(uint i = 0; i < manager->documents(); i++)
  {
    if(manager->document(i)->url().isValid() && manager->document(i)->isModified())
    {
      manager->document(i)->save();
    }
  }
}

int KateEditorUI::totalDocuments()
{
  return m_ui->plugin()->application()->documentManager()->documents();
}

int KateEditorUI::documentIndex(const KURL& url)
{
  return m_ui->plugin()->application()->documentManager()->findDocument(url);
}

Kate::Document* KateEditorUI::documentFromURL(const KURL& url)
{
  int id = m_ui->plugin()->application()->documentManager()->findDocument(url);
  if(id != -1)
  {
    return m_ui->plugin()->application()->documentManager()->documentWithID(id);
  }
  else
  {
    return NULL;
  }
}

void KateEditorUI::slotDocumentCreated(Kate::Document *doc)
{
  m_docMap[doc] = new KateDocWrapper(doc, this);
  //connect(doc, SIGNAL(completed()), this, SIGNAL(sigNewPage()));  
//     m_ui->plugin()->application()->activeMainWindow()->viewManager()->activateView(doc->documentNumber());
//     emit sigNewPage();
}

void KateEditorUI::slotDocumentOpened(const KURL& url)
{
  if(m_ui->plugin()->application()->activeMainWindow())
  {
    m_ui->plugin()->application()->activeMainWindow()->viewManager()->activateView(documentIndex(url));
    emit sigNewPage();
  }  
}

void KateEditorUI::slotDocumentDeleted(uint id)
{
  Kate::Document* doc = m_ui->plugin()->application()->documentManager()->documentWithID(id);
  if(doc)
  {
    m_docMap.remove(doc);
  }
}

Kate::Document* KateEditorUI::documentFromID(uint id)
{
  return m_ui->plugin()->application()->documentManager()->documentWithID(id);
}

#include "kateeditorui.moc"
