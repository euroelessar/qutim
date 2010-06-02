#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include "modulemanagerimpl.h"

namespace qutim_sdk_0_3
{
	class Config;
}

namespace Ui {
    class ProfileDialog;
}

namespace Core
{
class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
	ProfileDialog(Config &config, ModuleManager *parent = 0);
    ~ProfileDialog();
	static Config profilesInfo();
	static bool acceptProfileInfo(Config &config, const QString &password);

protected slots:
	void on_loginButton_clicked();
	void on_createButton_clicked();

protected:
    void changeEvent(QEvent *e);

private:
	ModuleManager *m_manager;
    Ui::ProfileDialog *ui;
};
}

#endif // PROFILEDIALOG_H
