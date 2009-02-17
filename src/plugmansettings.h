#ifndef PLUGMANSETTINGS_H
#define PLUGMANSETTINGS_H
#include "ui_settings.h"
#include <QSettings>

class plugmanSettings : public QWidget
{
	Q_OBJECT;
public:
	plugmanSettings(const QString &profile_name);
	~plugmanSettings();
private:
	Ui::plugmanSettings ui;
	QString m_profile_name;

signals:
	void installfromfileclick();

};
#endif
