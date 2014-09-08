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
#include "privacylists_p.h"
#include "icqcontact.h"
#include "icqaccount.h"
#include "icqprotocol.h"
#include <qutim/icon.h>

namespace qutim_sdk_0_3 {

namespace oscar {

const int actionType = ActionTypeContactList;

PrivacyLists *PrivacyLists::self = 0;

PrivateListActionGenerator::PrivateListActionGenerator(quint16 type, const QIcon &icon,
				const LocalizedString &text1, const LocalizedString &text2) :
	ActionGenerator(icon, text1, PrivacyLists::instance(), SLOT(onModifyPrivateList(QAction*,QObject*))),
	m_type(type), m_text(text1), m_text2(text2)
{
	setPriority(45);
	setType(actionType);
}

PrivateListActionGenerator::~PrivateListActionGenerator()
{
}

void PrivateListActionGenerator::updateActions(QObject *obj, bool isPrivacyItemAdded) const
{
	foreach (QAction *action, actions(obj))
		action->setText(isPrivacyItemAdded ? m_text2 : m_text);
}

void PrivateListActionGenerator::createImpl(QAction *action, QObject *obj) const
{
	IcqContact *contact = qobject_cast<IcqContact*>(obj);
	if (!contact)
		return;
	IcqAccount *account = contact->account();
	action->setText(account->feedbag()->containsItem(m_type, contact->id()) ? m_text2 : m_text);
}

QObject *PrivateListActionGenerator::generateHelper() const
{
	QAction *action = prepareAction(new QAction(NULL));
	action->setProperty("itemType", m_type);
	return action;
}

void PrivacyLists::onModifyPrivateList(QAction *action, QObject *object)
{
	quint16 type = action->property("itemType").toInt();
	Q_ASSERT(qobject_cast<IcqContact*>(object) != 0);
	IcqContact *contact = reinterpret_cast<IcqContact*>(object);
	FeedbagItem item = contact->account()->feedbag()->item(type, contact->id(), 0, Feedbag::GenerateId);
	if (item.isInList())
		item.remove();
	else
		item.add();
}

static LocalizedString visibilityToString(Visibility visibility)
{
	switch (visibility) {
	case AllowAllUsers:
		return QT_TRANSLATE_NOOP("Privacy", "Visible for all");
	case BlockAllUsers:
		return QT_TRANSLATE_NOOP("Privacy", "Invisible for all");
	case AllowPermitList:
		return QT_TRANSLATE_NOOP("Privacy", "Visible only for visible list");
	case BlockDenyList:
		return QT_TRANSLATE_NOOP("Privacy", "Invisible only for invisible list");
	case AllowContactList:
		return QT_TRANSLATE_NOOP("Privacy", "Visible only for contact list");
	default:
		return QT_TRANSLATE_NOOP("Privacy", "Unknown privacy");
	}
}

PrivacyActionGenerator::PrivacyActionGenerator(Visibility visibility) :
	ActionGenerator(QIcon(), LocalizedString(visibilityToString(visibility)),
					PrivacyLists::instance(), SLOT(onModifyPrivacy(QAction*,QObject*))),
	m_visibility(visibility)
{
	setPriority(100 - visibility);
}

PrivacyActionGenerator::~PrivacyActionGenerator()
{
}

void PrivacyActionGenerator::createImpl(QAction *action, QObject *obj) const
{
	IcqAccount *account = qobject_cast<IcqAccount*>(obj);
	if (!account)
		return;

	bool isInvisible = account->status() == Status::Invisible;
	if (isInvisible && m_visibility != AllowPermitList && m_visibility != BlockAllUsers) {
		action->setVisible(false);
		return;
	}
	action->setVisible(true);

	FeedbagItem item = account->feedbag()->itemByType(SsiVisibility);
	Visibility curVisibility = item.isNull() ? NoVisibility : static_cast<Visibility>(item.field<quint8>(0x00CA));
	if (curVisibility == NoVisibility)
		curVisibility = PrivacyLists::instance()->getCurrentMode(account, isInvisible);
	action->setChecked(curVisibility == m_visibility);
}

QObject *PrivacyActionGenerator::generateHelper() const
{
	static QActionGroup group(0);
	QAction *action = prepareAction(new QAction(NULL));
	action->setCheckable(true);
	action->setProperty("visibility", m_visibility);
	group.addAction(action);
	return action;
}

void PrivacyLists::onModifyPrivacy(QAction *action, QObject *object)
{
	Q_ASSERT(qobject_cast<IcqAccount*>(object) != 0);
	IcqAccount *account = reinterpret_cast<IcqAccount*>(object);
	// Set new visibility
	Visibility visibility = (Visibility)action->property("visibility").toInt();
	setVisibility(account, visibility);
	// Store it into the config
	Config cfg = account->config("privacy");
	if (account->status() == Status::Invisible) {
		account->setProperty("invisibleMode", visibility);
		cfg.setValue("invisibleMode", visibility);
	} else {
		account->setProperty("visibility", visibility);
		cfg.setValue("visibility", visibility);
	}
}

PrivacyLists::PrivacyLists() :
	FeedbagItemHandler({ SsiPermit, SsiDeny, SsiIgnore, SsiVisibility }, 30)
{
	Q_ASSERT(!self);
	self = this;
	foreach (Account *account, IcqProtocol::instance()->accounts())
		accountAdded(account);
	connect(IcqProtocol::instance(), SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), SLOT(accountAdded(qutim_sdk_0_3::Account*)));
	IcqProtocol::instance()->installEventFilter(this);

