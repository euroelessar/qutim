#include "kdeemoticonsprovider.h"
#include <shared/shareddata.h>
#include <QtDebug>

namespace KdeIntegration
{
	KdeEmoticonsProvider::KdeEmoticonsProvider(const KEmoticonsTheme &theme) : m_theme(theme)
	{
		reloadTheme();
	}

	void KdeEmoticonsProvider::reloadTheme()
	{
		clearEmoticons();
		QHash<QString, QStringList> map = m_theme.emoticonsMap();
		QHashIterator<QString, QStringList> it(map);
		while (it.hasNext()) {
			it.next();
			appendEmoticon(it.key(), it.value());
		}
	}

	QString KdeEmoticonsProvider::themeName() const
	{
		return m_theme.themeName();
	}

	bool KdeEmoticonsProvider::addEmoticon(const QString &imgPath, const QStringList &codes)
	{
		if (m_theme.addEmoticon(imgPath, codes.join(" "), KEmoticonsProvider::Copy)) {
			appendEmoticon(imgPath, codes);
			return true;
		}
		return false;
	}

	bool KdeEmoticonsProvider::removeEmoticon(const QStringList &codes)
	{
		if (m_theme.removeEmoticon(codes.join(" "))) {
			reloadTheme();
			return true;
		}
		return false;
	}

	bool KdeEmoticonsProvider::saveTheme()
	{
		m_theme.save();
		return true;
	}
}
