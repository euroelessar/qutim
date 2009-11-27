#include "kopeteemoticonsbackend.h"
#include <libqutim/libqutim_global.h>
#include "kopeteemoticonsprovider.h"
#include <QDebug>

qutim_sdk_0_3::EmoticonsProvider* KopeteEmoticonsBackend::loadTheme(const QString& name)
{
	//TODO 
	QString path = getThemePath("emoticons",name);
	KopeteEmoticonsProvider *provider = new KopeteEmoticonsProvider(path);
	provider->loadTheme();
	qDebug() << "load emoticons theme:" << name << provider->themeName();
	return provider;
}

QStringList KopeteEmoticonsBackend::themeList()
{
	//TODO
	QStringList themes = listThemes("emoticons");
	QStringList::const_iterator it;
	QStringList themeList;
	for (it=themes.constBegin();it!=themes.constEnd();it++) {
		QString themePath = getThemePath("emoticons",*it);
		KopeteEmoticonsProvider provider (themePath);
		qDebug() << "emoticons theme name : " << themePath << provider.themeName();
		if (!provider.themeName().isEmpty())
			themeList.append(provider.themeName());
	}
	return themeList;
}

KopeteEmoticonsBackend::~KopeteEmoticonsBackend()
{

}
