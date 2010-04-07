/****************************************************************************
 *  andrq.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef ANDRQ_H_
#define ANDRQ_H_
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include "include/qutim/historymanager.h"

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
