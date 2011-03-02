#ifndef WSETTINGS_H
#define WSETTINGS_H

#include <qutim/settingswidget.h>
#include "ui_wsettings.h"

class WSettingsWidget : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT

public:
	WSettingsWidget();

protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

private:
	Ui::WSettingsForm *ui;

private slots:
	void onCbStateChanged();

};

#endif // WSETTINGS_H
