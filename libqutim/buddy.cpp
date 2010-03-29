#include "buddy_p.h"

namespace qutim_sdk_0_3
{
	Buddy::Buddy(Account *account) :
			ChatUnit(*new BuddyPrivate(this), account)
	{
	}

	Buddy::Buddy(BuddyPrivate &d, Account *account) :
			ChatUnit(d, account)
	{
	}

	Buddy::~Buddy()
	{
	}

	QString Buddy::avatar() const
	{
		return QString();
	}

	QString Buddy::title() const
	{
		QString n = name();
		return n.isEmpty() ? id() : n;
	}

	QString Buddy::name() const
	{
		return id();
	}

	Status Buddy::status() const
	{
		return Status::Online;
	}

	void Buddy::setName(const QString &name)
	{
		Q_UNUSED(name);
	}

	InfoRequest *Buddy::infoRequest() const
	{
		return 0;
	}

//	QIcon Contact::statusIcon() const
//	{
//		switch(status())
//		{
//		default:
//			if(status() >= Connecting)
//				return Icon("network-connect");
//		case Online:
//		case AtHome:
//		case FreeChat:
//		case Invisible:
//			return Icon("user-online");
//		case Offline:
//			return Icon("user-offline");
//		case Away:
//		case OutToLunch:
//			return Icon("user-away");
//		case DND:
//		case Evil:
//		case Depression:
//		case Occupied:
//		case AtWork:
//		case OnThePhone:
//			return Icon("user-busy");
//		case NA:
//			return Icon("user-away-extended");
//		}
//	}
}
