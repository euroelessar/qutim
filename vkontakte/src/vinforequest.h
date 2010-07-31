/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef VINFOREQUEST_H
#define VINFOREQUEST_H

#include <qutim/inforequest.h>

using namespace qutim_sdk_0_3;

class VConnection;

class VInfoRequest : public InfoRequest
{
	Q_OBJECT
public:
	enum DataType
	{
		NickName,
		FirstName,
		LastName,
		Sex,
		BDate,
		City,
		Country,
		Photo,
		HomePhone,
		MobilePhone,
		University,
		Faculty,
		Graduation
	};

    VInfoRequest(QObject *parent);
	
	virtual DataItem item(const QString &name = QString()) const;
	virtual State state() const;
private slots:
	void onRequestFinished();
	void onAddressEnsured();
private:
	void ensureAddress(DataType type);
	void addItem(DataType type, DataItem &group, const QVariant &data) const;
	inline void addItem(DataType type, DataItem &group, const char *name) const
	{ addItem(type, group, m_data.value(QLatin1String(name))); }
	
	VConnection *m_connection;
	State m_state;
	int m_unknownCount;
	mutable DataItem *m_item;
	mutable QMap<QString, DataItem> m_items;
	QVariantMap m_data;
};

#endif // VINFOREQUEST_H
