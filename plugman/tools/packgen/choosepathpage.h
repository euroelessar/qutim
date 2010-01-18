#ifndef CHOOSEPATHPAGE_H
#define CHOOSEPATHPAGE_H

#include <QtGui/QWizardPage>

namespace Ui {
    class ChoosePathPage;
}

class ChoosePathPage : public QWizardPage {
    Q_OBJECT
    Q_DISABLE_COPY(ChoosePathPage)
public:
    explicit ChoosePathPage(QWidget *parent = 0);
    virtual ~ChoosePathPage();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::ChoosePathPage *m_ui;
};

#endif // CHOOSEPATHPAGE_H
