/****************************************************************************
 * kdespellerlayer.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "kdespellerlayer.h"
#include "kdespellersettings.h"
#include <qutim/configbase.h>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

Q_GLOBAL_STATIC(Speller, speller)

KdeSpellerLayer::KdeSpellerLayer()
{
	Settings::registerItem(new GeneralSettingsItem<KdeSpellerSettings>(
			Settings::General,
			KIcon("tools-check-spelling"),
			QT_TRANSLATE_NOOP("Settings", "Spell checker")));
	loadSettings();
}

KdeSpellerLayer::~KdeSpellerLayer()
{
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
