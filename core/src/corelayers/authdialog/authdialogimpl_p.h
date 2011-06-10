#ifndef AUTHDIALOGIMPL_P_H
#define AUTHDIALOGIMPL_P_H
#include "ui_authdialog.h"
#include <qutim/contact.h>
#include <qutim/systemintegration.h>
#include <QPushButton>
#include <QVBoxLayout>

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
		m_contactActionsBtn = new QPushButton(tr("Actions"), this);
#ifndef Q_WS_S60
		ui->buttonBox->addButton(m_contactActionsBtn, QDialogButtonBox::ActionRole);
#else
		int index = ui->verticalLayout->indexOf(ui->buttonBox);
		ui->verticalLayout->insertWidget(index, m_contactActionsBtn);
#endif
		setAttribute(Qt::WA_DeleteOnClose);
	}
	void show(qutim_sdk_0_3::Contact* contact, const QString& text, bool incoming)
	{
		m_isIncoming = incoming;
		m_contact = contact;
		QMenu *menu = contact->menu(false);
		m_contactActionsBtn->setMenu(menu);
		menu->setParent(m_contactActionsBtn);
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
		SystemIntegration::show(this);
		raise();
	}
	QString text() const
	{
		return ui->requestMessage->toPlainText();
	}
	bool isIncoming() { return m_isIncoming; }
	Contact *contact() { return m_contact; }
	virtual ~AuthDialogPrivate() {
		delete ui;
	}
private:
	Ui::AuthDialog *ui;
	QPushButton *m_contactActionsBtn;
	bool m_isIncoming;
	Contact *m_contact;
};
}
#endif // AUTHDIALOGIMPL_P_H
