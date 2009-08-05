 /*
    AbstractEmoticonsLayer

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>
						  Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "abstractemoticonslayer.h"
#include <QFile>
#include <QDir>
#include <QtXml>
#include <QtGui>

namespace CoreEmoticons
{

AbstractEmoticonsLayer::AbstractEmoticonsLayer()
{

}

void AbstractEmoticonsLayer::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("gui");
	m_emoticons_path = settings.value("emoticons",":/style/emoticons/emoticons.xml").toString();
    settings.endGroup();
    setEmoticonPath(m_emoticons_path);
}

void AbstractEmoticonsLayer::setProfileName(const QString &profile_name)
{
    m_profile_name = profile_name;
    loadSettings();
}

void AbstractEmoticonsLayer::setEmoticonPath(const QString &path)
{
	m_emoticon_list.clear();
	m_emoticons.clear();
	QFile file(path);
	QString dirPath = QFileInfo( path ).absolutePath();
	m_dir_path = dirPath;
	QDir dir ( dirPath );
	QStringList fileList = dir.entryList(QDir::Files);
	if (file.exists() && file.open(QIODevice::ReadOnly) )
	{
		QDomDocument doc;
		if ( doc.setContent(&file) )
		{
			QDomElement rootElement = doc.documentElement();
			int emoticonCount = rootElement.childNodes().count();
			QDomElement emoticon = rootElement.firstChild().toElement();
			for ( int i = 0; i < emoticonCount ; i++ )
			{
				if ( emoticon.tagName() == "emoticon")
				{
					QStringList fileName = fileList.filter(emoticon.attribute("file"));
					if ( !fileName.isEmpty())
					{
						QStringList strings;
						QPixmap tmp;
						int stringCount = emoticon.childNodes().count();
						QDomElement emoticonString = emoticon.firstChild().toElement();
						for(int j = 0; j < stringCount; j++)
						{
							if ( emoticonString.tagName() == "string")
							{
								if(tmp.isNull())
									tmp = QPixmap(dirPath + "/" + fileName.at(0));
								QString text = Qt::escape(emoticonString.text());
								m_urls.insert(Qt::escape(emoticonString.text()),
								QString("<img src=\"%1\" width=\"%2\" height=\"%3\" alt=\"%4\" title=\"%4\"/>")
								.arg(dirPath + "/" + fileName.at(0)).arg(tmp.size().width())
								.arg(tmp.size().height()).arg(text.replace("\"", "&quot;"))
								);


								strings.append(emoticonString.text());
							}

							emoticonString = emoticonString.nextSibling().toElement();
						}
						m_emoticon_list.insert(dirPath + "/" + fileName.at(0),strings);
					}
				}
				emoticon = emoticon.nextSibling().toElement();
			}
			QStringList emoticon_keys = m_urls.keys();
			qSort(emoticon_keys.begin(), emoticon_keys.end(), lengthLessThan);
			m_emoticons.clear();
			foreach( const QString &emoticon, emoticon_keys )
			{
				m_emoticons << qMakePair( emoticon, m_urls.value(emoticon) );
			}
		}
	}
}

bool AbstractEmoticonsLayer::lengthLessThan(const QString &s1, const QString &s2)
{
    return s1.size() > s2.size();
}

enum HtmlState
{
	OutsideHtml = 0,
	FirstTag,
	TagText,
	SecondTag
};

bool inline compareEmoticon(const QChar *c, const QString &smile)
{
	const QChar *s = smile.constData();
	while(*c == *s)
	{
		if(s->isNull())
			return true;
		s++;
		c++;
	}
	return s->isNull();
}

void AbstractEmoticonsLayer::checkMessageForEmoticons(QString &message)
{
	HtmlState state = OutsideHtml;
	const QChar *chars = message.constData();
	QChar cur;
	QString result;
	QList<QPair<QString,QString> >::const_iterator it;
	while(!chars->isNull())
	{
		cur = *chars;
		if(cur == '<')
		{
			if(state == OutsideHtml)
				state = FirstTag;
			else
				state = SecondTag;
		}
		else if(state == FirstTag || state == SecondTag)
		{
			switch(cur.unicode())
			{
			case L'/':
				state = SecondTag;
				break;
			case L'"':
			case L'\'':
				do result += *(chars++);
				while(!chars->isNull() && *chars != cur);
				break;
			case L'>':
				state = static_cast<HtmlState>((state + 1) % 4);
				break;
			default:
				break;
			}
		}
		else if(state != TagText && cur == '&')
		{
			do result += *(chars++);
			while(!chars->isNull() && *chars != ';');
			cur = *chars;
		}
		else if(state != TagText)
		{
			bool found = false;
			it = m_emoticons.constBegin();
			for( ; it != m_emoticons.constEnd(); it++ )
			{
				if(compareEmoticon(chars, (*it).first))
				{
					chars += (*it).first.length();
					cur = *chars;
					result += (*it).second;
					found = true;
					break;
				}
			}
			if(found)
				continue;
		}
		if(cur.isNull())
			break;
		result += cur;
		chars++;
	}

	message = result;
}

}
