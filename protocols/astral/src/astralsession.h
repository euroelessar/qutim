/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef ASTRALSESSION_H
#define ASTRALSESSION_H

#include <qutim/chatunit.h>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/Message>

using namespace qutim_sdk_0_3;
using namespace Tp;

struct AstralSessionPrivate;
class AstralAccount;

class AstralSession : public ChatUnit
{
	Q_OBJECT
public:
	AstralSession(const TextChannelPtr &channel, AstralAccount *acc);
	virtual ~AstralSession();
	virtual QString id() const;
	virtual bool sendMessage(const qutim_sdk_0_3::Message &message);
private slots:
	void onMessageReceived(const Tp::ReceivedMessage &message);
	void onMessageSent(Tp::PendingOperation *operation);
private:
	QScopedPointer<AstralSessionPrivate> p;
};

#endif // ASTRALSESSION_H

