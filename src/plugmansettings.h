#ifndef PLUGMANSETTINGS_H
#define PLUGMANSETTINGS_H
#include "ui_settings.h"
#include <QSettings>
#include "plugpackagemodel.h"
#include "plugmanager.h"

class plugmanSettings : public QWidget, Ui::plugmanSettings
{
	Q_OBJECT;
public:
	plugmanSettings(const QString& profile_name);
	~plugmanSettings();
	void setPlugManager(plugManager *plug_manager);
	plugManager *getPlugManager();
private:
	QString m_profile_name;
	plugPackageModel *m_package_model;
};
#endif
