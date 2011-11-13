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

#ifndef ANDRQ_H_
#define ANDRQ_H_
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include "../../include/qutim/historymanager.h"

namespace HistoryManager {

class andrq : public HistoryImporter
{
public:
	andrq();
	virtual ~andrq();
	static QString getString(QDataStream &in, int key);
	static bool isValidUtf8(const QByteArray &array);
	static QDateTime getDateTime(QDataStream &in);
	virtual void loadMessages(const QString &path);
	virtual bool validate(const QString &path);
	virtual QString name();
	virtual QIcon icon();
};

}

#endif /*ANDRQ_H_*/

