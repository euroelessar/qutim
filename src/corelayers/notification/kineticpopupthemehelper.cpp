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

#include "kineticpopupthemehelper.h"
#include <QTextDocument>
#include <QFile>
#include "kineticpopupbackend.h"
#include "libqutim/systeminfo.h"
#include "libqutim/configbase.h"
#include <QDebug>

namespace KineticPopupThemeHelper
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

	PopupSettings loadThemeSetting ( const QString& themePath )
	{
		PopupSettings popup_settings;
		popup_settings.themePath = themePath;
		popup_settings.styleSheet = loadContent(themePath,"content.css");
	    popup_settings.content = loadContent(themePath,"content.html");
		ConfigGroup appearance = Config(themePath + "/settings.json").group("appearance");
		popup_settings.defaultSize = appearance.value<QSize>("defaultSize",QSize(250,150));
		popup_settings.margin = appearance.value("margin",10);
		popup_settings.widgetFlags = static_cast<Qt::WindowFlags>(appearance.value<int>("widgetFlags",Qt::ToolTip));
		return popup_settings;
	}

	QList< PopupSettings > getThemes()
	{
		//TODO need optimization
		QList< PopupSettings > popup_settings_list;
		QDir theme_dir = SystemInfo::getDir(SystemInfo::ShareDir);
		theme_dir.cd("kineticpopups");
		QStringList theme_list = theme_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		theme_dir = SystemInfo::getDir(SystemInfo::SystemShareDir);
		theme_dir.cd("kineticpopups");
		theme_list << theme_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		theme_list.removeDuplicates();
		qDebug() << theme_list;
		foreach (QString theme_name, theme_list)
		{
			popup_settings_list.append(loadThemeSetting(getThemePath(theme_name,"kineticpopups")));
			qDebug() << getThemePath(theme_name,"kineticpopups");
		}
		return popup_settings_list;
	}



};