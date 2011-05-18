#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H
#include <QDialog>
#include "settingslayerimpl.h"

namespace Ui
{
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
	SettingsDialog (const SettingsItemList &settings);
	virtual ~SettingsDialog();
private:
	SettingsItemList m_setting_list;
	Ui::SettingsDialog *ui;
private slots:
	void currentRowChanged ( int );
};

#endif // SETTINGDIALOG_H
