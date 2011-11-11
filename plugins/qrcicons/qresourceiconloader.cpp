/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "qresourceiconloader.h"
#include <QIcon>
#include <QDir>
#include <QLatin1Literal>
//#include "modulemanagerimpl.h"
#include <qutim/debug.h>
#include <qutim/systeminfo.h>
#include <QFile>

namespace Embedded
{

ResourceIconLoader::ResourceIconLoader()
{
	//TODO implement custom resource loading logic
	QDir dir(QLatin1String(":/icons"));
	QFileInfoList files = dir.entryInfoList(QDir::Files);
	for (int i = 0; i < files.size(); i++) {
		QString name = files.at(i).baseName();
		QStringRef iconName(&m_buffer, m_buffer.size(), name.size());
		m_buffer.append(name);
		m_icons.insert(iconName, files.at(i).absoluteFilePath());
	}
}

QIcon ResourceIconLoader::doLoadIcon(const QString &originName)
{
	// TODO: implement IconEngine, look XdgIconEngine for details
	return QIcon(doIconPath(originName,0));
}

QMovie *ResourceIconLoader::doLoadMovie(const QString &)
{
	//IMPLEMENT ME
	return 0;
}

QString ResourceIconLoader::doIconPath(const QString &name, uint iconSize)
{
	Q_UNUSED(iconSize);
	int index = name.length();
	IconHash::const_iterator it;
	while (index != -1) {
		it = m_icons.constFind(QStringRef(&name, 0, index));
		if (it != m_icons.constEnd())
			return it.value();
		index = name.lastIndexOf('-', index - 1);
	}
	return QString();
}

QString ResourceIconLoader::doMoviePath(const QString &, uint)
{
	//IMPLEMENT ME
	return QString();
}

} //namespace Embedded

