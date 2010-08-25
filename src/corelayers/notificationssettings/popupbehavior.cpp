#include "popupbehavior.h"
#include "ui_notifications.h"
#include <qutim/configbase.h>
#include <qutim/localizedstring.h>
#include <qutim/notificationslayer.h>
#include <QToolButton>
#include <qutim/icon.h>
#include <qutim/debug.h>
#include <QStandardItemModel>

namespace Core
{
	
	PopupBehavior::PopupBehavior() :
	ui(new Ui::BehaviorSettings)
	{
		ui->setupUi(this);
		
		ui->animationSpeed->setItemData(0,0);
		ui->animationSpeed->setItemData(1,200);
		ui->animationSpeed->setItemData(2,400);
		ui->animationSpeed->setItemData(3,600);
		ui->animationSpeed->setItemData(4,1000);
		ui->animationSpeed->setItemData(5,1500);
		
		//TODO move to libqutim
		LocalizedString strings[] = 
		{
			QT_TRANSLATE_NOOP("Notifications", "User online"),
			QT_TRANSLATE_NOOP("Notifications", "User offline"),
			QT_TRANSLATE_NOOP("Notifications", "Status changed"),
			QT_TRANSLATE_NOOP("Notifications", "User has birthday today!"),
			QT_TRANSLATE_NOOP("Notifications", "qutIM Launched"),
			QT_TRANSLATE_NOOP("Notifications", "Message recieved"),
			QT_TRANSLATE_NOOP("Notifications", "Message sent"),
			QT_TRANSLATE_NOOP("Notifications", "System notification"),
			QT_TRANSLATE_NOOP("Notifications", "Typing"),
			QT_TRANSLATE_NOOP("Notifications", "Message blocked"),
			QT_TRANSLATE_NOOP("Notifications", "Count")
		};

		m_model = new QStandardItemModel(this);
		QStringList headers;
		headers.append(QT_TRANSLATE_NOOP("Notifications", "Type"));
		headers.append(QT_TRANSLATE_NOOP("Notifications", "Play sound"));
		headers.append(QT_TRANSLATE_NOOP("Notifications", "Show popup"));
		m_model->setHorizontalHeaderLabels(headers);
		ui->notificationsView->setModel(m_model);

		for (int i = 0, size = sizeof(strings)/sizeof(LocalizedString); i < size; i++) {
			addNotification(strings[i], static_cast<Notifications::Type>(i));
		}

		ui->notificationsView->resizeColumnToContents(0);
		
		connect(ui->animationSpeed,SIGNAL(currentIndexChanged(int)),SLOT(onAnimationSpeedIndexChanged(int)));
		connect(ui->timeout,SIGNAL(textChanged(QString)),SLOT(onTimeoutTextChanged(QString)));
		connect(m_model,SIGNAL(itemChanged(QStandardItem*)),SLOT(onItemChanged(QStandardItem*)));
	}

	void PopupBehavior::loadImpl()
	{
		Config behavior = Config("behavior").group("notifications/popups");
		int animationDuration = behavior.value<int>("animationDuration",600);
		int timeout = behavior.value<int>("timeout",5000);
		m_popup_flags = behavior.value<int>("showFlags",0xfffffff);
		m_sound_flags = behavior.value<int>("soundFlags",0xfffffff);
		loadFlags();
		
		int index = indexOfDuration(animationDuration);
		if (index == 6)
			ui->animationSpeed->setItemData(6, animationDuration);
		
		ui->animationSpeed->setCurrentIndex(index);
		ui->timeout->setText(QString::number(timeout));
		
	}

	void PopupBehavior::cancelImpl()
	{

	}

	void PopupBehavior::saveImpl()
	{
		Config behavior = Config("behavior").group("notifications/popups");
		behavior.setValue("animationDuration",ui->animationSpeed->itemData(ui->animationSpeed->currentIndex()));
		behavior.setValue("timeout",ui->timeout->text().toInt());
		behavior.setValue("showFlags",m_popup_flags);
		behavior.setValue("soundFlags",m_sound_flags);
		behavior.sync();
		QObject *popup = getService("Popup");
		if (popup)
			QMetaObject::invokeMethod(popup, SLOT(updateSettings()));
	}

	PopupBehavior::~PopupBehavior()
	{
		delete ui;
	}
	
	int PopupBehavior::indexOfDuration ( int duration )
	{
		switch (duration) {
			case 0:
				return 0;
			case 200:
				return 1;
			case 400:
				return 2;
			case 600:
				return 3;
			case 1000:
				return 4;
			case 1500:
				return 5;
			default:
				return 6;
		}
	}
	
	
	void PopupBehavior::onAnimationSpeedIndexChanged ( int index )
	{
		Q_UNUSED(index);
		emit modifiedChanged(true);
	}

	void PopupBehavior::onTimeoutTextChanged ( const QString& str )
	{
		Q_UNUSED(str);
		emit modifiedChanged(true);
	}

	void PopupBehavior::addNotification ( const QString& localized_string, int index )
	{
		int type = 1 << index;
		QStandardItem *item = new QStandardItem(localized_string);
		item->setData(type,ItemTypeRole);
		item->setEditable(false);
		QList<QStandardItem *> items;
		items << item;
		item = new QStandardItem();
		item->setCheckable(true);
		items << item;
		item = new QStandardItem();
		item->setCheckable(true);
		items << item;
		m_model->appendRow(items);
	}

	void PopupBehavior::loadFlags()
	{
		for (int i = 0;i != m_model->rowCount();i++) {
			Qt::CheckState popup_checked = (m_popup_flags & (1 << i)) ? Qt::Checked : Qt::Unchecked;
			Qt::CheckState sound_checked = (m_sound_flags & (1 << i)) ? Qt::Checked : Qt::Unchecked;
			m_model->item(i,1)->setCheckState(sound_checked);
			m_model->item(i,2)->setCheckState(popup_checked);
		}
	}

	void PopupBehavior::onItemChanged(QStandardItem *item)
	{
		Qt::CheckState state = static_cast<Qt::CheckState>(item->data(Qt::CheckStateRole).toInt());
		int type = 1 << item->row();

		int &flags = (item->column() == 1) ? m_sound_flags : m_popup_flags;

		if (state == Qt::Checked) {
			flags |= type;
		}
		else {
			flags &= ~type;
		}
		emit modifiedChanged(true);
	}

}
