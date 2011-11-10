/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2003  Zack Rusin <zack@kde.org>
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

#include "aspellchecker.h"
#include <qutim/systeminfo.h>
#include <aspell.h>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/config.h>
#include "aspellsettings.h"

ASpellChecker *ASpellChecker::self = 0;

QHash<QString, LocalizedString> variantList()
{
	static QHash<QString, LocalizedString> list;
	if (list.isEmpty()) {
		list.insert("40", QT_TRANSLATE_NOOP("SpellChecker", "40")); // what does 40 mean?
		list.insert("60", QT_TRANSLATE_NOOP("SpellChecker", "60")); // what does 60 mean?
		list.insert("80", QT_TRANSLATE_NOOP("SpellChecker", "80")); // what does 80 mean?
		list.insert("ise", QT_TRANSLATE_NOOP("SpellChecker", "-ise suffixes"));
		list.insert("ize", QT_TRANSLATE_NOOP("SpellChecker", "-ize suffixes"));
		list.insert("ise-w_accents", QT_TRANSLATE_NOOP("SpellChecker", "-ise suffixes and with accents"));
		list.insert("ise-wo_accents", QT_TRANSLATE_NOOP("SpellChecker", "-ise suffixes and without accents"));
		list.insert("ize-w_accents", QT_TRANSLATE_NOOP("SpellChecker", "-ize suffixes and with accents"));
		list.insert("ize-wo_accents", QT_TRANSLATE_NOOP("SpellChecker", "-ize suffixes and without accents"));
		list.insert("lrg", QT_TRANSLATE_NOOP("SpellChecker", "large"));
		list.insert("med", QT_TRANSLATE_NOOP("SpellChecker", "medium"));
		list.insert("sml", QT_TRANSLATE_NOOP("SpellChecker", "small"));
		list.insert("variant_0", QT_TRANSLATE_NOOP("SpellChecker", "variant 0"));
		list.insert("variant_1", QT_TRANSLATE_NOOP("SpellChecker", "variant 1"));
		list.insert("variant_2", QT_TRANSLATE_NOOP("SpellChecker", "variant 2"));
		list.insert("wo_accents", QT_TRANSLATE_NOOP("SpellChecker", "without accents"));
		list.insert("w_accents", QT_TRANSLATE_NOOP("SpellChecker", "with accents"));
		list.insert("ye", QT_TRANSLATE_NOOP("SpellChecker", "with ye"));
		list.insert("yeyo", QT_TRANSLATE_NOOP("SpellChecker", "with yeyo"));
		list.insert("yo", QT_TRANSLATE_NOOP("SpellChecker", "with yo"));
		list.insert("extended", QT_TRANSLATE_NOOP("SpellChecker", "extended"));
	}
	return list;
}

ASpellChecker::ASpellChecker() :
	m_speller(0)
{
	Q_ASSERT(!self);
	self = this;
	m_config = new_aspell_config();
	/* All communication with Aspell is done in UTF-8 */
	/* For reference, please look at BR#87250         */
	aspell_config_replace(m_config, "encoding", "utf-8");

#ifdef Q_WS_WIN
	QByteArray dataDir = SystemInfo::getPath(SystemInfo::SystemShareDir).toLocal8Bit() + "/aspell/data/";
	QByteArray dictDir = SystemInfo::getPath(SystemInfo::SystemShareDir).toLocal8Bit() + "/aspell/dict/";
	aspell_config_replace(m_config, "data-dir", dataDir.constData());
	aspell_config_replace(m_config, "dict-dir", dictDir.constData());
#endif

	Settings::registerItem(new GeneralSettingsItem<AspellSettings>(
			Settings::General,
			Icon("tools-check-spelling"),
			QT_TRANSLATE_NOOP("Settings", "Spell checker")));
	QString lang = Config().group("speller").value("language", QString());
	if (lang == QLatin1String("system"))
		lang.clear();
	loadSettings(lang);
}

ASpellChecker::~ASpellChecker()
{
	if (m_speller) {
		aspell_speller_save_all_word_lists(m_speller);
		delete_aspell_speller(m_speller);
	}
	delete_aspell_config(m_config);
}

bool ASpellChecker::isCorrect(const QString &word) const
{
	/* ASpell is expecting length of a string in char representation */
	/* word.length() != word.toUtf8().length() for nonlatin strings    */
	if (!m_speller)
		return false;
	QByteArray data = word.toUtf8();
	int correct = aspell_speller_check(m_speller, data, data.length());
	return correct;
}

bool ASpellChecker::isMisspelled(const QString &word) const
{
	return !isCorrect(word);
}

QStringList ASpellChecker::suggest(const QString &word) const
{
	if (!m_speller)
		return QStringList();

	/* ASpell is expecting length of a string in char representation */
	/* word.length() != word.toUtf8().length() for nonlatin strings    */
	QByteArray data = word.toUtf8();
	const AspellWordList *suggestions = aspell_speller_suggest(
			m_speller, data, data.length());

	AspellStringEnumeration *elements = aspell_word_list_elements(suggestions);

	QStringList qsug;
	const char * cword;

	while ((cword = aspell_string_enumeration_next(elements))) {
		/* Since while creating the class ASpellDict the encoding is set */
		/* to utf-8, one has to convert output from Aspell to Unicode    */
		qsug.append(QString::fromUtf8(cword));
	}

	delete_aspell_string_enumeration(elements);
	return qsug;
}

void ASpellChecker::store(const QString &word) const
{
	if (!m_speller)
		return;
	QByteArray data = word.toUtf8();
	aspell_speller_add_to_personal(m_speller, data, data.length());
}

void ASpellChecker::storeReplacement(const QString &bad, const QString &good)
{
	QByteArray badData = bad.toUtf8();
	QByteArray goodData = good.toUtf8();
	aspell_speller_store_replacement(m_speller, badData, badData.length(), goodData, goodData.length());
}

QStringList ASpellChecker::languages() const
{
	AspellDictInfoList *l = get_aspell_dict_info_list(m_config);
	AspellDictInfoEnumeration *el = aspell_dict_info_list_elements(l);

	QStringList langs;
	const AspellDictInfo *di = 0;
	while ((di = aspell_dict_info_enumeration_next(el))) {
		langs.append(di->name);
	}

	delete_aspell_dict_info_enumeration(el);

	return langs;
}

void ASpellChecker::loadSettings(const QString &lang)
{
	if (m_speller) {
		aspell_speller_save_all_word_lists(m_speller);
		delete_aspell_speller(m_speller);
	}

	aspell_config_replace(m_config, "lang", lang.toLatin1());

	AspellCanHaveError * possible_err = new_aspell_speller(m_config);
	if (aspell_error_number(possible_err) != 0) {
		warning()<< "Error : "<< aspell_error_message(possible_err);
	} else {
		m_speller = to_aspell_speller(possible_err);
	}
	emit dictionaryChanged();
}

QString ASpellChecker::toPrettyLanguageName(const QString &lang)
{
	QString localeName = lang.mid(0, lang.indexOf('-'));
	QString type = variantList().value(lang.mid(localeName.length()+1));
	QLocale locale(localeName);
	QString name = QString("%1 / %2")
				   .arg(QLocale::languageToString(locale.language()))
				   .arg(QLocale::countryToString(locale.country()));
	if (!type.isEmpty())
		name += " (" + type + ")";
	return name;
}

