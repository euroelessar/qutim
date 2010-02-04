/****************************************************************************
 *  feedbag.cpp
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


#include "feedbag.h"
#include "snac.h"
#include "oscarconnection.h"
#include <QQueue>
#include <qutim/objectgenerator.h>

namespace Icq {

QString getCompressedName(const QString &name)
{
	QString compressedName;
	foreach (const QChar &c, name) {
		if (c != ' ')
			compressedName += c.toLower();
	}
	return compressedName;
}

class SSIItem
{
public:
	SSIItem():
		groupId(0), itemId(0)
	{}
	~SSIItem(){}
	QString toString() const;
	bool isEmpty() const { return itemId == 0 && groupId == 0; }
	inline QString compressedName() const { return getCompressedName(recordName); }
	QString recordName;
	quint16 groupId;
	quint16 itemId;
	quint16 itemType;
	TLVMap tlvs;
};

class FeedbagItemPrivate: public QSharedData, public SSIItem
{
public:
	FeedbagItemPrivate(Feedbag *bag, quint16 type, quint16 item, quint16 group, const QString &name, bool inList = false);
	FeedbagItemPrivate(Feedbag *bag, const SSIItem &item, bool inList = true);
	FeedbagItemPrivate(Feedbag *bag, const SNAC &snac, bool inList = false);
	Feedbag *feedbag;
	bool isInList;
};

struct FeedbagQueueItem
{
	FeedbagQueueItem(const FeedbagItem &item_, Feedbag::ModifyType type_):
		item(item_), type(type_)
	{
	}
	FeedbagItem item;
	Feedbag::ModifyType type;
};

class FeedbagPrivate
{
	Q_DECLARE_PUBLIC(Feedbag)
public:
	FeedbagPrivate(OscarConnection *c, Feedbag *q):
		modifyStarted(false), conn(c), q_ptr(q)
	{}
	void sendItem(const FeedbagItem &item, Feedbag::ModifyType operation);
	void handleItem(const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	quint16 generateId() const;
	void finishLoading();
	QHash<quint32, SSIItem> items;
	QQueue<FeedbagQueueItem> ssiQueue;
	OscarConnection *conn;
	bool modifyStarted;
	QHash<quint16, FeedbagItemHandler*> handlers;
	Feedbag *q_ptr;
};

QString SSIItem::toString() const
{
	QString str;
	QTextStream stream(&str);
	if (!recordName.isEmpty())
		stream << "Name: " << recordName << "; type: ";
	else
		stream << "Type: ";
	stream << itemType << "; ";
	if (itemType != SsiGroup)
		stream << "item id: " << itemId << "; ";
	stream << "group id: " << groupId << " (";
	bool first = true;
	foreach(const TLV &tlv, tlvs)
	{
		if (!first)
			stream << ", ";
		else
			first = false;
		stream << "0x" << hex << tlv.type();
	}
	stream << ")";
	return str;
}

FeedbagError::FeedbagError(const SNAC &sn)
{
	m_error = static_cast<ErrorEnum>(sn.readSimple<quint16>());
}

FeedbagError::FeedbagError(FeedbagError::ErrorEnum error):
	m_error(error)
{
}

FeedbagError::ErrorEnum FeedbagError::code()
{
	return m_error;
}

QString FeedbagError::errorString()
{
	QString errorStr;
	if (m_error == NoError)
		errorStr = QT_TRANSLATE_NOOP("FeedbagError", "No error");
	if (m_error == ItemNotFound)
		errorStr = QT_TRANSLATE_NOOP("FeedbagError", "Item you want to modify not found in list");
	else if (m_error == ItemAlreadyExists)
		errorStr = QT_TRANSLATE_NOOP("FeedbagError", "Item you want to add allready exists");
	else if (m_error == CommonError)
		errorStr = QT_TRANSLATE_NOOP("FeedbagError", "Error adding item (invalid id, allready in list, invalid data)");
	else if (m_error == LimitExceeded)
		errorStr = QT_TRANSLATE_NOOP("FeedbagError", "Can't add item. Limit for this type of items exceeded");
	else if (m_error == AttemtToAddIcqContactToAimList)
		errorStr = QT_TRANSLATE_NOOP("FeedbagError", "Trying to add ICQ contact to an AIM list");
	else if (m_error == RequiresAuthorization)
		errorStr = QT_TRANSLATE_NOOP("FeedbagError", "Can't add this contact because it requires authorization");
	else
		errorStr = QT_TRANSLATE_NOOP("FeedbagError", "Unknown error");
	return errorStr;
}

FeedbagItemPrivate::FeedbagItemPrivate(Feedbag *bag, quint16 type, quint16 item, quint16 group, const QString &name, bool inList):
	feedbag(bag), isInList(inList)
{
	itemType = type;
	itemId = item;
	groupId = group;
	recordName = name;
}

FeedbagItemPrivate::FeedbagItemPrivate(Feedbag *bag, const SSIItem &item, bool inList):
	SSIItem(item), feedbag(bag), isInList(inList)
{
}

FeedbagItemPrivate::FeedbagItemPrivate(Feedbag *bag, const SNAC &snac, bool inList):
	feedbag(bag), isInList(inList)
{
	recordName = snac.readString<quint16>();
	groupId = snac.readSimple<quint16>();
	itemId = snac.readSimple<quint16>();
	itemType = snac.readSimple<quint16>();
	tlvs = DataUnit(snac.readData<quint16>()).readTLVChain();
}

FeedbagItem::FeedbagItem():
	d(0)
{
}

FeedbagItem::FeedbagItem(Feedbag *feedbag, quint16 type, quint16 itemId, quint16 groupId, const QString &name):
	d(new FeedbagItemPrivate(feedbag, type, itemId, groupId, name))
{
}

FeedbagItem::FeedbagItem(FeedbagItemPrivate *d):
	d(d)
{
}

FeedbagItem::FeedbagItem(const FeedbagItem &item):
	d(new FeedbagItemPrivate(*item.d))
{
}

FeedbagItem::~FeedbagItem()
{

}

const FeedbagItem &FeedbagItem::operator=(const FeedbagItem &item)
{
	d = item.d;
	return *this;
}

void FeedbagItem::update()
{
	bool modify = d->feedbag->isModifyStarted();
	if (!modify)
		d->feedbag->beginModify();
	feedbag()->updateItem(*this);
	if (!modify)
		d->feedbag->endModify();
}

void FeedbagItem::remove()
{
	Q_ASSERT(isInList());
	bool modify = d->feedbag->isModifyStarted();
	if (!modify)
		d->feedbag->beginModify();
	feedbag()->removeItem(*this);
	if (!modify)
		d->feedbag->endModify();
}

Feedbag *FeedbagItem::feedbag() const
{
	return d->feedbag;
}

bool FeedbagItem::isInList() const
{
	return !isNull() && d->isInList;
}

bool FeedbagItem::isEmpty() const
{
	return isNull();
}

bool FeedbagItem::isNull() const
{
	return d.data() == 0;
}

void FeedbagItem::setName(const QString &name)
{
	d->recordName = name;
}

void FeedbagItem::setGroup(quint16 groupId)
{
	Q_ASSERT_X(d->itemType != SsiGroup, Q_FUNC_INFO, "Cannot change groupId for group");
	d->groupId = groupId;
}

void FeedbagItem::setField(quint16 field)
{
	d->tlvs.replace(field, TLV(field));
}

void FeedbagItem::setField(const TLV &tlv)
{
	d->tlvs.replace(tlv.type(), tlv);
}

bool FeedbagItem::removeField(quint16 field)
{
	return d->tlvs.remove(field) > 0;
}

QString FeedbagItem::name() const
{
	return d->recordName;
}

quint16 FeedbagItem::type() const
{
	return d->itemType;
}

quint16 FeedbagItem::itemId() const
{
	return d->itemId;
}

quint16 FeedbagItem::groupId() const
{
	return d->groupId;
}

TLV FeedbagItem::field(quint16 field) const
{
	return d->tlvs.value(field);
}

bool FeedbagItem::containsField(quint16 field) const
{
	return d->tlvs.contains(field);
}

TLVMap &FeedbagItem::data()
{
	return d->tlvs;
}

const TLVMap &FeedbagItem::constData() const
{
	return d->tlvs;
}

void FeedbagPrivate::sendItem(const FeedbagItem &item, Feedbag::ModifyType operation)
{
	ssiQueue.enqueue(FeedbagQueueItem(item, operation));
	SNAC snac(ListsFamily, operation);
	snac.appendData<quint16>(item.name());
	snac.appendSimple<quint16>(item.groupId());
	snac.appendSimple<quint16>(item.itemId());
	snac.appendSimple<quint16>(item.type());
	snac.appendSimple<quint16>(item.constData().valuesSize());
	snac.appendData(item.constData());
	conn->send(snac);
}

void FeedbagPrivate::handleItem(const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	Q_Q(Feedbag);
	if (error.code() != FeedbagError::NoError)
		return;
	quint32 id = item.type() << 16 | (item.type() == SsiGroup ? item.groupId() : item.itemId());
	if (type == Feedbag::Remove) {
		items.remove(id);
	} else {
		items.insert(id, *item.d);
	}
	bool found = false;
	foreach (FeedbagItemHandler *handler, handlers.values(item.type())) {
		found = true;
		handler->handleFeedbagItem(q, item, type, error);
	}
	if (!found)
		debug(Verbose) << "Dump of unknown SSI item:" << item.d->toString();
}

quint16 FeedbagPrivate::generateId() const
{
	return rand() % 0x03e6;
}

void FeedbagPrivate::finishLoading()
{
	Q_Q(Feedbag);
	SNAC snac(ListsFamily, ListsGotList);
	conn->send(snac); // Roster ack
	conn->finishLogin();
	emit q->loaded();
}

Feedbag::Feedbag(OscarConnection *conn, QObject *parent):
	SNACHandler(parent), d(new FeedbagPrivate(conn, this))
{
	m_infos << SNACInfo(ListsFamily, ListsError)
			<< SNACInfo(ListsFamily, ListsUpToDate)
			<< SNACInfo(ListsFamily, ListsList)
			<< SNACInfo(ListsFamily, ListsUpdateGroup)
			<< SNACInfo(ListsFamily, ListsAddToList)
			<< SNACInfo(ListsFamily, ListsRemoveFromList)
			<< SNACInfo(ListsFamily, ListsAck)
			<< SNACInfo(ListsFamily, ListsCliModifyStart)
			<< SNACInfo(ListsFamily, ListsCliModifyEnd)
			<< SNACInfo(ListsFamily, ListsSrvReplyLists);
	foreach(const ObjectGenerator *gen, moduleGenerators<FeedbagItemHandler>())
		registerHandler(gen->generate<FeedbagItemHandler>());
}

Feedbag::~Feedbag()
{
}

void Feedbag::beginModify()
{
	SNAC snac(ListsFamily, ListsCliModifyStart);
	d->conn->send(snac);
	d->modifyStarted = true;
}

void Feedbag::endModify()
{
	SNAC snac(ListsFamily, ListsCliModifyEnd);
	d->conn->send(snac);
	d->modifyStarted = false;
}

bool Feedbag::isModifyStarted()
{
	return d->modifyStarted;
}

void Feedbag::updateItem(const FeedbagItem &item)
{
	Q_ASSERT(!item.isNull());
	Q_ASSERT(item.feedbag() == this);
	d->sendItem(item, item.isInList() ? Modify : Add);
}

void Feedbag::removeItem(const FeedbagItem &item)
{
	Q_ASSERT(!item.isNull());
	Q_ASSERT(item.feedbag() == this);
	FeedbagItem i = item;
	i.data().clear();
	d->sendItem(i, Remove);
}

bool Feedbag::removeItem(quint16 type, quint16 id)
{
	FeedbagItem i = item(type, id);
	if (!i.isNull()) {
		removeItem(i);
		return true;
	} else {
		return false;
	}
}

bool Feedbag::removeItem(quint16 type, const QString &name)
{
	FeedbagItem i = item(type, name);
	if (!i.isNull()) {
		removeItem(i);
		return true;
	} else {
		return false;
	}
}

bool Feedbag::removeGroup(quint16 groupId)
{
	return removeItem(SsiGroup, groupId);
}

bool Feedbag::removeGroup(const QString &name)
{
	FeedbagItem i = group(name);
	if (!i.isNull()) {
		removeItem(i);
		return true;
	} else {
		return false;
	}
}

FeedbagItem Feedbag::item(quint16 type, quint16 id, ItemLoadFlags flags) const
{
	if (!(flags & DontLoadLocal)) {
		QHash<quint32, SSIItem>::const_iterator itr = d->items.find(type << 16 | id);
		if (itr != d->items.constEnd()) {
			return FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), *itr));
		}
	}
	if (flags & CreateItem) {
		if (flags & GenerateId)
			id = uniqueItemId(id);
		return FeedbagItem(const_cast<Feedbag*>(this), type,
						   type == SsiGroup ? 0 : id,
						   type == SsiGroup ? id : 0, "");
	}
	return FeedbagItem();
}

FeedbagItem Feedbag::item(quint16 type, const QString &name, ItemLoadFlags flags) const
{
	Q_ASSERT_X(type != SsiGroup, Q_FUNC_INFO, "Use Feedbag::group() instead");
	QString compressedName = getCompressedName(name);
	if (!(flags & DontLoadLocal)) {
		foreach (const SSIItem &item, d->items) {
			if (item.itemType == type && item.compressedName() == compressedName)
				return FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), item));
		}
	}
	if (flags & CreateItem)
		return FeedbagItem(const_cast<Feedbag*>(this), type, uniqueItemId(type), 0, name);
	return FeedbagItem();
}

FeedbagItem Feedbag::group(quint16 groupId, ItemLoadFlags flags) const
{
	return item(SsiGroup, groupId, flags);
}

FeedbagItem Feedbag::group(const QString &name, ItemLoadFlags flags) const
{
	if (!(flags & DontLoadLocal)) {
		foreach (const SSIItem &item, d->items) {
			if (item.itemType == SsiGroup && name.compare(item.recordName, Qt::CaseInsensitive) == 0)
				return FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), item));
		}
	}
	if (flags & CreateItem)
		return FeedbagItem(const_cast<Feedbag*>(this), SsiGroup, 0, uniqueGroupId(), name);
	return FeedbagItem();
}

bool Feedbag::containsItem(quint16 type, quint16 id)
{
	return !item(type, id).isNull();
}

bool Feedbag::containsItem(quint16 type, const QString &name)
{
	return !item(type, name).isNull();
}

bool Feedbag::containsGroup(quint16 groupId)
{
	return !group(groupId).isNull();
}

bool Feedbag::containsGroup(const QString &name)
{
	return !group(name).isNull();
}

bool Feedbag::testItemName(quint16 type, const QString &name) const
{
	return item(type, name).isNull();
}

bool Feedbag::testGroupName(const QString &name) const
{
	return group(name).isNull();
}

quint16 Feedbag::uniqueItemId(quint16 type, quint16 value) const
{
	if (value == 0)
		value = d->generateId();
	forever {
		if (d->items.contains(type << 16 | value))
			value = d->generateId();
		else
			break;
	}
	return value;
}

quint16 Feedbag::uniqueGroupId(quint16 value) const
{
	return uniqueItemId(SsiGroup, value);
}

void Feedbag::registerHandler(FeedbagItemHandler *handler)
{
	foreach (quint16 type, handler->types())
		d->handlers.insertMulti(type, handler);
}

void Feedbag::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	Q_ASSERT(conn == d->conn);
	switch ((sn.family() << 16) | sn.subtype()) {
	case ListsFamily << 16 | ListsError: {
		 ProtocolError error(sn);
		 debug() << QString("Error (%1, %2): %3")
				 .arg(error.code, 2, 16)
				 .arg(error.subcode, 2, 16)
				 .arg(error.str);
		 break;
	}
	case ListsFamily << 16 | ListsUpToDate: {
		 debug() << "Local contactlist is up to date";
		 d->finishLoading();
		 break;
	}
	case ListsFamily << 16 | ListsList: { // Server sends contactlist
		quint8 version = sn.readSimple<quint8>();
		quint16 count = sn.readSimple<quint16>();
		bool isLast = !(sn.flags() & 0x0001);
		debug() << "SSI: number of entries is" << count << "version is" << version;
		for (uint i = 0; i < count; i++) {
			FeedbagItem item(new FeedbagItemPrivate(this, sn));
			d->handleItem(item, AddModify, FeedbagError::NoError);
		}
		if (isLast) {
			quint32 last_info_update = sn.readSimple<quint32>();
			debug() << "SrvLastUpdate" << last_info_update;
			setProperty("SrvLastUpdate", last_info_update);
			d->finishLoading();
		}
		break;
	}
	case ListsFamily << 16 | ListsUpdateGroup: // Server sends contact list updates
	case ListsFamily << 16 | ListsAddToList: // Server sends new items
	case ListsFamily << 16 | ListsRemoveFromList: { // Items have been removed
		while (sn.dataSize() != 0) {
			FeedbagItem item(new FeedbagItemPrivate(this, sn));
			d->handleItem(item, static_cast<ModifyType>(sn.subtype()), FeedbagError::NoError);
		}
		break;
	}
	case ListsFamily << 16 | ListsAck: {
		sn.skipData(8); // cookie
		while (sn.dataSize() != 0) {
			FeedbagError error(sn);
			FeedbagQueueItem operation = d->ssiQueue.dequeue();
			if (error.code() == FeedbagError::NoError) {
				debug() << "SSI operation has been successfully done"
						<< operation.type
						<< operation.item.d->toString();
			} else {
				debug() << "SSI operation has failed:" << error.errorString();
			}
			d->handleItem(operation.item, operation.type, error);
		}
		break;
	}
	case ListsFamily << 16 | ListsCliModifyStart:
		debug(Verbose) << "The server has started modification of the contact list";
		break;
	case ListsFamily << 16 | ListsCliModifyEnd:
		debug(Verbose) << "The server has ended modification of the contact list";
		break;
	// Server sends SSI service limitations to client
	case ListsFamily << 16 | ListsSrvReplyLists: {
		debug() << IMPLEMENT_ME << "ListsFamily, ListsSrvReplyLists";
		break;
	}
	}
}

FeedbagItemHandler::FeedbagItemHandler(QObject *parent):
	QObject(parent)
{
}

FeedbagItemHandler::~FeedbagItemHandler()
{
}

} // namespace Icq
