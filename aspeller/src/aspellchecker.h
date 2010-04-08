/****************************************************************************
 * aspellchecker.h
 *
 *  Copyright (c) 2010 by Ruslan Nigmatullin <euroelessar@gmail.com>
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

#ifndef ASPELLCHECKER_H
#define ASPELLCHECKER_H

#include <qutim/messagesession.h>
#include "spellhighlighter.h"

#include <QObject>

struct AspellConfig;
struct AspellSpeller;

using namespace qutim_sdk_0_3;

class ASpellChecker : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "SpellChecker")
	Q_CLASSINFO("Uses", "ChatLayer")
public:
	explicit ASpellChecker();
	virtual ~ASpellChecker();
	QStringList languages() const;
public slots:
	bool isCorrect(const QString &word) const;
	bool isMisspelled(const QString &word) const;
	QStringList suggest(const QString &word) const;
protected slots:
	void loadSettings();
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
private:
	AspellConfig  *m_config;
	AspellSpeller *m_speller;
};

#endif // ASPELLCHECKER_H
