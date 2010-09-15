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
#include <QLayout>
#include <QLayout>
#include "ui_popupappearance.h"
#include <qutim/configbase.h>
#include <qutim/objectgenerator.h>
#include <qutim/thememanager.h>
#include "manager.h"
#include "backend.h"

namespace Core
{
namespace KineticPopups
{

PopupAppearance::PopupAppearance ()
		:    ui(new Ui::AppearanceSettings)
{
	ui->setupUi(this);
	connect(ui->pushButton,SIGNAL(clicked(bool)),SLOT(onTestButtonClicked(bool)));
	setProperty("name",tr("Preview"));
	setProperty("avatar",QLatin1String(":/icons/qutim_64"));

	const ObjectGenerator *gen = ObjectGenerator::module<AbstractPopupWidget>().value(0);
	Q_ASSERT(gen);
	m_popup_widget = gen->generate<AbstractPopupWidget>();
	Q_ASSERT(m_popup_widget);

	ui->verticalLayout_2->addWidget(m_popup_widget);

	connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),SLOT(onCurrentIndexChanged(int)));
}

PopupAppearance::~PopupAppearance()
{
	m_popup_widget->deleteLater();
	delete ui;
}



void PopupAppearance::loadImpl()
{
	Config general = Config("appearance").group("qmlpopups/general");
	m_current_theme = general.value<QString>("themeName","default");
	getThemes();
}


void PopupAppearance::saveImpl()
{
	Config general = Config("appearance").group("qmlpopups/general");
	general.setValue("themeName",m_current_theme);
	general.sync();
	Manager::self()->loadSettings();
}

void PopupAppearance::cancelImpl()
{

}

void PopupAppearance::getThemes()
{
	ui->comboBox->blockSignals(true);
	QString category = "qmlpopups";
	QStringList list = ThemeManager::list(category);
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
	ui->comboBox->blockSignals(false);
}

void PopupAppearance::onCurrentIndexChanged(int index)
{
	m_current_theme = ui->comboBox->itemData(index).toString();
	emit modifiedChanged(true);
	preview();
}

void PopupAppearance::onTestButtonClicked(bool )
{
	Manager::self()->themeName = ui->comboBox->itemData(ui->comboBox->currentIndex()).toString();
	Notifications::send(tr("Preview"),tr("This is a simple popup"));
	Notifications::send(qutim_sdk_0_3::Notifications::MessageGet,this,tr("Simple message"));
	Notifications::send(qutim_sdk_0_3::Notifications::MessageGet,this,tr("Another message"));
	Manager::self()->themeName = m_current_theme;
}

void PopupAppearance::preview()
{
	m_popup_widget->setTheme(m_current_theme);
	m_popup_widget->setData(tr("Preview"),tr("Simple kinetic popup widget."), this, QVariant());
}

}
}
