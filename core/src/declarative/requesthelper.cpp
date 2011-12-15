/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev degtep@gmail.com
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "requesthelper.h"
#include <qutim/libqutim_version.h>
#include <qutim/systeminfo.h>
#include <QNetworkRequest>
#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <qutim/debug.h>

namespace Core {
using namespace qutim_sdk_0_3;
RequestHelper::RequestHelper(QObject *parent) : QNetworkAccessManager(parent)
{
	QRect size = qApp->desktop()->screenGeometry();
	QLocale locale = QLocale::system();

	QUrl url(QLatin1String("http://qutim.org/stats"));

	url.addQueryItem(QLatin1String("os"), SystemInfo::getVersion());
	url.addQueryItem(QLatin1String("short"), SystemInfo::getName());
	url.addQueryItem(QLatin1String("full"), SystemInfo::getFullName());
	url.addQueryItem(QLatin1String("version"), versionString());
	url.addQueryItem(QLatin1String("qt"), QLatin1String(qVersion()));
	url.addQueryItem(QLatin1String("wordSize"), QString::number(QSysInfo::WordSize));
	url.addQueryItem(QLatin1String("width"), QString::number(size.width()));
	url.addQueryItem(QLatin1String("height"), QString::number(size.height()));
	url.addQueryItem(QLatin1String("locale"), locale.name());

	QNetworkRequest request(url);
	QNetworkReply *reply = get(request);
	connect(reply, SIGNAL(finished()), SLOT(onFinished()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
			this, SLOT(onError(QNetworkReply::NetworkError)));
}

void RequestHelper::onFinished()
{
	deleteLater();
}

void RequestHelper::onError(QNetworkReply::NetworkError code)
{
	debug() << code;
	deleteLater();
}

} // namespace Core

