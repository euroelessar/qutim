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
#include "icqaccount.h"
#include <QQueue>
#include <QDateTime>
#include <qutim/objectgenerator.h>

namespace qutim_sdk_0_3 {

namespace oscar {

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
	quint32 id() const { return (itemType << 16 | (itemType == SsiGroup ? groupId : itemId)); }
	quint64 id2() const { return (quint64)id() << 32 | groupId; }
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
	void send(const FeedbagItem &item, Feedbag::ModifyType operation);
	inline void remove(FeedbagItem item);
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
	FeedbagPrivate(IcqAccount *acc, Feedbag *q):
		account(acc), conn(acc->connection()), modifyStarted(false), q_ptr(q)
	{}
	void handleItem(FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	quint16 generateId() const;
	void saveItem(const SSIItem &item, ConfigBase &config);
	void finishLoading();
	QMultiHash<quint32, SSIItem> items;
	QQueue<FeedbagQueueItem> ssiQueue;
	IcqAccount *account;
	OscarConnection *conn;
	bool modifyStarted;
	QHash<quint16, FeedbagItemHandler*> handlers;
	uint lastUpdateTime;
	bool firstPacket;
	Feedbag *q_ptr;
};

FeedbagError::FeedbagError(const SNAC &sn)
{
	m_error = static_cast<ErrorEnum>(sn.read<quint16>());
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
	recordName = snac.read<QString, quint16>();
	groupId = snac.read<quint16>();
	itemId = snac.read<quint16>();
	itemType = snac.read<quint16>();
	tlvs = snac.read<DataUnit, quint16>().read<TLVMap>();
}

void FeedbagItemPrivate::send(const FeedbagItem &item, Feedbag::ModifyType operation)
{
	feedbag->d->ssiQueue.enqueue(FeedbagQueueItem(item, operation));
	SNAC snac(ListsFamily, operation);
	snac.append<quint16>(recordName);
	snac.append<quint16>(groupId);
	snac.append<quint16>(itemId);
	snac.append<quint16>(itemType);
	snac.append<quint16>(tlvs.valuesSize());
	snac.append(tlvs);
	feedbag->d->conn->send(snac);
}

void FeedbagItemPrivate::remove(FeedbagItem item)
{
	item.d->tlvs.clear();
	isInList = false;
	send(item, Feedbag::Remove);
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
	bool modify = feedbag()->isModifyStarted();
	FeedbagItem item = *this;
	if (!modify)
		feedbag()->beginModify();
	d->send(item, d->isInList ? Feedbag::Modify : Feedbag::Add);
	d->isInList = true;
	if (!modify)
		feedbag()->endModify();
}

void FeedbagItem::remove()
{
	Q_ASSERT(isInList());
	Feedbag *f = feedbag();
	bool modify = f->isModifyStarted();
	if (!modify)
		f->beginModify();
	FeedbagItem item = *this;
	item.d->tlvs.clear();
	d->isInList = false;
	d->send(item, Feedbag::Remove);
	if (!modify)
		f->endModify();
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

void FeedbagItem::setId(quint16 itemId)
{
	d->itemId = itemId;
}

void FeedbagItem::setGroup(quint16 groupId)
{
	d->groupId = groupId;
}

void FeedbagItem::setField(quint16 field)
{
	d->tlvs.insert(field);
}

void FeedbagItem::setField(const TLV &tlv)
{
	d->tlvs.insert(tlv);
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

void FeedbagItem::setData(const TLVMap &tlvs)
{
	d->tlvs = tlvs;
}

QDebug &operator<<(QDebug &stream, const FeedbagItem &item)
{
	const FeedbagItemPrivate *d = item.d;
	if (!d->recordName.isEmpty())
		stream.nospace() << "Name: " << d->recordName << "; type: ";
	else
		stream.nospace() << "Type: ";
	stream.nospace() << d->itemType << "; ";
	if (d->itemType != SsiGroup)
		stream.nospace() << "item id: " << d->itemId << "; ";
	stream.nospace() << "group id: " << d->groupId << " (";
	bool first = true;
	foreach(const TLV &tlv, d->tlvs)
	{
		if (!first)
			stream.nospace() << ", ";
		else
			first = false;
		stream.nospace() << "0x" << hex << tlv.type();
	}
	stream.nospace() << ")";
	return stream;
}

void FeedbagPrivate::handleItem(FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	Q_Q(Feedbag);
	quint32 id = item.d->id();
	bool isInList = items.contains(id);
	item.d->isInList = type != Feedbag::Remove;
	bool found = false;
	foreach (FeedbagItemHandler *handler, handlers.values(item.type())) {
		found = found || handler->handleFeedbagItem(q, item, type, error);
	}
	if (!found) {
		if (error == FeedbagError::NoError) {
			if (type == Feedbag::Remove) {
				debug(Verbose) << "The feedbag item has been removed:" << item;
			} else {
				if (isInList) {
					debug(Verbose) << "The feedbag item has been updated:" << item;
				} else {
					debug(Verbose) << "The feedbag item has been added:" << item;
				}
			}
		} else {
			if (type == Feedbag::Remove) {
				debug(Verbose).nospace() << "The feedbag item has not been removed: "
						<< error.errorString() << ". " << item;
			} else {
				if (isInList) {
					debug(Verbose) << "The feedbag item has not been updated:"
							<< error.errorString() << ". " << item;
				} else {
					debug(Verbose) << "The feedbag item has not been added:"
							<< error.errorString() << ". " << item;
				}
			}
		}
	}
	if (error == FeedbagError::NoError) {
		bool updateConfig = account->status() != Status::Connecting;
		ConfigGroup cfg = account->config("feedbag");
		if (type == Feedbag::Remove) {
			items.remove(id);
			if (updateConfig) {
				cfg.group("cache").removeGroup(QString::number(item.d->id2()));
			}
		} else {
			items.insertMulti(id, *item.d);
			if (updateConfig) {
				cfg = cfg.group("cache");
				saveItem(*item.d, cfg);
				account->config().sync();
			}
		}
		if (updateConfig) {
			lastUpdateTime = QDateTime::currentDateTime().toTime_t();
			cfg.setValue("lastUpdateTime", lastUpdateTime);
			cfg.sync();
		}
	}
}

quint16 FeedbagPrivate::generateId() const
{
	return rand() % 0x03e6;
}

void FeedbagPrivate::saveItem(const SSIItem &item, ConfigBase &config)
{
	ConfigGroup cfg = config.group(QString::number(item.id2()));
	cfg.setValue("type", item.itemType);
	cfg.setValue("itemId", item.itemId);
	cfg.setValue("groupId", item.groupId);
	cfg.setValue("name", item.recordName);
	cfg = cfg.group("tlvs");
	foreach (const TLV &tlv, item.tlvs)
		cfg.setValue(QString::number(tlv.type()), tlv.data());
}

void FeedbagPrivate::finishLoading()
{
	Q_Q(Feedbag);
	SNAC snac(ListsFamily, ListsGotList);
	conn->send(snac); // Roster ack
	conn->finishLogin();
	emit q->loaded();
}

Feedbag::Feedbag(IcqAccount *acc):
	SNACHandler(acc), d(new FeedbagPrivate(acc, this))
{
	m_infos << SNACInfo(ListsFamily, ListsError)
			<< SNACInfo(ServiceFamily, ServiceServerAsksServices)
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
	ConfigGroup config = acc->config("feedbag");
	d->lastUpdateTime = config.value("lastUpdateTime", 0);
	config = config.group("cache");
	foreach (const QString &itemIdStr, config.groupList()) {
		config = config.group(itemIdStr);
		SSIItem item;
		item.itemId = config.value("itemId", 0);
		item.groupId = config.value("groupId", 0);
		item.itemType = config.value("type", 0);
		item.recordName = config.value<QString>("name", "");
		config =  config.group("tlvs");
		foreach (const QString &tlvIdStr, config.groupList()) {
			quint16 tlvId = tlvIdStr.toInt();
			if (tlvId > 0) {
				item.tlvs.insert(tlvId, config.value(tlvIdStr, QByteArray()));
			};
		}
		config = config.parent().parent();
		d->items.insert(item.id(), item);
	}

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

bool Feedbag::isModifyStarted() const
{
	return d->modifyStarted;
}

bool Feedbag::removeItem(quint16 type, quint16 id)
{
	QList<FeedbagItem> list = items(type, id);
	foreach (FeedbagItem item, list)
		item.remove();
	return list.isEmpty();
}

bool Feedbag::removeItem(quint16 type, const QString &name)
{
	QList<FeedbagItem> list = items(type, name);
	foreach (FeedbagItem item, list)
		item.remove();
	return !list.isEmpty();
}

FeedbagItem Feedbag::item(quint16 type, quint16 id, quint16 group, ItemLoadFlags flags) const
{
	if (!(flags & DontLoadLocal)) {
		QList<SSIItem> items = d->items.values(type << 16 | id);
		foreach (const SSIItem &item, items) {
			if (item.groupId == group)
				return FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), item));
		}
	}
	if (flags & CreateItem) {
		if (flags & GenerateId)
			id = uniqueItemId(id);
		return FeedbagItem(const_cast<Feedbag*>(this), type,
						   type == SsiGroup ? 0 : id,
						   type == SsiGroup ? id : group, "");
	}
	return FeedbagItem();
}

FeedbagItem Feedbag::item(quint16 type, const QString &name, quint16 group, ItemLoadFlags flags) const
{
	QString n;
	if (type != SsiGroup)
		n = getCompressedName(name);
	if (!(flags & DontLoadLocal)) {
		foreach (const SSIItem &item, d->items) {
			if (item.itemType == type && item.groupId == group) {
				bool found = type != SsiGroup ?
							 n == getCompressedName(item.recordName) :
							 name.compare(item.recordName, Qt::CaseInsensitive) == 0;
				if (found)
					return FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), item));
			}
		}
	}
	if (flags & CreateItem)
		return FeedbagItem(const_cast<Feedbag*>(this), type,
						   type != SsiGroup ? uniqueItemId(type) : 0,
						   type == SsiGroup ? uniqueItemId(type) : group,
						   name);
	return FeedbagItem();
}

