/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "yandexnarod.h"
#include "requestauthdialog.h"
#include "yandexnarodauthorizator.h"
#include "yandexnaroduploadjob.h"
#include <qutim/actiongenerator.h>
#include <qutim/contact.h>
#include <qutim/message.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/settingslayer.h>
#include <qutim/configbase.h>
#include <qutim/debug.h>

struct YandexNarodScope
{
	QNetworkAccessManager *networkManager;
	YandexNarodCookieJar *cookieJar;
	YandexNarodAuthorizator *authorizator;
};

Q_GLOBAL_STATIC(YandexNarodScope, scope)

void YandexNarodPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "YandexNarod"),
			QT_TRANSLATE_NOOP("Plugin", "Send files via Yandex.Narod filehosting service"),
			PLUGIN_VERSION(0, 2, 1, 0));
	setCapabilities(Loadable);
	addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
			  QT_TRANSLATE_NOOP("Task","Developer"),
			  QLatin1String("gorthauer87@yandex.ru"),
			  QLatin1String("sauron.me"));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Ruslan Nigmatullin"),
			  QT_TRANSLATE_NOOP("Task", "Developer"),
			  QLatin1String("euroelessar@yandex.ru"));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	addAuthor(QT_TRANSLATE_NOOP("Author","Alexander Kazarin"),
			  QT_TRANSLATE_NOOP("Task","Author"),
			  QLatin1String("boiler@co.ru"));
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Yandex.Narod"),
				 QT_TRANSLATE_NOOP("Plugin", "Send files via Yandex.Narod filehosting service"),
				 new SingletonGenerator<YandexNarodFactory>(),
				 ExtensionIcon(""));
}

bool YandexNarodPlugin::load()
{
	SettingsItem *settings = new GeneralSettingsItem<YandexNarodSettings>(
			Settings::Plugin,
			QIcon(),
			QT_TRANSLATE_NOOP("Yandex", "Yandex Narod"));
	settings->connect(SIGNAL(testclick()), this,  SLOT(on_btnTest_clicked()));
	Settings::registerItem(settings);
	scope()->networkManager = new QNetworkAccessManager(this);
	scope()->cookieJar = new YandexNarodCookieJar(scope()->networkManager);
	scope()->authorizator = new YandexNarodAuthorizator(scope()->networkManager);
	loadCookies();
	connect(scope()->authorizator, SIGNAL(needSaveCookies()), SLOT(saveCookies()));
	connect(scope()->networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(saveCookies()));

	return true;
}

bool YandexNarodPlugin::unload()
{
	return false;
}

void YandexNarodPlugin::loadCookies()
{
#if HAS_NO_TOKEN_AUTHORIZATION
	Config config;
	config.beginGroup("yandex");
	QList<QNetworkCookie> cookies;
	QStringList rawCookies = config.value("cookies", QStringList(), Config::Crypted);
	foreach (const QString &cookie, rawCookies)
		cookies << QNetworkCookie::parseCookies(cookie.toLatin1());
	scope()->cookieJar->setAllCookies(cookies);
#endif
}

void YandexNarodPlugin::saveCookies()
{
#if HAS_NO_TOKEN_AUTHORIZATION
	QStringList cookies;
	foreach (const QNetworkCookie &cookie, scope()->cookieJar->allCookies())
		cookies << QLatin1String(cookie.toRawForm());
	Config config;
	config.beginGroup("yandex");
	config.setValue("cookies", cookies, Config::Crypted);
#else
	Config config;
	config.beginGroup(QLatin1String("yandex"));
	config.setValue(QLatin1String("token"), scope()->authorizator->token(), Config::Crypted);
#endif
}

void YandexNarodPlugin::onActionClicked(QObject *obj)
{
	Q_UNUSED(obj);
//	m_uploadWidget = new YandexNarodUploadDialog();
//	connect(m_uploadWidget, SIGNAL(canceled()), this, SLOT(removeUploadWidget()));
//
//	m_uploadWidget->show();
//
//		fi.setFile(filepath);
//		group.setValue("lastdir", fi.dir().path());
//		group.sync();
//
//		m_netMan = new YandexNarodNetMan(m_uploadWidget);
//		m_uploadWidget->setContact(contact);
//		connect(m_netMan, SIGNAL(statusText(QString)), m_uploadWidget, SLOT(setStatus(QString)));
//		connect(m_netMan, SIGNAL(statusFileName(QString)), m_uploadWidget, SLOT(setFilename(QString)));
//		connect(m_netMan, SIGNAL(transferProgress(qint64,qint64)), m_uploadWidget, SLOT(progress(qint64,qint64)));
//		connect(m_netMan, SIGNAL(uploadFileURL(QString)), this, SLOT(onFileURL(QString)));
//		connect(m_netMan, SIGNAL(uploadFinished()), m_uploadWidget, SLOT(setDone()));
//		m_netMan->startUploadFile(filepath);
//	}
//	else {
//		delete m_uploadWidget; m_uploadWidget=0;
//	}
//
//	authtest=false;
}

