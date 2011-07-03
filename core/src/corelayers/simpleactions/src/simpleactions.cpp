/****************************************************************************
 *  simpleactions.cpp
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "simpleactions.h"
#include <qutim/contact.h>
#include "simpletagseditor/simpletagseditor.h"
#include <qutim/systemintegration.h>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <qutim/account.h>
#include <qutim/chatunit.h>
#include <qutim/protocol.h>
#include <qutim/utils.h>
#include <qutim/servicemanager.h>
#include <qutim/inforequest.h>
#include <qutim/notification.h>
#include <QClipboard>
#include <QApplication>
#include <QDesktopWidget>
#include <QInputDialog>
#include <QMessageBox>

namespace Core {

using namespace qutim_sdk_0_3;

namespace ShowInfo
{
void checkAction(QObject *controller, QAction *action)
{
	if (!controller || !action)
		return;
	InfoRequestCheckSupportEvent info_event;
	qApp->sendEvent(controller, &info_event);
	if (info_event.supportType() != InfoRequestCheckSupportEvent::NoSupport) {
		if (info_event.supportType() == InfoRequestCheckSupportEvent::Read)
			action->setText(QT_TRANSLATE_NOOP("ContactInfo", "Show information"));
		else if (info_event.supportType() == InfoRequestCheckSupportEvent::ReadWrite)
			action->setText(QT_TRANSLATE_NOOP("ContactInfo", "Edit information"));
		action->setEnabled(true);
	} else {
		action->setText(QT_TRANSLATE_NOOP("ContactInfo", "Information unavailable"));
		action->setEnabled(false);
	}
}
}

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
	m_tagEditGen->subscribe(this, SLOT(onTagsEditCreated(QAction*, QObject*)));
	MenuController::addAction<Contact>(m_tagEditGen.data());

	m_copyIdGen.reset(new CopyIdGenerator(this));
	m_copyIdGen->subscribe(this, SLOT(onCopyIdCreated(QAction*,QObject*)));
	m_copyIdGen->setPriority(-100);
	MenuController::addAction<ChatUnit>(m_copyIdGen.data());

	m_contactRenameGen.reset(new ActionGenerator(Icon("user-properties"),
												 QT_TRANSLATE_NOOP("ContactList", "Rename contact"),
												 this, SLOT(onContactRenameAction(QObject*))));
	m_contactRenameGen->subscribe(this, SLOT(onContactRenameCreated(QAction*,QObject*)));
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
	QObject *contactList = ServiceManager::getByName("ContactList");
	if (contactList)
		QMetaObject::invokeMethod(contactList, "addButton", Q_ARG(ActionGenerator*, m_disableSound.data()));
	enableSound(isSoundEnabled());
}

SimpleActions::~SimpleActions()
{

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

void SimpleActions::onTagsEditCreated(QAction *a, QObject *o)
{
	Q_UNUSED(a);
	Q_UNUSED(o);
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


void SimpleActions::onContactRenameCreated(QAction *, QObject *)
{

}

void SimpleActions::onContactRenameAction(QObject *o)
{
	Contact *contact = sender_cast<Contact*>(o);
	QString result = QInputDialog::getText(QApplication::activeWindow(), tr("Rename contact %1").arg(contact->title()),
										   tr("Input new name for contact %1").arg(contact->title()),
										   QLineEdit::Normal,
										   contact->name());
	contact->setName(result);
}

void SimpleActions::onShowInfoAction(QObject *obj)
{
	QObject *contactInfo = ServiceManager::getByName("ContactInfo");
	QMetaObject::invokeMethod(contactInfo, "show", Q_ARG(QObject*, obj));
}

void SimpleActions::onShowInfoActionCreated(QAction *action, QObject *controller)
{
	if(Account *a = qobject_cast<Account*>(controller)) {
		connect(a, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
				this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	}
	ShowInfo::checkAction(controller,action);
}

void SimpleActions::onAccountStatusChanged(const qutim_sdk_0_3::Status &)
{
	QMap<QObject*, QAction*> actions = m_showInfoGen->actions();
	QMap<QObject*, QAction*>::const_iterator it = actions.constBegin();
	for(;it != actions.constEnd(); it++)
		ShowInfo::checkAction(it.key(), it.value());

	actions = m_contactAddRemoveGen->actions();
	it = actions.constBegin();
	for(;it != actions.constEnd(); it++)
		AddRemove::checkContact(it.value(), sender_cast<Contact*>(it.key()));
}

void SimpleActions::onContactAddRemoveActionCreated(QAction *a, QObject *o)
{
	Contact *contact = sender_cast<Contact*>(o);
	AddRemove::checkContact(a, contact);
	connect(contact->account(), SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(contact, SIGNAL(inListChanged(bool)),
			this, SLOT(inListChanged(bool)));
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
	return Icon(QLatin1String(isEnabled ? "audio-volume-high" : "audio-volume-muted"));
}

void SimpleActions::onDisableSoundActionCreated(QAction *action, QObject *obj)
{
	Q_UNUSED(obj);
	bool isEnabled = isSoundEnabled();
	action->setChecked(isEnabled);
	action->setIcon(soundIcon(isEnabled));
}

void SimpleActions::onDisableSoundAction(QAction *action)
{
	bool isEnabled = action->isChecked();
	enableSound(isEnabled);
	action->setIcon(soundIcon(isEnabled));
}

void SimpleActions::enableSound(bool enable)
{
	QByteArray sound("Sound");
	for (int i = 0; i <= Notification::LastType; ++i) {
		Notification::Type type = static_cast<Notification::Type>(i);
		if (enable)
			NotificationRequest::unblockBackend(type, sound);
		else
			NotificationRequest::blockBackend(type, sound);
	}

	Config cfg;
	cfg.beginGroup(QLatin1String("notification"));
	cfg.setValue(QLatin1String("soundEnabled"), enable);
	cfg.endGroup();
}

bool SimpleActions::isSoundEnabled()
{
	Config cfg;
	cfg.beginGroup(QLatin1String("notification"));
	return cfg.value("soundEnabled", true);
}

} // namespace Core

