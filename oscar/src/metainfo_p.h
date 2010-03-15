/****************************************************************************
 *  metainfo_p.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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


#ifndef METAINFO_P_H
#define METAINFO_P_H

#include "metainfo.h"
#include "icqaccount.h"
#include <QTimer>

namespace qutim_sdk_0_3 {

namespace oscar {

class AbstractMetaInfoRequestPrivate : public QSharedData
{
public:
	quint16 id;
	IcqAccount *account;
	bool ok;
	mutable QTimer timer;
};

class ShortInfoMetaRequestPrivate : public AbstractMetaInfoRequestPrivate
{
public:
	MetaInfoValuesHash values;
	quint32 uin;
	inline void readString(MetaInfoFieldEnum value, const DataUnit &data);
	inline void readFlag(MetaInfoFieldEnum value, const DataUnit &data);
	void dump();
};

class FullInfoMetaRequestPrivate : public ShortInfoMetaRequestPrivate
{
public:
	template <typename T>
	void readField(MetaInfoFieldEnum value, const DataUnit &data, FieldNamesList *list);
	void readCatagories(MetaInfoFieldEnum value, const DataUnit &data, FieldNamesList *list);
	void handleBasicInfo(const DataUnit &data);
	void handleMoreInfo(const DataUnit &data);
	void handleEmails(const DataUnit &data);
	void handleHomepage(const DataUnit &data);
	void handleWork(const DataUnit &data);
};

class FindContactsMetaRequestPrivate : public AbstractMetaInfoRequestPrivate
{
public:
	void addString(quint16 id, MetaInfoFieldEnum value, DataUnit &data, bool test = true) const;
	void addString(const QString &str, DataUnit &data) const;
	template <typename T>
	void addField(quint16 id, MetaInfoFieldEnum value, DataUnit &data, bool test = true) const;
	template <typename T>
	void addCategoryId(quint16 id, MetaInfoFieldEnum value, DataUnit &data, FieldNamesList *list) const;
	void addCategory(quint16 id, MetaInfoFieldEnum value, DataUnit &data, FieldNamesList *list) const;
	MetaInfoValuesHash values;
	QHash<QString, FindContactsMetaRequest::FoundContact> contacts;
};

class MetaInfo: public QObject, public SNACHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler)
public:
	MetaInfo();
	static MetaInfo &instance() { Q_ASSERT(self); return *self; }
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	void addRequest(AbstractMetaInfoRequest *request);
	bool removeRequest(AbstractMetaInfoRequest *request);
	quint16 nextId() { return ++m_sequence; }
private slots:
	void onNewAccount(qutim_sdk_0_3::Account *account);
	void onAccountStatusChanged(const qutim_sdk_0_3::Status &status);
private:
	quint16 m_sequence;
	QHash<quint16, AbstractMetaInfoRequest*> m_requests;
	static MetaInfo *self;
};

template <typename T>
void FullInfoMetaRequestPrivate::readField(MetaInfoFieldEnum value, const DataUnit &data, FieldNamesList *list)
{
	QString str = list->value(data.read<T>(LittleEndian));
	if (!str.isEmpty())
		values.insert(value, str);
}

} } // namespace qutim_sdk_0_3::oscar

#endif // METAINFO_P_H
