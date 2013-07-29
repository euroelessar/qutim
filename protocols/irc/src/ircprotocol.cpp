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

#include "ircprotocol_p.h"
#include "ircaccount.h"
#include "ircchannel.h"
#include "ircchannelparticipant.h"
#include "ircconnection.h"
#include "ircchannel_p.h"
#include "ircgroupchatmanager.h"
#include "ui/ircaccountmainsettings.h"
#include "ui/ircaccountnicksettings.h"
#include <qutim/actiongenerator.h>
#include <qutim/statusactiongenerator.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <QStringList>
#include <QRegExp>
#include <QTextDocument>

Q_DECLARE_METATYPE(qutim_sdk_0_3::irc::IrcAccount*)

namespace qutim_sdk_0_3 {

namespace irc {

IrcProtocol *IrcProtocol::self = 0;
bool IrcProtocolPrivate::enableColoring;
QMultiHash<QString, IrcCommandAlias*> IrcProtocolPrivate::aliases;

IrcProtocol::IrcProtocol() :
	d(new IrcProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
	updateSettings();
	IrcAccount::registerLogMsgColor("ERROR", "red");
	IrcAccount::registerLogMsgColor("Notice", "magenta");
	IrcAccount::registerLogMsgColor("MOTD", "green");
	IrcAccount::registerLogMsgColor("Welcome", "green");
	IrcAccount::registerLogMsgColor("Support", "green");
	IrcAccount::registerLogMsgColor("Users", "green");
	IrcAccount::registerLogMsgColor("Away", "green");
	IrcCommandAlias::initStandartAliases();

	Settings::registerItem<IrcAccount>(
			new GeneralSettingsItem<IrcAccountNickSettingsWidget>(
					Settings::Protocol,
					Icon("im-irc"),
					QT_TRANSLATE_NOOP_UTF8("Settings", "User information")));
	Settings::registerItem<IrcAccount>(
			new GeneralSettingsItem<IrcAccountMainSettingsWidget>(
					Settings::Protocol,
					Icon("im-irc"),
					QT_TRANSLATE_NOOP_UTF8("Settings", "Servers")));

	d->autojoinAction = new ActionGenerator(QIcon(), QT_TR_NOOP("Auto-join"),
											this, SLOT(onAutojoinChecked(QObject*)));
	d->autojoinAction->setCheckable(true);
	d->autojoinAction->addHandler(ActionCreatedHandler, this);
	MenuController::addAction<IrcChannel>(d->autojoinAction);
}

IrcProtocol::~IrcProtocol()
{
	self = 0;
}

void IrcProtocol::loadAccounts()
{
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	// Register actions.
	ActionGenerator *gen = new ActionGenerator(QIcon(),
					QT_TRANSLATE_NOOP("IRC", "Show console..."),
					SLOT(showConsole()));
	gen->setPriority(35);
	gen->setType(ActionTypeContactList | 0x2000);
	MenuController::addAction<IrcAccount>(gen);
	gen = new ActionGenerator(QIcon(),
					QT_TRANSLATE_NOOP("IRC", "Search channel..."),
					SLOT(showChannelList()));
	gen->setPriority(35);
	gen->setType(ActionTypeContactList | 0x2000);
	MenuController::addAction<IrcAccount>(gen);

	// Register status actions.
	Status status(Status::Online);
	status.initIcon("irc");
	MenuController::addAction<IrcAccount>(new StatusActionGenerator(status));
	status.setType(Status::Away);
	status.initIcon("irc");
	MenuController::addAction<IrcAccount>(new StatusActionGenerator(status));
	status.setType(Status::Offline);
	status.initIcon("irc");
	MenuController::addAction<IrcAccount>(new StatusActionGenerator(status));
	// Load accounts.
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &network, accounts) {
		IrcAccount *acc = new IrcAccount(network);
		d->accounts_hash.insert(network, acc);
		acc->updateSettings();
		emit accountCreated(acc);
	}
}

QList<Account *> IrcProtocol::accounts() const
{
	QList<Account *> accounts;
	QHash<QString, QPointer<IrcAccount> >::const_iterator it;
	for (it = d->accounts_hash.begin(); it != d->accounts_hash.end(); it++)
		if (!it.value().isNull())
			accounts.append(it.value().data());
	return accounts;
}

Account *IrcProtocol::account(const QString &id) const
{
	return d->accounts_hash.value(id).data();
}

IrcAccount *IrcProtocol::getAccount(const QString &id, bool create)
{
	IrcAccount *account = d->accounts_hash.value(id).data();
	if (!account && create) {
		account = new IrcAccount(id);
		d->accounts_hash.insert(id, account);
		emit accountCreated(account);
	}
	return account;
}

ChatSession *IrcProtocol::activeSession() const
{
	return d->activeSession.data();
}

void IrcProtocol::registerCommandAlias(IrcCommandAlias *alias)
{
	IrcProtocolPrivate::aliases.insert(alias->name(), alias);
}

void IrcProtocol::removeCommandAlias(const QString &name)
{
	qDeleteAll(IrcProtocolPrivate::aliases.values(name));
	IrcProtocolPrivate::aliases.remove(name);
}

void IrcProtocol::removeCommandAlias(IrcCommandAlias *alias)
{
	QHash<QString, IrcCommandAlias*>::iterator itr = IrcProtocolPrivate::aliases.begin();
	QHash<QString, IrcCommandAlias*>::iterator endItr = IrcProtocolPrivate::aliases.end();
	while (itr != endItr) {
		if (*itr == alias) {
			delete alias;
			IrcProtocolPrivate::aliases.erase(itr);
			return;
		}
		++itr;
	}
}

static QRegExp formatRx("(\\002|\\037|\\026|\\017|\\003((\\d{0,2})(,\\d{1,2}|)|))");

QString IrcProtocolPrivate::getColorByMircCode(const QString &code)
{
	static QStringList colors = QStringList()
								<< "white"
								<< "black"
								<< "blue"
								<< "green"
								<< "#FA5A5A" //lightred
								<< "brown"
								<< "purple"
								<< "orange"
								<< "yellow"
								<< "lightgreen"
								<< "cyan"
								<< "lightcyan"
								<< "lightblue"
								<< "pink"
								<< "grey"
								<< "lightgrey";
	bool ok;
	int c = code.toInt(&ok);
	if (ok)
		return colors.value(c);
	else
		return QString();
}

QString IrcProtocol::ircFormatToHtml(const QString &msg_helper)
{
	// \002 bold
	// \037 underlined
	// \026 italic
	// \017 normal
	// \003xx,xx color
	QString msg = msg_helper.toHtmlEscaped();
	QString result;
	result.reserve(msg.size() + 20);
	QStringList resettingTags; // list of tags for resetting format
	bool bold = false;
	bool underlined = false;
	bool italic = false;
	int pos = 0, oldPos = 0;
	while ((pos = formatRx.indexIn(msg, pos)) != -1) {
		QString tmp = msg.mid(oldPos, pos - oldPos);
		result += tmp;
		QChar f = formatRx.cap(1).at(0);
		if (f == '\002') { // bold
			if (!bold) {
				result += "<b>";
				resettingTags.prepend("</b>");
			} else {
				result += "</b>";
				resettingTags.removeOne("</b>");
			}
			bold = !bold;
		} else if (f == '\037') { // underlined
			if (!underlined) {
				result += "<u>";
				resettingTags.prepend("</u>");
			} else {
				result += "</u>";
				resettingTags.removeOne("</u>");
			}
			underlined = !underlined;
		} else if (f == '\026') { // italic
			if (!italic) {
				result += "<i>";
				resettingTags.prepend("</i>");
			} else {
				result += "</i>";
				resettingTags.removeOne("</i>");
			}
			italic = !italic;
		} else if (f == '\017') { // normal
			result += resettingTags.join("");
			resettingTags.clear();
		} else { // color
			if (IrcProtocolPrivate::enableColoring) {
				QString fontColor = IrcProtocolPrivate::getColorByMircCode(formatRx.cap(3));
				QString backgroundColor = IrcProtocolPrivate::getColorByMircCode(formatRx.cap(4).mid(1));
				// Resetting all colors
				if (resettingTags.removeOne("</font>"))
					result += "</font>";
				if (!fontColor.isEmpty() || !backgroundColor.isEmpty()) {
					result += "<span style=\"";
					if (!fontColor.isEmpty())
						result += "color: " + fontColor + ";";
					if (!backgroundColor.isEmpty())
						result += "background-color: " + backgroundColor + ";";
					result += "\">";
					resettingTags.prepend("</font>");
				}
			}
		}
		pos += formatRx.matchedLength();
		oldPos = pos;
	}
	QString tmp = msg.mid(oldPos);
	result += tmp;
	return result;
}

QString IrcProtocol::ircFormatToPlainText(const QString &msg)
{
	QString result;
	result.reserve(msg.size());
	int pos = 0, oldPos = 0;
	while ((pos = formatRx.indexIn(msg, pos)) != -1) {
		result += msg.mid(oldPos, pos - oldPos);
		pos += formatRx.matchedLength();
		oldPos = pos;
	}
	result += msg.mid(oldPos);
	return result;
}

void IrcProtocol::updateSettings()
{
	Config cfg = config("general");
	d->enableColoring = cfg.value("enableColoring", true);
	foreach (QPointer<IrcAccount> acc, d->accounts_hash)
		acc.data()->updateSettings();
}

bool IrcProtocol::event(QEvent *ev)
{
	if (ev->type() == ActionCreatedEvent::eventType()) {
		ActionCreatedEvent *event = static_cast<ActionCreatedEvent*>(ev);
		if (d->autojoinAction == event->generator()) {
			IrcChannel *channel = qobject_cast<IrcChannel*>(event->controller());
			if (!channel)
				return false;

			event->action()->setChecked(channel->autoJoin());
			connect(channel, SIGNAL(autoJoinChanged(bool)),
					event->action(), SLOT(setChecked(bool)));
			return true;
		}
		return false;
	}
	return QObject::event(ev);
}

QVariant IrcProtocol::data(DataType type)
{
	switch (type) {
		case ProtocolIdName:
			return "Nick";
		case ProtocolContainsContacts:
			return false;
		default:
			return QVariant();
	}
}

void IrcProtocol::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(activated(bool)), this, SLOT(onSessionActivated(bool)));
}

void IrcProtocol::onSessionActivated(bool active)
{
	ChatSession *session = qobject_cast<ChatSession*>(sender());
	if (!active && session == d->activeSession.data())
		d->activeSession.clear();
	else if (session && active && qobject_cast<IrcChannel*>(session->getUnit()))
		d->activeSession = session;
}

void IrcProtocol::onJoinLeftChannel(QObject *channel_helper)
{
	IrcChannel *channel = qobject_cast<IrcChannel*>(channel_helper);
	if (!channel)
		return;
	if (channel->isJoined())
		channel->leave();
	else
		channel->join();
}

void IrcProtocol::onAutojoinChecked(QObject *channel_helper)
{
	IrcChannel *channel = qobject_cast<IrcChannel*>(channel_helper);
	if (!channel)
		return;

	channel->setAutoJoin(!channel->autoJoin());
}

} } // namespace qutim_sdk_0_3::irc

