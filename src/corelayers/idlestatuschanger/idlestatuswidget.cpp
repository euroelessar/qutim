#include "idlestatuswidget.h"
#include "ui_idlestatuswidget.h"

IdleStatusWidget::IdleStatusWidget() :
    ui(new Ui::IdleStatusWidget)
{
    ui->setupUi(this);
}

IdleStatusWidget::~IdleStatusWidget()
{
    delete ui;
}

void IdleStatusWidget::loadImpl()
{
}

void IdleStatusWidget::saveImpl()
{
}

void IdleStatusWidget::cancelImpl()
{
}

void IdleStatusWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
