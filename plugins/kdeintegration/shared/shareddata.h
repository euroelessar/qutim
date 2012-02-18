/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Rustam Chakin <qutim.develop@gmail.com>
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <KAboutData>
#include <KComponentData>
#include <KGlobal>
#include <QApplication>
#include <KGlobalSettings>

namespace KdeIntegration
{
	static KComponentData activeComponent()
	{
		KComponentData data = KGlobal::activeComponent();
		if(!data.isValid())
		{
			QApplication::setPalette(KGlobalSettings::createApplicationPalette());
			KAboutData aboutData("qutim",
								  0,
								  ki18n("qutIM Instant Messenger"),
								  QCoreApplication::applicationVersion().toUtf8());
			aboutData.setHomepage("http://qutim.org");
			aboutData.addAuthor(ki18n("Ruslan Nigmatullin"), ki18n("Main developer"), "euroelessar@yandex.ru");
			aboutData.setShortDescription(ki18n("Communicate over IM"));
			aboutData.addLicense(KAboutData::License_GPL_V3);
			aboutData.setBugAddress("euroelessar@yandex.ru");
			aboutData.setOrganizationDomain(QCoreApplication::organizationDomain().toUtf8());
			data = KComponentData(aboutData);
			KGlobal::setActiveComponent(data);
		}
		return data;
	}

	inline static void ensureActiveComponent() { Q_UNUSED(activeComponent()); }
}

#endif // SHAREDDATA_H

