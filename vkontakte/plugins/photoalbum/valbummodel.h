#ifndef VALBUMMODEL_H
#define VALBUMMODEL_H
#include <QStandardItemModel>

class VConnection;
namespace Vkontakte
{

//class VAlbumModel : public QStandardItemModel
//{
//	Q_OBJECT
//public:
//	enum AlbumRole
//	{
//		SmallImageRole,
//		BigImageRole
//	};
//	VAlbumModel(VConnection *c,const QString &uid,const QString &aid);
//	Q_INVOKABLE void update();
//private slots:
//	void onUpdateFinished();
//private:
//	VConnection *m_connection;
//	QString m_aid;
//	QString m_uid;
//};

class VAlbum : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(QVariantList photos READ photos NOTIFY photosChanged)
public:
	VAlbum(VConnection *c,const QString &uid,const QString &aid);
	void setTitle(const QString &string);
	QString title() const;
	QVariantList photos() const;
public slots:
	void update();
signals:
	void titleChanged();
	void photosChanged();
private slots:
	void onUpdateFinished();
private:
	VConnection *m_connection;
	QString m_aid;
	QString m_uid;
	QVariantList m_photos;
	QString m_title;
};

}

#endif // VALBUMMODEL_H
