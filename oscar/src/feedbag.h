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

namespace Icq {

class Feedbag;
class FeedbagPrivate;
class FeedbagItemPrivate;
class FeedbagItemHandler;
class OscarConnection;

class FeedbagError
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

class FeedbagItem
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
protected:
	FeedbagItem(FeedbagItemPrivate *d);
private:
	friend class Feedbag;
	friend class FeedbagPrivate;
	QExplicitlySharedDataPointer<FeedbagItemPrivate> d;
};

template<typename T>
T FeedbagItem::field(quint16 f, const T &def) const
{
	return constData().value<T>(f, def);
}

template<typename T>
void FeedbagItem::setField(quint16 field, const T &d)
{
	data().insert(field, d);
}

class Feedbag : public SNACHandler
{
	Q_OBJECT
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
		DontLoadLocal = 0x0010
	};
	Q_DECLARE_FLAGS(ItemLoadFlags, ItemLoadFlag);
	Feedbag(OscarConnection *conn, QObject *parent = 0);
	virtual ~Feedbag();
	void beginModify();
	void endModify();
	bool isModifyStarted();
	void updateItem(const FeedbagItem &item);
	void removeItem(const FeedbagItem &item);
	bool removeItem(quint16 type, quint16 id);
	bool removeItem(quint16 type, const QString &name);
	bool removeGroup(quint16 groupId);
	bool removeGroup(const QString &name);
	FeedbagItem item(quint16 type, quint16 id, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem item(quint16 type, const QString &name, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem group(quint16 groupId, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem group(const QString &name, ItemLoadFlags flags = NoFlags) const;
	bool containsItem(quint16 type, quint16 id);
	bool containsItem(quint16 type, const QString &name);
	bool containsGroup(quint16 groupId);
	bool containsGroup(const QString &name);
	bool testItemName(quint16 type, const QString &name) const;
	bool testGroupName(const QString &name) const;
	quint16 uniqueItemId(quint16 type, quint16 value = 0) const;
	quint16 uniqueGroupId(quint16 value = 0) const;
	void registerHandler(FeedbagItemHandler *handler);
signals:
	void loaded();
protected:
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private:
	friend class FeedbagPrivate;
	friend class FeedbagItem;
	QScopedPointer<FeedbagPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Feedbag::ItemLoadFlags);

class FeedbagItemHandler: public QObject
{
	Q_OBJECT
public:
	explicit FeedbagItemHandler(QObject *parent = 0);
	virtual ~FeedbagItemHandler();
	const QList<quint16> &types() { return m_types; }
	virtual void handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error) = 0;
protected:
	QList<quint16> m_types;
};

} // namespace Icq

#endif // FEEDBAG_H
