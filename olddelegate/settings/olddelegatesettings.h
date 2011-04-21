#ifndef OLDDELEGATESETTINGS_H
#define OLDDELEGATESETTINGS_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <contactlistitemdelegate.h>

class QCheckBox;

namespace Ui {
	class OldDelegateSettings;
}

using namespace qutim_sdk_0_3;

class OldDelegateSettings : public SettingsWidget
{
    Q_OBJECT

public:
	explicit OldDelegateSettings();
	~OldDelegateSettings();
    virtual void loadImpl();
    virtual void cancelImpl();
    virtual void saveImpl();
private:
	Ui::OldDelegateSettings *ui;
	void reloadCombobox();
	QHash<QString, QCheckBox *> m_statusesBoxes;
};

#endif // SIMPLECONTACTLISTSETTINGS_H