void YandexNarodPlugin::onManageClicked()
{
//	if (m_manageDialog.isNull()) {
//		m_manageDialog = new YandexNarodManager();
//		m_manageDialog->show();
//	}
}

void YandexNarodPlugin::on_btnTest_clicked()
{
	YandexNarodSettings *settingsWidget = qobject_cast<YandexNarodSettings *>(sender());
//	testnetman = new YandexNarodNetMan(settingsWidget);
//	connect(testnetman, SIGNAL(statusText(QString)), settingsWidget, SLOT(setStatus(QString)));
//	connect(testnetman, SIGNAL(finished()), this , SLOT(on_TestFinished()));
//	debug() << settingsWidget->getLogin() << settingsWidget->getPasswd();
//	testnetman->startAuthTest(settingsWidget->getLogin(), settingsWidget->getPasswd());
	YandexNarodAuthorizator *auth = new YandexNarodAuthorizator(settingsWidget);
	auth->requestAuthorization(settingsWidget->getLogin(), settingsWidget->getPasswd());
}

void YandexNarodPlugin::on_TestFinished()
{
//	delete testnetman;
}

void YandexNarodPlugin::actionStart()
{
}

void YandexNarodPlugin::onFileURL(const QString &)
{
//	YandexNarodNetMan *netMan = qobject_cast<YandexNarodNetMan *>(sender());
//	Q_ASSERT(netMan);
//
//	if (Contact *contact = netMan->contact()) {
//		QString sendmsg = Config().group("yandexnarod").value("template", QString("File sent: %N (%S bytes)\n%U"));
//		sendmsg.replace("%N", fi.fileName());
//		sendmsg.replace("%U", url);
//		sendmsg.replace("%S", QString::number(fi.size()));
//		m_uploadWidget->setStatus(tr("File sent"));
//		m_uploadWidget->close();
//		Message message;
//		message.setText(sendmsg);
//		contact->account()->getUnitForSession(contact)->sendMessage(message);
//	}
}

YandexNarodFactory::YandexNarodFactory() :
	FileTransferFactory(tr("Yandex.Narod"), 0)
{
	setIcon(QIcon(":/icons/yandexnarodplugin.png"));
	foreach (Protocol *protocol, Protocol::all()) {
		foreach (Account *account, protocol->accounts())
			onAccountAdded(account);
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				SLOT(onAccountAdded(qutim_sdk_0_3::Account*)));
	}
}

bool YandexNarodFactory::checkAbility(ChatUnit *unit)
{
	Q_ASSERT(unit);
	Status status = unit->account()->status();
	return status != Status::Offline && status != Status::Connecting;
}

bool YandexNarodFactory::startObserve(ChatUnit *unit)
{
	Q_ASSERT(unit);
	m_observedUnits.insert(unit->account(), unit);
	return true;
}

bool YandexNarodFactory::stopObserve(ChatUnit *unit)
{
	Q_ASSERT(unit);
	Observers::iterator itr = m_observedUnits.begin();
	while (itr != m_observedUnits.end()) {
		if (*itr == unit)
			itr = m_observedUnits.erase(itr);
		else
			++itr;
	}
	return true;
}

FileTransferJob *YandexNarodFactory::create(ChatUnit *unit)
{
	return new YandexNarodUploadJob(unit, this);
}

QNetworkAccessManager *YandexNarodFactory::networkManager()
{
	return scope()->networkManager;
}

YandexNarodAuthorizator *YandexNarodFactory::authorizator()
{
	return scope()->authorizator;
}

void YandexNarodFactory::onAccountAdded(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
}

void YandexNarodFactory::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	bool isOnline = status != Status::Offline && status != Status::Connecting;
	foreach (ChatUnit *unit, m_observedUnits.values(sender()))
		changeAvailability(unit, isOnline);
}

YandexRequest::YandexRequest(const QUrl &url)
{
	QUrl tmp = url;
	tmp.addQueryItem(QLatin1String("oauth_token"), scope()->authorizator->token());
	setUrl(tmp);
	debug() << tmp;
//	QByteArray token = scope()->authorizator->token().toLatin1();
//	setRawHeader("Authorization", "OAuth " + token);
//	debug() << token;
//	setRawHeader("Authorization", token);
}

QUTIM_EXPORT_PLUGIN(YandexNarodPlugin)

