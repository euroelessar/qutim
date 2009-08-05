#include "contactlistsettings.h"
#include <QDebug>

ContactListSettings::ContactListSettings(const QString &profile_name, QWidget *parent)
: QWidget(parent)
{
	ui.setupUi(this);
	m_profile_name = profile_name;
	m_gui_changed=false;
	loadSettings();
	connect (ui.showAccountsBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.showGroupsBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.accountFontBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.groupFontBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.onlineFontBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.offlineFontBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.separatorFontBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.accountFontComboBox, SIGNAL(currentFontChanged ( const QFont & )), this, SLOT(widgetSettingsChanged()));
	connect (ui.groupFontComboBox, SIGNAL(currentFontChanged ( const QFont & )), this, SLOT(widgetSettingsChanged()));
	connect (ui.onlineFontComboBox, SIGNAL(currentFontChanged ( const QFont & )), this, SLOT(widgetSettingsChanged()));
	connect (ui.offlineFontComboBox, SIGNAL(currentFontChanged ( const QFont & )), this, SLOT(widgetSettingsChanged()));
	connect (ui.separatorFontComboBox, SIGNAL(currentFontChanged ( const QFont & )), this, SLOT(widgetSettingsChanged()));
	connect (ui.accountFontSizeBox, SIGNAL(valueChanged ( int )), this, SLOT(widgetSettingsChanged()));
	connect (ui.groupFontSizeBox, SIGNAL(valueChanged ( int )), this, SLOT(widgetSettingsChanged()));
	connect (ui.onlineFontSizeBox, SIGNAL(valueChanged ( int )), this, SLOT(widgetSettingsChanged()));
	connect (ui.offlineFontSizeBox, SIGNAL(valueChanged ( int )), this, SLOT(widgetSettingsChanged()));
	connect (ui.separatorFontSizeBox, SIGNAL(valueChanged ( int )), this, SLOT(widgetSettingsChanged()));		
	connect (ui.showAccountsBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.hideEmptyBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.offlineBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.separatorBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.sortstatusCheckBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.alternatingCheckBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.clientIconCheckBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect (ui.avatarIconCheckBox, SIGNAL(stateChanged(int)), this, SLOT(widgetSettingsChanged()));
	connect(ui.opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySliderValueChanged(int)));
    connect(ui.winStyleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetGuiSettingsChanged()));
}

ContactListSettings::~ContactListSettings()
{
}

void ContactListSettings::loadSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("contactlist");
#if defined(Q_OS_WIN32)
	int nWindowStyle = settings.value("windowstyle", 1).toInt();
#else
	int nWindowStyle = settings.value("windowstyle", 0).toInt();
