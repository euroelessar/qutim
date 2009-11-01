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

#include "popupsettings.h"
#include "themehelper.h"
#include <QLayout>
#include "popupwidget.h"
#include <QLayout>
#include "ui_popupsettings.h"

namespace KineticPopups
{

  PopupSettings::PopupSettings ()
  :    ui(new Ui::AppearanceSettings)
  {
	  ui->setupUi(this);
  // 	QList<KineticPopupThemeHelper::PopupSettings> theme_list = KineticPopupThemeHelper::getThemes();
  // 	QGridLayout *layout = new QGridLayout(this);
  // 	foreach (KineticPopupThemeHelper::PopupSettings theme, theme_list)
  // 	{
  // 		KineticPopupWidget *popup_widget = new KineticPopupWidget(theme);
  // 		popup_widget->setData("Title", "Message", QLatin1String(":/icons/qutim_64"));
  //  		layout->addWidget(popup_widget);
  // 	}

  }

  PopupSettings::~PopupSettings()
  {
	  delete ui;
  }



  void PopupSettings::loadImpl()
  {

  }


  void PopupSettings::saveImpl()
  {

  }

  void PopupSettings::cancelImpl()
  {

  }

}