QList<FeedbagItem> Feedbag::items(quint16 type, quint16 id, ItemLoadFlags flags) const
{
	QList<FeedbagItem> items;
	if (!(flags & DontLoadLocal)) {
		QHash<quint32, SSIItem>::const_iterator itr = d->items.find(type << 16 | id);
		if (itr != d->items.constEnd()) {
			items << FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), *itr));
		}
	}
	if (items.isEmpty() && flags & CreateItem) {
		if (flags & GenerateId)
			id = uniqueItemId(id);
		items << FeedbagItem(const_cast<Feedbag*>(this), type,
						   type == SsiGroup ? 0 : id,
						   type == SsiGroup ? id : 0, "");
	}
	return items;
}

QList<FeedbagItem> Feedbag::items(quint16 type, const QString &name, ItemLoadFlags flags) const
{
	QList<FeedbagItem> items;
	QString n;
	if (type != SsiGroup)
		n = getCompressedName(name);
	if (!(flags & DontLoadLocal)) {
		foreach (const SSIItem &item, d->items) {
			if (item.itemType == type) {
				bool found = type != SsiGroup ?
							 n == getCompressedName(item.recordName) :
							 name.compare(item.recordName, Qt::CaseInsensitive) == 0;
				if (found) {
					items << FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), item));
					if (flags & ReturnOne)
						return items;
				}
			}
		}
	}
	if (items.isEmpty() && flags & CreateItem)
		items << FeedbagItem(const_cast<Feedbag*>(this), type,
						   type != SsiGroup ? uniqueItemId(type) : 0,
						   type == SsiGroup ? uniqueItemId(type) : 0,
						   name);
	return items;
}

