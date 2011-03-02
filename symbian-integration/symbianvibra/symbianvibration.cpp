#include "symbianvibration.h"
#include <QLatin1Literal>

namespace Symbian
{
using namespace qutim_sdk_0_3;

Vibration::Vibration() : m_vibra(new XQVibra(this))
{
}

Vibration::~Vibration()
{
}

void Vibration::show(Notifications::Type type, QObject *sender, const QString &body, const QVariant &data)
{
	Q_UNUSED(type);
	Q_UNUSED(sender);
	Q_UNUSED(body);
	Q_UNUSED(data);
	// Think about better value
	m_vibra->start(300);
}

} //namespace Symbian
