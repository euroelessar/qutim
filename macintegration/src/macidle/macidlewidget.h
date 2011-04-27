#ifndef IDLESTATUSWIDGET_H
#define IDLESTATUSWIDGET_H

#include <qutim/settingswidget.h>

namespace Ui {
    class MacIdleWidget;
}

using namespace qutim_sdk_0_3;

class MacIdleWidget : public SettingsWidget
{
	Q_OBJECT
public:
	MacIdleWidget();
	~MacIdleWidget();

protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::MacIdleWidget *ui;
};

#endif // IDLESTATUSWIDGET_H
