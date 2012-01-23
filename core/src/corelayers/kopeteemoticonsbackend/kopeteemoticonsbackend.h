/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef KOPETEEMOTICONSBACKEND_H
#define KOPETEEMOTICONSBACKEND_H
#include <qutim/emoticons.h>

using namespace qutim_sdk_0_3;

class KopeteEmoticonsBackend : public EmoticonsBackend
{
	Q_OBJECT
public:
    virtual EmoticonsProvider* loadTheme(const QString& name);
    virtual QStringList themeList();	
    virtual ~KopeteEmoticonsBackend();
};

#endif // KOPETEEMOTICONSBACKEND_H

