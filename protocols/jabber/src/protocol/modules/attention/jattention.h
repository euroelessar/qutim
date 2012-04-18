/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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
#ifndef JATTENTION_H
#define JATTENTION_H

#include <QHash>

#include <qutim/actiongenerator.h>
#include <qutim/localizedstring.h>
#include "../../../sdk/jabber.h"
#include <jreen/client.h>

namespace qutim_sdk_0_3 {
class Account;
}

namespace Jabber {

class JAttention : public QObject, public JabberExtension
{
	Q_OBJECT
	Q_INTERFACES(Jabber::JabberExtension)
public:
	JAttention();
	virtual void init(qutim_sdk_0_3::Account *account);
private:
	qutim_sdk_0_3::Account *m_account;
	QScopedPointer<qutim_sdk_0_3::ActionGenerator> m_sendAttentionActionGenerator;
private slots:
	void onAttentionClicked(QObject *obj);
	void onAttentionDone(bool success);
	void onMessageReceived(const Jreen::Message &message);
};

class SendAttentionActionGenerator : public qutim_sdk_0_3::ActionGenerator
{
public:
	SendAttentionActionGenerator(QObject *obj, const char *slot);
	void createImpl(QAction *action, QObject *obj) const;
	void showImpl(QAction *action, QObject *obj);
};

} // namespace Jabber
#endif // JATTENTION_H
