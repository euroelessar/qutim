#ifndef TESTSETTINGS_H
#define TESTSETTINGS_H

#include <qutim/settingswidget.h>
#include "ui_testsettings.h"

class TestSettings : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT

public:
	TestSettings();

protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

private:
	Ui::TestSettingsForm *ui;

};
#endif
