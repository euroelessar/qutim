#ifndef PLAYERSETTINGS_H
#define PLAYERSETTINGS_H

#include <QWidget>

namespace qutim_sdk_0_3 {

namespace nowplaying {

	class PlayerSettings : public QWidget
	{
		Q_OBJECT
	public:
		virtual void saveSettings() = 0;
		virtual void loadSettings() = 0;
	signals:
		void modified();
	};

} }

#endif // PLAYERSETTINGS_H
