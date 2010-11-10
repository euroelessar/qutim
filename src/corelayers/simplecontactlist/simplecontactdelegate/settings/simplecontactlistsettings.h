#ifndef SIMPLECONTACTLISTSETTINGS_H
#define SIMPLECONTACTLISTSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <contactdelegate.h>

class QCheckBox;

namespace Ui {
    class SimpleContactlistSettings;
}

namespace Core
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
	void onModified();
private:
    Ui::SimpleContactlistSettings *ui;
	void setFlag(ContactDelegate::ShowFlags flag, bool on = true);
	void reloadCombobox();
	int m_flags;
	QHash<QString, QCheckBox *> m_statusesBoxes;
};
	
}


#endif // SIMPLECONTACTLISTSETTINGS_H
