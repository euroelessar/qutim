#include "jactivitychooser.h"
#include "ui_jactivitychooser.h"
#include "jpersonactivityconverter.h"
#include <qutim/icon.h>
#include <qutim/event.h>
#include <qutim/actiongenerator.h>
#include "protocol/account/jaccount.h"

namespace Jabber {

	JActivityChooserWindow::JActivityChooserWindow(Account *account,
												   const QString &currentText,
												   const QString &currentGeneral,
												   const QString &currentSpecific,
												   QWidget *parent) :
		QDialog(parent),
		ui(new Ui::JActivityChooserWindow),
		m_account(account)
	{
		// Init dialog
		ui->setupUi(this);
		setWindowTitle(tr("Choose your activity"));
		ui->buttonBox->addButton("Choose", QDialogButtonBox::AcceptRole);
		connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
		connect(ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));
		connect(this, SIGNAL(accepted()), SLOT(sendMood()));
		connect(ui->activitiesWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
				SLOT(onCurrentItemChanged(QTreeWidgetItem *)));
		ui->textEdit->setPlainText(currentText);
		// Load available moods
		QHashIterator<QString, JPersonActivity> itr(*JPersonActivityConverter::allActivities());
		QHash<QString, QTreeWidgetItem *> generalActivities;
		QTreeWidgetItem *current = 0;
		while (itr.hasNext()) {
			itr.next();
			const JPersonActivity &activity = itr.value();
			bool isGeneral = activity.isGeneral();
			// If it is a general activity, it is possible that its item was already created.
			QTreeWidgetItem *item = isGeneral ? generalActivities.value(itr.key()) : 0;
			if (!item) {
				item = new QTreeWidgetItem(isGeneral ? ui->activitiesWidget : 0);
				if (isGeneral)
					generalActivities.insert(itr.key(), item);
			}
			// Init the item.
			item->setData(0, Qt::DisplayRole, activity.value().toString());
			item->setData(0, Qt::DecorationRole, activity.icon().toIcon());
			item->setData(0, Qt::UserRole+1, isGeneral ? itr.key() : activity.generalActivity());
			if (!isGeneral) {
				item->setData(0, Qt::UserRole+2, itr.key());
				// As it is the specific activity, we need to find the parent item ...
				QTreeWidgetItem *generalItem = generalActivities.value(activity.generalActivity());
				if (!generalItem) {
					// ... or create it.
					generalItem = new QTreeWidgetItem(ui->activitiesWidget);
					generalActivities.insert(activity.generalActivity(), generalItem);
				}
				generalItem->addChild(item);
			}
			if (!current) {
				if ((isGeneral && currentSpecific.isEmpty() && activity.name() == currentGeneral) ||
					(!isGeneral && activity.generalActivity() == currentGeneral &&
					 activity.name() == currentSpecific))
				{
					current = item;
				}
			}
		}
		ui->activitiesWidget->sortByColumn(0, Qt::AscendingOrder);
		m_noActivityItem = new QTreeWidgetItem();
		m_noActivityItem->setData(0, Qt::DisplayRole, tr("No activity"));
		m_noActivityItem->setData(0, Qt::DecorationRole, Icon("edit-delete-status"));
		ui->activitiesWidget->insertTopLevelItem(0, m_noActivityItem);
		ui->activitiesWidget->setCurrentItem(current ? current : m_noActivityItem);
	}

	JActivityChooserWindow::~JActivityChooserWindow()
	{
		delete ui;
	}

	void JActivityChooserWindow::sendMood()
	{
		QVariantHash data;
		QString general = ui->activitiesWidget->currentItem()->data(0, Qt::UserRole + 1).toString();
		if (!general.isEmpty()) {
			data.insert("general", general);
			data.insert("specific", ui->activitiesWidget->currentItem()->data(0, Qt::UserRole + 2).toString());
			data.insert("description", ui->textEdit->toPlainText());
		}
		qutim_sdk_0_3::Event ev("jabber-personal-event", "activity", data, true);
		qApp->sendEvent(m_account, &ev);
	}

	void JActivityChooserWindow::onCurrentItemChanged(QTreeWidgetItem *item)
	{
		ui->textEdit->setEnabled(item != m_noActivityItem);
	}

	JActivityChooser::JActivityChooser()
	{
	}

	void JActivityChooser::init(qutim_sdk_0_3::Account *account, const JabberParams &params)
	{
		Q_UNUSED(params);
		m_account = account;
		m_eventId = qutim_sdk_0_3::Event::registerType("jabber-personal-event");
		// Add action to context menu
		static JActivityChooserAction gen(QIcon(), tr("Set activity"), this, SLOT(show(QObject*)));
		gen.setType(0x60000);
		gen.setPriority(30);
		account->addAction(&gen);
		// Register event filter
		account->installEventFilter(this);
	}

	void JActivityChooser::show(QObject *obj)
	{
		Q_ASSERT(qobject_cast<Account*>(obj));
		Account *account = reinterpret_cast<Account*>(obj);
		JActivityChooserWindow *w = new JActivityChooserWindow(account, m_currentText,
															   m_currentGeneral, m_currentSpecific);
#ifdef QUTIM_MOBILE_UI
		w->showMaximized();
#else
		w->show();
#endif
		w->setAttribute(Qt::WA_DeleteOnClose, true);
	}

	bool JActivityChooser::eventFilter(QObject *obj, QEvent *ev)
	{
		if (ev->type() == qutim_sdk_0_3::Event::eventType()) {
			qutim_sdk_0_3::Event *customEvent = static_cast<qutim_sdk_0_3::Event*>(ev);
			if (customEvent->id == m_eventId && obj == m_account &&
				customEvent->at<QString>(0) == "activity")
			{
				QVariantHash data = customEvent->at<QVariantHash>(1);
				m_currentGeneral = data.value("general").toString();
				m_currentSpecific = data.value("specific").toString();
				m_currentText = data.value("description").toString();
			}
		}
		return false;
	}

	JActivityChooserAction::JActivityChooserAction(const QIcon &icon, const LocalizedString &text,
												   const QObject *receiver, const char *member) :
		ActionGenerator(icon, text, receiver, member)
	{
	}

	JActivityChooserAction::JActivityChooserAction(const QIcon &icon, const LocalizedString &text,
												   const char *member) :
		ActionGenerator(icon, text, member)
	{
	}

	void JActivityChooserAction::showImpl(QAction *action, QObject *obj)
	{
		JAccount *account = qobject_cast<JAccount*>(obj);
		if (!account)
			return;
		action->setEnabled(account->checkIdentity(QLatin1String("pubsub"), QLatin1String("pep")));
	}

} // namespace Jabber
