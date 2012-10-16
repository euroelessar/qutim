/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "vaccount.h"
#include <qutim/chatunit.h>
#include <qutim/passworddialog.h>
#include <qutim/systemintegration.h>

#include "vcontact.h"
#include "vprotocol.h"
#include "vroster.h"
#include "vinforequest.h"

#include <vreen/roster.h>
#include <vreen/contact.h>
#include <vreen/contentdownloader.h>

#include "vprotocol.h"
#include <vreen/auth/oauthconnection.h>

#include <QWebView>
#include <QWebFrame>

const static int qutimId = 1865463;

using namespace qutim_sdk_0_3;

VAccount::VAccount(const QString &email, VProtocol *protocol) :
	Account(email, protocol),
	m_client(new VClient(email, this))
{
	setObjectName("VAccount");

	connect(m_client, SIGNAL(connectionStateChanged(Vreen::Client::State)), SLOT(onClientStateChanged(Vreen::Client::State)));
	connect(m_client, SIGNAL(meChanged(Vreen::Buddy*)), SLOT(onMeChanged(Vreen::Buddy*)));
	connect(m_client, SIGNAL(invisibleChanged(bool)), SLOT(onInvisibleChanged(bool)));
	connect(m_client, SIGNAL(error(Vreen::Client::Error)), SLOT(onError(Vreen::Client::Error)));

	setInfoRequestFactory(new VInfoFactory(this));
	m_roster = new VRoster(this);
	VAccount::setStatus(Status::instance(Status::Offline, "vkontakte"));
}

VContact *VAccount::contact(int uid, bool create)
{
	return m_roster->contact(uid, create);
}

QString VAccount::name() const
{
	if (m_client->me())
		return m_client->me()->name();
	return email();
}

void VAccount::setStatus(Status status)
{
	m_client->setActivity(status.text());
	if (Account::status().type() != status.type()) {
		switch (status.type()) {
		case Status::Offline:
			m_client->disconnectFromHost();
			saveSettings();
			break;
		case Status::Connecting:
			break;
		default:
			m_client->connectToHost();
			m_client->setInvisible(status == Status::Invisible);
		};
	}
}

int VAccount::uid() const
{
	if (m_client->me())
		return m_client->me()->id();
	return 0;
}

QString VAccount::email() const
{
	return m_client->login();
}

Vreen::Connection *VAccount::connection() const
{
	return m_client->connection();
}

VRoster *VAccount::roster()
{
	return m_roster;
}

Vreen::Client *VAccount::client() const
{
	return m_client;
}

VContact *VAccount::me() const
{
	return m_me.data();
}

void VAccount::downloadAvatar(VContact *contact)
{
	QUrl url = contact->buddy()->photoSource(VK_PHOTO_SOURCE);
	QString path = contentDownloader()->download(url);
	m_contentRecieversHash.insert(path, contact);
}

void VAccount::loadSettings()
{
	Config cfg = config();
	Vreen::OAuthConnection *connection = new Vreen::OAuthConnection(qutimId, this);
	connection->setUid(cfg.value("access/uid", 0));
	connection->setAccessToken(cfg.value("access/token", QByteArray(), Config::Crypted),
							   cfg.value("access/expires", 0));
	connect(connection, SIGNAL(authConfirmRequested(QWebPage*)), SLOT(onAuthConfirmRequested(QWebPage*)));
	connect(connection, SIGNAL(accessTokenChanged(QByteArray,time_t)), SLOT(setAccessToken(QByteArray,time_t)));
	m_client->setConnection(connection);
}

void VAccount::saveSettings()
{
	config().setValue("access/uid", uid());
	if (Vreen::OAuthConnection *c = qobject_cast<Vreen::OAuthConnection*>(m_client->connection()))
		setAccessToken(c->accessToken(), c->expiresIn());
}

VRoster *VAccount::roster() const
{
	return m_roster.data();
}

Vreen::ContentDownloader *VAccount::contentDownloader() const
{
	return m_contentDownloader.data();
}

Vreen::ContentDownloader *VAccount::contentDownloader()
{
	if (!m_contentDownloader) {
		m_contentDownloader = new Vreen::ContentDownloader(this);
		connect(m_contentDownloader.data(), SIGNAL(downloadFinished(QString)),
				SLOT(onContentDownloaded(QString)), Qt::QueuedConnection);
	}
	return m_contentDownloader.data();
}

void VAccount::onMeChanged(Vreen::Buddy *me)
{
	if (!m_me || m_me->buddy() != me) {
		if (m_me)
			m_me->deleteLater();
		m_me = m_roster->contact(me->id());
		config().setValue("access/uid", uid());
		connect(m_me.data(), SIGNAL(nameChanged(QString, QString)),
				SIGNAL(nameChanged(QString, QString)));
	}
}

void VAccount::onInvisibleChanged(bool set)
{
	if (m_client->connectionState() == Vreen::Client::StateOnline) {
		Status s = status();
		s.setType(set ? Status::Invisible : Status::Online);
		Account::setStatus(s);
	}
}

void VAccount::onAuthConfirmRequested(QWebPage *page)
{
	QWebView *view = new QWebView;
	view->setPage(page);
	view->resize(1024, 768);
	connect(page, SIGNAL(destroyed()), view, SLOT(deleteLater()));
	SystemIntegration::show(view);
}

void VAccount::setAccessToken(const QByteArray &token, time_t expiresIn)
{
	Config cfg = config().group("access");
	cfg.setValue("token", token, Config::Crypted);
	cfg.setValue("expires", expiresIn);
}

void VAccount::onError(Vreen::Client::Error error)
{
	if (error == Vreen::Client::ErrorAuthorizationFailed)
		config("general").setValue("passwd", "");
}

void VAccount::onContentDownloaded(const QString &path)
{
	VContact *c = m_contentRecieversHash.take(path);
	if (c)
		c->setAvatar(path);
}

void VAccount::onClientStateChanged(Vreen::Client::State state)
{
	Status s = status();
	switch (state) {
	case Vreen::Client::StateOffline:
		s.setType(Status::Offline);
		break;
	case Vreen::Client::StateConnecting:
		s.setType(Status::Connecting);
		break;
	case Vreen::Client::StateOnline:
		s.setType(m_client->isInvisible() ? Status::Invisible : Status::Online);
		break;
	default:
		break;
	}
	Account::setStatus(s);

	if (m_client->isOnline())
		m_client->roster()->sync();
}

ChatUnit *VAccount::getUnit(const QString &unitId, bool create)
{
	return roster()->contact(unitId.toInt(), create);
}

