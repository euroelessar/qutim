#ifndef PASTERSETTINGS_H
#define PASTERSETTINGS_H


#include <qutim/settingswidget.h>
#include "ui_pastersettings.h"


class PasterSettings : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT

public:
	PasterSettings();
	~PasterSettings();

protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

private:
	Ui::Pastersettings *ui;

public slots:
	void checkAutoSubmit();

};

#endif // PASTERSETTINGS_H
