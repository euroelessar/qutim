#ifndef XSETTINGSGROUP_H
#define XSETTINGSGROUP_H
#include <QWidget>
#include "xsettingslayerimpl.h"

namespace Ui
{
	class XSettingsGroup;
}

class XSettingsGroup : public QWidget
{
	Q_OBJECT
public:
    XSettingsGroup (const SettingsItemList &settings,QWidget* parent = 0);
private:
	SettingsItemList m_setting_list;
	Ui::XSettingsGroup *ui;
private slots:
    void currentRowChanged ( int );
};

#endif // XSETTINGSGROUP_H
