/***************************************************************************
 *   Copyright (C) 2010 by Blade_Arma                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02111-1307 USA.           *
 ***************************************************************************/

#ifndef __PSXCOUNTERS_H__
#define __PSXCOUNTERS_H__

#include "psxcommon.h"
#include "r3000a.h"
#include "psxmem.h"
#include "plugins.h"

#ifdef __cplusplus

#include <imachine.h>

class PsxCnt : public QObject {
	Q_OBJECT
public:
	bool save(QDataStream &s);
	bool load(QDataStream &s);
};

extern PsxCnt psxCnt;

extern "C" {
#endif

extern u32 psxNextCounter, psxNextsCounter;

void psxRcntInit();
void psxRcntUpdate();

void psxRcntWcount(u32 index, u32 value);
void psxRcntWmode(u32 index, u32 value);
void psxRcntWtarget(u32 index, u32 value);

u32 psxRcntRcount(u32 index);
u32 psxRcntRmode(u32 index);
u32 psxRcntRtarget(u32 index);

#ifdef __cplusplus
}
#endif
#endif