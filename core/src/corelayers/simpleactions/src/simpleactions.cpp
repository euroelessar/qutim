/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "simpleactions.h"
#include <qutim/contact.h>
#include "simpletagseditor/simpletagseditor.h"
#include <qutim/systemintegration.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/chatunit.h>
#include <qutim/protocol.h>
#include <qutim/conference.h>
#include <qutim/utils.h>
#include <qutim/servicemanager.h>
#include <qutim/notification.h>
#include <QClipboard>
#include <QApplication>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QMessageBox>

Q_DECLARE_METATYPE(QWeakPointer<QAction>)

namespace Core {

using namespace qutim_sdk_0_3;

typedef QWeakPointer<QAction> ActionPtr;

namespace AddRemove
{

void checkContact(QAction *action, Contact *contact)
{
	action->setEnabled(contact->account()->status() != Status::Offline);
	bool isInList = contact->isInList();
	action->setText(isInList ? QT_TRANSLATE_NOOP("AddContact", "Remove from roster") :
							   QT_TRANSLATE_NOOP("AddContact", "Add to roster"));
	action->setIcon(isInList ? Icon("list-remove") :
							   Icon("list-add"));
}

}

namespace JoinLeave
{

void checkConference(QAction *action, Conference *room)
{
	action->setEnabled(room->account()->status() != Status::Offline);
	action->setText(!room->isJoined() ? QT_TRANSLATE_NOOP("Jabber", "Join conference") :
										QT_TRANSLATE_NOOP("Jabber", "Leave conference"));
	action->setIcon(!room->isJoined()  ? Icon("im-user") :
										 Icon("im-user-offline"));
}

}

class CopyIdGenerator : public ActionGenerator
{
public:
	CopyIdGenerator(QObject *obj) :
		ActionGenerator(Icon("edit-copy"),QT_TRANSLATE_NOOP("ContactList", "Copy id to clipboard"),obj,SLOT(onCopyIdTriggered(QObject*)))
	{
		setType(ActionTypeContactList|ActionTypeAdditional);
	}
};

SimpleActions::SimpleActions()
{
	m_tagEditGen.reset(new ActionGenerator(Icon("feed-subscribe"),
										   QT_TRANSLATE_NOOP("ContactList", "Edit tags"),
										   this, SLOT(onTagsEditAction(QObject*))));
	MenuController::addAction<Contact>(m_tagEditGen.data());

	m_copyIdGen.reset(new CopyIdGenerator(this));
	m_copyIdGen->subscribe(this, SLOT(onCopyIdCreated(QAction*,QObject*)));
	m_copyIdGen->setPriority(-100);
	MenuController::addAction<ChatUnit>(m_copyIdGen.data());

	m_contactRenameGen.reset(new ActionGenerator(Icon("user-properties"),
												 QT_TRANSLATE_NOOP("ContactList", "Rename contact"),
												 this, SLOT(onContactRenameAction(QObject*))));
	MenuController::addAction<Contact>(m_contactRenameGen.data());

	m_showInfoGen.reset(new ActionGenerator(Icon("dialog-information"),
											QT_TRANSLATE_NOOP("ContactInfo", "Information unavailable"),
											this,
											SLOT(onShowInfoAction(QObject*))
											));
	m_showInfoGen->setType(ActionTypeContactList |
						   ActionTypeChatButton |
						   ActionTypeContactInfo | 0x7000);
	m_showInfoGen->subscribe(this, SLOT(onShowInfoActionCreated(QAction*,QObject*)));
	MenuController::addAction<Buddy>(m_showInfoGen.data());
	MenuController::addAction<Account>(m_showInfoGen.data());

	m_contactAddRemoveGen.reset(new ActionGenerator(QIcon(),
													QT_TRANSLATE_NOOP("AddContact", "Unavailable"),
													this, SLOT(onContactAddRemoveAction(QObject*))));
	m_contactAddRemoveGen->setType(ActionTypeAdditional);
	m_contactAddRemoveGen->subscribe(this, SLOT(onContactAddRemoveActionCreated(QAction*,QObject*)));
	MenuController::addAction<Contact>(m_contactAddRemoveGen.data());

	m_disableSound.reset(new ActionGenerator(QIcon(),
											 QT_TRANSLATE_NOOP("ContactList", "Enable/disable sound"),
											 this, SLOT(onDisableSoundAction(QAction*))));
	m_disableSound->setCheckable(true);
	m_disableSound->subscribe(this, SLOT(onDisableSoundActionCreated(QAction*,QObject*)));
	connect(NotificationManager::instance(),
			SIGNAL(backendCreated(QByteArray,qutim_sdk_0_3::NotificationBackend*)),
			SLOT(onNotificationBackendCreated(QByteArray)));
	connect(NotificationManager::instance(),
			SIGNAL(backendDestroyed(QByteArray,qutim_sdk_0_3::NotificationBackend*)),
			SLOT(onNotificationBackendDestroyed(QByteArray)));
	QObject *contactList = ServiceManager::getByName("ContactList");
	if (contactList)
		QMetaObject::invokeMethod(contactList, "addButton", Q_ARG(ActionGenerator*, m_disableSound.data()));

	foreach (Protocol *proto, Protocol::all()) {
		foreach (Account *acc, proto->accounts())
			onAccountCreated(acc);
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	}
	connect(NotificationManager::instance(), SIGNAL(backendStateChanged(QByteArray,bool)),
			SLOT(onNotificationBackendStateChanged(QByteArray,bool)));

	m_joinGroupLeaveGen.reset(new ActionGenerator(Icon("im-user"), QT_TRANSLATE_NOOP("Jabber", "Join conference"),
												  this, SLOT(onJoinLeave(QObject*))));
	m_joinGroupLeaveGen->subscribe(this, SLOT(onJoinLeaveActionCreated(QAction*, QObject*)));
	MenuController::addAction<Conference>(m_joinGroupLeaveGen.data());
}

SimpleActions::~SimpleActions()
{
	m_disableSound.reset(0);
}

void SimpleActions::onTagsEditAction(QObject *controller)
{
	Contact *contact = qobject_cast<Contact*>(controller);
	if (!contact)
		return;
	SimpleTagsEditor *editor = new SimpleTagsEditor (contact);
	centerizeWidget(editor);
	editor->load();
	SystemIntegration::show(editor);
}

void SimpleActions::onCopyIdCreated(QAction *action, QObject *obj)
{
	ChatUnit *unit = sender_cast<ChatUnit*>(obj);
	QString id =  unit->account()->protocol()->data(Protocol::ProtocolIdName).toString();
	action->setText(QObject::tr("Copy %1 to clipboard").arg(id));
}

void SimpleActions::onCopyIdTriggered(QObject *obj)
{
	ChatUnit *unit = sender_cast<ChatUnit*>(obj);
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(unit->id());
}

void SimpleActions::onContactRenameAction(QObject *o)
{
	Contact *contact = sender_cast<Contact*>(o);
	QInputDialog *dialog = new QInputDialog(QApplication::activeWindow());
	dialog->setWindowTitle(tr("Rename contact %1").arg(contact->title()));
    dialog->setLabelText(tr("Input new name for contact %1").arg(contact->title()));
    dialog->setTextValue(contact->name());
	dialog->setProperty("contact", qVariantFromValue(contact));
	SystemIntegration::show(dialog);
	connect(dialog, SIGNAL(textValueSelected(QString)), SLOT(onContactNameSelected(QString)));
	connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
	connect(contact, SIGNAL(destroyed()), dialog, SLOT(deleteLater()));
}

void SimpleActions::onContactNameSelected(const QString &name)
{
	Contact *contact = sender()->property("contact").value<Contact*>();
	contact->setName(name);
}

void SimpleActions::onShowInfoAction(QObject *obj)
{
	QObject *contactInfo = ServiceManager::getByName("ContactInfo");
	QMetaObject::invokeMethod(contactInfo, "show", Q_ARG(QObject*, obj));
}

static void updatInfoAction(QAction *action, InfoRequestFactory::SupportLevel level)
{
	action->setVisible(level > InfoRequestFactory::Unavailable);
	action->setText(level == InfoRequestFactory::ReadOnly ?
						QT_TRANSLATE_NOOP("ContactInfo", "Show information") :
						QT_TRANSLATE_NOOP("ContactInfo", "Edit information"));
}

void SimpleActions::onShowInfoActionCreated(QAction *action, QObject *controller)
{
	InfoObserver *observer = new InfoObserver(controller);
	updatInfoAction(action, observer->supportLevel());
	observer->setProperty("action", qVariantFromValue<ActionPtr>(action));
	connect(observer, SIGNAL(supportLevelChanged(qutim_sdk_0_3::InfoRequestFactory::SupportLevel)),
			SLOT(onInformationSupportLevelChanged(qutim_sdk_0_3::InfoRequestFactory::SupportLevel)));
	connect(action, SIGNAL(destroyed()), observer, SLOT(deleteLater()));
}

void SimpleActions::onInformationSupportLevelChanged(InfoRequestFactory::SupportLevel level)
{
	ActionPtr action = sender()->property("action").value<ActionPtr>();
	if (!action)
		return;
	updatInfoAction(action.data(), level);
}

void SimpleActions::onContactAddRemoveActionCreated(QAction *a, QObject *o)
{
	Contact *contact = sender_cast<Contact*>(o);
	a->setProperty("contact", qVariantFromValue(contact));
	AddRemove::checkContact(a, contact);
	connect(contact, SIGNAL(inListChanged(bool)),
			this, SLOT(inListChanged(bool)),
			Qt::UniqueConnection);
	connect(a, SIGNAL(destroyed()),
			SLOT(onContactAddRemoveActionDestroyed()));
}

void SimpleActions::onContactAddRemoveAction(QObject *obj)
{
	Contact *contact = sender_cast<Contact*>(obj);
	if(contact->isInList()) {
		int ret = QMessageBox::question(qApp->activeWindow(),
										QT_TRANSLATE_NOOP("AddContact", "Remove contact"),
										tr("Are you sure you want to delete a contact %1 from the roster?").arg(contact->title()),
										QMessageBox::Ok,
										QMessageBox::Cancel);
		if(ret != QMessageBox::Ok)
			return;

	}
	contact->setInList(!contact->isInList());
}

void SimpleActions::onContactAddRemoveActionDestroyed()
{
	Contact *contact = sender()->property("contact").value<Contact*>();
	if (contact && m_contactAddRemoveGen->actions(contact).isEmpty()) {
		disconnect(contact, SIGNAL(inListChanged(bool)),
				   this, SLOT(inListChanged(bool)));
	}
}

void SimpleActions::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
}

