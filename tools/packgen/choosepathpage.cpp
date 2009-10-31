#include "choosepathpage.h"
#include "ui_choosepathpage.h"

ChoosePathPage::ChoosePathPage(QWidget *parent) :
    QWizardPage(parent),
    m_ui(new Ui::ChoosePathPage)
{
    m_ui->setupUi(this);
}

ChoosePathPage::~ChoosePathPage()
{
    delete m_ui;
}

void ChoosePathPage::changeEvent(QEvent *e)
{
    QWizardPage::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
