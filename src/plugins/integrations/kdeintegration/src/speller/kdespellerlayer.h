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

#ifndef KDESPELLERLAYER_H
#define KDESPELLERLAYER_H

#include <QObject>
#include <qutim/chatsession.h>
#include <sonnet/speller.h>
#include <qutim/spellchecker.h>

using namespace qutim_sdk_0_3;
using namespace Sonnet;

class KdeSpellerLayer : public SpellChecker
{
	Q_OBJECT
	Q_CLASSINFO("Service", "SpellChecker")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "SettingsLayer")
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
	SettingsItem *m_settingsItem;
	bool m_autodetect;
	QString m_dictionary;
};

#endif // KDESPELLERLAYER_H

