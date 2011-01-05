#include "jmoodchooser.h"
#include "ui_jmoodchooser.h"
#include <qutim/icon.h>
#include <qutim/event.h>
#include "protocol/account/jaccount.h"
#include <jreen/mood.h>

namespace Jabber {

	using namespace qutim_sdk_0_3;
	JMoodChooserWindow::JMoodChooserWindow(Account *account, const QString &text,
										   const QString &mood, QWidget *parent) :
		QDialog(parent),
		ui(new Ui::JMoodChooserWindow),
		m_account(account)
	{
		// Init dialog
		ui->setupUi(this);
		setWindowTitle(tr("Choose your mood"));
		ui->buttonBox->addButton("Choose", QDialogButtonBox::AcceptRole);
		connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
		connect(ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));
		connect(this, SIGNAL(accepted()), SLOT(sendMood()));
		connect(ui->moodsWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
				SLOT(onCurrentItemChanged(QListWidgetItem*)));
		ui->textEdit->setPlainText(text);
		// Load available moods
		
//		QHashIterator<QString, LocalizedString> itr(JPersonMoodConverter::moods());
		const QList<LocalizedString> titles = JPersonMoodConverter::moods();
		jreen::Mood tmp;
		QListWidgetItem *current = 0;
		for (int i = 0; i < titles.size(); i++) {
//		while (itr.hasNext()) {
//			itr.next();
			tmp.setType(static_cast<jreen::Mood::Type>(i));
			QListWidgetItem *item = new QListWidgetItem(Icon("user-status-" + tmp.typeName()),
														titles[i], ui->moodsWidget);
			item->setData(QListWidgetItem::UserType + 1, tmp.typeName());
			if (!current && tmp.typeName() == mood)
				current = item;
		}
		ui->moodsWidget->sortItems();
		m_noMoodItem = new QListWidgetItem(Icon("edit-delete-status"), tr("No mood"));
		ui->moodsWidget->insertItem(0, m_noMoodItem);
		ui->moodsWidget->setCurrentItem(current ? current : m_noMoodItem);
	}

	JMoodChooserWindow::~JMoodChooserWindow()
	{
		delete ui;
	}

	void JMoodChooserWindow::sendMood()
	{
		QVariantHash data;
		QString mood = ui->moodsWidget->currentItem()->data(QListWidgetItem::UserType + 1).toString();
		if (!mood.isEmpty()) {
			data.insert("description", ui->textEdit->toPlainText());
			data.insert("mood", mood);
		}
		qutim_sdk_0_3::Event ev("jabber-personal-event", "mood", data, true);
		qApp->sendEvent(m_account, &ev);
	}

	void JMoodChooserWindow::onCurrentItemChanged(QListWidgetItem *item)
	{
		ui->textEdit->setEnabled(item != m_noMoodItem);
	}

	JMoodChooser::JMoodChooser()
	{
	}

	void JMoodChooser::init(qutim_sdk_0_3::Account *account, const JabberParams &params)
	{
		Q_UNUSED(params);
		m_account = account;
		m_eventId = qutim_sdk_0_3::Event::registerType("jabber-personal-event");
		// Add action to context menu
		static JMoodChooserAction gen(QIcon(), tr("Set mood"), this, SLOT(showMoodChooser(QObject*)));
		gen.setType(0x60000);
		gen.setPriority(30);
		account->addAction(&gen);
		// Register event filter
		account->installEventFilter(this);
	}

	void JMoodChooser::showMoodChooser(QObject *obj)
	{
		Q_ASSERT(qobject_cast<Account*>(obj));
		Account *account = reinterpret_cast<Account*>(obj);
		JMoodChooserWindow *w = new JMoodChooserWindow(account, m_currentText, m_currentMood);
#ifdef QUTIM_MOBILE_UI
		w->showMaximized();
#else
		w->show();
#endif
		w->setAttribute(Qt::WA_DeleteOnClose, true);
	}

	bool JMoodChooser::eventFilter(QObject *obj, QEvent *ev)
	{
		if (ev->type() == qutim_sdk_0_3::Event::eventType()) {
			qutim_sdk_0_3::Event *customEvent = static_cast<qutim_sdk_0_3::Event*>(ev);
			if (customEvent->id == m_eventId && obj == m_account &&
				customEvent->at<QString>(0) == "mood")
			{
				QVariantHash data = customEvent->at<QVariantHash>(1);
				m_currentMood = data.value("mood").toString();
				m_currentText = data.value("description").toString();
			}
		}
		return false;
	}

	JMoodChooserAction::JMoodChooserAction(const QIcon &icon, const LocalizedString &text,
										   const QObject *receiver, const char *member) :
		ActionGenerator(icon, text, receiver, member)
	{
	}

	JMoodChooserAction::JMoodChooserAction(const QIcon &icon, const LocalizedString &text,
										   const char *member) :
		ActionGenerator(icon, text, member)
	{
	}

	void JMoodChooserAction::showImpl(QAction *action, QObject *obj)
	{
		JAccount *account = qobject_cast<JAccount*>(obj);
		if (!account)
			return;
		action->setEnabled(account->checkIdentity(QLatin1String("pubsub"), QLatin1String("pep")));
	}

} // namespace Jabber
