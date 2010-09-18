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

#include "themehelper.h"
#include <QTextDocument>
#include <QFile>
#include "backend.h"
#include <qutim/systeminfo.h>
#include <qutim/configbase.h>
#include <qutim/thememanager.h>
#include <QDebug>

namespace Core
{
namespace KineticPopups
{

namespace ThemeHelper
{
QString loadContent ( const QString& path,  const QString &file )
{
	QFile content (path + "/" + file);
	QString output;
	if (content.open(QIODevice::ReadOnly)) {
		output = content.readAll();
		output.replace("{themepath}",Qt::escape(path));
		content.close();
	}
	return output;
}

PopupSettings loadThemeSetting ( const QString& themeName )
{
	QString themePath = ThemeManager::path("textpopups",themeName);
	PopupSettings popup_settings;
	popup_settings.themePath = themePath;
	popup_settings.styleSheet = loadContent(themePath,"content.css");
	popup_settings.content = loadContent(themePath,"content.html");
	ConfigGroup appearance = Config(themePath + "/settings.json").group("appearance");
	popup_settings.defaultSize = appearance.value<QSize>("defaultSize",QSize(250,150));
	popup_settings.margin = appearance.value("margin",10);
	popup_settings.widgetFlags = static_cast<Qt::WindowFlags>(appearance.value<int>("widgetFlags",Qt::ToolTip));
	popup_settings.popupFlags = static_cast<PopupWidgetFlags>(appearance.value<int>("popupFlags",Transparent));
	return popup_settings;
}
}

}
}
