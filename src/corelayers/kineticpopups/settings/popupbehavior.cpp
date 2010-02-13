#include "popupbehavior.h"
#include "ui_notifications.h"
#include <libqutim/configbase.h>
#include <corelayers/kineticpopups/manager.h>
#include <libqutim/localizedstring.h>
#include <QToolButton>
#include <libqutim/icon.h>
#include <libqutim/debug.h>

namespace Core
{
	
	PopupBehavior::PopupBehavior() :
	ui(new Ui::BehaviorSettings), m_layout(new QGridLayout)
	{
		ui->setupUi(this);
		ui->notificationsBox->setLayout(m_layout);
		
		ui->animationSpeed->setItemData(0,0);
		ui->animationSpeed->setItemData(1,200);
		ui->animationSpeed->setItemData(2,400);
		ui->animationSpeed->setItemData(3,600);
		ui->animationSpeed->setItemData(4,1000);
		ui->animationSpeed->setItemData(5,1500);
		
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

		for (int i = 0, size = sizeof(strings)/sizeof(LocalizedString); i < size; i++) {
			addNotification(strings[i], static_cast<Notifications::Type>(i));
		}
		
		connect(ui->animationSpeed,SIGNAL(currentIndexChanged(int)),SLOT(onAnimationSpeedIndexChanged(int)));
		connect(ui->timeout,SIGNAL(textChanged(QString)),SLOT(onTimeoutTextChanged(QString)));
	}

	void PopupBehavior::loadImpl()
	{
		ConfigGroup behavior = Config("behavior/notifications").group("popups");
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
		ConfigGroup behavior = Config("behavior/notifications").group("popups");
		behavior.setValue("animationDuration",ui->animationSpeed->itemData(ui->animationSpeed->currentIndex()));
		behavior.setValue("timeout",ui->timeout->text().toInt());
		behavior.setValue("showFlags",m_popup_flags);
		behavior.setValue("soundFlags",m_sound_flags);
		behavior.sync();
		KineticPopups::Manager::self()->loadSettings();
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
		
		QLabel *label = new QLabel(localized_string);
		
		QToolButton *popupBtn = new QToolButton();
		popupBtn->setIcon(Icon("services"));
		popupBtn->setCheckable(true);
		popupBtn->setProperty("notificationType",type);
		
		QToolButton *soundBtn = new QToolButton();
		soundBtn->setIcon(Icon("speaker"));
		soundBtn->setCheckable(true);
		soundBtn->setProperty("notificationType",type);
		
		m_sound_btn_list.append(soundBtn);
		m_popup_btn_list.append(popupBtn);
		
		m_layout->addWidget(label,index,0);
		m_layout->addWidget(soundBtn,index,1);
		m_layout->addWidget(popupBtn,index,2);

		connect(popupBtn,SIGNAL(toggled(bool)),SLOT(onPopupBtnToggled(bool)));
		connect(soundBtn,SIGNAL(toggled(bool)),SLOT(onSoundBtnToggled(bool)));
	}
		
	void PopupBehavior::onPopupBtnToggled ( bool checked )
	{
		emit modifiedChanged(true);
		int type = sender()->property("notificationType").toInt();
		if (checked)
			m_popup_flags |= type;
		else
			m_popup_flags &= ~type;
	}

	void PopupBehavior::onSoundBtnToggled ( bool checked )
	{
		emit modifiedChanged(true);
		int type = sender()->property("notificationType").toInt();
		if (checked)
			m_sound_flags |= type;
		else
			m_sound_flags &= ~type;
	}

	void PopupBehavior::loadFlags()
	{
		for (int i = 0;i != m_popup_btn_list.count();i++) {
			m_popup_btn_list.at(i)->setChecked(m_popup_flags & (1 << i));
			m_sound_btn_list.at(i)->setChecked(m_sound_flags & (1 << i));
		}
	}

}
