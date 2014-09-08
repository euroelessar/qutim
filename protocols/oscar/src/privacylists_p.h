/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef PRIVACYLISTS_H
#define PRIVACYLISTS_H

#include "feedbag.h"
#include <qutim/actiongenerator.h>

namespace qutim_sdk_0_3 {

class Account;
class Contact;

namespace oscar {

class IcqAccount;

enum Visibility
{
	NoVisibility     = 0,
	AllowAllUsers    = 1,
	BlockAllUsers    = 2,
	AllowPermitList  = 3,
	BlockDenyList    = 4,
	AllowContactList = 5
};
class PrivateListActionGenerator : public ActionGenerator
{
public:
	typedef QSharedPointer<PrivateListActionGenerator> Ptr;
	PrivateListActionGenerator(quint16 type, const QIcon &icon,
				const LocalizedString &text1, const LocalizedString &text2);
	virtual ~PrivateListActionGenerator();
	void updateActions(QObject *obj, bool isPrivacyItemAdded) const;
protected:
	virtual void createImpl(QAction *action, QObject *obj) const;
	virtual QObject *generateHelper() const;
private:
	quint16 m_type;
	LocalizedString m_text;
	LocalizedString m_text2;
};

class PrivacyActionGenerator : public ActionGenerator
{
public:
	typedef QSharedPointer<PrivacyActionGenerator> Ptr;
	PrivacyActionGenerator(Visibility visibility);
	virtual ~PrivacyActionGenerator();
protected:
	virtual void createImpl(QAction *action, QObject *obj) const;
	virtual QObject *generateHelper() const;
private:
	Visibility m_visibility;
};

class PrivacyLists : public QObject, public FeedbagItemHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::FeedbagItemHandler)
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
    PrivacyLists();
	static PrivacyLists *instance() { Q_ASSERT(self); return self; }
	bool handleFeedbagItem(const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error) override;
	void setVisibility(IcqAccount *account, int visibility);
	Visibility getCurrentMode(IcqAccount *account, bool invisibleMode);
protected:
	bool eventFilter(QObject *obj, QEvent *e);
private slots:
	void onModifyPrivateList(QAction *action, QObject *object);
	void onModifyPrivacy(QAction *action, QObject *object);
	void accountAdded(qutim_sdk_0_3::Account *account);
	void statusChanged(const qutim_sdk_0_3::Status &status, const qutim_sdk_0_3::Status &previous);
private:
	bool handleVisibility(const FeedbagItem &item, Feedbag::ModifyType type);
	bool handlePrivacyListItem(const FeedbagItem &item, Feedbag::ModifyType type);
private:
	static PrivacyLists *self;
	QHash<quint16, PrivateListActionGenerator::Ptr> contactMenuHash;
	QHash<Visibility, PrivacyActionGenerator::Ptr> accountMenuHash;
	Visibility m_currentVisibility;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // PRIVACYLISTS_H

