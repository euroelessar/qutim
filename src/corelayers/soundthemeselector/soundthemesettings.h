#ifndef SOUNDTHEMESETTINGS_H
#define SOUNDTHEMESETTINGS_H

#include <QObject>

namespace Core
{

	class SoundThemeSettings : public QObject
	{
		Q_OBJECT
	public:
		explicit SoundThemeSettings(QObject *parent = 0);
	};

}
#endif // SOUNDTHEMESETTINGS_H
