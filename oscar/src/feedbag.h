/****************************************************************************
 *  feedbag.h
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

#ifndef FEEDBAG_H
#define FEEDBAG_H

#include <icq_global.h>
#include <snachandler.h>
#include "tlv.h"
#include <QSharedData>

namespace qutim_sdk_0_3 {

namespace oscar {

class Feedbag;
class FeedbagPrivate;
class FeedbagItemPrivate;
class FeedbagItemHandler;
class IcqAccount;

class LIBOSCAR_EXPORT FeedbagError
{
public:
	enum ErrorEnum
	{
		NoError = 0x0000,
		ItemNotFound = 0x0002,
		ItemAlreadyExists = 0x0003,
		CommonError = 0x000a,
		LimitExceeded = 0x000c,
		AttemtToAddIcqContactToAimList = 0x000d,
		RequiresAuthorization = 0x000e
	};
	FeedbagError(ErrorEnum error);
	FeedbagError(const SNAC &snac);
	ErrorEnum code();
	QString errorString();
	bool operator==(ErrorEnum code) { return m_error == code; }
	bool operator!=(ErrorEnum code) { return m_error != code; }
protected:
	ErrorEnum m_error;
};

class LIBOSCAR_EXPORT FeedbagItem
{
public:
	FeedbagItem();
	FeedbagItem(Feedbag *feedbag, quint16 type, quint16 itemId, quint16 groupId, const QString &name = QString());
	FeedbagItem(const FeedbagItem &item);
	virtual ~FeedbagItem();
	const FeedbagItem &operator=(const FeedbagItem &item);
	void update();
	void remove();
	Feedbag *feedbag() const;
	bool isInList() const;
	bool isEmpty() const;
	bool isNull() const;
	void setName(const QString &name);
	void setId(quint16 itemId);
	void setGroup(quint16 groupId);
	void setField(quint16 field);
	void setField(const TLV &tlv);
	template<typename T>
	void setField(quint16 field, const T &data);
	bool removeField(quint16 field);
	QString name() const;
	quint16 type() const;
	quint16 itemId() const;
	quint16 groupId() const;
	TLV field(quint16 field) const;
	template<typename T>
	T field(quint16 field, const T &def = T()) const;
	bool containsField(quint16 field) const;
	TLVMap &data();
	const TLVMap &constData() const;
	void setData(const TLVMap &tlvs);
protected:
	FeedbagItem(FeedbagItemPrivate *d);
private:
	friend class Feedbag;
	friend class FeedbagPrivate;
	friend class FeedbagItemPrivate;
	friend QDebug &operator<<(QDebug &stream, const FeedbagItem &item);
	QSharedDataPointer<FeedbagItemPrivate> d;
};

template<typename T>
Q_INLINE_TEMPLATE T FeedbagItem::field(quint16 f, const T &def) const
{
	return constData().value(f, def);
}

template<typename T>
Q_INLINE_TEMPLATE void FeedbagItem::setField(quint16 field, const T &d)
{
	data().insert(field, d);
}

class LIBOSCAR_EXPORT Feedbag : public QObject, public SNACHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler)
public:		
	enum ModifyType {
		Add = ListsAddToList,
		AddModify = ListsList,
		Modify = ListsUpdateGroup,
		Remove =  ListsRemoveFromList
	};
	enum ItemLoadFlag
	{
		NoFlags = 0x0000,
		CreateItem = 0x0001,
		GenerateId = CreateItem | 0x0002,
		DontLoadLocal = 0x0010,
		ReturnOne = 0x0020
	};
	Q_DECLARE_FLAGS(ItemLoadFlags, ItemLoadFlag);
	Feedbag(IcqAccount *acc);
	virtual ~Feedbag();
	void beginModify();
	void endModify();
	bool isModifyStarted() const;
	bool removeItem(quint16 type, quint16 id);
	bool removeItem(quint16 type, const QString &name);
	FeedbagItem item(quint16 type, quint16 id, quint16 group, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem item(quint16 type, const QString &name, quint16 group, ItemLoadFlags flags = NoFlags) const;
	QList<FeedbagItem> items(quint16 type, quint16 id, ItemLoadFlags flags = NoFlags) const;
	QList<FeedbagItem> items(quint16 type, const QString &name, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem groupItem(quint16 id, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem groupItem(const QString &name, ItemLoadFlags flags = NoFlags) const;
	QList<FeedbagItem> group(quint16 groupId) const;
	QList<FeedbagItem> group(const QString &name) const;
	QList<FeedbagItem> type(quint16 type, ItemLoadFlags flags = NoFlags) const;
	bool containsItem(quint16 type, quint16 id) const;
	bool containsItem(quint16 type, const QString &name) const;
	quint16 uniqueItemId(quint16 type, quint16 value = 0) const;
	void registerHandler(FeedbagItemHandler *handler);
	IcqAccount *account() const;
signals:
	void loaded();
	void reloadingStarted();
protected:
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private:
	friend class FeedbagPrivate;
	friend class FeedbagItem;
	friend class FeedbagItemPrivate;
	QScopedPointer<FeedbagPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Feedbag::ItemLoadFlags);

class LIBOSCAR_EXPORT FeedbagItemHandler
{
public:
	virtual ~FeedbagItemHandler();
	const QSet<quint16> &types() { return m_types; }
	virtual bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error) = 0;
protected:
	QSet<quint16> m_types;
};

} } // namespace qutim_sdk_0_3::oscar

Q_DECLARE_INTERFACE(qutim_sdk_0_3::oscar::FeedbagItemHandler, "org.qutim.oscar.FeedbagItemHandler");

#endif // FEEDBAG_H
