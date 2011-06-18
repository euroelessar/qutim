#ifndef KDEICONLOADER_H
#define KDEICONLOADER_H

#include <qutim/iconloader.h>

using namespace qutim_sdk_0_3;

class KdeIconLoader : public IconLoader
{
	Q_OBJECT
public:
	explicit KdeIconLoader();

protected:
	QIcon doLoadIcon(const QString &name);
	QMovie *doLoadMovie(const QString &name);
	QString doIconPath(const QString &name, uint iconSize);
	QString doMoviePath(const QString &name, uint iconSize);
};

#endif // KDEICONLOADER_H
