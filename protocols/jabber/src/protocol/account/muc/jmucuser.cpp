#include "jmucuser.h"
#include "jmucsession.h"
#include "../jaccount.h"
#include "../roster/jcontactresource_p.h"
#include "../vcard/jinforequest.h"
#include "../vcard/jvcardmanager.h"
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
	JMUCSession *muc;
};

JMUCUser::JMUCUser(JMUCSession *muc, const QString &name) :
	JContactResource(muc, *new JMUCUserPrivate(muc))
{
	Q_D(JMUCUser);
	d->name = name;
	d->id = muc->id() % QLatin1Char('/') % name;
	d->muc = muc;
}

JMUCUser::~JMUCUser()
{
}

QString JMUCUser::title() const
{
	return Buddy::title();
}

QString JMUCUser::name() const
{
	return d_ptr->name;
}

void JMUCUser::setName(const QString &name)
{
	Q_UNUSED(name);
	//		d_func()->name = name;
	//		emit nameChanged(name);
}

JMUCSession *JMUCUser::muc() const
{
	return d_func()->muc;
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
	if (d->avatar == hex)
		return;
	d->avatar = static_cast<JAccount *>(account())->getAvatarPath() % QLatin1Char('/') % hex;
	int pos = d->avatar.lastIndexOf('/') + 1;
	int length = d->avatar.length() - pos;
	d->hash = QStringRef(&d->avatar, pos, length);
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

void JMUCUser::setMUCAffiliation(MUCRoom::Affiliation affiliation)
{
	d_func()->affiliation = affiliation;
}

MUCRoom::Role JMUCUser::role()
{
	return d_func()->role;
}

void JMUCUser::setMUCRole(MUCRoom::Role role)
{
	d_func()->role = role;
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
			affiliation = "Owner";
			break;
		case MUCRoom::AffiliationAdmin:
			affiliation = "Administrator";
			break;
		case MUCRoom::AffiliationMember:
			affiliation = "Registered member";
			break;
		default:
			affiliation = "";
		}
		if (!affiliation.isEmpty())
			event->addField(QT_TRANSLATE_NOOP("Conference", "Affiliation"), affiliation, 30);
		QString role;
		switch (d->role) {
		case MUCRoom::RoleModerator:
			role = "Moderator";
			break;
		case MUCRoom::RoleParticipant:
			role = "Participant";
			break;
		case MUCRoom::RoleVisitor:
			role = "Visitor";
			break;
		default:
			role = "";
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
	} else if (ev->type() == InfoRequestCheckSupportEvent::eventType()) {
		Status::Type status = account()->status().type();
		if (status >= Status::Online && status <= Status::Invisible && d_func()->muc->isJoined()) {
			InfoRequestCheckSupportEvent *event = static_cast<InfoRequestCheckSupportEvent*>(ev);
			event->setSupportType(InfoRequestCheckSupportEvent::Read);
			event->accept();
		} else {
			ev->ignore();
		}
		return true;
	} else if (ev->type() == InfoRequestEvent::eventType()) {
		Q_D(JMUCUser);
		JAccount * const acc = static_cast<JAccount*>(account());
		const JID jid = d->id;
		InfoRequestEvent *event = static_cast<InfoRequestEvent*>(ev);
		if(!acc->vCardManager()->containsRequest(jid))
			event->setRequest(new JInfoRequest(acc->vCardManager(), jid));
		event->accept();
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
	return d_func()->muc->sendPrivateMessage(id(), message);
}

void JMUCUser::kick(const QString &reason)
{
	Q_D(JMUCUser);
	d->muc->room()->kick(d->name, reason);
}

void JMUCUser::ban(const QString &reason)
{
	Q_D(JMUCUser);
	d->muc->room()->ban(d->name, reason);
}
}
