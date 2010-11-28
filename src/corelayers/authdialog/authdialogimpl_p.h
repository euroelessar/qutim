#ifndef AUTHDIALOGIMPL_P_H
#define AUTHDIALOGIMPL_P_H
#include "ui_authdialog.h"
#include <qutim/contact.h>

namespace Core {

using namespace qutim_sdk_0_3;

class AuthDialogPrivate : public QDialog
{
	Q_OBJECT
public:
	AuthDialogPrivate() :
		ui(new Ui::AuthDialog)
	{
		ui->setupUi(this);
		setAttribute(Qt::WA_DeleteOnClose);
	};
	void show(qutim_sdk_0_3::Contact* contact, const QString& text, bool incoming)
	{
		m_isIncoming = incoming;
		m_contact = contact;
		ui->requestMessage->setText(text);
		QString title;
		if (incoming) {
			title = QT_TRANSLATE_NOOP("ContactInfo", "Recieved authorizarion request from %1:")
					.toString().arg(contact->title());
			ui->requestMessage->setReadOnly(true);
		} else {
			title = QT_TRANSLATE_NOOP("ContactInfo", "Send authorizarion request to %1:")
					.toString().arg(contact->title());
		}
		ui->requestTitle->setText(title);
		setWindowTitle(title);
		centerizeWidget(this);
#ifdef QUTIM_MOBILE_UI
		QDialog::showMaximized();
#else
		QDialog::show();
#endif
		raise();
	}
	QString text() const
	{
		return ui->requestMessage->toPlainText();
	}
	bool isIncoming() {return m_isIncoming;}
	Contact *contact() {return m_contact;}
	virtual ~AuthDialogPrivate() {
		delete ui;
	};
private:
	Ui::AuthDialog *ui;
	bool m_isIncoming;
	Contact *m_contact;
};
}
#endif // AUTHDIALOGIMPL_P_H
