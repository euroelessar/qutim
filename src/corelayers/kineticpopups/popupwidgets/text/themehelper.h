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

#ifndef THEMEHELPER_H
#define THEMEHELPER_H
#include <QString>
#include <QSize>

namespace Core
{
namespace KineticPopups
{
namespace ThemeHelper
{
	enum PopupWidgetFlag
	{
		None = 0x0,
		Preview = 0x1,
		AeroThemeIntegration	=	0x2,
		Transparent = 0x4,
	};
	Q_DECLARE_FLAGS(PopupWidgetFlags,PopupWidgetFlag)
	struct PopupSettings {
		QString styleSheet;
		QString content;
		QString themePath;
		QSize defaultSize;
		int margin;
		Qt::WindowFlags widgetFlags;
		PopupWidgetFlags popupFlags;

	};
	PopupSettings loadThemeSetting(const QString &themePath);
}
}
}

#endif // THEMEHELPER_H
