#include "jmucuser.h"
#include "jmucsession.h"
#include "../jaccount.h"
#include "../roster/jcontactresource_p.h"
#include "../vcard/jinforequest.h"
#include <QStringBuilder>

namespace Jabber
{
	class JMUCUserPrivate : public JContactResourcePrivate
	{
		public:
			QString avatar;
			QStringRef hash;
			MUCRoomAffiliation affiliation;
			MUCRoomRole role;
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

	InfoRequest *JMUCUser::infoRequest() const
	{
		return new JInfoRequest(((JAccount *) account())->connection()->vCardManager(), id());
	}

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
}
