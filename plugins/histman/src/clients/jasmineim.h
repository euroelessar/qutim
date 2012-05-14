/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 DeKaN (DeKaNszn) <dekanszn@gmail.com>
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

#ifndef JASMINEIM_H
#define JASMINEIM_H
#include "../../include/qutim/historymanager.h"
#include <QTextCodec>

namespace HistoryManager {

#define SIGNATURE "JPHA"

struct ContactHistory{
	QString uin;
	QList<qutim_sdk_0_3::Message> history;
};

class jasmineim : public HistoryImporter
{
public:
	jasmineim();
	virtual ~jasmineim();
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
	virtual bool chooseFile() { return true; }
	static QString ReadMUTF8String(QDataStream &in);
	static QString ReadWin1251String(QDataStream &in);
	friend QDataStream &operator >> (QDataStream &in, ContactHistory &contact);
	friend QDataStream &operator >> (QDataStream &in, qutim_sdk_0_3::Message &message);
private:
	static QTextCodec *codec;
};

}

#endif // JASMINEIM_H
