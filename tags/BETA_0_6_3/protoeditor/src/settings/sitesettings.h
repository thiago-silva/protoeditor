// This file is generated by kconfig_compiler from sitesettings.kcfg.
// All changes you do to this file will be lost.

#ifndef SITESETTINGS_H
#define SITESETTINGS_H

#include <kconfigskeleton.h>

class SiteSettings : public KConfigSkeleton
{
  public:
    SiteSettings( const QString &number );
    ~SiteSettings();

    void load(const QString& name, const QString& host, int port,
     const QString& remoteBaseDir, const QString& localBaseDir,
     const QString& defaultFile, bool matchFileInLowerCase,
     const QString& debuggerClient);

    void setName(const QString& name)
    {
      mName = name;
    }

    QString name() const
    {
      return mName;
    }


    void setHost(const QString& host)
    {
      mHost = host;
    }

    QString host() const
    {
      return mHost;
    }

    void setPort(int port)
    {
      mPort = port;
    }

    int port() const
    {
      return mPort;
    }

    void setLocalBaseDir(const QString& localBaseDir)
    {
      mLocalBaseDir = localBaseDir;
    }

    QString localBaseDir() const
    {
      return mLocalBaseDir;
    }

    void setRemoteBaseDir(const QString& remoteBaseDir)
    {
      mRemoteBaseDir = remoteBaseDir;
    }

    QString remoteBaseDir() const
    {
      return mRemoteBaseDir;
    }

    void setDefaultFile(const QString& defaultFile)
    {
      mDefaultFile = defaultFile;
    }

    QString defaultFile() const
    {
      return mDefaultFile;
    }

    void setMatchFileInLowerCase(bool value)
    {
      mMatchFileInLowerCase = value;
    }

    bool matchFileInLowerCase() const
    {
      return mMatchFileInLowerCase;
    }

    void setDebuggerClient(const QString& name)
    {
      mDebuggerClient = name;
    }

    QString debuggerClient() const
    {
      return mDebuggerClient;
    }

  protected:
    QString mParamnumber;

    // Site_$(number)
    QString mName;
    QString mHost;
    int mPort;
    QString mRemoteBaseDir;
    QString mLocalBaseDir;
    QString mDefaultFile;
    bool mMatchFileInLowerCase;
    QString mDebuggerClient;

  private:
};

#endif
