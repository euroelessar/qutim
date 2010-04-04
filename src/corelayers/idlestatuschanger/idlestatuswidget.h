#ifndef IDLESTATUSWIDGET_H
#define IDLESTATUSWIDGET_H

#include "libqutim/settingswidget.h"

namespace Ui {
    class IdleStatusWidget;
}

using namespace qutim_sdk_0_3;

class IdleStatusWidget : public SettingsWidget
{
	Q_OBJECT
public:
	IdleStatusWidget();
	~IdleStatusWidget();

protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::IdleStatusWidget *ui;
};

#endif // IDLESTATUSWIDGET_H
