/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "kdespellerlayer.h"
#include "kdespellersettings.h"
#include <qutim/configbase.h>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

Q_GLOBAL_STATIC(Speller, speller)

KdeSpellerLayer::KdeSpellerLayer()
{
	m_settingsItem = new GeneralSettingsItem<KdeSpellerSettings>(
	            Settings::General,
				KIcon("tools-check-spelling"),
				QT_TRANSLATE_NOOP("Settings", "Spell checker"));
	Settings::registerItem(m_settingsItem);
	loadSettings();
}

KdeSpellerLayer::~KdeSpellerLayer()
{
	Settings::removeItem(m_settingsItem);
}

Speller *KdeSpellerLayer::spellerInstance()
{
	return speller();
}

void KdeSpellerLayer::loadSettings()
{
	ConfigGroup group = Config().group("speller");
	m_autodetect = group.value("autodetect", false);
	QString lang = KdeSpellerSettings::suggestLanguage(group.value("language", QString()), speller());
	if (!lang.isEmpty())
		m_dictionary = lang;
	else if (!speller()->availableDictionaries().isEmpty())
		m_dictionary = speller()->availableDictionaries().begin().value();
	speller()->setLanguage(m_dictionary);
	emit dictionaryChanged();
}

bool KdeSpellerLayer::isCorrect(const QString &word) const
{
	return speller()->isCorrect(word);
}

bool KdeSpellerLayer::isMisspelled(const QString &word) const
{
	return speller()->isMisspelled(word);
}

QStringList KdeSpellerLayer::suggest(const QString &word) const
{
	return speller()->suggest(word);
}

void KdeSpellerLayer::store(const QString &word) const
{
	speller()->addToPersonal(word);
}

void KdeSpellerLayer::storeReplacement(const QString &bad, const QString &good)
{
	speller()->storeReplacement(bad, good);
}

