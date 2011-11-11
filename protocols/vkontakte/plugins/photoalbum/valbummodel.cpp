/****************************************************************************
**
** qutIM - instant messenger
**
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "valbummodel.h"
#include "../../src/vconnection.h"
#include <qutim/json.h>
#include <qutim/debug.h>

namespace Vkontakte
{

//VAlbumModel::VAlbumModel(VConnection *c,const QString &uid,const QString &aid)
//	:	m_connection(c),m_aid(aid),m_uid(uid)
//{
//	QHash<int,QByteArray> names;
//	names.insert(SmallImageRole,"smallImage");
//	names.insert(BigImageRole,"bigImage");
//	setRoleNames(names);
//	update();
//}

VAlbum::VAlbum(VConnection *c,const QString &uid,const QString &aid) :
		m_connection(c),m_aid(aid),m_uid(uid)
{
	update();
}

void VAlbum::update()
{
	QVariantMap query;
	query.insert("aid",m_aid);
	query.insert("uid",m_uid);
	QNetworkReply *reply = m_connection->get("photos.get",query);
	connect(reply,SIGNAL(finished()),SLOT(onUpdateFinished()));
}

void VAlbum::onUpdateFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QByteArray rawData = reply->readAll();
	m_photos = Json::parse(rawData).toMap().value("response").toList();
	emit photosChanged();
}

QString VAlbum::title() const
{
	return m_title;
}

void VAlbum::setTitle(const QString &title)
{
	m_title = title;
}

QVariantList VAlbum::photos() const
{
	return m_photos;
}

}

