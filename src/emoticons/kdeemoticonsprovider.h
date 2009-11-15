#ifndef KDEEMOTICONSPLUGIN_H
#define KDEEMOTICONSPLUGIN_H

#include <qutim/emoticons.h>
#include <kemoticons.h>

using namespace qutim_sdk_0_3;

namespace KdeIntegration
{
	class KdeEmoticonsProvider : public EmoticonsProvider
	{
	public:
		KdeEmoticonsProvider(const KEmoticonsTheme &theme);
		void reloadTheme();
		virtual QString themeName() const;
		virtual bool addEmoticon(const QString &imgPath, const QStringList &codes);
		virtual bool removeEmoticon(const QStringList &codes);
		virtual bool saveTheme();
	private:
		KEmoticonsTheme m_theme;
	};
}

#endif // KDEEMOTICONSPLUGIN_H
