#ifndef VPHOTOALBUMPLUGIN_H
#define VPHOTOALBUMPLUGIN_H
#include <qutim/plugin.h>
#include <QMap>

namespace Vkontakte
{
using namespace qutim_sdk_0_3;
class VPhotoView;
class VPhotoAlbumPlugin : public Plugin
{
    Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private slots:
	void onViewPhotoTriggered(QObject *obj);
private:
	QMap<QObject*,VPhotoView*> m_albums;
};

}

#endif // VPHOTOALBUMPLUGIN_H
