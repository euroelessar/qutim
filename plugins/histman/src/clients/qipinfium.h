/****************************************************************************
 *  qipinfium.h
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
