#ifndef VACCOUNTSETTINGS_H
#define VACCOUNTSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

using namespace qutim_sdk_0_3;

namespace Ui {
	class VAccountSettings;
}
class VAccount;
class VAccountSettings : public SettingsWidget
{
    Q_OBJECT
public:
    explicit VAccountSettings(QWidget *parent = 0);
	virtual ~VAccountSettings();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();
	virtual void setController(QObject *controller);
private slots:
	void onPasswdChanged(const QString &passwd);
private:
	Ui::VAccountSettings *ui;
	VAccount *m_account;
};

#endif // VACCOUNTSETTINGS_H
