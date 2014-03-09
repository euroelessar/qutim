/*

    Copyright (c) 2010 by Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "otrcrypt.h"
#include <qutim/config.h>
#include <qutim/icon.h>
#include <qutim/contact.h>
#include <QtDebug>

using namespace qutim_sdk_0_3;

enum OtrState {
	StartConversation,
	EndConversation,
	VerifyFingerprint,
	ShowSessionID,
	ShowFingerprint
};

struct OtrStateActionInfo
{
	OtrState state;
	ChatUnit *unit;
};

struct OtrPolicyActionInfo
{
	int policy;
	ChatUnit *unit;
};

struct OtrActionInfo
{
	QActionGroup *group;
	ChatUnit *unit;
};

Q_DECLARE_METATYPE(OtrStateActionInfo)
Q_DECLARE_METATYPE(OtrPolicyActionInfo)
Q_DECLARE_METATYPE(OtrActionInfo)

static OTRCrypt *self = NULL;

OtrActionGenerator::OtrActionGenerator()
    : ActionGenerator(Icon("dialog-password"), QT_TRANSLATE_NOOP("OTRCrypt", "OTR"), 0)
{
}

void OtrActionGenerator::createImpl(QAction *action, QObject *obj) const
{
	QMenu *actionMenu = new QMenu();
	OtrStateActionInfo stateInfo;
	stateInfo.unit = qobject_cast<ChatUnit*>(obj);
	OTRCrypt::instance()->ensureClosure(stateInfo.unit);
	QAction *startSessionAction = actionMenu->addAction(Icon("security-high"), OTRCrypt::tr("Start private Conversation"));
	stateInfo.state = StartConversation;
	startSessionAction->setData(qVariantFromValue(stateInfo));
	QAction *endSessionAction = actionMenu->addAction(Icon("security-low"), OTRCrypt::tr("End private Conversation"));
	stateInfo.state = EndConversation;
	endSessionAction->setData(qVariantFromValue(stateInfo));
	actionMenu->insertSeparator(NULL);
	QAction *verifyAction = actionMenu->addAction(Icon("security-medium"), OTRCrypt::tr("Verify Fingerprint"));
	stateInfo.state = VerifyFingerprint;
	verifyAction->setData(qVariantFromValue(stateInfo));
	QAction *sessionIdAction = actionMenu->addAction(OTRCrypt::tr("Show secure Session ID"));
	stateInfo.state = ShowSessionID;
	sessionIdAction->setData(qVariantFromValue(stateInfo));
	QAction *fingerprintAction = actionMenu->addAction(OTRCrypt::tr("Show own Fingerprint"));
	stateInfo.state = ShowFingerprint;
	fingerprintAction->setData(qVariantFromValue(stateInfo));
	action->setMenu(actionMenu);
	QMenu *settingsMenu = actionMenu->addMenu(OTRCrypt::tr("Personal settings"));
	OtrActionInfo info;
	info.unit = stateInfo.unit;
	info.group = new QActionGroup(action);
	info.group->setExclusive(true);
	OtrPolicyActionInfo policyInfo;
	policyInfo.unit = stateInfo.unit;
	struct {
		QString text;
		int value;
	} types[] = {
		{ OTRCrypt::tr("System settings"), -1 },
		{ OTRCrypt::tr("OTR disabled"),    OTRL_POLICY_NEVER },
		{ OTRCrypt::tr("Manual"),          OTRL_POLICY_MANUAL },
		{ OTRCrypt::tr("Auto"),            OTRL_POLICY_OPPORTUNISTIC },
		{ OTRCrypt::tr("Force OTR"),       OTRL_POLICY_REQUIRE_ENCRYPTION }
	};
	for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); ++i) {
		QAction *action = info.group->addAction(types[i].text);
		action->setCheckable(true);
		action->setChecked(i == 0);
		policyInfo.policy = types[i].value;
		action->setData(qVariantFromValue(policyInfo));
	}
	action->setData(qVariantFromValue(info));
	settingsMenu->addActions(info.group->actions());
	actionMenu->addMenu(settingsMenu);
	QObject::connect(actionMenu, SIGNAL(triggered(QAction*)), OTRCrypt::instance(), SLOT(onActionTriggered(QAction*)));
}

void OtrActionGenerator::showImpl(QAction *action, QObject *obj)
{
	OtrActionInfo info = action->data().value<OtrActionInfo>();
	Q_ASSERT(obj == info.unit);
	
	OtrClosure *closure = OTRCrypt::instance()->getClosure(info.unit);
	OtrMessaging *otr = closure->getMessaging();
	TreeModelItem item(info.unit);
	QString stateString(OTRCrypt::tr("OTR Plugin [%1]").arg(
	                        otr->getMessageStateString(item.m_account_name,
	                                                   item.m_item_name,
	                                                   item)
	                        ));
	action->setText(stateString);
	
	MessageState state = otr->getMessageState(item.m_account_name,
	                                          item.m_item_name,
	                                          item);

	QList<QAction*> actions = action->menu()->actions();
	QAction *verifyAction = actions.at(VerifyFingerprint);
	QAction *sessionIdAction = actions.at(ShowSessionID);
	QAction *startSessionAction = actions.at(StartConversation);
	QAction *endSessionAction = actions.at(EndConversation);
	if (state == MessageStateEncrypted) {
		verifyAction->setEnabled(true);
		sessionIdAction->setEnabled(true);
		startSessionAction->setEnabled(false);
		endSessionAction->setEnabled(true);
	} else if (state == MessageStatePlainText) {
		verifyAction->setEnabled(false);
		sessionIdAction->setEnabled(false);
		startSessionAction->setEnabled(true);
		endSessionAction->setEnabled(false);
	} else { // finished, unknown
		startSessionAction->setEnabled(true);
		endSessionAction->setEnabled(true);
		verifyAction->setEnabled(false);
		sessionIdAction->setEnabled(false);
	}
	
	const int policy = closure->getPolicy();
	foreach(QAction *act, info.group->actions()) {
		OtrPolicyActionInfo info = act->data().value<OtrPolicyActionInfo>();
		if(policy == info.policy)
			act->setChecked(true);
	}

	if (otr->getPolicy() < PolicyEnabled) {
		startSessionAction->setEnabled(false);
		endSessionAction->setEnabled(false);
	}
}

OTRCrypt::OTRCrypt() : m_notify(false)
{
	self = this;
}

OTRCrypt::~OTRCrypt()
{
	self = NULL;
}

void OTRCrypt::init()
{
	addAuthor(QLatin1String("euroelessar"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Off-the-Record Messaging"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin provides encryption of messages by OTR"),
			PLUGIN_VERSION(0, 0, 1, 0),
			ExtensionIcon("dialog-password"));
	setCapabilities(Loadable);
	OTRL_INIT;
}

bool OTRCrypt::load()
{
	Config config;
	config.beginGroup("otr");
	Policy policy = config.value("policy", PolicyAuto);
	m_notify = config.value("notify", true);
	m_state = otrl_userstate_create();
	Policy policies[] = {
	    PolicyOff,
	    PolicyEnabled,
	    PolicyAuto,
	    PolicyRequire,
	    policy
	};
	for (size_t i = 0; i < sizeof(policies) / sizeof(policies[0]); ++i)
		m_connections << new OtrMessaging(policies[i], m_state);
	m_action.reset(new OtrActionGenerator);
	m_preHandler.reset(new OtrMessagePreHandler);
	MessageHandler::registerHandler(m_preHandler.data(),
	                                QLatin1String("PreOTR"),
	                                MessageHandler::HighPriority + 0x1000,
	                                MessageHandler::SenderPriority + 0x1000);
	m_postHandler.reset(new OtrMessagePostHandler);
	MessageHandler::registerHandler(m_postHandler.data(),
	                                QLatin1String("PostOTR"),
	                                MessageHandler::HighPriority - 0x1000,
	                                MessageHandler::SenderPriority - 0x1000);
	MenuController::addAction<Contact>(m_action.data());
	m_settingsItem.reset(new GeneralSettingsItem<OtrSettingsWidget>(
	                         Settings::Plugin,
	                         Icon("dialog-password"),
	                         QT_TRANSLATE_NOOP("OTRCrypt", "OTR Messaging")));
	m_settingsItem->connect(SIGNAL(saved()), this, SLOT(loadSettings()));
	Settings::registerItem(m_settingsItem.data());
	return true;
}

bool OTRCrypt::unload()
{
	Settings::removeItem(m_settingsItem.data());
	m_settingsItem.reset(0);
	m_preHandler.reset(0);
	m_postHandler.reset(0);
	m_action.reset(0);
	qDeleteAll(m_closures);
	m_closures.clear();
	qDeleteAll(m_connections);
	m_connections.clear();
	otrl_userstate_free(m_state);
	return true;
}

void OTRCrypt::loadSettings()
{
	Config config;
	config.beginGroup("otr");
	Policy policy = config.value("policy", PolicyAuto);
	connectionForPolicy(-1)->setPolicy(policy);
	m_notify = config.value("notify", true);
}

OTRCrypt *OTRCrypt::instance()
{
	return self;
}

OtrClosure *OTRCrypt::ensureClosure(qutim_sdk_0_3::ChatUnit *unit)
{
	OtrClosure * &closure = m_closures[unit];
	if (!closure)
		closure = new OtrClosure(unit);
	updateAction(closure);
	return closure;
}

OtrClosure *OTRCrypt::getClosure(ChatUnit *unit)
{
	return m_closures.value(unit);
}

OtrMessaging *OTRCrypt::connectionForPolicy(int policy)
{
	switch (policy) {
    case OTRL_POLICY_NEVER:
		return m_connections[PolicyOff];
    case OTRL_POLICY_MANUAL:
		return m_connections[PolicyEnabled];
    case OTRL_POLICY_OPPORTUNISTIC:
		return m_connections[PolicyAuto];
    case OTRL_POLICY_REQUIRE_ENCRYPTION:
		return m_connections[PolicyRequire];
    default:
		return m_connections.last();
	}
}

bool OTRCrypt::notifyUser() const
{
	return m_notify;
}

void OTRCrypt::updateAction(OtrClosure *closure)
{
	foreach (QAction *action, m_action->actions(closure->unit()))
		m_action->showImpl(action, closure->unit());
}

void OTRCrypt::disableAccount(Account *account)
{
	m_disabledAccounts << account;
}

void OTRCrypt::enableAccount(Account *account)
{
	m_disabledAccounts.remove(account);
}

bool OTRCrypt::isEnabledAccount(Account *account)
{
	return !m_disabledAccounts.contains(account);
}

void OTRCrypt::onActionTriggered(QAction *action)
{
	QVariant data = action->data();
	if (data.userType() == qMetaTypeId<OtrPolicyActionInfo>()) {
		OtrPolicyActionInfo info = data.value<OtrPolicyActionInfo>();
		qDebug() << "setting policy:" << info.policy;
		OtrClosure *closure = ensureClosure(info.unit);
		closure->setPolicy(info.policy);
	} else if (data.userType() == qMetaTypeId<OtrStateActionInfo>()) {
		OtrStateActionInfo info = data.value<OtrStateActionInfo>();
		qDebug() << "setting state:" << info.state;
		OtrClosure *closure = ensureClosure(info.unit);
	    switch (info.state) {
	    case StartConversation:
	        closure->initiateSession(true);
	        break;
	    case EndConversation:
	        closure->endSession(true);
	        break;
	    case VerifyFingerprint:
	        closure->verifyFingerprint(true);
	        break;
	    case ShowSessionID:
	        closure->sessionID(true);
	        break;
	    case ShowFingerprint:
	        closure->fingerprint(true);
	        break;
	    default:
	        qDebug() << "Wow, it is interesting!";
	    }
	}
}

QUTIM_EXPORT_PLUGIN(OTRCrypt)
