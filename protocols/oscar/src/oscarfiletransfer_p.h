/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef OSCAR_FILETRANSFER_H
#define OSCAR_FILETRANSFER_H

#include "messages.h"
#include "icqcontact.h"
#include "settingsextension.h"
#include <qutim/filetransfer.h>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QTcpServer>
#include <QPointer>
#include <QHostInfo>
#include <QThread>

namespace qutim_sdk_0_3 {

class Account;

namespace oscar {

class OftFileTransferFactory;
class OftConnection;

const int FILETRANSFER_WAITING_TIMEOUT = 15000;

enum OftPacketType
{
	OftPrompt = 0x0101,
	OftAcknowledge = 0x0202,
	OftDone = 0x0204,
	OftReceiverResume = 0x0205,
	OftSenderResume = 0x0106,
	OftResumeAcknowledge = 0x0207,
	OftSender = 0x0100,
	OftReceiver = 0x0200
};

struct OftHeader
{
public:
	OftHeader();
	void readData(QIODevice *dev);
	void writeData(QIODevice *dev);
	bool isFinished() { return m_state == Finished; }
public:
	OftPacketType type;
	quint64 cookie;
	bool encrypt;
	bool compress;
	quint16 totalFiles;
	quint16 filesLeft;
	quint16 totalParts;
	quint16 partsLeft;
	quint32 totalSize;
	quint32 size;
	quint32 modTime;
	quint32 checksum;
	quint32 receivedResourceForkChecksum;
	quint32 resourceForkSize;
	quint32 creationTime;
	quint32 resourceForkChecksum;
	quint32 bytesReceived;
	quint32 receivedChecksum;
	QString identification;
	quint8 flags;
	QByteArray macFileInfo;
	QString fileName;
private:
	enum State {
		ReadHeader,
		ReadData,
		Finished
	} m_state;
	quint16 m_length;
	QByteArray m_data;
};

class OftSocket : public QTcpSocket
{
	Q_OBJECT
public:
	enum ReadingState {
		ProxyInit = 0x1001,
		ProxyReceive = 0x1002,
		Proxy = 0x1000,
		ReadHeader = 0x0003,
		ReadData = 0x0004
	};
	explicit OftSocket(QObject *parent = 0);
	OftSocket(int socketDescriptor, QObject *parent = 0);
	const OftHeader &lastHeader() const { return m_lastHeader; }
	OftHeader &lastHeader() { return m_lastHeader; }
	ReadingState readingState() const { return m_state; }
	void directConnect(const QHostAddress &addr, quint16 port);
	void proxyConnect(const QString &uin);
	void proxyConnect(const QString &uin, QHostAddress addr, quint16 port);
	const QHostAddress &proxyIP() { return m_proxyIP; }
	quint16 proxyPort() { return m_proxyPort; }
	quint16 clientPort() { return m_clientPort; }
	void setCookie(quint64 cookie) { m_cookie = cookie; }
signals:
	void proxyInitialized();
	void initialized();
	void headerReaded(const OftHeader &header);
	void newData();
	void timeout();
public slots:
	void dataReaded();
protected:
	void connectToProxy(const QHostAddress &address, quint16 port);
private slots:
	void onReadyRead();
	void connected();
	void disconnected();
	void onTimeout();
	void proxyFound(const QHostInfo &proxyInfo);
private:
	void init();
	ReadingState m_state;
	OftHeader m_lastHeader;
	QString m_uin;
	quint64 m_cookie;
	QHostAddress m_proxyIP;
	quint16 m_proxyPort;
	quint16 m_len;
	int m_hostReqId;
	QTimer m_timer;
	quint16 m_clientPort;
};

class OftServer : public QTcpServer
{
	Q_OBJECT
public:
	explicit OftServer(quint16 port);
	void listen();
	void close();
	void setConnection(OftConnection *conn);
	OftConnection *conn() { return m_conn; }
signals:
	void closed(OftConnection *conn);
	void timeout(OftConnection *conn);
protected:
	void incomingConnection(int socketDescriptor);
private slots:
	void onTimeout();
private:
	OftConnection *m_conn;
	quint16 m_port;
	QTimer m_timer;
};

class OftChecksumThread : public QThread
{
	Q_OBJECT
public:
	OftChecksumThread(QIODevice *file, int bytes = 0);
	static quint32 chunkChecksum(const char *buffer, int len, quint32 checksum, int offset);
protected:
	void run();
signals:
	void done(quint32 checksum);
private:
	QIODevice *file;
	int bytes;
};

class OftConnection : public FileTransferJob
{
	Q_OBJECT
public:
	OftConnection(IcqContact *contact, Direction direction, quint64 cookie, OftFileTransferFactory *transfer, bool forceProxy);
	virtual ~OftConnection();
	inline IcqContact *contact() const { return m_contact.data(); }
	quint64 cookie() const { return m_cookie; }
	OftFileTransferFactory *transfer() const { return m_transfer; }
	int localPort() const;
	int remotePort() const;
	QHostAddress remoteAddress() const;
protected:
	virtual void doSend();
	virtual void doStop();
	virtual void doReceive();
private:
	void close(bool error);
	void handleRendezvous(quint16 reqType, const TLVMap &tlvs);
	void setSocket(OftSocket *socket);
	void startFileSending();
	void startFileReceiving(const int index);
	void startFileReceivingImpl(bool resume);
private slots:
	void close() { close(true); }
	void startNextStage();
	void sendFileRequest();
	void connected();
	void onError(QAbstractSocket::SocketError);
	void onHeaderReaded();
	void onNewData();
	void onSendData();
	void startFileSendingImpl(quint32 checksum);
	void startFileReceivingImpl(quint32 checksum);
	void resumeFileReceivingImpl(quint32 checksum);
private:
	friend class OftServer;
	friend class OftFileTransferFactory;
	QPointer<OftSocket> m_socket;
	QPointer<OftServer> m_server;
	QScopedPointer<QIODevice> m_data;
	OftFileTransferFactory *m_transfer;
	QPointer<IcqContact> m_contact;
	QPointer<IcqAccount> m_account;
	quint64 m_cookie;
	quint16 m_stage;
	bool m_proxy;
	OftHeader m_header;
	bool m_connInited;
	QString m_outputDir;
	QHostAddress m_clientVerifiedIP;
};

class OftFileTransferFactory : public FileTransferFactory, public MessagePlugin
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::MessagePlugin)
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
	explicit OftFileTransferFactory();
	virtual void processMessage(IcqContact *contact,
								const oscar::Capability &guid,
								const QByteArray &data,
								quint16 reqType,
								quint64 cookie);
	virtual bool checkAbility(ChatUnit *unit);
	bool checkAbility(IcqContact *contact);
	virtual bool startObserve(ChatUnit *unit);
	virtual bool stopObserve(ChatUnit *unit);
	virtual FileTransferJob *create(ChatUnit *unit);
	static OftServer *getFreeServer();
private slots:
	void capabilitiesChanged(const qutim_sdk_0_3::oscar::Capabilities &capabilities);
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountDestroyed(QObject *account);
	void reloadSettings();
private:
	friend class OftConnection;
	OftConnection *connection(IcqAccount *account, quint64 cookie);
	void addConnection(OftConnection *connection);
	void removeConnection(OftConnection *connection);
private:
	typedef QHash<quint64, OftConnection*> AccountConnections;
	typedef QHash<Account*, AccountConnections> Connections;
	Connections m_connections;
	static QHash<quint16, OftServer*> m_servers;
	static bool m_allowAnyPort;
	bool m_forceProxy;
};

class OscarFileTransferSettings : public QObject, public SettingsExtension
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SettingsExtension)
public:
	virtual void loadSettings(DataItem &item, Config cfg);
	virtual void saveSettings(const DataItem &item, Config cfg);
private slots:
	void onAllowAnyPortChanged(const QString &fieldName, const QVariant &data,
							   qutim_sdk_0_3::AbstractDataForm *dataForm);
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCAR_FILETRANSFER_H

