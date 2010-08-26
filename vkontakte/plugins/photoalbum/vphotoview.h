#ifndef VPHOTOVIEW_H
#define VPHOTOVIEW_H

#include <QWidget>
#include <QtDeclarative/QDeclarativeView>
#include <QVariantList>

class VContact;
class VConnection;
class QAbstractItemModel;
namespace Vkontakte
{

class VReply : public QObject
{
};

class VPhotoView : public QDeclarativeView
{
    Q_OBJECT
//	Q_PROPERTY (QObjectList albums READ albums NOTIFY albumsChanged)
public:
	explicit VPhotoView(QObject *contact);
	QObjectList albums() const;
signals:
	void albumsChanged();
public slots:
	void updateAlbums();
private slots:
	void onUpdateAlbumsFinished();
private:
	QObject *m_owner;
	QString m_uid;
	QObjectList m_albums;
	VConnection *m_connection;
	QMap<QString,QAbstractItemModel*> m_models;
};

}
#endif // VPHOTOVIEW_H
