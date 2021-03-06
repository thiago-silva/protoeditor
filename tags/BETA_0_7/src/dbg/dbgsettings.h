// This file is generated by kconfig_compiler from dbgsettings.kcfg.
// All changes you do to this file will be lost.
#ifndef DBGSETTINGS_H
#define DBGSETTINGS_H

#include "debuggersettingsinterface.h"

class DBGSettingsWidget;

class DBGSettings : public DebuggerSettingsInterface
{
  public:
    DBGSettings(const QString& name);
    ~DBGSettings();

    bool breakOnLoad() const
    {
      return mBreakOnLoad;
    }

    bool sendErrors() const
    {
      return mSendErrors;
    }

    bool sendLogs() const
    {
      return mSendLogs;
    }

    bool sendOutput() const
    {
      return mSendOutput;
    }

    bool sendDetailedOutput() const
    {
      return mSendDetailedOutput;
    }

    int listenPort() const
    {
      return mListenPort;
    }

    bool enableJIT() const
    {
      return mEnableJIT;
    }

    virtual QString name() {
      return m_name;
    }

    virtual void loadValuesFromWidget();
    virtual DebuggerTab* widget();
  protected:

    // DBG
    bool mBreakOnLoad;
    bool mSendErrors;
    bool mSendLogs;
    bool mSendOutput;
    bool mSendDetailedOutput;
    int  mListenPort;
    bool mEnableJIT;

    QString m_name;

  private:
    DBGSettingsWidget* m_widget;
};

#endif
