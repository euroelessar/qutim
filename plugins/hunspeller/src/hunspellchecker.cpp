/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011  Montel Laurent <montel@kde.org>
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

#include "hunspellchecker.h"
#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/config.h>
#include "hunspellsettings.h"
#include <QDir>
#include <QApplication>
#include <QTextCodec>

HunSpellChecker *HunSpellChecker::self = 0;

HunSpellChecker::HunSpellChecker() :
	m_speller(0)
{
	Q_ASSERT(!self);
	self = this;

#ifdef Q_WS_WIN
	m_dictPath = QCoreApplication::applicationDirPath() + "/dicts/";
#else
	m_dictPath = "/usr/share/myspell/dicts/";
#endif

	Settings::registerItem(new GeneralSettingsItem<HunSpellSettings>(
			Settings::General,
			Icon("tools-check-spelling"),
			QT_TRANSLATE_NOOP("Settings", "Spell checker")));
	QString lang = Config().group("speller").value("language", "system");
	loadSettings(lang);
}

HunSpellChecker::~HunSpellChecker()
{
	if (m_speller)
		delete m_speller;
}

bool HunSpellChecker::isCorrect(const QString &word) const
{
	if (!m_speller)
		return true; //unnecessary underline all words
	return m_speller->spell(convert(word));
}

QStringList HunSpellChecker::suggest(const QString &word) const
{
	if(!m_speller)
		return QStringList();
	char **selection;
	QStringList lst;
	int count = m_speller->suggest(&selection, convert(word));
	for(int i = 0; i < count; ++i)
		lst << (m_codec ? m_codec->toUnicode(selection[i]) : QString::fromUtf8(selection[i]));
	m_speller->free_list(&selection, count);
	return lst;
}

void HunSpellChecker::store(const QString &word) const
{
	if (!m_speller)
		return;
	m_speller->add(convert(word));
}

void HunSpellChecker::storeReplacement(const QString &bad, const QString &good)
{
	Q_UNUSED(bad);
	Q_UNUSED(good);
	debug() << "HunSpellChecker::storeReplacement not implemented";
}

QStringList HunSpellChecker::languages() const
{
	QStringList lst;
	QDir dir(m_dictPath);
	if(dir.exists()) {
		QStringList lstDic = dir.entryList(QStringList("*.dic"), QDir::Files);
		foreach(QString tmp, lstDic) {
			if (tmp.startsWith("hyph_"))
				continue;
			if (tmp.startsWith("th_"))
				continue;
			if (tmp.endsWith(".dic"))
				tmp = tmp.mid(0, tmp.length() - 4);
			lst << tmp;
		}
	}
	return lst;
}

void HunSpellChecker::loadSettings(QString lang)
{
	if (m_speller)
		delete m_speller;
	if (lang == QLatin1String("system"))
		lang = QLocale::system().name();
	QString dic = QString("%1/%2.dic").arg(m_dictPath).arg(lang);
	if (QFileInfo(dic).exists()) {
		m_speller = new Hunspell(QString("%1/%2.aff").arg(m_dictPath).arg(lang).toUtf8().constData(),
								 dic.toUtf8().constData());
		m_codec = QTextCodec::codecForName(m_speller->get_dic_encoding());
	} else {
		m_speller = 0;
	}
	emit dictionaryChanged();
}

QString HunSpellChecker::toPrettyLanguageName(const QString &lang)
{
	QString helper = lang;
	QLocale locale(helper.replace('-', '_'));
	if (locale.language() == QLocale::C)
		return lang;
	return QString("%1 / %2 (%3)")
			.arg(QLocale::languageToString(locale.language()))
			.arg(QLocale::countryToString(locale.country()))
			.arg(lang);
}

inline QByteArray HunSpellChecker::convert(const QString &word) const
{
	return m_codec ? m_codec->fromUnicode(word) : word.toUtf8();
}

