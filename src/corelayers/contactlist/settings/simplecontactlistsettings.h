#ifndef SIMPLECONTACTLISTSETTINGS_H
#define SIMPLECONTACTLISTSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include "../simplecontactlistdelegate.h"

namespace Ui {
    class SimpleContactlistSettings;
}

namespace Core
{
namespace SimpleContactList
{

using namespace qutim_sdk_0_3;

class SimpleContactlistSettings : public SettingsWidget
{
    Q_OBJECT

public:
    explicit SimpleContactlistSettings();
    ~SimpleContactlistSettings();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();
private slots:
    void onAvatarBoxToggled(bool);
    void onExtendedInfoBoxToggled(bool);
    void onStatusBoxToggled(bool);
    void onSizesCurrentIndexChanged(int);	
private:
    Ui::SimpleContactlistSettings *ui;
	void setFlag(Delegate::ShowFlags flag, bool on = true);
	void reloadCombobox();
	int m_flags;
};

}
	
}


#endif // SIMPLECONTACTLISTSETTINGS_H
