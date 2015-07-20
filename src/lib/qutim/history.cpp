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

#include "history.h"
#include "objectgenerator.h"
#include "buddy.h"
#include "metacontact.h"
#include "account.h"
#include "protocol.h"
#include "servicemanager.h"
#include <QEventLoop>
#include <QTimer>
#include <tuple>
#include <QDebug>

namespace qutim_sdk_0_3
{
	class NullHistory : public History
	{
	public:
		void store(const Message &) override
		{
		}

		AsyncResult<MessageList> read(const ContactInfo &, const QDateTime &, const QDateTime &, int) override
		{
			return AsyncResult<MessageList>::create(MessageList());
		}

		void showHistory(const ChatUnit *) override
		{
		}

		AsyncResult<QVector<AccountInfo>> accounts() override
		{
			return AsyncResult<QVector<AccountInfo>>::create(QVector<AccountInfo>());
		}

		AsyncResult<QVector<ContactInfo>> contacts(const AccountInfo &) override
		{
			return AsyncResult<QVector<ContactInfo>>::create(QVector<ContactInfo>());
		}

		AsyncResult<QList<QDate>> months(const ContactInfo &, const QRegularExpression &) override
		{
			return AsyncResult<QList<QDate>>::create(QList<QDate>());
		}

		AsyncResult<QList<QDate>> dates(const ContactInfo &, const QDate &, const QRegularExpression &) override
		{
			return AsyncResult<QList<QDate>>::create(QList<QDate>());
		}
	};

	struct Private
	{
		ServicePointer<History> service;
		NullHistory null;
	};

	Q_GLOBAL_STATIC(Private, self)

	bool History::AccountInfo::operator ==(const History::AccountInfo &other) const
	{
		return protocol == other.protocol &&
				account == other.account;
	}

	bool History::AccountInfo::operator <(const History::AccountInfo &other) const
	{
		return std::tie(protocol, account)
				< std::tie(other.protocol, other.account);
	}

	bool History::ContactInfo::operator ==(const History::ContactInfo &other) const
	{
		return AccountInfo::operator ==(other) &&
				contact == other.contact;
	}

	bool History::ContactInfo::operator <(const History::ContactInfo &other) const
	{
		return std::tie(protocol, account, contact)
				< std::tie(other.protocol, other.account, other.contact);
	}

	History::History()
	{
		QMetaType::registerComparators<AccountInfo>();
		QMetaType::registerComparators<ContactInfo>();
	}

	History::~History()
	{
	}

	History *History::instance()
	{
		auto p = self();
		return p->service ? p->service.data() : &p->null;
	}

	AsyncResult<MessageList> History::read(const ChatUnit *unit, const QDateTime &to, int max_num)
	{
		return read(info(unit), QDateTime(), to, max_num);
	}

	AsyncResult<MessageList> History::read(const ChatUnit *unit, int max_num)
	{
		return read(info(unit), QDateTime(), QDateTime::currentDateTime(), max_num);
	}

	MessageList History::readSync(const ChatUnit *unit, const QDateTime &to, int max_num) {
		AsyncResult<MessageList> asyncResult = read(unit, to, max_num);
		MessageList result;

		QEventLoop loop;

		QTimer::singleShot(0, this, [this, &loop, &asyncResult, &result] () {
			asyncResult.connect(this, [this, &loop, &result] (const MessageList &messages) {
				result = messages;
				loop.quit();
			});
		});

		loop.exec();

		return result;
	}

	MessageList History::readSync(const ChatUnit *unit, int max_num)
	{
		return readSync(unit, QDateTime::currentDateTime(), max_num);
	}

	History::ContactInfo History::info(const ChatUnit *unit)
	{
		unit = unit->getHistoryUnit();
		const Account *account = unit->account();
		const Protocol *protocol = account->protocol();

		ContactInfo info;
		info.protocol = protocol->id();
		info.account = account->id();
		info.contact = unit->id();
		return info;
	}

	void History::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}
}

