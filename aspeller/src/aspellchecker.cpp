/**
 * aspellchecker.cpp
 *
 * Copyright (C)  2003  Zack Rusin <zack@kde.org>
 *                2010  Ruslan Nigmatullin <euroelessar@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include "aspellchecker.h"
#include <qutim/systeminfo.h>
#include <aspell.h>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>

ASpellChecker::ASpellChecker()
{
//	QString lang = "ru_RU";
	m_config = new_aspell_config();
//	aspell_config_replace( m_config, "lang", lang.toLatin1() );
	/* All communication with Aspell is done in UTF-8 */
	/* For reference, please look at BR#87250         */
	aspell_config_replace( m_config, "encoding", "utf-8" );

#ifdef Q_WS_WIN
	QByteArray dataDir = SystemInfo::getPath(SystemInfo::SystemShareDir).toLocal8Bit();
	aspell_config_replace( m_config, "data-dir", dataDir.constData());
	aspell_config_replace( m_config, "dict-dir", dataDir.constData());
#endif

	AspellCanHaveError * possible_err = new_aspell_speller( m_config );

	if ( aspell_error_number( possible_err ) != 0 )
		warning()<< "Error : "<< aspell_error_message( possible_err );
	else
		m_speller = to_aspell_speller( possible_err );
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
//
//	QStringList langs;
//	QRegExp regexp("(\\w+(_\\w+)?)");
//	foreach (QString lang, languages()) {
//		lang.indexOf(regexp);
//		qDebug() << lang << QLocale(lang).name() << regexp.cap(0);
//		langs << regexp.cap(0);
//	}
//	langs.removeDuplicates();
//	qDebug() << langs;
//    struct variantListType
//    {
//        const char* variantShortName;
//        const char* variantEnglishName;
//    };
//
//    const variantListType variantList[] = {
//        { "40", I18N_NOOP2("dictionary variant", "40") }, // what does 40 mean?
//        { "60", I18N_NOOP2("dictionary variant", "60") }, // what does 60 mean?
//        { "80", I18N_NOOP2("dictionary variant", "80") }, // what does 80 mean?
//        { "ise", I18N_NOOP2("dictionary variant", "-ise suffixes") },
//        { "ize", I18N_NOOP2("dictionary variant", "-ize suffixes") },
//        { "ise-w_accents", I18N_NOOP2("dictionary variant", "-ise suffixes and with accents") },
//        { "ise-wo_accents", I18N_NOOP2("dictionary variant", "-ise suffixes and without accents") },
//        { "ize-w_accents", I18N_NOOP2("dictionary variant", "-ize suffixes and with accents") },
//        { "ize-wo_accents", I18N_NOOP2("dictionary variant", "-ize suffixes and without accents") },
//        { "lrg", I18N_NOOP2("dictionary variant", "large") },
//        { "med", I18N_NOOP2("dictionary variant", "medium") },
//        { "sml", I18N_NOOP2("dictionary variant", "small") },
//        { "variant_0", I18N_NOOP2("dictionary variant", "variant 0") },
//        { "variant_1", I18N_NOOP2("dictionary variant", "variant 1") },
//        { "variant_2", I18N_NOOP2("dictionary variant", "variant 2") },
//        { "wo_accents", I18N_NOOP2("dictionary variant", "without accents") },
//        { "w_accents", I18N_NOOP2("dictionary variant", "with accents") },
//        { "ye", I18N_NOOP2("dictionary variant", "with ye") },
//        { "yeyo", I18N_NOOP2("dictionary variant", "with yeyo") },
//        { "yo", I18N_NOOP2("dictionary variant", "with yo") },
//        { "extended", I18N_NOOP2("dictionary variant", "extended") },
//        { 0, 0 }
//    };
}

ASpellChecker::~ASpellChecker()
{
	delete_aspell_speller( m_speller );
	delete_aspell_config( m_config );
}

bool ASpellChecker::isCorrect(const QString &word) const
{
	/* ASpell is expecting length of a string in char representation */
	/* word.length() != word.toUtf8().length() for nonlatin strings    */
	if (!m_speller)
		return false;
	QByteArray data = word.toUtf8();
	int correct = aspell_speller_check( m_speller, data, data.length() );
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

	while ( (cword = aspell_string_enumeration_next( elements )) ) {
		/* Since while creating the class ASpellDict the encoding is set */
		/* to utf-8, one has to convert output from Aspell to Unicode    */
		qsug.append(QString::fromUtf8(cword));
	}

	delete_aspell_string_enumeration( elements );
	return qsug;
}

QStringList ASpellChecker::languages() const
{
	AspellDictInfoList *l = get_aspell_dict_info_list( m_config );
	AspellDictInfoEnumeration *el = aspell_dict_info_list_elements( l );

	QStringList langs;
	const AspellDictInfo *di = 0;
	while ( ( di = aspell_dict_info_enumeration_next( el ) ) ) {
		langs.append( di->name );
	}

	delete_aspell_dict_info_enumeration( el );

	return langs;
}

void ASpellChecker::loadSettings()
{
}

void ASpellChecker::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	new ASpell::SpellHighlighter(this, session->getInputField());
}
