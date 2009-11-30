/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "popupappearance.h"
#include "../themehelper.h"
#include <QLayout>
#include "../popupwidget.h"
#include <QLayout>
#include "ui_popupappearance.h"
#include <libqutim/configbase.h>
#include "../manager.h"

namespace KineticPopups
{

	PopupAppearance::PopupAppearance ()
			:    ui(new Ui::AppearanceSettings)
	{
		ui->setupUi(this);
		connect(ui->pushButton,SIGNAL(clicked(bool)),SLOT(onTestButtonClicked(bool)));
		setProperty("id",tr("Preview"));
		ThemeHelper::PopupSettings settings = Manager::self()->popupSettings;
		settings.popupFlags = ThemeHelper::Preview;
		m_popup_widget = new PopupWidget(settings);
		layout()->addWidget(m_popup_widget);
		// 	QList<KineticPopupThemeHelper::PopupAppearance> theme_list = KineticPopupThemeHelper::getThemes();
		// 	QGridLayout *layout = new QGridLayout(this);
		// 	foreach (KineticPopupThemeHelper::PopupAppearance theme, theme_list)
		// 	{
		// 		KineticPopupWidget *popup_widget = new KineticPopupWidget(theme);
		// 		popup_widget->setData("Title", "Message", QLatin1String(":/icons/qutim_64"));
		//  		layout->addWidget(popup_widget);
		// 	}

	}

	PopupAppearance::~PopupAppearance()
	{
		delete ui;
	}



	void PopupAppearance::loadImpl()
	{
		disconnect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onCurrentIndexChanged(int)));
		ConfigGroup general = Config("appearance/kineticpopups").group("general");
		m_current_theme = general.value<QString>("themeName","default");
		getThemes();
		connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),SLOT(onCurrentIndexChanged(int)));
	}


	void PopupAppearance::saveImpl()
	{
		ConfigGroup general = Config("appearance/kineticpopups").group("general");
		general.setValue("themeName",m_current_theme);
		general.sync();
	}

	void PopupAppearance::cancelImpl()
	{

	}

	void PopupAppearance::getThemes()
	{
		QString category = "kineticpopups";
		QStringList list = listThemes(category);
		ui->comboBox->clear();
		int index = -1;
		foreach (QString theme,list)
		{
			ui->comboBox->addItem(theme,QVariant(theme));
			if ((index == -1) && (m_current_theme == theme))
				index = ui->comboBox->count() - 1;
		}
		ui->comboBox->setCurrentIndex(index == -1 ? 0 : index);
		onCurrentIndexChanged(ui->comboBox->currentIndex());
	}

	void PopupAppearance::onCurrentIndexChanged(int index)
	{
		m_current_theme = ui->comboBox->itemData(index).toString();
		emit modifiedChanged(true);
		preview();
	}

	void PopupAppearance::onTestButtonClicked(bool )
	{
		Manager::self()->loadTheme(ui->comboBox->itemData(ui->comboBox->currentIndex()).toString());
		Notifications::sendNotification(tr("Preview"),tr("This is a simple popup"));
		Notifications::sendNotification(qutim_sdk_0_3::Notifications::MessageGet,this,tr("Simple message"));
		Notifications::sendNotification(qutim_sdk_0_3::Notifications::MessageGet,this,tr("Another message"));
		Manager::self()->loadTheme(m_current_theme);
	}

	void PopupAppearance::preview()
	{
		QString theme_path = getThemePath("kineticpopups", m_current_theme);
		m_popup_widget->setTheme(ThemeHelper::loadThemeSetting(theme_path));
		m_popup_widget->setData(tr("Preview"),tr("Simple messagebox"), QLatin1String(":/icons/qutim_64"));
	}

}
