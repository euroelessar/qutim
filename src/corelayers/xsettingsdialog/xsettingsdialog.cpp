#include "xsettingsdialog.h"
#include "ui_xsettingsdialog.h"
#include "xtoolbar.h"
#include <QLayout>
#include <QPropertyAnimation>
#include <QStateMachine>
#include <QDebug>
#include <libqutim/icon.h>
#include "xsettingsgroup.h"
#include <libqutim/configbase.h>
#include <libqutim/settingswidget.h>

XSettingsDialog::XSettingsDialog(const SettingsItemList& settings, QWidget* parent) :
        QDialog(parent),    ui(new Ui::XSettingsDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	//load settings
	ConfigGroup general_group = Config("appearance/xsettings").group("general");
	animated = general_group.value<bool>("animated", true);
	//init toolbar
	ui->setupUi(this);
	connect(ui->xtoolBar,SIGNAL(actionTriggered(QAction*)),SLOT(onActionTriggered(QAction*)));

	//init actions
	//TODO FIXME get rid of copypaste
	QAction *general =  new QAction(Icon("preferences-system"),tr("General"),ui->xtoolBar);
	general->setToolTip(tr("General configuration"));
	addAction(general,Settings::General);

	ui->xtoolBar->addSeparator();

	QAction *protocols =  new QAction(Icon("applications-internet"),tr("Protocols"),ui->xtoolBar);
	protocols->setToolTip(tr("Accounts and protocols settings"));
	addAction(protocols,Settings::Protocol);

	QAction *appearance =  new QAction(Icon("applications-graphics"),tr("Appearance"),ui->xtoolBar);
	appearance->setToolTip(tr("Appearance settings"));
	addAction(appearance,Settings::Appearance);

	QAction *plugins =  new QAction(Icon("applications-other"),tr("Plugins"),ui->xtoolBar);
	plugins->setToolTip(tr("Additional plugins settings"));
	addAction(plugins,Settings::Plugin);

	//init categories

	foreach (SettingsItem *item, settings)
	{
		if (item->type() >= m_settings_items.size())
			m_settings_items.resize(item->type() + 1);
		m_settings_items[item->type()].append(settings);
	}
	general->trigger();
	if (animated)
		initAnimation();
}

XSettingsDialog::~XSettingsDialog()
{
    delete ui;
}


void XSettingsDialog::addAction (QAction* action, Settings::Type type)
{
	action->setProperty("category",type);
	action->setCheckable(true);
	ui->xtoolBar->addAction(action);
}


void XSettingsDialog::initAnimation()
{
	//init state machine
	ConfigGroup animation_group = Config("appearance/xsettings").group("animation");
	m_machine = new QStateMachine(this);
	QPropertyAnimation *animation = new QPropertyAnimation (ui->xtoolBar,"geometry",this);
	animation->setDuration(animation_group.value<int>("duration",500));
	animation->setEasingCurve(static_cast<QEasingCurve::Type>(animation_group.value<int>("easingCurve",QEasingCurve::OutSine)));
	m_machine->addDefaultAnimation(animation);
	//init states
	m_hide_state = new QState(m_machine);
	m_show_state = new QState(m_machine);

	m_hide_state->assignProperty(ui->xtoolBar,
								 "geometry",
								 QRect(0,-ui->xtoolBar->sizeHint().height(),width(),ui->xtoolBar->sizeHint().height())
								 );

	m_show_state->assignProperty(ui->xtoolBar,
								 "geometry",
								 QRect(0,0,width(),ui->xtoolBar->sizeHint().height())
								 );
	//init transitions
	m_hide_state->addTransition(this,SIGNAL(showed()),m_show_state);
	connect(m_show_state,SIGNAL(polished()),SLOT(showState()));
	//start machine
	m_machine->setInitialState(m_show_state);
	m_machine->start();
}

void XSettingsDialog::changeEvent(QEvent *e)
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


void XSettingsDialog::showEvent(QShowEvent* e)
{
	if (animated)
	{
		layout()->setEnabled(false);
		ui->xtoolBar->setGeometry(0,-ui->xtoolBar->sizeHint().height(),width(),ui->xtoolBar->sizeHint().height());
	}
    QDialog::showEvent(e);
	emit showed();
}

void XSettingsDialog::showState()
{
	if (animated)
		layout()->setEnabled(true);
}

void XSettingsDialog::onActionTriggered ( QAction* action )
{
	Settings::Type type = static_cast<Settings::Type>(action->property("category").toInt());
	SettingsItemList setting_items = m_settings_items.value(type);
	if (setting_items.count()>=0) // ==0 or >=0 need for testing, for normally usage use >1
	{
		//TODO need way to add custom group
		XSettingsGroup *group = m_group_widgets.value(type);
		if (group == 0)
		{
			group = new XSettingsGroup(setting_items,this);
			ui->settingsStackedWidget->addWidget(group);
		}
		ui->settingsStackedWidget->setCurrentWidget(group);
	}
	else
	{
		if (setting_items.count() == 0)
			return;
		SettingsWidget *widget = setting_items.at(0)->widget();
		if (widget == 0)
			return;
		widget->load();
		if (ui->settingsStackedWidget->indexOf(widget) == -1)
			ui->settingsStackedWidget->addWidget(widget);
		ui->settingsStackedWidget->setCurrentWidget(widget);
	}
}
