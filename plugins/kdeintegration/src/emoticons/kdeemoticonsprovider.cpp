/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#include "kdeemoticonsprovider.h"
#include <shared/shareddata.h>
#include <QtDebug>

namespace KdeIntegration
{
	KdeEmoticonsProvider::KdeEmoticonsProvider(const KEmoticonsTheme &theme) : m_theme(theme)
	{
		reloadTheme();
	}

	void KdeEmoticonsProvider::reloadTheme()
	{
		clearEmoticons();
		QHash<QString, QStringList> map = m_theme.emoticonsMap();
		QHashIterator<QString, QStringList> it(map);
		while (it.hasNext()) {
			it.next();
			appendEmoticon(it.key(), it.value());
		}
	}

	QString KdeEmoticonsProvider::themeName() const
	{
		return m_theme.themeName();
	}

	bool KdeEmoticonsProvider::addEmoticon(const QString &imgPath, const QStringList &codes)
	{
		if (m_theme.addEmoticon(imgPath, codes.join(" "), KEmoticonsProvider::Copy)) {
			appendEmoticon(imgPath, codes);
			return true;
		}
		return false;
	}

	bool KdeEmoticonsProvider::removeEmoticon(const QStringList &codes)
	{
		if (m_theme.removeEmoticon(codes.join(" "))) {
			reloadTheme();
			return true;
		}
		return false;
	}

	bool KdeEmoticonsProvider::saveTheme()
	{
		m_theme.save();
		return true;
	}
}

