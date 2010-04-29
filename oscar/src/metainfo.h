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
#include <QHash>

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;
class IcqContact;
class AbstractMetaInfoRequestPrivate;
class ShortInfoMetaRequestPrivate;
class FullInfoMetaRequestPrivate;
class TlvBasedMetaInfoRequestPrivate;
class UpdateAccountInfoMetaRequestPrivate;
class FindContactsMetaRequestPrivate;

typedef QPair<quint16, quint16> AgeRange;
struct Category
{
	QString category;
	QString keyword;
};
typedef QList<Category> CategoryList;
typedef QHash<quint16, QString> FieldNamesList;

enum MetaInfoFieldEnum {
	Uin,
	// Basic info
	Nick,
	FirstMetaField = Nick,
	FirstName,
	LastName,
	Email,
	HomeCity,
	HomeState,
	HomePhone,
	HomeFax,
	HomeAddress,
	CellPhone,
	HomeZipCode,
	HomeCountry,
	GMT,
	AuthFlag,
	WebawareFlag,
	DirectConnectionFlag,
	PublishPrimaryEmailFlag,
	// More info
	Age,
	Gender,
	Homepage,
	Birthday,
	Languages,
	OriginalCity,
	OriginalState,
	OriginalCountry,
	// Work info
	WorkCity,
	WorkState,
	WorkPhone,
	WorkFax,
	WorkAddress,
	WorkZip,
	WorkCountry,
	WorkCompany,
	WorkDepartment,
	WorkPosition,
	WorkOccupation,
	WorkWebpage,
	// Other
	Emails,
	Notes,
	Interests,
	Pasts,
	Affilations,
	LastMetaField = Affilations,

	Ages,
	Whitepages,
	OnlineFlag
};

class LIBOSCAR_EXPORT MetaInfoField
{
public:
	MetaInfoField(const QString &name);
	MetaInfoField(int value);
	MetaInfoField(const MetaInfoField &field);
	QString name() const;
	LocalizedString group() const;
	QList<LocalizedString> titleAlternatives() const;
	QList<LocalizedString> alternatives() const;
	QVariant defaultValue() const;
	MetaInfoFieldEnum value() const { return m_value; };
	QString toString() const;
	operator QString() const { return toString(); }
	MetaInfoField &operator=(int value) { m_value = static_cast<MetaInfoFieldEnum>(value); return *this; }
	bool operator==(int value) const { return m_value == value; }
	bool operator!=(int value) const { return m_value != value; }
	bool operator==(const MetaInfoField &value) const { return m_value == value.m_value; }
	bool operator!=(const MetaInfoField &value) const { return m_value != value.m_value; }
private:
	mutable QString m_name;
	MetaInfoFieldEnum m_value;
};
typedef QHash<MetaInfoField, QVariant> MetaInfoValuesHash;

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
	void setTimeout(int msec);
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
	MetaInfoValuesHash values() const;
	QVariant value(MetaInfoField value, const QVariant &defaultValue = QVariant()) const;
	template <typename T>
	T value(MetaInfoField value, const T &defaultValue = T());
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
		StateBasicInfo = 0x00c8,
		StateMoreInfo = 0x00dc,
		StateEmails = 0x00eb,
		StateHomepage = 0x010e,
		StateWork = 0x00d2,
		StateNotes = 0x00e6,
		StateInterests = 0x00f0,
		StateAffilations = 0x00fa
	};
	FullInfoMetaRequest(IcqAccount *account, IcqContact *contact = 0);
	virtual void send() const;
signals:
	void infoUpdated(State state);
protected:
	FullInfoMetaRequest();
	virtual bool handleData(quint16 type, const DataUnit &data);
};

class LIBOSCAR_EXPORT TlvBasedMetaInfoRequest : public AbstractMetaInfoRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(TlvBasedMetaInfoRequest)
public:
	void setValue(const MetaInfoField &field, const QVariant &value);
protected:
	TlvBasedMetaInfoRequest(IcqAccount *account, TlvBasedMetaInfoRequestPrivate *d);
	void sendTlvBasedRequest(quint16 type) const;
};

class LIBOSCAR_EXPORT UpdateAccountInfoMetaRequest : public TlvBasedMetaInfoRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(UpdateAccountInfoMetaRequest)
public:
	UpdateAccountInfoMetaRequest(IcqAccount *account, const MetaInfoValuesHash &values = MetaInfoValuesHash());
	void setValue(MetaInfoField data, const QVariant &value);
	virtual void send() const;
signals:
	void infoUpdated();
protected:
	virtual bool handleData(quint16 type, const DataUnit &data);
};

class LIBOSCAR_EXPORT FindContactsMetaRequest : public TlvBasedMetaInfoRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FindContactsMetaRequest)
public:
	struct FoundContact
	{
		QString uin;
		QString nick;
		QString firstName;
		QString lastName;
		QString email;
		bool authFlag;
		enum Status {
			Offline = 0,
			Online = 1,
			NonWebaware
		} status;
		QString gender;
		quint16 age;
	};

	FindContactsMetaRequest(IcqAccount *account);
	virtual void send() const;
	const QHash<QString, FoundContact> &contacts() const;
signals:
	void contactFound(const FoundContact &contact);
protected:
	virtual bool handleData(quint16 type, const DataUnit &data);
};

template <typename T>
T ShortInfoMetaRequest::value(MetaInfoField val, const T &defaultValue) {
	QVariant res = value(val);
	if (!res.isValid())
		return defaultValue;
	return res.value<T>();
}

QDebug operator<<(QDebug dbg, const Category &cat);

} } // namespace qutim_sdk_0_3::oscar

inline uint qHash(const qutim_sdk_0_3::oscar::MetaInfoField &field)
{
	return qHash(static_cast<int>(field.value()));
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::AgeRange);
Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::Category);
Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::CategoryList);

#endif /* METAINFO_H_ */
