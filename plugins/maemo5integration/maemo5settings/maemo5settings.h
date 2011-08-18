#ifndef MAEMO5SETTINGS_H
#define MAEMO5SETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <qutim/settingslayer.h>
#include "ui_maemo5settings.h"

using namespace qutim_sdk_0_3;

class Maemo5Settings : public SettingsWidget
{
	Q_OBJECT

public:
	explicit Maemo5Settings();
	~Maemo5Settings();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
private slots:
	void onCheckedStateChanged(int state);
private:
	Ui::Maemo5Settings *ui;
	QString currentPattern;


};


#endif // MAEMO5SETTINGS_H
