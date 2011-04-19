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