FeedbagItem Feedbag::groupItem(quint16 id, ItemLoadFlags flags) const
{
	QList<FeedbagItem> list = items(SsiGroup, id, flags | ReturnOne);
	if (list.isEmpty())
		return FeedbagItem();
	return list.first();
}

FeedbagItem Feedbag::groupItem(const QString &name, ItemLoadFlags flags) const
{
	QList<FeedbagItem> list = items(SsiGroup, name, flags | ReturnOne);
	if (list.isEmpty())
		return FeedbagItem();
	return list.first();
}

QList<FeedbagItem> Feedbag::group(quint16 groupId) const
{
	QList<FeedbagItem> items;
	foreach (const SSIItem &item, d->items) {
		if ((item.itemType != SsiGroup && item.groupId == groupId) ||
			(item.itemType == SsiGroup && groupId == 0 && item.groupId != 0))
		{
				items << FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), item));
		}
	}
	return items;
}

QList<FeedbagItem> Feedbag::group(const QString &name) const
{
	FeedbagItem i = groupItem(name);
	if (i.isInList())
		return group(i.groupId());
	return QList<FeedbagItem>();
}

QList<FeedbagItem> Feedbag::type(quint16 type, ItemLoadFlags flags) const
{
	QList<FeedbagItem> items;
	if (!(flags & DontLoadLocal)) {
		foreach (const SSIItem &item, d->items) {
			if (item.itemType == type)
				items << FeedbagItem(new FeedbagItemPrivate(const_cast<Feedbag*>(this), item));
		}
	}
	if (items.isEmpty() && flags & CreateItem)
		items << FeedbagItem(const_cast<Feedbag*>(this), type,
							 type != SsiGroup ? uniqueItemId(type) : 0,
							 type == SsiGroup ? uniqueItemId(type) : 0,
							 "");
	return items;
}

