#ifndef SHORTCUTSETTINGS_H
#define SHORTCUTSETTINGS_H

#include <QObject>
#include <qutim/startupmodule.h>

namespace Core
{

	class ShortcutSettings : public QObject, public qutim_sdk_0_3::StartupModule
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::StartupModule)
	public:
		explicit ShortcutSettings(QObject *parent = 0);
	};

}
#endif // SHORTCUTSETTINGS_H
