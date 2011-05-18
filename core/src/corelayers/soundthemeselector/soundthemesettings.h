#ifndef SOUNDTHEMESETTINGS_H
#define SOUNDTHEMESETTINGS_H

#include <QObject>
#include <qutim/startupmodule.h>

namespace Core
{

	class SoundThemeSettings : public QObject, public qutim_sdk_0_3::StartupModule
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::StartupModule)
	public:
		explicit SoundThemeSettings(QObject *parent = 0);
	};

}
#endif // SOUNDTHEMESETTINGS_H
