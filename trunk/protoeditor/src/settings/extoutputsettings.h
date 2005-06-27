/***************************************************************************
 *   Copyright (C) 2005 by Thiago Silva                                    *
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

#ifndef EXTOUTPUTSETTINGS_H
#define EXTOUTPUTSETTINGS_H

#include <kconfigskeleton.h>

//TODO: add a Console output!

class ExtOutputSettings : public KConfigSkeleton
{
  public:
    class EnumBrowser
    {
      public:
      enum { Konqueror, Mozilla, Firefox, Opera, COUNT };
    };

    ExtOutputSettings( );
    ~ExtOutputSettings();

    void setUseExternalBrowser(bool value)
    {
      mUseExternalBrowser = value;
    }

    bool useExternalBrowser() const
    {
      return mUseExternalBrowser;
    }

    void setBrowser(int value)
    {
      mBrowser = value;
    }

    int browser() const
    {
      return mBrowser;
    }

  protected:

    // ExternalOutput
    bool mUseExternalBrowser;
    int mBrowser;

  private:
};

#endif