#endif
	ui.winStyleComboBox->setCurrentIndex(nWindowStyle);
	ui.alternatingCheckBox->setChecked(settings.value("alternatingrc",false).toBool());
	ui.avatarIconCheckBox->setChecked(settings.value("showicon1",false).toBool());
	ui.clientIconCheckBox->setChecked(settings.value("showicon12",false).toBool());
	ui.showAccountsBox->setChecked((settings.value("modeltype",0).toInt()&1)==0);
	ui.showGroupsBox->setChecked((settings.value("modeltype",0).toInt()&2)==0);
	ui.offlineBox->setChecked(!settings.value("showoffline",false).toBool());
	ui.hideEmptyBox->setChecked(!settings.value("showempty",false).toBool());
	ui.sortstatusCheckBox->setChecked(settings.value("sortstatus",false).toBool());
	ui.separatorBox->setChecked(!settings.value("showseparator",false).toBool());
	ui.opacitySlider->setValue(settings.value("opacity",1).toDouble()*100);
	ui.clOpacityPersLbl->setText(QString("%1%").arg(ui.opacitySlider->value()));
	QString family=settings.value("accountfontfml",QFont().family()).toString();
	int size=settings.value("accountfontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	ui.accountFontComboBox->setCurrentFont(QFont(family));
	ui.accountFontSizeBox->setValue(size);
	
	family=settings.value("groupfontfml",QFont().family()).toString();
	size=settings.value("groupfontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	ui.groupFontComboBox->setCurrentFont(QFont(family));
	ui.groupFontSizeBox->setValue(size);	
	
	family=settings.value("onlinefontfml",QFont().family()).toString();
	size=settings.value("onlinefontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	ui.onlineFontComboBox->setCurrentFont(QFont(family));
	ui.onlineFontSizeBox->setValue(size);	
	
	family=settings.value("offlinefontfml",QFont().family()).toString();
	size=settings.value("offlinefontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	ui.offlineFontComboBox->setCurrentFont(QFont(family));
	ui.offlineFontSizeBox->setValue(size);	
	
	family=settings.value("separatorfontfml",QFont().family()).toString();
	size=settings.value("separatorfontpnt",QFont().pointSize()).toInt();
	size=qBound(6, size, 24);
	ui.separatorFontComboBox->setCurrentFont(QFont(family));
	ui.separatorFontSizeBox->setValue(size);	
	
	QVariant color;
	color=settings.value("accountcolor");
	m_account_col=color.canConvert<QColor>()?qvariant_cast<QColor>(color):QColor(0,0,0);
	color=settings.value("groupcolor");
	m_group_col=color.canConvert<QColor>()?qvariant_cast<QColor>(color):QColor(0,0,0);
	color=settings.value("onlinecolor");
	m_online_col=color.canConvert<QColor>()?qvariant_cast<QColor>(color):QColor(0,0,0);
	color=settings.value("offlinecolor");
	m_offline_col=color.canConvert<QColor>()?qvariant_cast<QColor>(color):QColor(0,0,0);
	color=settings.value("separatorcolor");
	m_separator_col=color.canConvert<QColor>()?qvariant_cast<QColor>(color):QColor(0,0,0);
	ui.accountFontBox->setChecked(settings.value("useaccountfont",false).toBool());
	ui.groupFontBox->setChecked(settings.value("usegroupfont",false).toBool());
	ui.onlineFontBox->setChecked(settings.value("useonlinefont",false).toBool());
	ui.offlineFontBox->setChecked(settings.value("useofflinefont",false).toBool());
	ui.separatorFontBox->setChecked(settings.value("useseparatorfont",false).toBool());
	settings.endGroup();	
}
void ContactListSettings::saveSettings()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup("contactlist");
    settings.setValue("windowstyle", ui.winStyleComboBox->currentIndex());
	settings.setValue("opacity",((double)ui.opacitySlider->value())/100.0);
	settings.setValue("alternatingrc",ui.alternatingCheckBox->isChecked());
	settings.setValue("showicon1",ui.avatarIconCheckBox->isChecked());
	settings.setValue("showicon12",ui.clientIconCheckBox->isChecked());
	int model_type = ui.showAccountsBox->isChecked()?0:1;
	model_type += ui.showGroupsBox->isChecked()?0:2;
	settings.setValue("modeltype",model_type);
	settings.setValue("showoffline",!ui.offlineBox->isChecked());
	settings.setValue("showempty",!ui.hideEmptyBox->isChecked());
	settings.setValue("sortstatus",ui.sortstatusCheckBox->isChecked());
	settings.setValue("showseparator",!ui.separatorBox->isChecked());
	settings.setValue("accountfontfml",ui.accountFontComboBox->currentFont().family());
	settings.setValue("accountfontpnt",ui.accountFontSizeBox->value());
	settings.setValue("useaccountfont",ui.accountFontBox->isChecked());
	settings.setValue("groupfontfml",ui.groupFontComboBox->currentFont().family());
	settings.setValue("groupfontpnt",ui.groupFontSizeBox->value());
	settings.setValue("usegroupfont",ui.groupFontBox->isChecked());
	settings.setValue("onlinefontfml",ui.onlineFontComboBox->currentFont().family());
	settings.setValue("onlinefontpnt",ui.onlineFontSizeBox->value());
	settings.setValue("useonlinefont",ui.onlineFontBox->isChecked());
	settings.setValue("offlinefontfml",ui.offlineFontComboBox->currentFont().family());
	settings.setValue("offlinefontpnt",ui.offlineFontSizeBox->value());
	settings.setValue("useofflinefont",ui.offlineFontBox->isChecked());
	settings.setValue("separatorfontfml",ui.separatorFontComboBox->currentFont().family());
	settings.setValue("separatorfontpnt",ui.separatorFontSizeBox->value());
	settings.setValue("useseparatorfont",ui.separatorFontBox->isChecked());
	settings.setValue("accountcolor",m_account_col);
	settings.setValue("groupcolor",m_group_col);
	settings.setValue("onlinecolor",m_online_col);
	settings.setValue("offlinecolor",m_offline_col);
	settings.setValue("separatorcolor",m_separator_col);
	settings.endGroup();
//	if(m_gui_changed)
//		AbstractContactList::instance().loadGuiSettings();
	emit settingsSaved();
}
void ContactListSettings::on_accountColorButton_clicked()
{
	QColor color = QColorDialog::getColor(m_account_col, this);
	if(color.isValid() && m_account_col!=color)
	{
		m_account_col=color;
		emit settingsChanged();
	}
}
void ContactListSettings::on_groupColorButton_clicked()
{	
	QColor color = QColorDialog::getColor(m_group_col, this);
	if(color.isValid() && m_group_col!=color)
	{
		m_group_col=color;
		emit settingsChanged();
	}
}
void ContactListSettings::on_onlineColorButton_clicked()
{	
	QColor color = QColorDialog::getColor(m_online_col, this);
	if(color.isValid() && m_online_col!=color)
	{
		m_online_col=color;
		emit settingsChanged();
	}
}
void ContactListSettings::on_offlineColorButton_clicked()
{
	QColor color = QColorDialog::getColor(m_offline_col, this);
	if(color.isValid() && m_offline_col!=color)
	{
		m_offline_col=color;
		emit settingsChanged();
	}	
}
void ContactListSettings::on_separatorColorButton_clicked()
{	
	QColor color = QColorDialog::getColor(m_separator_col, this);
	if(color.isValid() && m_separator_col!=color)
	{
		m_separator_col=color;
		emit settingsChanged();
	}
}
void ContactListSettings::widgetSettingsChanged()
{
	emit settingsChanged();
}
void ContactListSettings::widgetGuiSettingsChanged()
{
	m_gui_changed=true;
	emit settingsChanged();
}
void ContactListSettings::onOpacitySliderValueChanged(int value)
{
        ui.clOpacityPersLbl->setText(QString("%1%").arg(value));
        m_gui_changed=true;
        emit settingsChanged();
}

