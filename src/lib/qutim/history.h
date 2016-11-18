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

#ifndef HISTORY_H
#define HISTORY_H

#include "message.h"
#include "asyncresult.h"
#include <QDateTime>

namespace qutim_sdk_0_3
{
	class ChatUnit;

	/**
	 * @brief The History class is base class to implement history storage plugins
	 */
	class LIBQUTIM_EXPORT History : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("Service", "History")
	public:
		virtual ~History();

		static History *instance();

		struct LIBQUTIM_EXPORT AccountInfo
		{
			QString protocol;
			QString account;

			bool operator ==(const AccountInfo &other) const;
			bool operator <(const AccountInfo &other) const;
		};

		struct LIBQUTIM_EXPORT ContactInfo : AccountInfo
		{
			QString contact;

			bool operator ==(const ContactInfo &other) const;
			bool operator <(const ContactInfo &other) const;
		};

		virtual void store(const Message &message) = 0;
		/**
		 * Read messages from history.
		 * \warning \a from or/and \a to parameter can be invalid, which means that
		 * you should ignore that parameter
		 */
		virtual AsyncResult<MessageList> read(const ContactInfo &contact, const QDateTime &from, const QDateTime &to, int max_num) = 0;
		virtual AsyncResult<QVector<AccountInfo>> accounts() = 0;
		virtual AsyncResult<QVector<ContactInfo>> contacts(const AccountInfo &account) = 0;
		/**
		 * Returns list of date(year, month). If search string is presented, you may return list of months where
		 * at least message contains search string.
		 */
		virtual AsyncResult<QList<QDate>> months(const ContactInfo &contact, const QString &needle) = 0;
		/**
		 * Returns list of days in month of certain year. If search string is not empty, you should return list of days
		 * where at least one message with search string is presented
		 */
		virtual AsyncResult<QList<QDate>> dates(const ContactInfo &contact, const QDate &month, const QString &needle) = 0;

		AsyncResult<MessageList> read(const ChatUnit *unit, const QDateTime &to, int max_num);
		AsyncResult<MessageList> read(const ChatUnit *unit, int max_num);

		MessageList readSync(const ChatUnit *unit, const QDateTime &to, int max_num);
		MessageList readSync(const ChatUnit *unit, int max_num);

		static ContactInfo info(const ChatUnit *unit);

	protected:
		History();

		virtual void virtual_hook(int id, void *data);
	};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::History::AccountInfo)
Q_DECLARE_METATYPE(qutim_sdk_0_3::History::ContactInfo)

#endif // HISTORY_H

