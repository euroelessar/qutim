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

#ifndef QIPINFIUM_H_
#define QIPINFIUM_H_
#include <QHash>
#include "../../include/qutim/historymanager.h"

namespace HistoryManager {

class qipinfium : public HistoryImporter
{
public:
	qipinfium();
	virtual ~qipinfium();
	static quint8 getUInt8(const uchar * &data);
	static quint16 getUInt16(const uchar * &data);
	static quint32 getUInt32(const uchar * &data);
	static QString getString(const uchar * &data, int len, bool crypt = false);
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
	virtual QList<ConfigWidget> config();
	virtual bool useConfig();
	virtual QString additionalInfo();
private:
	QList<ConfigWidget> m_config_list;
	QHash<QString, QString> m_accounts;
};

}

#endif /*QIPINFIUM_H_*/

