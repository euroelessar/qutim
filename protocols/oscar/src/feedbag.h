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

#ifndef FEEDBAG_H
#define FEEDBAG_H

#include "icq_global.h"
#include "snachandler.h"
#include "tlv.h"
#include <QSharedData>
#include <qutim/config.h>

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
	FeedbagItem &operator=(const FeedbagItem &item);
	void add();
	void update();
	void updateOrAdd();
	void remove();
	void clear();
	Feedbag *feedbag() const;
	bool isInList() const;
	void setInList(bool inList);
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
	QPair<quint16, quint16> pairId() const;
	QPair<quint16, QString> pairName() const;
	TLV field(quint16 field) const;
	template<typename T>
	T field(quint16 field, const T &def = T()) const;
	bool containsField(quint16 field) const;
	TLVMap &data();
	const TLVMap &constData() const;
	void setData(const TLVMap &tlvs);
	bool operator==(const FeedbagItem &rhs) const;
protected:
	FeedbagItem(FeedbagItemPrivate *d);
private:
	friend class Feedbag;
	friend class FeedbagPrivate;
	friend class FeedbagItemPrivate;
	friend QDataStream &operator<<(QDataStream &out, const FeedbagItem &item);
	friend QDataStream &operator>>(QDataStream &in, FeedbagItem &item);
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
	setField(TLV(field, d));
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
		ReturnOne = 0x0020,
		DontCheckGroup = 0x0040
	};
	Q_DECLARE_FLAGS(ItemLoadFlags, ItemLoadFlag)
	
	Feedbag(IcqAccount *acc);
	virtual ~Feedbag();
	
	bool event(QEvent *event);
	
	bool removeItem(quint16 type, quint16 id);
	bool removeItem(quint16 type, const QString &name);
	
	FeedbagItem buddyForChange(const QString &uin) const;
	FeedbagItem itemByType(quint16 type, ItemLoadFlags flags = NoFlags) const;
	
	
	FeedbagItem item(quint16 type, quint16 id, quint16 group, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem item(quint16 type, const QString &name, quint16 group, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem item(quint16 type, const QString &name, ItemLoadFlags flags = NoFlags) const;
	
	QList<FeedbagItem> items(quint16 type, const QString &name, ItemLoadFlags flags = NoFlags) const;
	
	FeedbagItem groupItem(quint16 id, ItemLoadFlags flags = NoFlags) const;
	FeedbagItem groupItem(const QString &name, ItemLoadFlags flags = NoFlags) const;
	bool containsItem(quint16 type, quint16 id) const;
	bool containsItem(quint16 type, const QString &name) const;
	
	quint16 uniqueItemId(quint16 type) const;
	
	void registerHandler(FeedbagItemHandler *handler);
	IcqAccount *account() const;
	Config config();
	Config config(const QString &group);
signals:
	void loaded();
	void reloadingStarted();
protected:
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private slots:
	void statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
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
	quint16 priority() { return m_priority; }
protected:
	FeedbagItemHandler(quint16 priority = 50);
	QSet<quint16> m_types;
private:
	quint16 m_priority;
};

} } // namespace qutim_sdk_0_3::oscar

LIBOSCAR_EXPORT QDebug &operator<<(QDebug &stream, const qutim_sdk_0_3::oscar::FeedbagItem &item);
LIBOSCAR_EXPORT QDebug &operator<<(QDebug &stream, qutim_sdk_0_3::oscar::Feedbag::ModifyType type);
Q_DECLARE_INTERFACE(qutim_sdk_0_3::oscar::FeedbagItemHandler, "org.qutim.oscar.FeedbagItemHandler");

#endif // FEEDBAG_H

