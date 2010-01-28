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

namespace Icq
{

class OscarConnection;
class IcqAccount;

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
	void sendShortInfoRequest(OscarConnection *conn, QObject *reqObject);
	void sendFullInfoRequest(OscarConnection *conn, QObject *reqObject);
private:
	void sendRequest(OscarConnection *conn, QObject *reqObject, quint16 type, const DataUnit &data);
	void sendInfoRequest(OscarConnection *conn, QObject *reqObject, quint16 type);
	void handleShortInfo(QObject *reqObject, const DataUnit &data);
	void handleBasicInfo(QObject *reqObject, const DataUnit &data);
	void handleMoreInfo(QObject *reqObject, const DataUnit &data);
	void handleEmails(QObject *reqObject, const DataUnit &data);
	void handleHomepage(QObject *reqObject, const DataUnit &data);
	void handleWork(QObject *reqObject, const DataUnit &data);
	void handleNotes(QObject *reqObject, const DataUnit &data);
	void handleInterests(QObject *reqObject, const DataUnit &data);
	void handleAffilations(QObject *reqObject, const DataUnit &data);
	QList<Category> handleCatagories(const DataUnit &data);
	QString readString(const DataUnit &data);
	quint16 m_sequence;
	QHash<quint16, QObject*> m_requests;
};

} //namespace Icq

#endif /* METAINFO_H_ */
