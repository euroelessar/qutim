/****************************************************************************
 *  metainfo.h
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

#ifndef METAINFO_H_
#define METAINFO_H_

#include "snachandler.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;
class IcqContact;
class AbstractMetaInfoRequestPrivate;
class ShortInfoMetaRequestPrivate;
class FullInfoMetaRequestPrivate;

struct Category
{
	QString category;
	QString keyword;
};
typedef QList<Category> CategoryList;
typedef QHash<quint16, QString> FieldNamesList;

class LIBOSCAR_EXPORT Gender
{
public:
	enum Enum
	{
		Unknown,
		Female = 1,
		Male = 2,
		Female2 = 'F',
		Male2 = 'M'
	};
	Gender(int value = Unknown) :
		m_value(static_cast<Enum>(value))
	{}
	Enum value() { return m_value; }
	bool operator==(int value) { return value == m_value; }
	bool operator!=(int value) { return value != m_value; }
	Gender &operator=(int value) { m_value = static_cast<Enum>(value); return *this; }
	Gender &operator=(const Gender &gender) { m_value = gender.m_value; return *this; }
	QString toString();
	operator QString() { return toString(); }
private:
	Enum m_value;
};

class LIBOSCAR_EXPORT AbstractMetaInfoRequest : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(AbstractMetaInfoRequest)
public:
	AbstractMetaInfoRequest();
	AbstractMetaInfoRequest(const AbstractMetaInfoRequest &request);
	virtual ~AbstractMetaInfoRequest();
	quint16 id() const;
	IcqAccount *account() const;
	bool isDone() const;
signals:
	void done(bool ok);
public slots:
	virtual void send() const = 0;
	void cancel();
protected:
	friend class MetaInfo;
	AbstractMetaInfoRequest(IcqAccount *account, AbstractMetaInfoRequestPrivate *d);
	virtual bool handleData(quint16 type, const DataUnit &data) = 0;
	void sendRequest(quint16 type, const DataUnit &data) const;
	void close(bool ok);
protected:
	QScopedPointer<AbstractMetaInfoRequestPrivate> d_ptr;
};

class LIBOSCAR_EXPORT ShortInfoMetaRequest : public AbstractMetaInfoRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ShortInfoMetaRequest)
public:
	ShortInfoMetaRequest(IcqAccount *account, IcqContact *contact = 0);
	QVariantHash values() const;
	QVariant value(const QString &value, const QVariant &defaultValue = QVariant()) const;
	template <typename T>
	T value(const QString &value, const T &defaultValue = T());
	virtual void send() const;
protected:
	ShortInfoMetaRequest();
	virtual bool handleData(quint16 type, const DataUnit &data);
};

class LIBOSCAR_EXPORT FullInfoMetaRequest : public ShortInfoMetaRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FullInfoMetaRequest)
public:
	enum State {
		BasicInfo = 0x00c8,
		MoreInfo = 0x00dc,
		Emails = 0x00eb,
		Homepage = 0x010e,
		Work = 0x00d2,
		Notes = 0x00e6,
		Interests = 0x00f0,
		Affilations = 0x00fa
	};
	FullInfoMetaRequest(IcqAccount *account, IcqContact *contact = 0);
	virtual void send() const;
signals:
	void infoUpdated(State state);
protected:
	FullInfoMetaRequest();
	virtual bool handleData(quint16 type, const DataUnit &data);
};

template <typename T>
T ShortInfoMetaRequest::value(const QString &val, const T &defaultValue) {
	QVariant res = value(val);
	if (!res.isValid())
		return defaultValue;
	return res.value<T>();
}

QDebug operator<<(QDebug dbg, const Category &cat);

} } // namespace qutim_sdk_0_3::oscar

Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::Gender);
Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::Category);
Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::CategoryList);

#endif /* METAINFO_H_ */
