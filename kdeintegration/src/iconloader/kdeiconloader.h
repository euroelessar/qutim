#ifndef KDEICONLOADER_H
#define KDEICONLOADER_H

#include <qutim/iconloader.h>

using namespace qutim_sdk_0_3;

class KdeIconLoader : public IconLoader
{
	Q_OBJECT
public:
	explicit KdeIconLoader();
	virtual QIcon loadIcon(const QString &name);
	virtual QMovie *loadMovie(const QString &name);
	virtual QString iconPath(const QString &name, uint iconSize);
	virtual QString moviePath(const QString &name, uint iconSize);
};

#endif // KDEICONLOADER_H
