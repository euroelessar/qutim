/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/


#include "feedbag.h"
#include "snac.h"
#include "oscarconnection.h"
#include "icqaccount.h"
#include <qutim/protocol.h>
#include <QQueue>
#include <QDateTime>
#include <QLatin1String>

Q_DECLARE_METATYPE(qutim_sdk_0_3::oscar::FeedbagItem);

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

class FeedbagItemPrivate: public QSharedData
{
public:
	FeedbagItemPrivate();
	FeedbagItemPrivate(Feedbag *bag, quint16 type, quint16 item, quint16 group, const QString &name, bool inList = false);
	void send(const FeedbagItem &item, Feedbag::ModifyType operation);
	inline void remove(FeedbagItem item);
	bool isSendingAllowed(const FeedbagItem &item, Feedbag::ModifyType operation);
	quint16 id() const { return itemType == SsiGroup ? groupId : itemId; }
	QString configId() const { return QString::number((quint64)(itemType << 16 | id()) << 32 | groupId); }
	QString recordName;
	quint16 groupId;
	quint16 itemId;
	quint16 itemType;
	TLVMap tlvs;
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

typedef QMultiHash<quint16, FeedbagItem> ItemsHash;

class FeedbagPrivate
{
	Q_DECLARE_PUBLIC(Feedbag)
public:
	FeedbagPrivate(IcqAccount *acc, Feedbag *q):
		account(acc), conn(static_cast<OscarConnection*>(acc->connection())),
		modifyStarted(false), q_ptr(q)
	{}
	void handleItem(FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	quint16 generateId() const;
	void finishLoading();
	FeedbagItemPrivate *getFeedbagItemPrivate(const SNAC &snac);
	QHash<quint16, ItemsHash> items;
	QQueue<FeedbagQueueItem> ssiQueue;
	IcqAccount *account;
	OscarConnection *conn;
	bool modifyStarted;
	QHash<quint16, FeedbagItemHandler*> handlers;
	uint lastUpdateTime;
	bool firstPacket;
	QList<quint16> limits;
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

FeedbagItemPrivate::FeedbagItemPrivate() :
	feedbag(0), isInList(false)
{
}

FeedbagItemPrivate::FeedbagItemPrivate(Feedbag *bag, quint16 type, quint16 item, quint16 group, const QString &name, bool inList):
	feedbag(bag), isInList(inList)
{
	itemType = type;
	itemId = item;
	groupId = group;
	recordName = name;
}

void FeedbagItemPrivate::send(const FeedbagItem &item, Feedbag::ModifyType operation)
{
	FeedbagPrivate *d = feedbag->d.data();
	d->ssiQueue.enqueue(FeedbagQueueItem(item, operation));
	SNAC snac(ListsFamily, operation);
	snac.append<quint16>(recordName);
	snac.append<quint16>(groupId);
	snac.append<quint16>(itemId);
	snac.append<quint16>(itemType);
	snac.append<quint16>(tlvs.valuesSize());
	snac.append(tlvs);
	d->conn->send(snac);
}

void FeedbagItemPrivate::remove(FeedbagItem item)
{
	item.d->tlvs.clear();
	isInList = false;
	send(item, Feedbag::Remove);
}

bool FeedbagItemPrivate::isSendingAllowed(const FeedbagItem &item, Feedbag::ModifyType operation)
{
	FeedbagPrivate *d = feedbag->d.data();
	Status::Type status = d->account->status().type();
	if (status == Status::Offline || status == Status::Connecting) {
		warning() << "Trying to send the feedbag item while offline:" << item;
		return false;
	}
	if (operation == Feedbag::Add) {
		quint16 limit = d->limits.value(item.type());
		if (limit > 0 && d->items.value(item.type()).count() >= limit) {
			warning() << "Limit for feedbag item type" << item.type() << "exceeded";
			return false;
		}
	}
	return true;
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

FeedbagItem::FeedbagItem(const FeedbagItem &item) :
	d(item.d)
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
	FeedbagItem item = *this;
	Feedbag::ModifyType op = d->isInList ? Feedbag::Modify : Feedbag::Add;
	if (!d->isSendingAllowed(item, op))
		return;
	Feedbag *f = feedbag();
	bool modify = f->isModifyStarted();
	if (!modify)
		f->beginModify();
	d->send(item, op);
	d->isInList = true;
	if (!modify)
		f->endModify();
}

void FeedbagItem::remove()
{
	Q_ASSERT(isInList());
	FeedbagItem item = *this;
	Feedbag *f = feedbag();
	if (!d->isSendingAllowed(item, Feedbag::Remove))
		return;
	bool modify = f->isModifyStarted();
	if (!modify)
		f->beginModify();
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

bool FeedbagItem::operator==(const FeedbagItem &rhs) const
{
	const FeedbagItemPrivate *rd = rhs.d;
	return (d->itemType == rd->itemType &&
			d->itemId == rd->itemId &&
			d->groupId == rd->groupId);
}

QDataStream &operator<<(QDataStream &out, const FeedbagItem &item)
{
	out << item.d->recordName << item.d->itemId
		<< item.d->groupId << item.d->itemType
		<< item.d->tlvs.count();
	TLVMap::const_iterator itr = item.d->tlvs.constBegin();
	TLVMap::const_iterator endItr = item.d->tlvs.constEnd();
	while (itr != endItr) {
		out << itr.key() << itr.value().data();
		++itr;
	}
	return out;
}

QDataStream &operator>>(QDataStream &in, FeedbagItem &item)
{
	item.d = new FeedbagItemPrivate();
	in >> item.d->recordName >> item.d->itemId >> item.d->groupId >> item.d->itemType;
	int count;
	in >> count;
	for (int i = 0; i < count; ++i) {
		quint16 id;
		QByteArray data;
		in >> id >> data;
		item.d->tlvs.insert(id, data);
	}
	return in;
}

QDebug &operator<<(QDebug &stream, const FeedbagItem &item)
{
	const FeedbagItemPrivate *d = item.d;
	QString name = qPrintable(d->recordName);
	if (!name.isEmpty())
		stream.nospace() << "Name: " << name << "; type: ";
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

static bool handlerLessThan(FeedbagItemHandler *lhs, FeedbagItemHandler *rhs)
{
	return lhs->priority() > rhs->priority();
}

void FeedbagPrivate::handleItem(FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	Q_Q(Feedbag);
	if (!handlers.contains(item.type())) {
//		debug() << "The feedbag item ignored:" << item;
		return;
	}
	quint16 id = item.d->id();
	// Iterator on a ItemsHash that contains feedbag items with matched type.
	QHash<quint16, ItemsHash>::iterator itemsGroupItr = items.find(item.type());
	bool isItemGroupItrCreated = itemsGroupItr != items.end();
	// Is the item in the feedbag list?
	bool isInList = isItemGroupItrCreated && itemsGroupItr->contains(id);
	// Prepare isInList flag of the item.
	if (type == Feedbag::Add || (type == Feedbag::AddModify && !isInList))
		item.d->isInList = error == FeedbagError::NoError;
	else if (type == Feedbag::Remove)
		item.d->isInList = error != FeedbagError::NoError;
	else
		item.d->isInList = true;
	// Handle the item.
	bool found = false;
	QList<FeedbagItemHandler*> suitableHandlers = handlers.values(item.type());
	if (suitableHandlers.count() > 1)
		qSort(suitableHandlers.begin(), suitableHandlers.end(), &handlerLessThan);
	foreach (FeedbagItemHandler *handler, suitableHandlers)
		found = handler->handleFeedbagItem(q, item, type, error) || found;
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
		// Update the feedbag list.
		if (type == Feedbag::Remove) {
			if (isItemGroupItrCreated) {
				itemsGroupItr->remove(id);
				if (itemsGroupItr->isEmpty())
					items.erase(itemsGroupItr);
			}
		} else {
			if (!isItemGroupItrCreated)
				itemsGroupItr = items.insert(item.type(), ItemsHash());
			itemsGroupItr->insertMulti(id, item);
		}
		// Update the feedbag config.
		Status::Type status = account->status().type();
		if (status != Status::Connecting && status != Status::Offline) {
			Config cfg = q->config("feedbag/cache");
			if (type == Feedbag::Remove)
				cfg.remove(item.d->configId());
			else
				cfg.setValue(item.d->configId(), QVariant::fromValue(item));
			//lastUpdateTime = QDateTime::currentDateTime().toTime_t();
			//cfg.setValue("lastUpdateTime", lastUpdateTime);
		}
	}
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

FeedbagItemPrivate *FeedbagPrivate::getFeedbagItemPrivate(const SNAC &snac)
{
	QString recordName = snac.read<QString, quint16>(Util::utf8Codec());
	quint16 groupId = snac.read<quint16>();
	quint16 itemId = snac.read<quint16>();
	quint16 itemType = snac.read<quint16>();
	if (!handlers.contains(itemType)) {
		// TODO: add better debugging.
		debug() << "The feedbag item ignored with type" << itemType << "and name" << recordName;
		snac.skipData(snac.read<quint16>());
		return 0;
	}
	FeedbagItemPrivate *item = new FeedbagItemPrivate(q_func(), itemType, itemId, groupId, recordName);
	item->tlvs = snac.read<DataUnit, quint16>().read<TLVMap>();
	return item;
}

Feedbag::Feedbag(IcqAccount *acc):
	QObject(acc), d(new FeedbagPrivate(acc, this))
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
	QList<SNACInfo> m_initSnacs;
	m_initSnacs << SNACInfo(ListsFamily, ListsCliCheck)
			<< SNACInfo(ListsFamily, ListsCliRequest)
			<< SNACInfo(ListsFamily, ListsCliReqLists)
			<< SNACInfo(ListsFamily, ListsGotList);
	acc->connection()->registerInitializationSnacs(m_initSnacs);
	Config cfg = config("feedbag");
	d->lastUpdateTime = cfg.value("lastUpdateTime", 0);
	cfg.beginGroup("cache");
	foreach (const QString &itemIdStr, cfg.childKeys()) {
		FeedbagItem item = cfg.value<FeedbagItem>(itemIdStr);
		if (item.isNull())
			continue;
		item.d->feedbag = this;
		QHash<quint16, ItemsHash>::iterator itemsItr = d->items.find(item.type());
		if (itemsItr == d->items.end())
			itemsItr = d->items.insert(item.type(), ItemsHash());
		itemsItr->insert(item.d->id(), item);
	}
	cfg.endGroup();
	connect(acc, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
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
		QList<FeedbagItem> items = d->items.value(type).values(id);
		foreach (const FeedbagItem &item, items) {
			if (item.groupId() == group)
				return item;
		}
	}
	if (flags & CreateItem) {
		if (flags & 0x0002) // GenerateId flag
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
		foreach (const FeedbagItem &item, d->items.value(type)) {
			if (item.groupId() == group) {
				bool found = type != SsiGroup ?
							 n == getCompressedName(item.name()) :
							 name.compare(item.name(), Qt::CaseInsensitive) == 0;
				if (found)
					return item;
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
	if (!(flags & DontLoadLocal))
		items = d->items.value(type).values(id);
	if (items.isEmpty() && (flags & CreateItem)) {
		if (flags & 0x0002) // GenerateId flag
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
		foreach (const FeedbagItem &item, d->items.value(type)) {
			bool found = type != SsiGroup ?
						 n == getCompressedName(item.name()) :
						 name.compare(item.name(), Qt::CaseInsensitive) == 0;
			if (found) {
				items << item;
				if (flags & ReturnOne)
					return items;
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
	foreach (const ItemsHash &hash, d->items) {
		foreach (const FeedbagItem &item, hash) {
			if ((item.type() != SsiGroup && item.groupId() == groupId) ||
				(item.type() == SsiGroup && groupId == 0 && item.groupId() != 0))
			{
				items << item;
			}
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
	if (!(flags & DontLoadLocal))
		items = d->items.value(type).values();
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
	while (value == 0 || d->items.contains(type << 16 | value)) {
		value = d->generateId();
	}
	return value;
}

void Feedbag::registerHandler(FeedbagItemHandler *handler)
{
	const QSet<quint16> &types = handler->types();
	foreach (quint16 type, types)
		d->handlers.insertMulti(type, handler);
	if (types.contains(SsiGroup)) {
		foreach (const FeedbagItem &item, d->items.value(SsiGroup)) {
			if (types.contains(item.type()))
				handler->handleFeedbagItem(this, item, AddModify, FeedbagError::NoError);
		}
	}
	QHash<quint16, ItemsHash>::const_iterator itr = d->items.constBegin();
	QHash<quint16, ItemsHash>::const_iterator endItr = d->items.constEnd();
	while (itr != endItr) {
		if (itr.key() != SsiGroup) {
			foreach (const FeedbagItem &item, *itr) {
				if (types.contains(item.type()))
					handler->handleFeedbagItem(this, item, AddModify, FeedbagError::NoError);
			}
		}
		++itr;
	}
}

IcqAccount *Feedbag::account() const
{
	return d->account;
}

Config Feedbag::config()
{
	return Config(QString("%1.%2/feedbag")
				  .arg(d->account->protocol()->id())
				  .arg(d->account->id()));
}

Config Feedbag::config(const QString &group)
{
	return config().group(group);
}

void Feedbag::handleSNAC(AbstractConnection *conn, const SNAC &sn)
{
	Q_ASSERT(conn == d->conn);
	switch ((sn.family() << 16) | sn.subtype()) {
	case ListsFamily << 16 | ListsError: {
		 ProtocolError error(sn);
		 debug() << QString("Error (%1, %2): %3")
				 .arg(error.code(), 2, 16)
				 .arg(error.subcode(), 2, 16)
				 .arg(error.errorString());
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
			emit reloadingStarted();
			d->items.clear();
			d->limits.clear();
			d->firstPacket = false;
		}
		quint8 version = sn.read<quint8>();
		quint16 count = sn.read<quint16>();
		bool isLast = !(sn.flags() & 0x0001);
		debug() << "SSI: number of entries is" << count << "version is" << version;
		for (uint i = 0; i < count; i++) {	
			FeedbagItemPrivate *itemPrivate = d->getFeedbagItemPrivate(sn);
			if (itemPrivate) {
				FeedbagItem item(itemPrivate);
				d->handleItem(item, AddModify, FeedbagError::NoError);
			}
		}
		if (isLast) {
			d->firstPacket = true;
			d->lastUpdateTime = sn.read<quint32>();
			d->account->config().remove("feedbag"); // TODO: remove it.
			Config cfg = config();
			cfg.remove("feedbag");
			cfg = cfg.group("feedbag");
			cfg.setValue("lastUpdateTime", d->lastUpdateTime);
			cfg.beginGroup("cache");
			foreach (const ItemsHash &hash, d->items)
				foreach (const FeedbagItem &item, hash)
					cfg.setValue(item.d->configId(), QVariant::fromValue(item));
			cfg.endGroup();
			d->finishLoading();
		}
		break;
	}
	case ListsFamily << 16 | ListsUpdateGroup: // Server sends contact list updates
	case ListsFamily << 16 | ListsAddToList: // Server sends new items
	case ListsFamily << 16 | ListsRemoveFromList: { // Items have been removed
		debug() << Q_FUNC_INFO << sn.data().toHex();
		while (sn.dataSize() != 0) {
			FeedbagItemPrivate *itemPrivate = d->getFeedbagItemPrivate(sn);
			if (itemPrivate) {
				FeedbagItem item(itemPrivate);
				d->handleItem(item, static_cast<ModifyType>(sn.subtype()), FeedbagError::NoError);
			}
		}
		break;
	}
	case ListsFamily << 16 | ListsAck: {
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
		TLVMap tlvs = sn.read<TLVMap>();
		if (tlvs.contains(0x04)) {
			DataUnit data = tlvs.value(0x04);
			while (data.dataSize() >= 2)
				d->limits << data.read<quint16>();
		}
		break;
	}
	}
}

void Feedbag::statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous)
{
	if (current == Status::Offline && previous != Status::Offline)
		d->ssiQueue.clear();
}

FeedbagItemHandler::~FeedbagItemHandler()
{
}

FeedbagItemHandler::FeedbagItemHandler(quint16 priority) :
	m_priority(priority)
{
}

} } // namespace qutim_sdk_0_3::oscar