void SimpleActions::onAccountStatusChanged(const qutim_sdk_0_3::Status &)
{
	QMap<QObject*, QAction*> actions = m_contactAddRemoveGen->actions();
	QMap<QObject*, QAction*>::const_iterator it = actions.constBegin();
	for(; it != actions.constEnd(); it++)
		AddRemove::checkContact(it.value(), sender_cast<Contact*>(it.key()));
}

void SimpleActions::inListChanged(bool)
{
	Contact *c = qobject_cast<Contact*>(sender());
	Q_ASSERT(c);
	QList<QAction*> actions = m_contactAddRemoveGen->actions(c);
	foreach (QAction *a, actions)
		AddRemove::checkContact(a, c);
}

static QIcon soundIcon(bool isEnabled)
{
#ifdef Q_WS_MAEMO_5
	return Icon(QLatin1String(isEnabled ? "general_speaker" : "call_speaker_muted"));
#else
	return Icon(QLatin1String(isEnabled ? "audio-volume-high" : "audio-volume-muted"));
#endif
}

void SimpleActions::onDisableSoundActionCreated(QAction *action, QObject *obj)
{
	Q_UNUSED(obj);
	bool isEnabled = NotificationManager::isBackendEnabled("Sound");
	action->setChecked(isEnabled);
	action->setIcon(soundIcon(isEnabled));
	action->setVisible(NotificationBackend::allTypes().contains("Sound"));
}

