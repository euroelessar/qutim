#ifndef RESOURCEICONLOADER_H
#define RESOURCEICONLOADER_H
#include <qutim/iconloader.h>
#include <QStringList>
#include <QHash>

namespace Embedded
{
using namespace qutim_sdk_0_3;

class ResourceIconLoader : public IconLoader
{
public:
	ResourceIconLoader();
	
protected:
	QIcon doLoadIcon(const QString &name);
	QMovie *doLoadMovie(const QString &name);
	QString doIconPath(const QString &name, uint iconSize);
	QString doMoviePath(const QString &name, uint iconSize);

private:
	typedef QHash<QStringRef, QString> IconHash;
	IconHash m_icons;
	QString m_buffer;
};

} //namespace Embedded

#endif // RESOURCEICONLOADER_H
