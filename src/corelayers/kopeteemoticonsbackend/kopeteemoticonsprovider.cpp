#include "kopeteemoticonsprovider.h"
#include <QFile>
#include <QDomDocument>
#include <QDir>


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


void KopeteEmoticonsProvider::getThemeName()
{
	QDir dir (m_theme_path);
	QStringList fileList = dir.entryList(QDir::Files);
	QFile file;
	file.setFileName(m_theme_path + "/emoticons.xml");
	if (!file.open(QIODevice::ReadOnly))
		return;
	QDomDocument doc;
	if(!doc.setContent(&file))
		return;
	m_theme_name = doc.documentElement().attribute("title");
}


void KopeteEmoticonsProvider::loadTheme()
{
	QDir dir (m_theme_path);
	QStringList fileList = dir.entryList(QDir::Files);
	QFile file;
	file.setFileName(m_theme_path + "/emoticons.xml");
	if (!file.open(QIODevice::ReadOnly))
		return;
	QDomDocument doc;
	if(!doc.setContent(&file))
		return;
	m_theme_name = doc.documentElement().attribute("title");
	QDomElement rootElement = doc.documentElement();
	int emoticonCount = rootElement.childNodes().count();
	QDomElement emoticon = rootElement.firstChild().toElement();
	for ( int i = 0; i < emoticonCount ; i++ ) {
		if ( emoticon.tagName() == "emoticon") {
			//HACK for STUPID old themes!
			QString regexp = "(^";
			regexp += QRegExp::escape(emoticon.attribute("file"));
			regexp += "\\.\\w+$)|(^";
			regexp += QRegExp::escape(emoticon.attribute("file"));
			regexp += "$)";
			QStringList fileName = fileList.filter(QRegExp(regexp));
			if ( !fileName.isEmpty()) {
				QStringList strings;
				int stringCount = emoticon.childNodes().count();
				QDomElement emoticonString = emoticon.firstChild().toElement();
				for(int j = 0; j < stringCount; j++) {
					if ( emoticonString.tagName() == "string") {
						strings.append(emoticonString.text());
					}
					emoticonString = emoticonString.nextSibling().toElement();
				}
				appendEmoticon(fileName.at(0),strings); //FIXME
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

