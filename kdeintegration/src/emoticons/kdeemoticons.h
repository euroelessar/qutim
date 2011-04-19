#ifndef KDEEMOTICONSLAYER_H
#define KDEEMOTICONSLAYER_H

#include <QObject>
#include "kdeemoticonsprovider.h"

using namespace qutim_sdk_0_3;

namespace KdeIntegration
{
class KdeEmoticons : public EmoticonsBackend
{
	Q_OBJECT
public:
	KdeEmoticons();
	KEmoticons *self();
	virtual QStringList themeList();
	virtual EmoticonsProvider *loadTheme(const QString &name);
private:
	KEmoticonsTheme m_theme;
};
}

#endif // KDEEMOTICONSLAYER_H
