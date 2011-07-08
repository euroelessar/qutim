#ifndef OLDDELEGATESETTINGS_H
#define OLDDELEGATESETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <contactlistitemdelegate.h>
#include <qutim/simplecontactlist/simplecontactlistsettings.h>
#include <qutim/config.h>

class QCheckBox;
class QComboBox;

namespace Ui {
	class OldDelegateSettings;
}

using namespace qutim_sdk_0_3;

class OldDelegateSettings : public Core::ContactListSettingsExtention
{
    Q_OBJECT
	Q_INTERFACES(Core::ContactListSettingsExtention)
	Q_CLASSINFO("ServiceSettings", "ContactListItemDelegate")
public:
	explicit OldDelegateSettings();
	~OldDelegateSettings();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();
private:
	Ui::OldDelegateSettings *ui;
	void initCombobox(QComboBox *box);
	void setSize(QComboBox *box, int size);
	void storeSizeToConfig(QComboBox *box, Config &cfg, const char *value);
	QHash<QString, QCheckBox *> m_statusesBoxes;
};

#endif // SIMPLECONTACTLISTSETTINGS_H
