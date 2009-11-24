/****************************************************************************
 *  metainfo.h
 *
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef METAINFO_H_
#define METAINFO_H_

#include "snachandler.h"

namespace Icq {

class OscarConnection;

class MetaInfo: public SNACHandler
{
	Q_OBJECT
public:
	struct Category
	{
		quint16 category;
		QString keyword;
	};

	MetaInfo(QObject *parent = 0);
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	void sendShortInfoRequest(OscarConnection *conn, const QString &uin);
	void sendFullInfoRequest(OscarConnection *conn, const QString &uin);
private:
	void sendRequest(OscarConnection *conn, quint16 type, const DataUnit &data);
	void handleShortInfo(const DataUnit &data);
	void handleBasicInfo(const DataUnit &data);
	void handleMoreInfo(const DataUnit &data);
	void handleEmails(const DataUnit &data);
	void handleHomepage(const DataUnit &data);
	void handleWork(const DataUnit &data);
	void handleNotes(const DataUnit &data);
	void handleInterests(const DataUnit &data);
	void handleAffilations(const DataUnit &data);
	QList<Category> handleCatagories(const DataUnit &data, QString &debug_str);
	QString readString(const DataUnit &data);
	quint16 m_sequence;
};

} //namespace Icq

#endif /* METAINFO_H_ */
