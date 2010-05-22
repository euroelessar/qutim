#include "jmucuser.h"
#include "jmucsession.h"
#include "../jaccount.h"
#include "../roster/jcontactresource_p.h"
#include "../vcard/jinforequest.h"
#include <QStringBuilder>
#include <qutim/tooltip.h>

namespace Jabber
{
	class JMUCUserPrivate : public JContactResourcePrivate
	{
		public:
			QString avatar;
			QStringRef hash;
			MUCRoomAffiliation affiliation;
			MUCRoomRole role;
			QString realJid;
	};

	JMUCUser::JMUCUser(JMUCSession *muc, const QString &name) :
		JContactResource(muc, *new JMUCUserPrivate)
	{
		Q_D(JMUCUser);
		d->contact = muc;
		d->name = name;
		d->id = muc->id() % QLatin1Char('/') % name;
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

	MUCRoomAffiliation JMUCUser::affiliation()
	{
		return d_func()->affiliation;
	}

	void JMUCUser::setMUCAffiliation(MUCRoomAffiliation affiliation)
	{
		d_func()->affiliation = affiliation;
	}

	MUCRoomRole JMUCUser::role()
	{
		return d_func()->role;
	}

	void JMUCUser::setMUCRole(MUCRoomRole role)
	{
		d_func()->role = role;
	}

	bool JMUCUser::event(QEvent *ev)
	{
		if (ev->type() == ToolTipEvent::eventType()) {
			Q_D(JMUCUser);
			ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
			if (!d->realJid.isEmpty())
				event->appendField(QT_TRANSLATE_NOOP("Conference", "Real JID"), d->realJid);
			QString client = property("client").toString();
			if (!client.isEmpty()) {
				QString os = property("os").toString();
				if (!os.isEmpty()) {
					client += " / ";
					client += os;
				}
				event->appendField(QT_TRANSLATE_NOOP("Contact", "Possible client"), client);
			}
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
}
