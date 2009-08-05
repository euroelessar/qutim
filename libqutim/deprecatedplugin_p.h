#ifndef DEPRECATEDPLUGIN_P_H
#define DEPRECATEDPLUGIN_P_H

#include "plugin.h"

namespace qutim_sdk_0_3
{
	Plugin *createDeprecatedPlugin(QObject *object);

	class DeprecatedPlugin : public Plugin
	{
		Q_OBJECT
	public:
		DeprecatedPlugin(QObject *object);
	private:
		QByteArray m_name;
		QByteArray m_description;
	};
}

#endif // DEPRECATEDPLUGIN_P_H
