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

#include "statisticshelper_p.h"
#include "config.h"
#include "systeminfo.h"
#include "libqutim_version.h"
#include "profile.h"
#include <QApplication>
#include <QLocale>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QUrlQuery>
#include <QRect>
#include <QDesktopWidget>

namespace qutim_sdk_0_3 {

class StatisticsHelperPrivate
{
	Q_DECLARE_PUBLIC(StatisticsHelper)
public:
	StatisticsHelperPrivate(StatisticsHelper *q) : q_ptr(q) {}
	void init();
	void sendInfo();
	void _q_on_finished();
	
	StatisticsHelper *q_ptr;
	QVariantMap systemInfo;
	StatisticsHelper::Action action;
	QNetworkAccessManager manager;
};

void StatisticsHelperPrivate::init()
{
	QRect size = qApp->desktop()->screenGeometry();
	systemInfo.insert(QLatin1String("os"), SystemInfo::getVersion());
	systemInfo.insert(QLatin1String("short"), SystemInfo::getName());
	systemInfo.insert(QLatin1String("full"), SystemInfo::getFullName());
	systemInfo.insert(QLatin1String("version"), versionString());
	systemInfo.insert(QLatin1String("qt"), QLatin1String(qVersion()));
	systemInfo.insert(QLatin1String("wordSize"), QString::number(QSysInfo::WordSize));
	systemInfo.insert(QLatin1String("width"), QString::number(size.width()));
	systemInfo.insert(QLatin1String("height"), QString::number(size.height()));
	systemInfo.insert(QLatin1String("locale"), QLocale::system().name());
	
	Config config = Profile::instance()->config();
	config.beginGroup(QLatin1String("statistics"));
	bool denied = config.value(QLatin1String("denied"), false);
	bool automatic = config.value(QLatin1String("automatic"), false);
	if (denied && automatic) {
		action = StatisticsHelper::DeniedToSend;
		return;
	}
	QString key = config.value(QLatin1String("key"), QString());
	if (key.isEmpty()) {
		action = StatisticsHelper::NeedToAskInit;
		return;
	}
	for (QVariantMap::ConstIterator it = systemInfo.constBegin();
	     it != systemInfo.constEnd(); ++it) {
		if (config.value(it.key(), QString()) != it.value().toString()) {
			if (automatic) {
				sendInfo();
				action = StatisticsHelper::NothingToAsk;
				return;
			}
			action = StatisticsHelper::NeedToAskUpdate;
			return;
		}
	}
	action = StatisticsHelper::NothingToAsk;
}

void StatisticsHelperPrivate::sendInfo()
{
    QUrl url(QStringLiteral("http://qutim.org/stats"));
	QUrlQuery query;
	Config config = Profile::instance()->config();
	config.beginGroup(QLatin1String("statistics"));
	query.addQueryItem(QLatin1String("api"), QString::number(2));
	QString id = config.value(QLatin1String("key"), QString());
	if (!id.isEmpty())
		query.addQueryItem(QLatin1String("key"), id);
	for (QVariantMap::ConstIterator it = systemInfo.constBegin();
	     it != systemInfo.constEnd(); ++it) {
		query.addQueryItem(it.key(), it.value().toString());
	}
    url.setQuery(query);
	QObject::connect(manager.get(QNetworkRequest(url)), SIGNAL(finished()),
	                 q_func(), SLOT(_q_on_finished()));
}

void StatisticsHelperPrivate::_q_on_finished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(q_func()->sender());
	Q_ASSERT(reply);
	if (reply->error() == QNetworkReply::NoError) {
		QString key = QLatin1String(reply->readAll());
		if (!key.isEmpty()) {
			Config config = Profile::instance()->config();
			config.beginGroup(QLatin1String("statistics"));
			config.setValue(QLatin1String("key"), key);
			for (QVariantMap::ConstIterator it = systemInfo.constBegin();
			     it != systemInfo.constEnd(); ++it) {
				config.setValue(it.key(), it.value());
			}
		}
	}
//	q_func()->deleteLater();
}

StatisticsHelper::StatisticsHelper(QObject *parent) :
    QObject(parent), d_ptr(new StatisticsHelperPrivate(this))
{
	Q_D(StatisticsHelper);
	d->init();
}

StatisticsHelper::~StatisticsHelper()
{
}

QString StatisticsHelper::infoHtml() const
{
	QRect size = qApp->desktop()->screenGeometry();
	return tr("<b>Short:</b> %1 <br />"
	          "<b>Version:</b> %2 <br />"
	          "<b>Full:</b> %3 <br />"
	          "<b>qutIM Version:</b> %4 <br />"
	          "<b>Qt Version:</b> %5 (%6 bit) <br />"
	          "<b>Screen resolution:</b> %7 x %8 <br />"
	          "<b>System locale:</b> %9<br />"
	          ).arg(SystemInfo::getName())
	        .arg(SystemInfo::getVersion())
	        .arg(SystemInfo::getFullName())
	        .arg(QLatin1String(versionString()))
	        .arg(QLatin1String(qVersion()))
	        .arg(QString::number(QSysInfo::WordSize))
	        .arg(size.width())
	        .arg(size.height())
	        .arg(QLocale::system().name());
}

StatisticsHelper::Action StatisticsHelper::action() const
{
	return d_func()->action;
}

void StatisticsHelper::setDecisition(bool denied, bool automatic)
{
	Q_D(StatisticsHelper);
	Config config = Profile::instance()->config();
	config.beginGroup(QLatin1String("statistics"));
	config.setValue(QLatin1String("denied"), denied);
	config.setValue(QLatin1String("automatic"), automatic);
	config.endGroup();
	if (!denied)
		d->sendInfo();
}

void StatisticsHelper::_q_on_finished()
{
	d_func()->_q_on_finished();
}

}
