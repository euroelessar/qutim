#include "kdeemoticons.h"
#include <QUrl>

K_GLOBAL_STATIC(KEmoticons, s_self)


namespace KdeIntegration
{
	KdeEmoticons::KdeEmoticons()
	{
		m_theme = self()->theme();
	}

	QStringList KdeEmoticons::themeList()
	{
		return self()->themeList();
	}

	EmoticonsProvider *KdeEmoticons::loadTheme(const QString &name)
	{
		KEmoticonsTheme theme = self()->theme(name);
		if (theme.isNull())
			return 0;
		return new KdeEmoticonsProvider(theme);
	}

	KEmoticons *KdeEmoticons::self()
	{
		return s_self;
	}
}
