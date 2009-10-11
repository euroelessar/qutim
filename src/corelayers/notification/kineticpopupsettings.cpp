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

#include "kineticpopupsettings.h"
#include "kineticpopupthemehelper.h"
#include <QLayout>
#include "kineticpopupwidget.h"
#include <QLayout>

KineticPopupSettings::KineticPopupSettings ( QWidget* parent, Qt::WindowFlags f ) : QWidget ( parent, f )
{
	QList<KineticPopupThemeHelper::PopupSettings> theme_list = KineticPopupThemeHelper::getThemes();
	QGridLayout *layout = new QGridLayout(this);
	foreach (KineticPopupThemeHelper::PopupSettings theme, theme_list)
	{
		KineticPopupWidget *popup_widget = new KineticPopupWidget(theme);
		popup_widget->setData("Title", "Message", QLatin1String(":/icons/qutim_64"));
 		layout->addWidget(popup_widget);
	}
}
