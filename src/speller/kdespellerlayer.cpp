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
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
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
}

void KdeSpellerLayer::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	if (QTextDocument *doc = session->getInputField())
		new Kde::SpellHighlighter(speller(), doc);
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

