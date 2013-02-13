#ifndef PASTERSETTINGS_H
#define PASTERSETTINGS_H


#include <qutim/settingswidget.h>
#include "ui_pastersettings.h"


class AutoPasterSettings : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT

public:
	AutoPasterSettings();
	~AutoPasterSettings();

protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

private:
	Ui::Pastersettings *ui;
};

#endif // PASTERSETTINGS_H
