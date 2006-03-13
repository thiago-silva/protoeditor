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

  virtual QString name()
  {
    return m_name;
  }

  QString startSessionScript()
  {
    return mStartSessionScript;
  }

  virtual void loadValuesFromWidget();
  virtual DebuggerTab* widget();
protected:

  // Gubed
  int  mListenPort;
  bool mEnableJIT;
  QString mStartSessionScript;
  
  QString m_name;  
private:
  GBSettingsWidget* m_widget;

};

#endif
