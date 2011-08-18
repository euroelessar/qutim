#ifndef MANAGERSETTINGS_H
#define MANAGERSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

namespace qutim_sdk_0_3
{
class Account;
}

namespace Ui {
class ManagerSettings;
}

class QCheckBox;

using namespace qutim_sdk_0_3;

class ManagerSettings : public SettingsWidget
{
	Q_OBJECT

public:
	explicit ManagerSettings();
	~ManagerSettings();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
private slots:
	void onCheckedStateChanged(int state);
private:
	void addAccount(Account *account);
	Ui::ManagerSettings *ui;
	QList<QCheckBox *> m_checkbox_list;
};

#endif // MANAGERSETTINGS_H
