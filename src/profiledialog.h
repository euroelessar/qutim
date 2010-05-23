#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>
#include "modulemanagerimpl.h"

namespace Ui {
    class ProfileDialog;
}

namespace Core
{
class ProfileDialog : public QDialog
{
    Q_OBJECT
public:
	ProfileDialog(const QVariantMap &value, ModuleManager *parent = 0);
    ~ProfileDialog();
	static QVariantMap profilesInfo();
	static bool acceptProfileInfo(const QVariantMap &map, const QString &password);

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
