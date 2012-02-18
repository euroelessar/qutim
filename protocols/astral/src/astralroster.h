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
#ifndef ASTRALROSTER_H
#define ASTRALROSTER_H

#include "astralcontact.h"

struct AstralRosterPrivate;

class AstralRoster : public QObject
{
	Q_OBJECT
public:
	AstralRoster(AstralAccount *acc, ConnectionPtr conn);
	virtual ~AstralRoster();
	AstralContact *contact(const QString &id);
	AstralContact *contact(const ContactPtr &ptr);

private slots:
	void onConnectionReady(Tp::PendingOperation *op);
//	void onPresencePublicationRequested(const Tp::Contacts &);
//	void onItemSelectionChanged();
//	void onAddButtonClicked();
//	void onAuthActionTriggered(bool);
//	void onDenyActionTriggered(bool);
//	void onRemoveActionTriggered(bool);
//	void onBlockActionTriggered(bool);
	void onContactRetrieved(Tp::PendingOperation *op);
//	void updateActions();
private:
	QScopedPointer<AstralRosterPrivate> p;
};

#endif // ASTRALROSTER_H

