/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef KOPETEEMOTICONSPROVIDER_H
#define KOPETEEMOTICONSPROVIDER_H
#include "kopeteemoticonsbackend.h"

class KopeteEmoticonsProvider : public EmoticonsProvider
{
public:
	KopeteEmoticonsProvider(const QString& themePath);
	KopeteEmoticonsProvider();
    virtual bool addEmoticon(const QString& imgPath, const QStringList& codes);
    virtual bool removeEmoticon(const QStringList& codes);
    virtual bool saveTheme();
    virtual QString themeName() const;
	void loadTheme(const QString &themePath);
	void loadTheme();
	void setThemePath(const QString& themePath);
private:
	void getThemeName();
	QString m_theme_name;
	QString m_theme_path;
};

#endif // KOPETEEMOTICONSPROVIDER_H

