#include "vphotoview.h"
#include "../../src/vcontact.h"
#include <qutim/config.h>
#include <QLatin1Literal>
#include <qutim/debug.h>
#include <QtDeclarative/QDeclarativeContext>
#include "../../src/vconnection.h"
#include "../../src/vaccount.h"
#include <qutim/json.h>
#include "valbummodel.h"
#include <qutim/thememanager.h>

#ifndef QT_NO_OPENGL
	#include <QtOpenGL/QtOpenGL>
#endif

namespace Vkontakte
{

VPhotoView::VPhotoView(QObject *obj) : m_owner(obj),m_connection(0)
{
#ifndef QT_NO_OPENGL
	setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

	if (VContact *c = qobject_cast<VContact*>(m_owner)) {
		m_connection = c->account()->connection();
		m_uid = c->id();
	}
	Q_ASSERT(m_connection);

	setAttribute(Qt::WA_DeleteOnClose);
	setResizeMode(QDeclarativeView::SizeRootObjectToView);

	QString theme_name = "default";
	QString themePath = ThemeManager::path("vphotoalbum",theme_name);

	QString filename =themePath % QLatin1Literal("/main.qml");

	rootContext()->setContextProperty("photoManager",this);

	setSource(QUrl::fromLocalFile(filename));//url - main.qml
	updateAlbums();
}

QObjectList VPhotoView::albums() const
{
	return m_albums;
}

void VPhotoView::updateAlbums()
{
	QVariantMap query;
	if (VContact *c = qobject_cast<VContact*>(m_owner)) {
		query.insert("uid",c->id());
	}

	QNetworkReply *reply = m_connection->get("photos.getAlbums",query);
	connect(reply,SIGNAL(finished()),SLOT(onUpdateAlbumsFinished()));
}

void VPhotoView::onUpdateAlbumsFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QByteArray rawData = reply->readAll();
	QVariantList items = Json::parse(rawData).toMap().value("response").toList();

	qDeleteAll(m_albums);
	m_albums.clear();

	foreach (QVariant item,items) {
		QVariantMap map = item.toMap();
		VAlbum *a = new VAlbum(m_connection,m_uid,map.value("aid").toString());
		a->setTitle(map.value("title").toString());
		m_albums.append(a);
	}
	rootContext()->setContextProperty("albumModel", QVariant::fromValue(m_albums));
	emit albumsChanged();
}


}
