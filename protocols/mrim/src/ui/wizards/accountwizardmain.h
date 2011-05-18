#ifndef ACCOUNTWIZARDMAIN_H
#define ACCOUNTWIZARDMAIN_H

#include <QWizardPage>

namespace Ui {
    class AccountWizardMain;
}

class AccountWizardMain : public QWizardPage
{
    Q_OBJECT
public:
    AccountWizardMain(QWidget *parent = 0);
    ~AccountWizardMain();

protected:
    void changeEvent(QEvent *e);
    bool validatePage();

private:
    Ui::AccountWizardMain *ui;
};

#endif // ACCOUNTWIZARDMAIN_H
