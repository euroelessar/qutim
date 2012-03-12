/****************************************************************************
**
** qutIM - instant messenger
**
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

#include "vreply.h"
#include <qutim/json.h>
#include <qutim/debug.h>

VReply::VReply(QNetworkReply *parent) :
    QObject(parent)
{
	connect(parent, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

void VReply::onRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	if (reply->error() != QNetworkReply::NoError) {
		QVariantMap error;
		error.insert("error_code", 1);
		error.insert("error_msg", reply->errorString());
		emit resultReady(error, true);
		return;
	}
	QVariantMap map = qutim_sdk_0_3::Json::parse(reply->readAll()).toMap();
	if (map.contains("error")) {
		QVariantMap error = map.value("error").toMap();
		if (error.value("error_code").toInt() == CaptchaNeeded) {
			QString sid = error.value("captcha_sid").toString();
			QString url = error.value("captcha_img").toString();
			QNetworkAccessManager *manager = reply->manager();
			qutim_sdk_0_3::debug() << Q_FUNC_INFO << sid << url;
#if 0
			QVariantMap params;
			QVariantList list = error.value("request_params").toList();
			for (int i = 0; i < list.size(); i++) {
				QVariantMap map = list.at(i).toMap();
				params.insert(map.value(QLatin1String("key")), map.value(QLatin1String("value")));
			}
			VCaptchaRequest *request = new VCaptchaRequest(params, sid, url, manager);
			setParent(request);
#else
			Q_UNUSED(manager);
#endif
		} else {
			qutim_sdk_0_3::debug() << "error" << error;
			emit resultReady(error, true);
		}
	} else {
		emit resultReady(map.value("response"), false);
	}
}

