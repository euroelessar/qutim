#ifndef SIMPLETRAYSETTINGS_H
#define SIMPLETRAYSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

namespace Ui {
	 class SimpletraySettings;
}

namespace Core {

	extern const char * traySettingsFilename;

	enum MsgNumberOption {
		DontShow,
		ShowMsgsNumber,
		ShowSessNumber
	};

	class SimpletraySettings : public qutim_sdk_0_3::SettingsWidget
	{
		 Q_OBJECT

	public:
		 explicit SimpletraySettings(QWidget *parent = 0);
		 ~SimpletraySettings();

	protected:
		void loadImpl();
		void saveImpl();
		void cancelImpl();

	private:
		 ::Ui::SimpletraySettings *ui;
	};

}

#endif // SIMPLETRAYSETTINGS_H
