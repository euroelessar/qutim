#ifndef VACCOUNTSETTINGS_H
#define VACCOUNTSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>

using namespace qutim_sdk_0_3;

namespace Ui {
	class VAccountSettings;
}

class VAccountSettings : public SettingsWidget
{
    Q_OBJECT
public:
    explicit VAccountSettings(QWidget *parent = 0);
    ~VAccountSettings();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();
private:
	Ui::VAccountSettings *ui;
};

#endif // VACCOUNTSETTINGS_H