bool Feedbag::containsItem(quint16 type, quint16 id) const
{
	return !items(type, id).isEmpty();
}

bool Feedbag::containsItem(quint16 type, const QString &name) const
{
	return !items(type, name).isEmpty();
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

void Feedbag::registerHandler(FeedbagItemHandler *handler)
{
	const QSet<quint16> &types = handler->types();
	foreach (quint16 type, types)
		d->handlers.insertMulti(type, handler);
	QList<SSIItem> items;
	foreach (const SSIItem &item, d->items) {
		if (item.itemType == SsiGroup) {
			if (types.contains(SsiGroup)) {
				FeedbagItem feedbagItem(new FeedbagItemPrivate(this, item));
				handler->handleFeedbagItem(this, feedbagItem, AddModify, FeedbagError::NoError);
			}
		} else {
			items << item;
		}
	}
	foreach (const SSIItem &item, items) {
		if (types.contains(item.itemType)) {
			FeedbagItem feedbagItem(new FeedbagItemPrivate(this, item));
			handler->handleFeedbagItem(this, feedbagItem, AddModify, FeedbagError::NoError);
		}
	}
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
	case ServiceFamily << 16 | ServiceServerAsksServices: {
		// Request server-stored information (SSI) service limitations
		SNAC snac(ListsFamily, ListsCliReqLists);
		snac.appendTLV<quint16>(0x0B, 0x000F); // mimic ICQ 6
		conn->send(snac);

		// Requesting roster
		if (d->lastUpdateTime > 0) {
			snac.reset(ListsFamily, ListsCliCheck);
			snac.append<quint32>(d->lastUpdateTime);
			snac.append<quint16>(d->items.count());
			conn->send(snac);
		} else {
			snac.reset(ListsFamily, ListsCliRequest);
			conn->send(snac);
		}
		break;
	}
	case ListsFamily << 16 | ListsUpToDate: {
		 debug() << "Local contactlist is up to date";
		 d->firstPacket = true;
		 d->finishLoading();
		 break;
	}
	case ListsFamily << 16 | ListsList: { // Server sends contactlist
		if (d->firstPacket) {
			d->items.clear();
			d->firstPacket = false;
		}
		quint8 version = sn.read<quint8>();
		quint16 count = sn.read<quint16>();
		bool isLast = !(sn.flags() & 0x0001);
		debug() << "SSI: number of entries is" << count << "version is" << version;
		for (uint i = 0; i < count; i++) {	
			FeedbagItem item(new FeedbagItemPrivate(this, sn));
			d->handleItem(item, AddModify, FeedbagError::NoError);
		}
		if (isLast) {
			d->firstPacket = true;
			d->lastUpdateTime = sn.read<quint32>();
			d->account->config().removeGroup("feedbag");
			ConfigGroup config = d->account->config("feedbag");
			config.setValue("lastUpdateTime", d->lastUpdateTime);
			config = config.group("cache");
			foreach (const SSIItem &item, d->items)
				d->saveItem(item, config);
			config.sync();
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

} } // namespace qutim_sdk_0_3::oscar
