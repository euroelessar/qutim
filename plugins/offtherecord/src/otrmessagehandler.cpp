/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "otrmessagehandler.h"
#include "otrcrypt.h"
#include <qutim/contact.h>
#include <QSystemSemaphore>

using namespace qutim_sdk_0_3;

MessageHandler::Result OtrMessagePreHandler::doHandle(Message &message, QString *reason)
{
	Q_UNUSED(reason);
	if (message.property("service", false) || message.property("history", false))
		return Accept;
	if (message.isIncoming())
		decrypt(message);
	else
		encrypt(message);
	return Accept;
}

void OtrMessagePreHandler::encrypt(Message &message)
{
	ChatUnit *unit = const_cast<ChatUnit*>(message.chatUnit()->getHistoryUnit());
	if (!qobject_cast<Contact*>(unit))
		return;
	TreeModelItem item = unit;
	OtrClosure *closure = OTRCrypt::instance()->ensureClosure(unit);
    QString encrypted = closure->getMessaging()->encryptMessage(
            unit->account()->id(),
            unit->id(),
            message.text(),
            item);
	if (encrypted != message.text()) {
		message.setProperty("__otr__text", message.text());
		message.setProperty("__otr__html", message.html());
		message.setText(encrypted);
		message.setHtml(QString());
	}
}

void OtrMessagePreHandler::decrypt(Message &message)
{
	ChatUnit *unit = const_cast<ChatUnit*>(message.chatUnit()->getHistoryUnit());
	if (!qobject_cast<Contact*>(unit))
		return;
	TreeModelItem item = unit;
    if (OTRCrypt::instance()->isEnabledAccount(unit->account())) {
		OtrClosure *closure = OTRCrypt::instance()->ensureClosure(unit);
		QString decrypted = closure->getMessaging()->decryptMessage(
					            unit->id(),
		                        unit->account()->id(),
		                        message.text(),
		                        item);
		if (message.text() != decrypted) {
			message.setText(decrypted);
			message.setHtml(QString());
		}
    }
}

MessageHandler::Result OtrMessagePostHandler::doHandle(Message &message, QString *reason)
{
	Q_UNUSED(reason);
	if (message.isIncoming()) {
		if (message.text().startsWith(QLatin1String("<Internal OTR message>\n"))) {
			message.setText(message.text().section(QLatin1Char('\n'), 1));
			message.setProperty("hide", true);
			message.setProperty("store", false);
			return Accept;
		}
	} else {
		if (message.property("service", false) || message.property("history", false))
			return Accept;
		QString text = message.property("__otr__text", QString());
		QString html = message.property("__otr__html", QString());
		if (!text.isEmpty()) {
			message.setText(text);
			message.setHtml(html);
			message.setProperty("__otr__text", QVariant());
			message.setProperty("__otr__html", QVariant());
		}
	}
	return Accept;
}