	// Create privacy actions for contacts.
#define ADD_PRIVACY_ACTION(Type, IconName, Text1, Text2) \
	contactMenuHash.insert(Type, PrivateListActionGenerator::Ptr( \
			new PrivateListActionGenerator(Type, Icon(IconName), Text1, Text2)))

	ADD_PRIVACY_ACTION(SsiPermit, "im-visible-contact-icq",
					   QT_TRANSLATE_NOOP("ContactList", "Add to visible list"),
					   QT_TRANSLATE_NOOP("ContactList", "Remove from visible list"));
	ADD_PRIVACY_ACTION(SsiDeny, "im-invisible-contact-icq",
					   QT_TRANSLATE_NOOP("ContactList", "Add to invisible list"),
					   QT_TRANSLATE_NOOP("ContactList", "Remove from invisible list"));
	ADD_PRIVACY_ACTION(SsiIgnore, "im-ignore-contact-icq",
					   QT_TRANSLATE_NOOP("ContactList", "Add to ignore list"),
					   QT_TRANSLATE_NOOP("ContactList", "Remove from ignore list"));

#undef ADD_PRIVACY_ACTION

	foreach (const PrivateListActionGenerator::Ptr &action, contactMenuHash)
		MenuController::addAction<IcqContact>(action.data());

	//  Create privacy actions for accounts.
#define ADD_PRIVACY_ACTION(Visibility) \
	accountMenuHash.insert(Visibility, PrivacyActionGenerator::Ptr(new PrivacyActionGenerator(Visibility)))

	ADD_PRIVACY_ACTION(AllowAllUsers);
	ADD_PRIVACY_ACTION(BlockAllUsers);
	ADD_PRIVACY_ACTION(AllowPermitList);
	ADD_PRIVACY_ACTION(BlockDenyList);
	ADD_PRIVACY_ACTION(AllowContactList);

#undef ADD_PRIVACY_ACTION

