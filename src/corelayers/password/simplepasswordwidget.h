#ifndef SIMPLEPASSWORDWIDGET_H
#define SIMPLEPASSWORDWIDGET_H

#include <QDialog>
#include "libqutim/account.h"

namespace Ui {
    class SimplePasswordWidget;
}

namespace Core
{
using namespace qutim_sdk_0_3;
class SimplePasswordWidget : public QDialog
{
    Q_OBJECT
public:
	SimplePasswordWidget(Account *account);
    ~SimplePasswordWidget();

protected:
    void changeEvent(QEvent *e);
public slots:
	void onAccept();
signals:
	void entered(const QString &password, bool remember);

private:
	Account *m_account;
    Ui::SimplePasswordWidget *ui;
};
}

#endif // SIMPLEPASSWORDWIDGET_H