void SimpleActions::onDisableSoundAction(QAction *action)
{
	NotificationManager::setBackendState("Sound", action->isChecked());
}

void SimpleActions::onNotificationBackendStateChanged(const QByteArray &type, bool enabled)
{
	if (type != "Sound")
		return;
	foreach (QAction *action, m_disableSound->actions())
		action->setIcon(soundIcon(enabled));
}

void SimpleActions::onNotificationBackendCreated(const QByteArray &type)
{
	setDisableSoundActionVisibility(type, true);
}

void SimpleActions::onNotificationBackendDestroyed(const QByteArray &type)
{
	setDisableSoundActionVisibility(type, false);
}

void SimpleActions::onJoinLeave(QObject *obj)
{
	Conference *room = sender_cast<Conference*>(obj);
	if (!room->isJoined())
		room->join();
	else
		room->leave();
}

void SimpleActions::onJoinLeaveActionCreated(QAction *action, QObject *obj)
{
	Conference *room = sender_cast<Conference*>(obj);
	connect(room, SIGNAL(joinedChanged(bool)), SLOT(onJoinedChanged(bool)));
	JoinLeave::checkConference(action, room);
}

void SimpleActions::onJoinedChanged(bool)
{
	Conference *room = sender_cast<Conference*>(sender());
	foreach (QAction *action, m_joinGroupLeaveGen->actions(room))
		JoinLeave::checkConference(action, room);
}

void SimpleActions::setDisableSoundActionVisibility(const QByteArray &type, bool visible)
{
	if (type != "Sound")
		return;
	// TODO: remove the generator from the contact list toolbar instead.
	foreach (QAction *action, m_disableSound->actions())
		action->setVisible(visible);
}

} // namespace Core

