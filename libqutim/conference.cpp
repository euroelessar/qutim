#include "conference.h"
#include "chatunit_p.h"
#include "tooltip.h"
#include <QTextDocument>
#include <QLatin1Literal>

namespace qutim_sdk_0_3
{
	class ConferencePrivate : public ChatUnitPrivate
	{
	public:
		ConferencePrivate(Conference *c) : ChatUnitPrivate(c) {}
	};

	Conference::Conference(Account *account) :
			ChatUnit(*new ConferencePrivate(this), account)
	{
	}

	Conference::~Conference()
	{
	}

	QString Conference::topic() const
	{
		return QString();
	}

	void Conference::setTopic(const QString &topic)
	{
		Q_UNUSED(topic);
	}

	bool Conference::event(QEvent *ev)
	{
		if (ev->type() == ToolTipEvent::eventType()) {
			ToolTipEvent *event = static_cast<ToolTipEvent*>(ev);
			if (event->fieldsTypes() & ToolTipEvent::GenerateFields) {
				QString text = QLatin1Literal("<p><strong>")
							   % Qt::escape(title())
							   % QLatin1Literal("</strong> &lt;")
							   % id()
							   % QLatin1Literal("&gt;</p>")
							   % QLatin1Literal("<p>")
							   % Qt::escape(topic())
							   % QLatin1Literal("</p>");
				event->addHtml(text, 90);
			}
		}
	}
}
