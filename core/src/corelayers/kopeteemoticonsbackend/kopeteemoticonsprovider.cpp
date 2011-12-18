/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "kopeteemoticonsprovider.h"
#include <QFile>
#include <QDomDocument>
#include <QDir>
#include <QDebug>

KopeteEmoticonsProvider::KopeteEmoticonsProvider(const QString& themePath)
: m_theme_path(themePath)
{
	getThemeName();
}

void KopeteEmoticonsProvider::loadTheme(const QString& themePath)
{
	clearEmoticons();
	m_theme_path = themePath;
	loadTheme();
}

KopeteEmoticonsProvider::KopeteEmoticonsProvider()
{

}

void KopeteEmoticonsProvider::getThemeName()
{
	QDir dir (m_theme_path);
	QFile file(m_theme_path + "/emoticons.xml");
	if (!file.open(QIODevice::ReadOnly))
		return;
	QDomDocument doc;
	if (doc.setContent(&file))
		m_theme_name = doc.documentElement().attribute("title");
	if (m_theme_name.isEmpty())
		m_theme_name = dir.dirName();
}

void KopeteEmoticonsProvider::setThemePath(const QString& themePath)
{
	m_theme_path = themePath;
	getThemeName();
}

void KopeteEmoticonsProvider::loadTheme()
{
	QDir dir (m_theme_path);
	QFileInfoList fileList = dir.entryInfoList(QDir::Files);
	QMap<QString, QString> files;
	for (int i = 0; i < fileList.size(); ++i) {
		const QFileInfo &info = fileList.at(i);
		files.insert(info.baseName(), info.absoluteFilePath());
		files.insert(info.fileName(), info.absoluteFilePath());
	}
	QFile file;
	file.setFileName(m_theme_path + "/emoticons.xml");
	if (!file.open(QIODevice::ReadOnly))
		return;
	QDomDocument doc;
	if(!doc.setContent(&file))
		return;
	m_theme_name = doc.documentElement().attribute("title");
	if (m_theme_name.isEmpty())
		m_theme_name = dir.dirName();
	QDomElement rootElement = doc.documentElement();
	int emoticonCount = rootElement.childNodes().count();
	QDomElement emoticon = rootElement.firstChild().toElement();
	for (int i = 0; i < emoticonCount; ++i) {
		if (emoticon.tagName() == QLatin1String("emoticon")) {
			QString fileName = files.value(emoticon.attribute(QLatin1String("file")));
			if (!fileName.isEmpty()) {
				QStringList strings;
				int stringCount = emoticon.childNodes().count();
				QDomElement emoticonString = emoticon.firstChild().toElement();
				for (int j = 0; j < stringCount; j++) {
					if (emoticonString.tagName() == QLatin1String("string"))
						strings.append(emoticonString.text());
					emoticonString = emoticonString.nextSibling().toElement();
				}
				appendEmoticon(fileName, strings);
			}
		}
		emoticon = emoticon.nextSibling().toElement();
	}
}

bool KopeteEmoticonsProvider::addEmoticon(const QString& imgPath, const QStringList& codes)
{
    return qutim_sdk_0_3::EmoticonsProvider::addEmoticon(imgPath, codes);
}

bool KopeteEmoticonsProvider::removeEmoticon(const QStringList& codes)
{
    return qutim_sdk_0_3::EmoticonsProvider::removeEmoticon(codes);
}

bool KopeteEmoticonsProvider::saveTheme()
{
    return qutim_sdk_0_3::EmoticonsProvider::saveTheme();
}

QString KopeteEmoticonsProvider::themeName() const
{
    return m_theme_name;
}

