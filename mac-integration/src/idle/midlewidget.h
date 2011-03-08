#ifndef IDLESTATUSWIDGET_H
#define IDLESTATUSWIDGET_H

#include <qutim/settingswidget.h>

namespace Ui {
    class MIdleWidget;
}

using namespace qutim_sdk_0_3;

class MIdleWidget : public SettingsWidget
{
	Q_OBJECT
public:
	MIdleWidget();
	~MIdleWidget();

protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::MIdleWidget *ui;
};

#endif // IDLESTATUSWIDGET_H
