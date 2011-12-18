/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rustam Chakin, Ruslan Nigmatullin"));
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
			KAboutData about_data("qutim",
								  0,
								  ki18n("qutIM Instant Messenger"),
								  QCoreApplication::applicationVersion().toUtf8());
			about_data.setHomepage("http://qutim.org");
			about_data.addAuthor(ki18n("Rustam Chakin"), ki18n("Project founder"), "qutim.develop@gmail.com");
			about_data.addAuthor(ki18n("Ruslan Nigmatullin"), ki18n("Main developer"), "euroelessar@gmail.com");
			about_data.setShortDescription(ki18n("Communicate over IM"));
			about_data.addLicense(KAboutData::License_GPL_V2);
			about_data.addLicense(KAboutData::License_GPL_V3);
			about_data.setCopyrightStatement(ki18n("(c) 2008-2009, Rustam Chakin, Ruslan Nigmatullin"));
			about_data.setBugAddress("euroelessar@gmail.com");
			about_data.setOrganizationDomain(QCoreApplication::organizationDomain().toUtf8());
			data = KComponentData(about_data);
			KGlobal::setActiveComponent(data);
		}
		return data;
	}

	inline static void ensureActiveComponent() { Q_UNUSED(activeComponent()); }
}

#endif // SHAREDDATA_H

