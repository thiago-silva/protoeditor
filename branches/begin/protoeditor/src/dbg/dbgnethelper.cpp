/***************************************************************************
 *   Copyright (C) 2004 by Thiago Silva                                    *
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


#include "dbgnethelper.h"

int DBGNetHelper::fromNetwork(char* data, int offset)
{
  return ((data[offset]   & 0xff) << 24)
       | ((data[offset+1] & 0xff) << 16)
       | ((data[offset+2] & 0xff) << 8)
       | (data[offset+3]  & 0xff);
}

char* DBGNetHelper::toNetwork(int num, char* buffer, int offset)
{
  buffer[offset] =   (0xff & (num >> 24));
  buffer[offset+1] = (0xff & (num >> 16));
  buffer[offset+2] = (0xff & (num >> 8 ));
  buffer[offset+3] = 0xff & num;
  return buffer;
}


