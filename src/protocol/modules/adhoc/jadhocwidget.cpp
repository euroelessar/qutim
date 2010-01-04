#include "jadhocwidget.h"
#include "ui_jadhocwidget.h"
#include <QPushButton>
#include <QLabel>

using namespace gloox;

namespace Jabber
{
	JAdHocWidget::JAdHocWidget(const JID &jid, gloox::Adhoc *adhoc, QWidget *parent) :
		QWidget(parent),
		m_ui(new Ui::JAdHocWidget),
		m_jid(jid),
		m_adhoc(adhoc)
	{
		m_ui->setupUi(this);
		adhoc->getCommands(jid, this);
	}

	JAdHocWidget::~JAdHocWidget()
	{
		delete m_ui;
	}

	void JAdHocWidget::handleAdhocSupport(const JID &remote, bool support)
	{
	}

	void JAdHocWidget::handleAdhocCommands(const JID &remote, const StringMap &commands)
	{
		StringMap::const_iterator it = commands.begin();
		for (; it != commands.end(); ++it) {
			QRadioButton *option = new QRadioButton(QString::fromStdString((*it).second), this);
			m_ui->layout->addWidget(option);
			m_options.insert(option,  QString::fromStdString((*it).first));
		}
		if (!commands.empty()) {
			addButton(tr("Next"), qutim_sdk_0_3::Icon("go-next"), SLOT(doExecute()));
		}
	}

	void JAdHocWidget::handleAdhocError(const JID &remote, const Error *error)
	{
	}

	void JAdHocWidget::handleAdhocExecutionResult(const JID &remote, const Adhoc::Command &command)
	{
		clear();
		m_sessionId = command.sessionID();

		StringList lst = command.form()->instructions();
		StringList::iterator it;
		for (it=lst.begin() ; it != lst.end(); it++) {
			m_ui->layout->addWidget(new QLabel(QString::fromStdString(*it), this));
		}

		if (command.form()) {
			m_dataForm = new JDataForm(command.form(), true, this);
			m_ui->layout->addWidget(m_dataForm);
		}

		if (command.actions() & Adhoc::Command::Execute) {
			addButton(tr("Finish"), qutim_sdk_0_3::Icon("dialog-ok-apply"), SLOT(doExecute()));
		}
		if (command.actions() & Adhoc::Command::Cancel) {
			addButton(tr("Cancel"), qutim_sdk_0_3::Icon("dialog-cancel"), SLOT(doCancel()));
		}
		if (command.actions() & Adhoc::Command::Previous) {
			addButton(tr("Previous"), qutim_sdk_0_3::Icon("go-previous"), SLOT(doPrevious()));
		}
		if (command.actions() & Adhoc::Command::Next) {
			addButton(tr("Next"), qutim_sdk_0_3::Icon("go-next"), SLOT(doNext()));
		}
		if (command.actions() & Adhoc::Command::Complete) {
			addButton(tr("Complete"), qutim_sdk_0_3::Icon("dialog-ok-apply"), SLOT(doComplete()));
		}
		if (command.actions() == 0) {
			addButton(tr("Ok"), qutim_sdk_0_3::Icon("dialog-ok-apply"), SLOT(doCancel()));
		}
	}

	void JAdHocWidget::addButton(const QString &text, const QIcon &icon, const char *member)
	{
		QPushButton *button = m_ui->buttonBox->addButton(text, QDialogButtonBox::AcceptRole);
		button->setIcon(icon);
		connect(button, SIGNAL(clicked()), this, member);
	}

	void JAdHocWidget::doExecute()
	{
		if (!m_options.isEmpty()) {
			m_node = gloox::EmptyString;
			QMapIterator<QRadioButton *, QString> it(m_options);
			while (it.hasNext()) {
				it.next();
				if (it.key()->isChecked()) {
					m_node = it.value().toStdString();
					m_adhoc->execute(m_jid, new Adhoc::Command(m_node, Adhoc::Command::Execute), this);
					return;
				}
			}
			return;
		}
		m_adhoc->execute(m_jid, new Adhoc::Command(m_node, m_sessionId, Adhoc::Command::Execute,
												   m_dataForm ? m_dataForm->getDataForm() : 0), this);
	}

	void JAdHocWidget::doCancel()
	{
		m_adhoc->execute(m_jid, new Adhoc::Command (m_node, m_sessionId, Adhoc::Command::Cancel), this );
	}

	void JAdHocWidget::doNext()
	{
		m_adhoc->execute(m_jid, new Adhoc::Command (m_node, m_sessionId, Adhoc::Command::Execute,
													m_dataForm ? m_dataForm->getDataForm() : 0), this );
	}

	void JAdHocWidget::doPrevious()
	{
		m_adhoc->execute(m_jid, new Adhoc::Command (m_node, m_sessionId, Adhoc::Command::Previous), this );
	}

	void JAdHocWidget::doComplete()
	{
		m_adhoc->execute(m_jid, new Adhoc::Command (m_node, m_sessionId, Adhoc::Command::Complete), this );
	}

	void JAdHocWidget::changeEvent(QEvent *e)
	{
		QWidget::changeEvent(e);
		switch (e->type()) {
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}

	void JAdHocWidget::clear()
	{
		m_ui->buttonBox->clear();
		for (int i = m_ui->layout->count(); i > 0; i--) {
			QLayoutItem *item = m_ui->layout->itemAt(0);
			m_ui->layout->removeItem(item);
			delete item;
		}
		delete m_dataForm.data();
	}
}
