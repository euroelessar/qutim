/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Evgeniy Degtyarev <degtep@gmail.com>
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
#include "emoticonswrapper.h"
#include <qutim/emoticons.h>

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;

Q_GLOBAL_STATIC(QList<EmoticonsWrapper*>, m_managers)

EmoticonsWrapper::EmoticonsWrapper(QObject *parent) :
	QObject(parent)
{
	m_managers()->append(this);
}

EmoticonsWrapper::~EmoticonsWrapper()
{
	m_managers()->removeOne(this);
}

QString EmoticonsWrapper::currentThemeName()
{
	return Emoticons::currentThemeName();
}

void EmoticonsWrapper::setCurrentTheme(const QString &themeName)
{
	Emoticons::setTheme(themeName);
	for (int i = 0; i < m_managers()->count();i++)
	{
		emit m_managers()->at(i)->currentThemeNameChanged(themeName);
		emit m_managers()->at(i)->currentEmoticonsChanged(currentEmoticons());
		emit m_managers()->at(i)->availableChanged(isCurrentEmoticonsAvailable());
	}
}

QStringList EmoticonsWrapper::themes()
{
	QStringList themeList =  Emoticons::themeList();
	int index = themeList.indexOf(QLatin1String(""));
	themeList.removeAt(index);
	themeList.sort();
	themeList.prepend(tr("No emoticons"));
	return themeList;
}

QStringList EmoticonsWrapper::currentEmoticons()
{
	m_emoticonsMap = Emoticons::theme().emoticonsMap();
	return m_emoticonsMap.keys();
}

QString EmoticonsWrapper::currentEmoticonString(const QString &emoticonPath)
{
	return m_emoticonsMap[emoticonPath].first();
}

QStringList EmoticonsWrapper::emoticons(const QString &themeName)
{
	m_emoticonsMap = Emoticons::theme(themeName).emoticonsMap();
	return m_emoticonsMap.keys();
}

bool EmoticonsWrapper::isCurrentEmoticonsAvailable()
{
	return (Emoticons::theme().emoticonsIndexes().count() > 0);
}

}
