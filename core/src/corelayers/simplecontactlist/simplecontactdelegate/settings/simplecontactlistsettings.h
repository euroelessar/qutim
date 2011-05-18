#ifndef SIMPLECONTACTLISTSETTINGS_H
#define SIMPLECONTACTLISTSETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <contactdelegate.h>
#include <simplecontactlistsettings.h>

class QCheckBox;

namespace Ui {
    class SimpleContactlistSettings;
}

namespace Core
{

using namespace qutim_sdk_0_3;

class SimpleContactlistSettings : public ContactListSettingsExtention
{
	Q_OBJECT
	Q_INTERFACES(Core::ContactListSettingsExtention)
	Q_CLASSINFO("ServiceSettings", "Core::ContactDelegate")
public:
	explicit SimpleContactlistSettings();
	~SimpleContactlistSettings();
	virtual void loadImpl();
	virtual void cancelImpl();
	virtual void saveImpl();
private:
	Ui::SimpleContactlistSettings *ui;
	void reloadCombobox();
	QHash<QString, QCheckBox *> m_statusesBoxes;
};
	
}


#endif // SIMPLECONTACTLISTSETTINGS_H
