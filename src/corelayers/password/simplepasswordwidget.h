#ifndef SIMPLEPASSWORDWIDGET_H
#define SIMPLEPASSWORDWIDGET_H

#include <QDialog>
#include <QValidator>
#include "libqutim/account.h"

namespace Ui {
    class SimplePasswordWidget;
}

namespace Core
{
using namespace qutim_sdk_0_3;
class SimplePasswordDialog;

class SimplePasswordWidget : public QDialog
{
    Q_OBJECT
public:
	SimplePasswordWidget(Account *account, SimplePasswordDialog *parent);
    ~SimplePasswordWidget();

	void setValidator(QValidator *validator);

protected:
    void changeEvent(QEvent *e);
public slots:
	void onAccept();
	void onReject();
signals:
	void entered(const QString &password, bool remember);

private:
	Account *m_account;
	SimplePasswordDialog *m_parent;
    Ui::SimplePasswordWidget *ui;
};
}

#endif // SIMPLEPASSWORDWIDGET_H
