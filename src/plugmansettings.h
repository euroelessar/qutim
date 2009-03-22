#ifndef PLUGMANSETTINGS_H
#define PLUGMANSETTINGS_H
#include "ui_settings.h"
#include <QSettings>

class plugmanSettings : public QWidget, Ui::plugmanSettings
{
	Q_OBJECT;
public:
	plugmanSettings(const QString &profile_name);
	~plugmanSettings();
	void getPackageList();	
private:
	QString m_profile_name;
public slots:
	void updatePackageList();
private slots:
	void on_installfromfileBtn_clicked();

};
#endif
