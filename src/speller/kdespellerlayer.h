/****************************************************************************
 * kdespellerlayer.h
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

#ifndef KDESPELLERLAYER_H
#define KDESPELLERLAYER_H

#include <QObject>
#include <qutim/messagesession.h>
#include <sonnet/speller.h>
#include <qutim/messagesession.h>
#include <qutim/spellchecker.h>

using namespace qutim_sdk_0_3;
using namespace Sonnet;

class KdeSpellerLayer : public SpellChecker
{
	Q_OBJECT
	Q_CLASSINFO("Service", "SpellChecker")
	Q_CLASSINFO("Uses", "ChatLayer")
public:
	KdeSpellerLayer();
	~KdeSpellerLayer();
	static Speller *spellerInstance();
	bool isCorrect(const QString &word) const;
	bool isMisspelled(const QString &word) const;
	QStringList suggest(const QString &word) const;
	void store(const QString &word) const;
	void storeReplacement(const QString &bad, const QString &good);
protected slots:
	void loadSettings();
private:
	bool m_autodetect;
	QString m_dictionary;
};

#endif // KDESPELLERLAYER_H
