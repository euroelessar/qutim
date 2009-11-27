#ifndef KOPETEEMOTICONSBACKEND_H
#define KOPETEEMOTICONSBACKEND_H
#include <libqutim/emoticons.h>

using namespace qutim_sdk_0_3;

class KopeteEmoticonsBackend : public EmoticonsBackend
{
	Q_OBJECT
public:
    virtual EmoticonsProvider* loadTheme(const QString& name);
    virtual QStringList themeList();	
    virtual ~KopeteEmoticonsBackend();
};

#endif // KOPETEEMOTICONSBACKEND_H
