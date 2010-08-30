#include "buddy_p.h"
#include "tooltip.h"
#include "account.h"
#include <QTextDocument>
#include <QLatin1Literal>

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

	bool Buddy::event(QEvent *ev)
	{
		if (ev->type() == ToolTipEvent::eventType()) {
			ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
			if (event->generateLayout()) {
				event->addHtml("<table><tr><td>", 98);
				QString ava = avatar();
				if (ava.isEmpty())
					ava = QLatin1String(":/icons/qutim_64.png");
				QString text = QLatin1Literal("</td><td><img width=\"64\" src=\"")
							   % Qt::escape(ava)
							   % QLatin1Literal("\"/></td></tr></table>");
				event->addHtml(text, 5);
			}
			event->addHtml("<font size=-1>", 50);
			QString text = QLatin1Literal("<b>")
						   % Qt::escape(name())
						   % QLatin1Literal("</b> &lt;")
						   % Qt::escape(id())
						   % QLatin1Literal("&gt;");
			event->addHtml("</font>", 10);
			event->addHtml(text, 90);
			event->addField(QT_TRANSLATE_NOOP("ToolTip", "Account"), account()->id(), 90);

			QHash<QString, QVariantHash> extStatuses = status().extendedInfos();
			foreach (const QVariantHash &extStatus, extStatuses) {
				if (extStatus.value("showInTooltip", false).toBool()) {
					int priority = extStatus.value("priorityInTooltip").toInt();
					if (priority < 10 || priority > 90)
						priority = 50;
					event->addField(extStatus.value("title").toString(),
									extStatus.value("description").toString(),
									extStatus.value("icon").value<ExtensionIcon>(),
									extStatus.value("iconPosition").value<ToolTipEvent::IconPosition>(),
									priority);
				}
			}
		}
		return ChatUnit::event(ev);
	}
}
