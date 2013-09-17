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
#include "jmucuser.h"
#include "jmucsession.h"
#include "../jaccount.h"
#include "../roster/jcontactresource_p.h"
#include <QStringBuilder>
#include <qutim/tooltip.h>

namespace Jabber
{
class JMUCUserPrivate : public JContactResourcePrivate
{
public:
	JMUCUserPrivate(qutim_sdk_0_3::ChatUnit *c) :
		JContactResourcePrivate(c) {}
	QString avatar;
	QStringRef hash;
	MUCRoom::Affiliation affiliation;
	MUCRoom::Role role;
	QString realJid;
	QPointer<JMUCSession> muc;
};

JMUCUser::JMUCUser(JMUCSession *muc, const QString &name) :
	JContactResource(muc, *new JMUCUserPrivate(muc))
{
	Q_D(JMUCUser);
	setUserName(name);
	d->id = muc->id() % QLatin1Char('/') % name;
	d->muc = muc;
}

JMUCUser::~JMUCUser()
{
	Q_D(JMUCUser);
	if (d->muc)
		d->muc.data()->handleDeath(d->name);
}

QString JMUCUser::title() const
{
	return Buddy::title();
}

QString JMUCUser::name() const
{
	return d_ptr->name;
}

void JMUCUser::setUserName(const QString &name)
{
	Q_D(JMUCUser);
	QString previous = d->name;
	d->name = name;
	emit nameChanged(name, previous);
	emit titleChanged(name, previous);
}

void JMUCUser::setName(const QString &name)
{
	Q_UNUSED(name);
	//		d_func()->name = name;
	//		emit nameChanged(name);
}

JMUCSession *JMUCUser::muc() const
{
	return d_func()->muc.data();
}

QString JMUCUser::avatar() const
{
	return d_func()->avatar;
}

QString JMUCUser::avatarHash() const
{
	return d_func()->hash.toString();
}

void JMUCUser::setAvatar(const QString &hex)
{
	Q_D(JMUCUser);
	if (d->hash == hex)
		return;
	d->avatar = static_cast<JAccount *>(account())->getAvatarPath() % QLatin1Char('/') % hex;
	d->hash = d->avatar.rightRef(hex.size());
	emit avatarChanged(d->avatar);
}

//	InfoRequest *JMUCUser::infoRequest() const
//	{
//		return new JInfoRequest(((JAccount *) account())->connection()->vCardManager(), id());
//	}

ChatUnit *JMUCUser::upperUnit()
{
	return qobject_cast<Conference *>(d_func()->contact);
}

MUCRoom::Affiliation JMUCUser::affiliation()
{
	return d_func()->affiliation;
}

void JMUCUser::setMUCAffiliationAndRole(MUCRoom::Affiliation affiliation, MUCRoom::Role role)
{
	int oldPriority = priority();
	d_func()->affiliation = affiliation;
	d_func()->role = role;
	int newPriority = priority();
	emit priorityChanged(oldPriority, newPriority);
	
	QString iconName;
	if (affiliation == MUCRoom::AffiliationOwner)
		iconName = QStringLiteral("user-role-owner");
	else if (affiliation == MUCRoom::AffiliationAdmin)
		iconName = QStringLiteral("user-role-admin");
	else if (role == MUCRoom::RoleModerator)
		iconName = QStringLiteral("user-role-moderator");
	else if (role == MUCRoom::RoleVisitor)
		iconName = QStringLiteral("user-role-visitor");
	else if (affiliation == MUCRoom::AffiliationMember)
		iconName = QStringLiteral("user-role-member");
	else
		iconName = QStringLiteral("user-role-participant");
	
	QVariantHash clientInfo;
	ExtensionIcon extIcon(iconName);
	clientInfo.insert("id", "mucRole");
	clientInfo.insert("icon", QVariant::fromValue(extIcon));
	clientInfo.insert("priority", 30);
	setExtendedInfo("mucRole", clientInfo);
}

MUCRoom::Role JMUCUser::role()
{
	return d_func()->role;
}

bool JMUCUser::event(QEvent *ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		Q_D(JMUCUser);
		ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
		if (!d->presence.status().isEmpty())
			event->addField(d->presence.status(), QString());
		if (!d->realJid.isEmpty())
			event->addField(QT_TRANSLATE_NOOP("Conference", "Real JID"), d->realJid);
		QString client = property("client").toString();
		QString affiliation;
		switch (d->affiliation) {
		case MUCRoom::AffiliationOwner:
			affiliation = tr("Owner");
			break;
		case MUCRoom::AffiliationAdmin:
			affiliation = tr("Administrator");
			break;
		case MUCRoom::AffiliationMember:
			affiliation = tr("Registered member");
			break;
		default:
			affiliation = QString();
		}
		if (!affiliation.isEmpty())
			event->addField(QT_TRANSLATE_NOOP("Conference", "Affiliation"), affiliation, 30);
		QString role;
		switch (d->role) {
		case MUCRoom::RoleModerator:
			role = tr("Moderator");
			break;
		case MUCRoom::RoleParticipant:
			role = tr("Participant");
			break;
		case MUCRoom::RoleVisitor:
			role = tr("Visitor");
			break;
		default:
			role = QString();
		}
		if (!role.isEmpty())
			event->addField(QT_TRANSLATE_NOOP("Conference", "Role"), role, 30);
		if (!client.isEmpty()) {
			event->addField(QT_TRANSLATE_NOOP("Conference", "Possible client"),
							client,
							property("clientIcon").toString(),
							ToolTipEvent::IconBeforeDescription,
							25);
			QString os = property("os").toString();
			if (!os.isEmpty())
				event->addField(QT_TRANSLATE_NOOP("Conference", "OS"), os, 25);
		}
		Buddy::event(ev);
		return true;
	}
	return JContactResource::event(ev);
}

QString JMUCUser::realJid() const
{
	return d_func()->realJid;
}

void JMUCUser::setRealJid(const QString &jid)
{
	d_func()->realJid = jid;
}

bool JMUCUser::sendMessage(const qutim_sdk_0_3::Message &message)
{
	return d_func()->muc.data()->sendPrivateMessage(this, message);
}

void JMUCUser::kick(const QString &reason)
{
	Q_D(JMUCUser);
	d->muc.data()->room()->kick(d->name, reason);
}

void JMUCUser::ban(const QString &reason)
{
	Q_D(JMUCUser);
	d->muc.data()->room()->ban(d->name, reason);
}

int JMUCUser::affiliation() const
{
	return d_func()->affiliation;
}

int JMUCUser::mucRole() const
{
	return d_func()->role;
}

int JMUCUser::priority() const
{
	int pr = d_func()->affiliation + d_func()->role * 10;
	// little hack for fix Owner priority
	if (d_func()->affiliation == MUCRoom::Affiliation::AffiliationOwner)
		return pr + 2;
	return pr;
}
}

