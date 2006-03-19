#ifndef GBSETTINGS_H
#define GBSETTINGS_H

#include "debuggersettingsinterface.h"

class GBSettingsWidget;

class GBSettings : public DebuggerSettingsInterface
{
public:
  GBSettings(const QString& name);
  ~GBSettings();

  int listenPort() const
  {
    return mListenPort;
  }

  bool enableJIT() const
  {
    return mEnableJIT;
  }

  bool breakOnLoad() const {
    return mBreakOnLoad;
  }

  QString startSessionScript()
  {
    return mStartSessionScript;
  }

  virtual QString name()
  {
    return m_name;
  }


  virtual void loadValuesFromWidget();
  virtual DebuggerTab* widget();
protected:

  // Gubed
  int  mListenPort;
  bool mEnableJIT;
  bool mBreakOnLoad;
  QString mStartSessionScript;
  
  QString m_name;  
private:
  GBSettingsWidget* m_widget;

};

#endif