	if (1) {} else Q_UNUSED(QT_TRANSLATE_NOOP("Menu", "Privacy status"));
	foreach (const PrivacyActionGenerator::Ptr &action, accountMenuHash)
		MenuController::addAction<IcqAccount>(action.data(), QList<QByteArray>() << "Privacy status");
}

bool PrivacyLists::handleFeedbagItem(const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	if (error != FeedbagError::NoError)
		return false;
	switch (item.type()) {
	case SsiVisibility:
		return handleVisibility(item, type);
	case SsiPermit:
	case SsiDeny:
	case SsiIgnore:
		return handlePrivacyListItem(item, type);
	default:
		return false;
	}
}

bool PrivacyLists::handleVisibility(const FeedbagItem &item, Feedbag::ModifyType type)
{
	if (type != Feedbag::Remove) {
		Visibility newVisibility = (Visibility)item.field<quint8>(0x00CA);
		PrivacyActionGenerator::Ptr actionGen = accountMenuHash.value(newVisibility);
		if (actionGen) {
			foreach (QAction *action, actionGen->actions(account()))
				action->setChecked(true);
		}
		m_currentVisibility = newVisibility;
	} else if (m_currentVisibility != NoVisibility) {
		PrivacyActionGenerator::Ptr actionGen = accountMenuHash.value(m_currentVisibility);
		Q_ASSERT(actionGen);
		foreach (QAction *action, actionGen->actions(account()))
			action->setChecked(false);
		m_currentVisibility = NoVisibility;
	}
	return true;
}

bool PrivacyLists::handlePrivacyListItem(const FeedbagItem &item, Feedbag::ModifyType type)
{
	QString name;
	QString icon;
	bool isItemAdded = type != Feedbag::Remove;
	switch (item.type()) {
	case SsiPermit: {
		name = "visible";
		if (isItemAdded) {
			icon = "im-visible-contact-icq";
			qDebug() << item.name() << "has been added to visible list";
		} else {
			qDebug() << item.name() << "has been removed from visible list";
		}
		break;
	}
	case SsiDeny: {
		name = "invisible";
		if (isItemAdded) {
			icon = "im-invisible-contact-icq";
			qDebug() << item.name() << "has been added to invisible list";
		} else {
			qDebug() << item.name() << "has been removed from invisible list";
		}
		break;
	}
	case SsiIgnore: {
		name = "ignore";
		if (isItemAdded) {
			icon = "im-ignore-contact-icq";
			qDebug() << item.name() << "has been added to ignore list";
		} else {
			qDebug() << item.name() << "has been removed from ignore list";
		}
		break;
	default:
		return false;
	}
	}
	IcqContact *contact = account()->getContact(item.name());
	if (!contact)
		return true;
	// Update contact's status
	Status status = contact->status();
	if (!icon.isEmpty()) {
		QVariantHash clientInfo;
		clientInfo.insert("id", name);
		clientInfo.insert("icon", qVariantFromValue(ExtensionIcon(icon)));
		clientInfo.insert("showInTooltip", false);
		clientInfo.insert("priority", 20);
		status.setExtendedInfo(name, clientInfo);
	} else {
		status.removeExtendedInfo(name);
	}
	contact->setStatus(status, false);
	// Update contact's actions.
	contactMenuHash.value(item.type())->updateActions(contact, isItemAdded);
	return true;
}

void PrivacyLists::setVisibility(IcqAccount *account, int visibility)
{
	FeedbagItem item = account->feedbag()->itemByType(SsiVisibility, Feedbag::CreateItem);
	TLV data = item.field(0x00CA);
	if (data.read<quint8>() != visibility) {
		item.setField<quint8>(0x00CA, visibility);
		item.updateOrAdd();
	}
//	TLV data(0x00CA);
//	data.append<quint8>(visibility);
//	item.setField(data);
	// Do we really want to change it?
//	item.setField<qint32>(0x00C9, 0xffffffff);
}

Visibility PrivacyLists::getCurrentMode(IcqAccount *account, bool invisibleMode)
{
	Visibility current;
	QVariant currentVariant = invisibleMode ?
							  account->property("invisibleMode") :
							  account->property("visibility");
	if (currentVariant.isNull()) {
		Config cfg = account->config("privacy");
		if (invisibleMode) {
			current = cfg.value("invisibleMode", AllowPermitList);
			account->setProperty("invisibleMode", current);
		} else {
			current = cfg.value("visibility", AllowContactList);
			account->setProperty("visibility", current);
		}
	} else {
		current = (Visibility)currentVariant.toInt();
	}
	return current;
}

bool PrivacyLists::eventFilter(QObject *obj, QEvent *e)
{
	if (e->type() == ExtendedInfosEvent::eventType() && obj == IcqProtocol::instance()) {
		ExtendedInfosEvent *event = static_cast<ExtendedInfosEvent*>(e);
		QVariantHash extStatus;
		// Visible
		extStatus.insert("id", "visible");
		extStatus.insert("settingsDescription", tr("Show \"visible\" icon if contact in visible list"));
		event->addInfo("visible", extStatus);
		// Invisible
		extStatus.insert("id", "invisible");
		extStatus.insert("settingsDescription", tr("Show \"invisible\" icon if contact in invisible list"));
		event->addInfo("invisible", extStatus);
		// Ignore
		extStatus.insert("id", "ignore");
		extStatus.insert("settingsDescription", tr("Show \"ignore\" icon if contact in ignore list"));
		event->addInfo("ignore", extStatus);
	}
	return QObject::eventFilter(obj, e);
}

void PrivacyLists::accountAdded(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

void PrivacyLists::statusChanged(const qutim_sdk_0_3::Status &status, const qutim_sdk_0_3::Status &previous)
{
	IcqAccount *account = qobject_cast<IcqAccount*>(sender());
	Q_ASSERT(account);
	bool wasOffline = previous == Status::Offline || previous == Status::Connecting;
	bool isOffline = status == Status::Offline || status == Status::Connecting;
	// Update account's actions
	bool isInvisible = status == Status::Invisible;
	if (previous == Status::Connecting || (isInvisible != (previous == Status::Invisible)))
	{
		setVisibility(account, getCurrentMode(account, isInvisible));
		foreach (QAction *action, accountMenuHash.value(AllowAllUsers)->actions(account))
			action->setVisible(!isInvisible);
		foreach (QAction *action, accountMenuHash.value(BlockDenyList)->actions(account))
			action->setVisible(!isInvisible);
		foreach (QAction *action, accountMenuHash.value(AllowContactList)->actions(account))
			action->setVisible(!isInvisible);
	}
	// Update contacts' actions
	if (wasOffline != isOffline) {
		foreach (const PrivateListActionGenerator::Ptr &gen, contactMenuHash) {
			typedef QMap<QObject*, QAction*> Map;
			Map actions = gen->actions();
			Map::iterator itr = actions.begin(), end = actions.end();
			for (; itr != end; ++itr) {
				IcqContact *contact = qobject_cast<IcqContact*>(itr.key());
				if (!contact || contact->account() != account)
					continue;
				(*itr)->setVisible(wasOffline);
			}
		}
	}
}

} } // namespace qutim_sdk_0_3::oscar

