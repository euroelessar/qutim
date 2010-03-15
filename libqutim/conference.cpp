#include "conference.h"
#include "chatunit_p.h"

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
}
