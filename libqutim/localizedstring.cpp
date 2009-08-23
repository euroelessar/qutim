#include "localizedstring.h"
#include <QtCore/QCoreApplication>

namespace qutim_sdk_0_3
{
	QString LocalizedString::toString() const
	{
		return QCoreApplication::translate(m_ctx.constData(), m_str.constData(), 0, QCoreApplication::UnicodeUTF8);
	}
}
