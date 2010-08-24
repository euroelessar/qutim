#include "simplestatusdialog.h"
#include "ui_simplestatusdialog.h"
#include <qutim/debug.h>

namespace Core
{
	namespace SimpleContactList
	{

		SimpleStatusDialog::SimpleStatusDialog(const QString &status, QWidget *parent) :
				QDialog(parent),
				ui(new Ui::SimpleStatusDialog)
		{
			ui->setupUi(this);
			setAttribute(Qt::WA_DeleteOnClose);
			ui->textEdit->setText(status);
		}

		SimpleStatusDialog::~SimpleStatusDialog()
		{
			delete ui;
		}

		void SimpleStatusDialog::changeEvent(QEvent *e)
		{
			QDialog::changeEvent(e);
			switch (e->type()) {
			case QEvent::LanguageChange:
				ui->retranslateUi(this);
				break;
			default:
				break;
			}
		}

		QString SimpleStatusDialog::statusText() const
		{
			return ui->textEdit->toPlainText();
		}
	}

}
