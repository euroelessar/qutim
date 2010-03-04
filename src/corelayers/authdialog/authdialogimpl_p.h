#ifndef AUTHDIALOGIMPL_P_H
#define AUTHDIALOGIMPL_P_H
#include "authdialogimpl.h"
#include "ui_authdialog.h"
#include <libqutim/contact.h>

namespace Core {
	class AuthDialogPrivate : public QDialog
	{
		Q_OBJECT
	public:
		AuthDialogPrivate() :
		ui(new Ui::AuthDialog)
		{
			ui->setupUi(this);
			connect(ui->buttonBox,SIGNAL(accepted()),SIGNAL(accepted()));
			connect(ui->buttonBox,SIGNAL(rejected()),SLOT(reject()));
			setAttribute(Qt::WA_DeleteOnClose);
		};
		void show(qutim_sdk_0_3::Contact* contact, const QString& text) {
			ui->requestMessage->setText(text);
			QString title = tr("Recieved authorizarion request from %1:").arg(contact->title());
			ui->requestTitle->setText(title);
			setWindowTitle(title);
			centerizeWidget(this);
			QDialog::show();
			raise();
		}
		virtual ~AuthDialogPrivate() {
			delete ui;
		};
	private:
		Ui::AuthDialog *ui;
	};
}
#endif // AUTHDIALOGIMPL_P_H