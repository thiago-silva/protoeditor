// This file is generated by kconfig_compiler from phpsettings.kcfg.
// All changes you do to this file will be lost.

#include "phpsettings.h"

PHPSettings::PHPSettings(  )
  : KConfigSkeleton( QString::fromLatin1( "protoeditorrc" ) )
{
  setCurrentGroup( QString::fromLatin1( "PHP Debugger" ) );

  KConfigSkeleton::ItemString  *itemDefaultDebugger;
  itemDefaultDebugger = new KConfigSkeleton::ItemString( currentGroup(), QString::fromLatin1( "DefaultDebugger" ), mDefaultDebugger );
  addItem( itemDefaultDebugger, QString::fromLatin1( "DefaultDebugger" ) );

}

PHPSettings::~PHPSettings()
{
}

