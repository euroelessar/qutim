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

#include "kopeteemoticonsbackend.h"
#include <qutim/libqutim_global.h>
#include "kopeteemoticonsprovider.h"
#include <qutim/thememanager.h>
#include <QDebug>

EmoticonsProvider* KopeteEmoticonsBackend::loadTheme(const QString& name)
{
	//TODO OPTIMIZE ME
	QStringList themes = ThemeManager::list("emoticons");
	QStringList::const_iterator it;
	KopeteEmoticonsProvider *provider = new KopeteEmoticonsProvider();
	for (it=themes.constBegin();it!=themes.constEnd();it++) {
		QString themePath = ThemeManager::path("emoticons",*it);
		provider->setThemePath(themePath);
		if (provider->themeName() == name) {
			provider->loadTheme();
			return provider;
		}
	}
	delete provider;
	return 0;
}

QStringList KopeteEmoticonsBackend::themeList()
{
	//TODO OPTIMIZE ME
	QStringList themes = ThemeManager::list("emoticons");
	QStringList::const_iterator it;
	QStringList themeList;
	for (it=themes.constBegin();it!=themes.constEnd();it++) {
		QString themePath = ThemeManager::path("emoticons",*it);
		KopeteEmoticonsProvider provider (themePath);
		if (!provider.themeName().isEmpty())
			themeList.append(provider.themeName());
	}
	return themeList;
}

KopeteEmoticonsBackend::~KopeteEmoticonsBackend()
{

}

