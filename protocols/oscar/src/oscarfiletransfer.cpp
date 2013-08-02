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

#include "oscarfiletransfer_p.h"
#include "buddycaps.h"
#include "icqcontact.h"
#include "tlv.h"
#include "icqaccount.h"
#include "oscarconnection.h"
#include "icqprotocol.h"
#include <QHostAddress>
#include <QDir>
#include <QTimer>
#include <QApplication>

namespace qutim_sdk_0_3 {

namespace oscar {

QHash<quint16, OftServer*> OftFileTransferFactory::m_servers;
bool OftFileTransferFactory::m_allowAnyPort;

const int BUFFER_SIZE = 4096;
using namespace Util;

OftHeader::OftHeader() :
	encrypt(false),
	compress(false),
	totalFiles(1),
	filesLeft(1),
	totalParts(1),
	partsLeft(1),
	totalSize(0),
	checksum(0xFFFF0000),
	receivedResourceForkChecksum(0xFFFF0000),
	resourceForkChecksum(0xFFFF0000),
	bytesReceived(0),
	receivedChecksum(0xFFFF0000),
	identification("Cool FileXfer"),
	flags(0x20),
	m_state(ReadHeader)
{
}

void OftHeader::readData(QIODevice *dev)
{
	if (m_state == ReadHeader) {
		DataUnit data(dev->read(6));
		/*quint32 protVersion = */data.read<quint32>();  // TODO: test it
		m_length = data.read<quint16>() - 6;
		m_state = ReadData;
		m_data.resize(m_length);
	}
	if (m_state == ReadData) {
		char *data = m_data.data() + m_data.size() - m_length;
		int readed = dev->read(data, m_length);
		m_length -= readed;
	}
	if (m_length == 0) {
		DataUnit data(m_data);
		type = static_cast<OftPacketType>(data.read<quint16>());
		cookie = data.read<quint64>();
		encrypt = data.read<quint16>();
		compress = data.read<quint16>();
		totalFiles = data.read<quint16>();
		filesLeft = data.read<quint16>();
		totalParts = data.read<quint16>();
		partsLeft = data.read<quint16>();
		totalSize = data.read<quint32>();
		size = data.read<quint32>();
		modTime = data.read<quint32>();
		checksum = data.read<quint32>();
		receivedResourceForkChecksum = data.read<quint32>();
		resourceForkSize = data.read<quint32>();
		creationTime = data.read<quint32>();
		resourceForkChecksum = data.read<quint32>();
		bytesReceived = data.read<quint32>();
		receivedChecksum = data.read<quint32>();
		identification = QString::fromLatin1(data.readData(32));
		flags = data.read<quint8>();
		data.skipData(71); // skipped List Name Offset, List Size Offset and "Dummy" block
		macFileInfo = data.readData(16);
		quint16 encoding = data.read<quint16>();
		//quint16 encodingSubcode = data.read<quint16>();
		data.skipData(2);
		QTextCodec *codec = 0;
		if (encoding == CodecUtf16Be)
			codec = utf16Codec();
		else
			codec = asciiCodec();
		QByteArray name = data.readAll();
		if (name.size() == 64) {
			uint c = qstrnlen(name.constData(), name.size());
			name.resize(c);
		} else if (encoding == CodecUtf16Be) {
			name.chop(2);
		} else {
			name.chop(1);
		}
		fileName = codec->toUnicode(name);
		m_state = Finished;
	}
}

static void resizeArray(QByteArray &data, int size)
{
	int oldSize = data.size();
	data.resize(size);
	if (oldSize < size)
		memset(data.data()+oldSize, 0, size-oldSize);
}

void OftHeader::writeData(QIODevice *dev)
{
	DataUnit data;
	qDebug() << "Outgoing oft message with type" << hex << type;
	data.append<quint16>(type);
	data.append<quint64>(cookie);
	data.append<quint16>(encrypt);
	data.append<quint16>(compress);
	data.append<quint16>(totalFiles);
	data.append<quint16>(filesLeft);
	data.append<quint16>(totalParts);
	data.append<quint16>(partsLeft);
	data.append<quint32>(totalSize);
	data.append<quint32>(size);
	data.append<quint32>(modTime);
	data.append<quint32>(checksum);
	data.append<quint32>(receivedResourceForkChecksum);
	data.append<quint32>(resourceForkSize);
	data.append<quint32>(creationTime);
	data.append<quint32>(resourceForkChecksum);
	data.append<quint32>(bytesReceived);
	data.append<quint32>(receivedChecksum);
	{
		QByteArray ident = identification.toLatin1();
		resizeArray(ident, 32);
		data.append(ident);
	}
	data.append<quint8>(flags);
	data.append<quint8>(0x1C);
	data.append<quint8>(0x11);
	{
		QByteArray dummy;
		resizeArray(dummy, 69);
		data.append(dummy);
	}
	resizeArray(macFileInfo, 16);
	data.append(macFileInfo);
	data.append<quint16>(CodecUtf16Be);
	data.append<quint16>(0);
	{
		QByteArray name = utf16Codec()->fromUnicode(fileName);
		name = name.mid(2);
		if (name.size() < 64)
			resizeArray(name, 64);
		else
			name.append("\0\0");
		data.append(name);
	}
	DataUnit header;
	header.append<quint32>(0x4F465432); // Protocol version: "OFT2"
	header.append<quint16>(data.dataSize() + 6);
	header.append(data);
	dev->write(header.data());
}

OftSocket::OftSocket(QObject *parent) :
	QTcpSocket(parent)
{
	init();
}

OftSocket::OftSocket(int socketDescriptor, QObject *parent) :
	QTcpSocket(parent)
{
	setSocketDescriptor(socketDescriptor);
	init();
}

void OftSocket::init()
{
	connect(this, SIGNAL(readyRead()), SLOT(onReadyRead()));
	connect(this, SIGNAL(connected()), this, SLOT(connected()));
	m_state = ReadHeader;
	m_len = 0;
	m_hostReqId = 0;
	m_timer.setInterval(FILETRANSFER_WAITING_TIMEOUT);
	m_timer.setSingleShot(true);
	connect(&m_timer, SIGNAL(timeout()), SLOT(onTimeout()));
}

void OftSocket::directConnect(const QHostAddress &addr, quint16 port)
{
	m_state = ReadHeader;
	connectToHost(addr, port);
	m_clientPort = port;
	m_timer.start();
	qDebug().nospace() << "Trying to establish a direct connection to "
			<< addr.toString().toLocal8Bit().constData()
			<< ":" << port;
}

void OftSocket::proxyConnect(const QString &uin)
{
	m_state = ProxyInit;
	m_lastHeader = OftHeader();
	m_len = 0;
	m_uin = uin;
	m_proxyPort = 0;
	m_hostReqId = QHostInfo::lookupHost("ars.oscar.aol.com", this, SLOT(proxyFound(QHostInfo)));
}

void OftSocket::proxyFound(const QHostInfo &proxyInfo)
{
	m_hostReqId = 0;
	QList<QHostAddress> addresses = proxyInfo.addresses();
	if (!addresses.isEmpty()) {
		connectToProxy(addresses.at(qrand() % addresses.size()), 5190);
	} else {
		setErrorString(tr("The file transfer proxy server is not available"));
		emit error(QAbstractSocket::ProxyNotFoundError);
	}
}

void OftSocket::proxyConnect(const QString &uin, QHostAddress addr, quint16 port)
{
	m_state = ProxyReceive;
	m_lastHeader = OftHeader();
	m_len = 0;
	m_uin = uin;
	m_proxyPort = port;
	connectToProxy(addr, port);
}

void OftSocket::connectToProxy(const QHostAddress &addr, quint16 port)
{
	connectToHost(addr, port);
	qDebug().nospace() << "Trying to connect to the proxy "
			<< addr.toString().toLocal8Bit().constData()
			<< ":" << port;
	m_timer.start();
}

void OftSocket::dataReaded()
{
	m_state = ReadHeader;
	m_lastHeader = OftHeader();
	if (bytesAvailable() > 0)
		onReadyRead();
}

void OftSocket::onReadyRead()
{
	if (m_state & Proxy) {
		DataUnit data;
		if (m_len == 0) {
			if (bytesAvailable() <= 4)
				return;
			data.setData(read(4));
			m_len = data.read<quint16>() - 2;
			if (data.read<quint16>() != 0x044A)
				qDebug() << "Unknown proxy protocol version";
		}
		if (bytesAvailable() <= m_len) {
			data.setData(read(m_len));
			m_len = 0;
		} else {
			return;
		}
		quint16 type = data.read<quint16>();
		data.skipData(4); // unknown
		quint16 flags = data.read<quint16>();
		Q_UNUSED(flags);
		qDebug() << "Rendezvous proxy packet. Type" << type;
		switch (type) {
		case 0x0001 : { // error
			quint16 code = data.read<quint16>();
			QString str;
			if (code == 0x000d || code == 0x000e)
				str = "Bad Request";
			else if (code == 0x0010)
				str = "Initial Request Timed Out";
			else if (code == 0x001a)
				str = "Accept Period Timed Out";
			else
				str = QString("Unknown rendezvous proxy error: %1").arg(code);
			qDebug() << "Rendezvous proxy error:" << str;
			setSocketError(QAbstractSocket::ProxyProtocolError);
			setErrorString(str);
			emit error(QAbstractSocket::ProxyProtocolError);
			break;
		}
		case 0x0003 : { // Acknowledge
			if (m_state != ProxyInit) {
				setSocketError(QAbstractSocket::ProxyProtocolError);
				setErrorString("Rendezvous proxy acknowledge packets are forbidden");
				emit error(QAbstractSocket::ProxyProtocolError);
				break;
			}
			m_proxyPort = data.read<quint16>();
			m_proxyIP.setAddress(data.read<quint32>());
			emit proxyInitialized();
			break;
		}
		case 0x0005 : { // Ready
			m_state = ReadHeader;
			emit initialized();
			break;
		}
		default:
			setSocketError(QAbstractSocket::ProxyProtocolError);
			setErrorString(QString("Unknown rendezvous proxy request").arg(type));
			emit error(QAbstractSocket::ProxyProtocolError);
		}
	} else {
		if (m_state == ReadHeader) {
			m_lastHeader.readData(this);
			if (m_lastHeader.isFinished()) {
				m_state = ReadData;
				emit headerReaded(m_lastHeader);
			}
		}
		if (m_state == ReadData && bytesAvailable() > 0)
			emit newData();
	}
}

void OftSocket::connected()
{
	m_timer.stop();
	if (m_state & Proxy) {
		DataUnit data;
		data.append<quint8>(m_uin, asciiCodec()); // uin or screen name
		if (m_state == ProxyReceive)
			data.append<quint16>(m_proxyPort);
		data.append<quint64>(m_cookie);
		data.appendTLV(0x0001, ICQ_CAPABILITY_AIMSENDFILE); // capability
		DataUnit header;
		header.append<quint16>(10 + data.dataSize());
		header.append<quint16>(0x044A); // proto version
		header.append<quint16>(m_state == ProxyInit ? 0x0002 : 0x0004); // request cmd
		header.append<quint32>(0); // unknown
		header.append<quint16>(0); // flags
		header.append(data.data());
		write(header.data());
		flush();
	} else {
		emit initialized();
	}
}

void OftSocket::disconnected()
{
}

void OftSocket::onTimeout()
{
	if (m_hostReqId != 0)
		QHostInfo::abortHostLookup(m_hostReqId);
	emit timeout();
	close();
}

OftServer::OftServer(quint16 port) :
	m_port(port)
{
	m_timer.setInterval(FILETRANSFER_WAITING_TIMEOUT);
	m_timer.setSingleShot(true);
	connect(&m_timer, SIGNAL(timeout()), SLOT(onTimeout()));
}

void OftServer::listen()
{
	m_timer.start();
	QTcpServer::listen();
	qDebug() << "Started listening for incoming connections on port" << serverPort();
	//emit m_conn->localPortChanged(serverPort());
}

void OftServer::close()
{
	m_conn = 0;
	m_timer.stop();
	QTcpServer::close();
}

void OftServer::setConnection(OftConnection *conn)
{
	m_conn = conn;
}

void OftServer::incomingConnection(int socketDescriptor)
{
	OftSocket *socket = new OftSocket(socketDescriptor);
	qDebug().nospace() << "Incoming oscar transfer connection from "
			<< socket->peerAddress().toString().toLatin1().constData()
			<< ":" << socket->peerPort();
	m_conn->setSocket(socket);
	emit closed(m_conn);
	m_conn->connected();
	close();
	m_timer.stop();
}

void OftServer::onTimeout()
{
	emit timeout(m_conn);
	close();
}

OftChecksumThread::OftChecksumThread(QIODevice *f, int b) :
	file(f), bytes(b)
{
}

quint32 OftChecksumThread::chunkChecksum(const char *buffer, int len, quint32 oldChecksum, int offset)
{
	// code adapted from miranda's oft_calc_checksum
	quint32 checksum = (oldChecksum >> 16) & 0xffff;
	for (int i = 0; i < len; i++)
	{
		quint16 val = buffer[i];
		//quint32 oldchecksum = checksum;
		if (((i + offset) & 1) == 0)
			val = val << 8;
		if (checksum < val)
			checksum -= val + 1;
		else // simulate carry
			checksum -= val;
	}
	checksum = ((checksum & 0x0000ffff) + (checksum >> 16));
	checksum = ((checksum & 0x0000ffff) + (checksum >> 16));
	return (quint32)checksum << 16;
}

void OftChecksumThread::run()
{
	quint32 checksum = 0xFFFF0000;
	QByteArray data;
	data.reserve(BUFFER_SIZE);
	int totalRead = 0;
	if (bytes <= 0)
		bytes = file->size();
	bool isOpen = file->isOpen();
	if (!isOpen)
		file->open(QIODevice::ReadOnly);
	while (totalRead < bytes) {
		data = file->read(qMin(BUFFER_SIZE, bytes - totalRead));
		checksum = chunkChecksum(data.constData(), data.size(), checksum, totalRead);
		totalRead += data.size();
		//QApplication::processEvents(); // This call causes crashes
	}
	if (!isOpen)
		file->close();
	emit done(checksum);
}

OftConnection::OftConnection(IcqContact *contact, Direction direction, quint64 cookie, OftFileTransferFactory *manager, bool forceProxy) :
	FileTransferJob(contact, direction, manager),
	m_transfer(manager),
	m_contact(contact),
	m_account(contact->account()),
	m_cookie(cookie),
	m_proxy(forceProxy),
	m_connInited(false)
{
	m_transfer->addConnection(this);
}

OftConnection::~OftConnection()
{
	m_transfer->removeConnection(this);
}

int OftConnection::localPort() const
{
	if (m_socket && m_socket.data()->isOpen())
		return m_socket.data()->localPort();
	if (m_server && m_server.data()->isListening())
		return m_socket.data()->localPort();
	return 0;
}

int OftConnection::remotePort() const
{
	if (m_socket && m_socket.data()->isOpen())
		return m_socket.data()->peerPort();
	return 0;
}

QHostAddress OftConnection::remoteAddress() const
{
	if (m_socket && m_socket.data()->isOpen())
		return m_socket.data()->peerAddress();
	return QHostAddress();
}

void OftConnection::doSend()
{
	m_stage = 1;
	if (!m_proxy) {
		sendFileRequest();
	} else {
		setSocket(new OftSocket(this));
		m_socket.data()->proxyConnect(m_account.data()->id());
	}
}

void OftConnection::doStop()
{
	Channel2BasicMessageData data(MsgCancel, ICQ_CAPABILITY_AIMSENDFILE, m_cookie);
	ServerMessage message(m_contact.data(), data);
	m_contact.data()->account()->connection()->send(message);
	close(false);
}

void OftConnection::doReceive()
{
	if (this->state() && m_connInited && direction() == Incoming)
		startFileReceiving(m_header.totalFiles - m_header.filesLeft);
}

void OftConnection::close(bool error)
{
	if (m_socket) {
		if (!error)
			m_socket.data()->close();
		m_socket.data()->deleteLater();
	}
	if (m_data)
		m_data.reset();
	if (error) {
		Channel2BasicMessageData data(MsgCancel, ICQ_CAPABILITY_AIMSENDFILE, m_cookie);
		ServerMessage message(m_contact.data(), data);
		m_contact.data()->account()->connection()->send(message);
		setState(Error);
		setError(NetworkError);
	}
}

void OftConnection::handleRendezvous(quint16 reqType, const TLVMap &tlvs)
{
	if (reqType == MsgRequest) {
		qDebug() << m_contact.data()->id() << "has sent file transfer request";
		m_stage = tlvs.value<quint16>(0x000A);
		QHostAddress proxyIP(tlvs.value<quint32>(0x0002));
		QHostAddress clientIP(tlvs.value<quint32>(0x0003));
		m_clientVerifiedIP = QHostAddress(tlvs.value<quint32>(0x0004));
		quint16 port = tlvs.value<quint16>(0x0005);
		bool forceProxy = m_proxy;
		m_proxy = tlvs.contains(0x0010);
		DataUnit tlv2711(tlvs.value(0x2711));
		bool multipleFiles = tlv2711.read<quint16>() > 1;
		quint16 filesCount = tlv2711.read<quint16>();
		quint32 totalSize = tlv2711.read<quint32>();
		Q_UNUSED(multipleFiles);
		QTextCodec *codec = 0;
		{
			QByteArray encoding = tlvs.value(0x2722);
			if (!encoding.isEmpty())
				codec = QTextCodec::codecForName(encoding.constData());
			if (!codec)
				codec = defaultCodec();
		}
		QString title = codec->toUnicode(tlv2711.readAll());
		title.chop(1);

		QString errorStr;
		if (m_stage == 1) {
			if (direction() == Incoming)
				init(filesCount, totalSize, title);
			else
				errorStr = "Stage 1 oscar file transfer request is forbidden during file sending";
		} else if (m_stage == 2) {
			if (direction() == Outgoing) {
				if (m_server)
					m_server.data()->close();
				if (m_socket) {
					qDebug() << "Sender has sent the request for reverse connection (stage 2)"
							<< "but the connection already initialized at stage 1";
					return;
				}
			} else {
				errorStr = "Stage 2 oscar file transfer request is forbidden during file receiving";
			}
		} else if (m_stage == 3) {
			if (direction() == Incoming)
				m_proxy = true;
			else
				errorStr = "Stage 3 oscar file transfer request is forbidden during file sending";
		} else {
			errorStr = QString("Unknown file transfer request at stage %1").arg(m_stage);
		}

		if (!errorStr.isEmpty()) {
			qDebug() << errorStr;
			close();
			return;
		}

		if (!m_proxy && clientIP.isNull()) {
			// The conntact has not sent us its IP, so skip that stage
			startNextStage();
			return;
		}
		setSocket(new OftSocket(this));
		if (!m_proxy && forceProxy) {
			// The contact wants a direct connection which
			// is forbidden by the user
			m_proxy = true;
			m_clientVerifiedIP = QHostAddress::Null;
			startNextStage();
			return;
		}
		if (!m_proxy)
			m_socket.data()->directConnect(clientIP, port);
		else
			m_socket.data()->proxyConnect(m_account.data()->id(), proxyIP, port);
		connect(m_socket.data(), SIGNAL(timeout()), SLOT(startNextStage()));
	} else if (reqType == MsgAccept) {
		qDebug() << m_contact.data()->id() << "accepted file transfing";
	} else if (reqType == MsgCancel) {
		qDebug() << m_contact.data()->id() << "canceled file transfing";
		close(false);
		setState(Error);
		setError(Canceled);
	}
}

void OftConnection::setSocket(OftSocket *socket)
{
	if (!m_socket) {
		m_socket = socket;
		m_socket.data()->setParent(this);
		m_socket.data()->setCookie(m_cookie);
		connect(m_socket.data(), SIGNAL(proxyInitialized()), SLOT(sendFileRequest()));
		connect(m_socket.data(), SIGNAL(initialized()), SLOT(connected()));
		connect(m_socket.data(), SIGNAL(error(QAbstractSocket::SocketError)),
				SLOT(onError(QAbstractSocket::SocketError)));
		connect(m_socket.data(), SIGNAL(headerReaded(OftHeader)), SLOT(onHeaderReaded()));
		connect(m_socket.data(), SIGNAL(disconnected()), m_socket.data(), SLOT(deleteLater()));
		if (m_socket.data()->readingState() == OftSocket::ReadData) {
			onHeaderReaded();
			if (m_socket.data()->bytesAvailable() > 0)
				onNewData();
		}
		// emit localPortChanged(socket->localPort());
	} else {
		socket->deleteLater();
		qDebug() << "Cannot change socket in an initialized oscar file transfer connection";
	}
}

void OftConnection::sendFileRequest()
{
	IcqAccount *account = contact()->account();
	Channel2BasicMessageData data(MsgRequest, ICQ_CAPABILITY_AIMSENDFILE, m_cookie);
	quint32 clientAddr = 0;
	quint32 proxyAddr;
	quint16 port;
	if (m_proxy) {
		proxyAddr = m_socket.data()->proxyIP().toIPv4Address();
		port = m_socket.data()->proxyPort();
	} else {
		m_server = OftFileTransferFactory::getFreeServer();
		if (m_server) {
			m_server.data()->setConnection(this);
			m_server.data()->listen();
			// That does not work well with all clients
			// connect(m_server.data(), SIGNAL(timeout(OftConnection*)), SLOT(close()));
			clientAddr = account->connection()->socket()->localAddress().toIPv4Address();
			port = m_server.data()->serverPort();
		} else {
			clientAddr = 0;
			port = 0;
		}
		proxyAddr = clientAddr;
	}
	data.appendTLV<quint16>(0x000A, m_stage);
	data.appendTLV<quint32>(0x0002, proxyAddr);
	data.appendTLV<quint32>(0x0016, proxyAddr ^ 0x0FFFFFFFF);
	data.appendTLV<quint32>(0x0003, clientAddr);
	data.appendTLV<quint16>(0x0005, port);
	data.appendTLV<quint16>(0x0017, port ^ 0x0FFFF);
	if (m_proxy)
		data.appendTLV(0x0010);
	if (m_stage == 1) {
		{
			// file info
			const int count = filesCount();
			DataUnit tlv2711;
			tlv2711.append<quint16>(count >= 2 ? 2 : 1);
			tlv2711.append<quint16>(count); // count
			tlv2711.append<quint32>(totalSize());
			tlv2711.append(title(), defaultCodec()); // ???
			tlv2711.append<quint8>(0);
			data.appendTLV(0x2711, tlv2711);
		}
		{
			// file name encoding
			DataUnit tlv2722;
			tlv2722.append(defaultCodec()->name());
			data.appendTLV(0x2722, tlv2722);
		}
	}
	ServerMessage message(m_contact.data(), data);
	m_contact.data()->account()->connection()->send(message);
	qDebug() << "A stage" << m_stage << "file transfer request has been sent";
}

void OftConnection::connected()
{
	// emit remoteAddressChanged(m_socket->peerAddress());
	// emit remotePortChanged(m_socket->peerPort());
	if (direction() == Incoming) {
		Channel2BasicMessageData data(MsgAccept, ICQ_CAPABILITY_AIMSENDFILE, m_cookie);
		ServerMessage message(m_contact.data(), data);
		m_contact.data()->account()->connection()->send(message);
	} else {
		startFileSending();
	}
}

void OftConnection::onError(QAbstractSocket::SocketError error)
{
	bool connClosed = error == QAbstractSocket::RemoteHostClosedError;
	if (m_stage == 1 && direction() == Incoming && !connClosed) {
		startNextStage();
	} else if (m_stage == 2 && !direction() == Outgoing && !connClosed) {
		startNextStage();
	} else {
		if (connClosed && m_header.bytesReceived == m_header.size && m_header.filesLeft <= 1) {
			qDebug() << "File transfer connection closed";
			setState(Finished);
			close(false);
		} else {
			qDebug() << "File transfer connection error" << m_socket.data()->errorString();
			close();
		}
	}
}

void OftConnection::onNewData()
{
	if (!m_data) {
		qDebug() << "File transfer data has been received when the output file is not initialized";
		return;
	}
	if (m_socket.data()->bytesAvailable() <= 0)
		return;
	QByteArray buf = m_socket.data()->read(m_header.size - m_header.bytesReceived);
	m_header.receivedChecksum =
			OftChecksumThread::chunkChecksum(buf.constData(), buf.size(),
											 m_header.receivedChecksum,
											 m_header.bytesReceived);
	m_header.bytesReceived += buf.size();
	m_data.data()->write(buf);
	setFileProgress(m_header.bytesReceived);
	if (m_header.bytesReceived == m_header.size) {
		disconnect(m_socket.data(), SIGNAL(newData()), this, SLOT(onNewData()));
		m_data.reset();
		m_header.type = OftDone;
		--m_header.filesLeft;
		m_header.writeData(m_socket.data());
		m_socket.data()->dataReaded();
		if (m_header.filesLeft == 0) {
			setState(Finished);
		}
	}
}

void OftConnection::onSendData()
{
	if (!m_data && m_socket.data()->bytesToWrite())
		return;
	QByteArray buf = m_data.data()->read(BUFFER_SIZE);
	m_header.receivedChecksum =
			OftChecksumThread::chunkChecksum(buf.constData(), buf.size(),
											 m_header.receivedChecksum,
											 m_header.bytesReceived);
	m_header.bytesReceived += buf.size();
	m_socket.data()->write(buf);
	setFileProgress(m_header.bytesReceived);
	if (m_header.bytesReceived == m_header.size) {
		disconnect(m_socket.data(), SIGNAL(bytesWritten(qint64)), this, SLOT(onSendData()));
		m_data.reset();
	}
}

void OftConnection::startFileSending()
{
	int index = currentIndex()+1;
	if (index < 0 || index >= filesCount()) {
		close(false);
		setState(Finished);
		return;
	}
	m_data.reset(setCurrentIndex(index));
	if (!m_data) {
		setState(Error);
		setError(IOError);
		close(false);
		return;
	}

	OftChecksumThread *checksum = new OftChecksumThread(m_data.data(), m_header.size);
	connect(checksum, SIGNAL(done(quint32)), SLOT(startFileSendingImpl(quint32)));
	checksum->start();
}

void OftConnection::startFileSendingImpl(quint32 checksum)
{
	sender()->deleteLater();
	QFileInfo file(baseDir().absoluteFilePath(fileName()));
	m_header.type = OftPrompt;
	m_header.cookie = m_cookie;
	m_header.modTime = file.lastModified().toTime_t();
	m_header.size = fileSize();
	m_header.fileName = fileName();
	m_header.checksum = checksum;
	m_header.receivedChecksum = 0xFFFF0000;
	m_header.bytesReceived = 0;
	m_header.totalSize = totalSize();
	m_header.writeData(m_socket.data());
	m_header.filesLeft = filesCount() - currentIndex();
	setState(Started);
}

void OftConnection::startFileReceiving(const int index)
{
	if (index < 0 || index >= filesCount())
		return;
	m_data.reset(setCurrentIndex(index));
	QFile *file = qobject_cast<QFile*>(m_data.data());
	bool exist = file && file->exists() && file->size() <= m_header.size;
	if (exist) {
		m_header.bytesReceived = file->size();
		m_header.type = m_header.bytesReceived == m_header.size ? OftDone : OftReceiverResume;
		OftChecksumThread *checksum = new OftChecksumThread(m_data.data(), m_header.size);
		connect(checksum, SIGNAL(done(quint32)), SLOT(startFileReceivingImpl(quint32)));
		checksum->start();
	} else {
		if (!m_data->open(QIODevice::WriteOnly)) {
			close(false);
			setState(Error);
			setError(IOError);
			if (file)
				setErrorString(tr("Could not open %1").arg(file->fileName()));
			return;
		}
		m_header.type = OftAcknowledge;
		onNewData();
		startFileReceivingImpl(false);
	}
}

void OftConnection::startFileReceivingImpl(quint32 checksum)
{
	sender()->deleteLater();
	m_header.receivedChecksum = checksum;
	startFileReceivingImpl(true);
}

void OftConnection::startFileReceivingImpl(bool resume)
{
	m_header.cookie = m_cookie;
	m_header.writeData(m_socket.data());
	if (resume)
		m_socket.data()->dataReaded();
	setState(Started);
	connect(m_socket.data(), SIGNAL(newData()), SLOT(onNewData()));
}

void OftConnection::resumeFileReceivingImpl(quint32 checksum)
{
	if (checksum != m_header.receivedChecksum) { // receiver's file is corrupt
		m_header.receivedChecksum = 0xffff0000;
		m_header.bytesReceived = 0;
	}
	m_header.type = OftSenderResume;
	m_header.writeData(m_socket.data());
}

void OftConnection::startNextStage()
{
	if (m_stage == 1) {
		if (!m_proxy && !m_clientVerifiedIP.isNull()) {
			m_socket.data()->close();
			m_socket.data()->directConnect(m_clientVerifiedIP, m_socket.data()->clientPort());
			m_clientVerifiedIP = QHostAddress::Null;
		} else {
			m_stage = 2;
			if (m_proxy) {
				m_socket.data()->close();
				m_socket.data()->proxyConnect(m_account.data()->id());
			} else {
				m_socket.data()->deleteLater();
				sendFileRequest();
			}
		}
	} else if (m_stage == 2) {
		if (!m_proxy && !m_clientVerifiedIP.isNull()) {
			m_socket.data()->close();
			m_socket.data()->directConnect(m_clientVerifiedIP, m_socket.data()->clientPort());
			m_clientVerifiedIP = QHostAddress::Null;
		} else {
			m_stage = 3;
			m_proxy = true;
			m_socket.data()->close();
			m_socket.data()->proxyConnect(m_account.data()->id());
		}
	} else {
		close();
	}
}

void OftConnection::onHeaderReaded()
{
	if (m_socket.data()->lastHeader().isFinished()) {
		m_header = m_socket.data()->lastHeader();
		QString error;
		if (direction() == Incoming) {
			if (m_header.type & OftReceiver)
				error = QString("Oft message type %1 is not allowed during receiving");
		} else {
			if (m_header.type & OftSender)
				error = QString("Oft message type %1 is not allowed during sending");
		}
		if (!error.isEmpty()) {
			qDebug() << error.arg(m_header.type);
			close();
			return;
		}
		qDebug() << "Incoming oft message with type" << hex << m_header.type;
		switch (m_header.type) {
		case OftPrompt: { // Sender has sent us info about file transfer
			if (m_data) {
				qDebug() << "Prompt messages are not allowed during resuming receiving";
				return;
			}

			m_connInited = true;

			int currentIndex = m_header.totalFiles - m_header.filesLeft;
			if (currentIndex < 0 || currentIndex >= filesCount()) {
				qDebug() << "Sender sent wrong OftPrompt filetransfer request";
				close();
				break;
			}

			FileTransferInfo fileInfo;
			fileInfo.setFileName(m_header.fileName);
			fileInfo.setFileSize(m_header.size);
			setFileInfo(currentIndex, fileInfo);

			if (isAccepted())
				startFileReceiving(currentIndex);
			break;
		}
		case OftDone: { // Receiver has informed us about received file
			startFileSending();
			break;
		}
		case OftReceiverResume: { // Receiver wants to resume old file transfer
			if (!m_data) {
				qDebug() << "Sender sent OftReceiverResume filetransfer request before OftPrompt";
				close();
				return;
			}

			OftChecksumThread *checksum = new OftChecksumThread(m_data.data(), m_header.bytesReceived);
			connect(checksum, SIGNAL(done(quint32)), SLOT(resumeFileReceivingImpl(quint32)));
			checksum->start();
			break;
		}
		case OftSenderResume: { // Sender responded at our resuming request
			if (!m_data) {
				qDebug() << "The sender had sent OftReceiverResume filetransfer request"
						<< "before the receiver sent OftPromt";
				close();
				return;
			}
			QIODevice::OpenMode flags;
			m_header.type = OftResumeAcknowledge;
			if (m_header.bytesReceived) { // ok. resume receiving
				flags = QIODevice::WriteOnly | QIODevice::Append;
				qDebug() << "Receiving of file" << m_header.fileName << "will be resumed";
			} else { // sender said that our local file is corrupt
				flags = QIODevice::WriteOnly;
				m_header.receivedChecksum = 0xffff0000;
				m_header.bytesReceived = 0;
				qDebug() << "File" << m_header.fileName << "will be rewritten";
			}
			if (m_data.data()->open(flags)) {
				m_header.writeData(m_socket.data());
			} else {
				close();
			}
			break;
		}
		case OftResumeAcknowledge:
		case OftAcknowledge: {	// receiver are waiting file
			m_socket.data()->dataReaded();
			if (m_data.data()->open(QFile::ReadOnly)) {
				connect(m_socket.data(), SIGNAL(bytesWritten(qint64)), this, SLOT(onSendData()));
				setState(Started);
				onSendData();
			} else {
				close();
			}
			break;
		}
		default:
			qDebug() << "Unknown oft message type" << hex << m_header.type;
			m_socket.data()->dataReaded();
		}
	}
}

OftFileTransferFactory::OftFileTransferFactory():
	FileTransferFactory(tr("Oscar"), CanSendMultiple)
{
	reloadSettings();
	m_capabilities << ICQ_CAPABILITY_AIMSENDFILE;
	foreach (IcqAccount *account, IcqProtocol::instance()->accountsHash())
		onAccountCreated(account);
	connect(IcqProtocol::instance(),
			SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
			SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	connect(IcqProtocol::instance(),
			SIGNAL(settingsUpdated()),
			SLOT(reloadSettings()));
}

void OftFileTransferFactory::processMessage(IcqContact *contact,
											const oscar::Capability &guid,
											const QByteArray &data,
											quint16 reqType,
											quint64 cookie)
{
	Q_UNUSED(guid);
	TLVMap tlvs = DataUnit(data).read<TLVMap>();
	OftConnection *conn = connection(contact->account(), cookie);
	if (conn && conn->contact() != contact) {
		qDebug() << "Cannot create two oscar file transfer with the same cookie" << cookie;
		return;
	}
	bool newRequest = reqType == MsgRequest && !conn;
	if (newRequest) {
		conn = new OftConnection(contact, FileTransferJob::Incoming, cookie, this, m_forceProxy);
	}
	if (conn) {
		conn->handleRendezvous(reqType, tlvs);
		if (conn->title().isNull())
			// We were waiting stage 1 message, but the contact had sent us something else.
			conn->deleteLater();
	} else {
		qDebug() << "Skipped oscar file transfer request with unknown cookie";
	}
}

bool OftFileTransferFactory::checkAbility(ChatUnit *unit)
{
	IcqContact *contact = qobject_cast<IcqContact*>(unit);
	return checkAbility(contact);
}

bool OftFileTransferFactory::checkAbility(IcqContact *contact)
{
	return contact && contact->capabilities().match(ICQ_CAPABILITY_AIMSENDFILE);
}

bool OftFileTransferFactory::startObserve(ChatUnit *unit)
{
	if (!qobject_cast<IcqContact*>(unit))
		return false;
	connect(unit,
			SIGNAL(capabilitiesChanged(qutim_sdk_0_3::oscar::Capabilities)),
			this,
			SLOT(capabilitiesChanged(qutim_sdk_0_3::oscar::Capabilities)));
	return true;
}

bool OftFileTransferFactory::stopObserve(ChatUnit *unit)
{
	if (!qobject_cast<IcqContact*>(unit))
		return false;
	disconnect(unit,
			   SIGNAL(capabilitiesChanged(qutim_sdk_0_3::oscar::Capabilities)),
			   this,
			   SLOT(capabilitiesChanged(qutim_sdk_0_3::oscar::Capabilities)));
	return true;
}

FileTransferJob *OftFileTransferFactory::create(ChatUnit *unit)
{
	IcqContact *contact = qobject_cast<IcqContact*>(unit);
	if (!checkAbility(contact))
		return 0;
	OftConnection *conn =
			new OftConnection(contact,
							  FileTransferJob::Outgoing,
							  Cookie::generateId(),
							  this,
							  m_forceProxy);
	return conn;
}

OftServer *OftFileTransferFactory::getFreeServer()
{
	if (m_allowAnyPort) {
		OftServer *server = new OftServer(0);
		connect(server, SIGNAL(closed(OftConnection*)),
				server, SLOT(deleteLater()));
		return server;
	} else {
		foreach (OftServer *server, m_servers) {
			if (!server->isListening())
				return server;
		}
	}
	return 0;
}

void OftFileTransferFactory::capabilitiesChanged(const qutim_sdk_0_3::oscar::Capabilities &capabilities)
{
	IcqContact *contact = qobject_cast<IcqContact*>(sender());
	if (!contact)
		return;
	changeAvailability(contact, capabilities.match(ICQ_CAPABILITY_AIMSENDFILE));
}

void OftFileTransferFactory::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	m_connections.insert(account, AccountConnections());
	connect(account, SIGNAL(destroyed(QObject*)), SLOT(onAccountDestroyed(QObject*)));
}

void OftFileTransferFactory::onAccountDestroyed(QObject *account)
{
	Connections::iterator itr = m_connections.find(static_cast<Account*>(account));
	foreach (OftConnection *conn, *itr)
		conn->deleteLater();
	m_connections.erase(itr);
}

void OftFileTransferFactory::reloadSettings()
{
	Config cfg = IcqProtocol::instance()->config("filetransfer");
	m_allowAnyPort = cfg.value("allowAnyPort", false);
	m_forceProxy = cfg.value("alwaysUseProxy", false);
	if (!m_allowAnyPort) {
		QSet<quint16> oldServers = m_servers.keys().toSet();

		// Update servers list
		QVariantList ports;
		ports << 7341 << 13117 << 21746;
		ports = cfg.value("localPorts", ports);
		foreach (const QVariant &portVar, ports) {
			bool ok;
			quint16 port = portVar.toInt(&ok);
			if (!ok)
				continue;
			if (!m_servers.contains(port))
				m_servers.insert(port, new OftServer(port));
			oldServers.remove(port);
		}

		// Remove disabled servers
		foreach (quint16 port, oldServers) {
			OftServer *server = m_servers.take(port);
			if (server->isListening())
				connect(server, SIGNAL(closed(OftConnection*)),
						server, SLOT(deleteLater()));
			else
				server->deleteLater();
		}
	} else {
		qDeleteAll(m_servers);
		m_servers.clear();
	}
}

OftConnection *OftFileTransferFactory::connection(IcqAccount *account, quint64 cookie)
{
	return m_connections.value(account).value(cookie);
}

void OftFileTransferFactory::addConnection(OftConnection *connection)
{
	IcqAccount *account = connection->m_account.data();
	Connections::iterator itr = m_connections.find(account);
	Q_ASSERT(itr != m_connections.end());
	itr->insert(connection->cookie(), connection);
}

void OftFileTransferFactory::removeConnection(OftConnection *connection)
{
	IcqAccount *account = connection->m_account.data();
	Connections::iterator itr = m_connections.find(account);
	if (itr != m_connections.end())
		itr->remove(connection->cookie());
}

void OscarFileTransferSettings::loadSettings(DataItem &item, Config cfg)
{
	cfg.beginGroup("filetransfer");
	bool allowAnyPort = cfg.value("allowAnyPort", false);
	DataItem settings("filetransferSettings", tr("File transfer"), QVariant());
	{
		DataItem item("alwaysUseProxy",
					  tr("Always use file transfer proxy"),
					  cfg.value("alwaysUseProxy", false));
		settings.addSubitem(item);
	}
	{
		DataItem item("allowAnyPort",
					  tr("Accept incoming connections on any port"),
					  allowAnyPort);
		item.setDataChangedHandler(
				this,
				SLOT(onAllowAnyPortChanged(QString,QVariant,qutim_sdk_0_3::AbstractDataForm*))
				);
		settings.addSubitem(item);
	}
	{
		// TODO: That item must have type QList<quint16>
		QVariant def = QVariantList() << 7341 << 13117 << 21746;
		QString ports = cfg.value("localPorts", def).toStringList().join(", ");
		DataItem item("localPorts", tr("Local ports"), ports);
		item.setProperty("enabled", !allowAnyPort);
		settings.addSubitem(item);
	}
	item.addSubitem(settings);
	cfg.endGroup();
}

void OscarFileTransferSettings::saveSettings(const DataItem &item, Config cfg)
{
	DataItem subitem = item.subitem("filetransferSettings");
	cfg.beginGroup("filetransfer");
	cfg.setValue("alwaysUseProxy", subitem.subitem("alwaysUseProxy").data(false));
	cfg.setValue("allowAnyPort", subitem.subitem("allowAnyPort").data(false));

	QString ports = subitem.subitem("localPorts", true).data<QString>();
	QVariantList portsVar;
	foreach (const QString &portStr, ports.split(',')) {
		quint16 port = portStr.trimmed().toInt();
		if (port)
			portsVar.push_back(port);
	}
	cfg.setValue("localPorts", portsVar);

	cfg.endGroup();
}

void OscarFileTransferSettings::onAllowAnyPortChanged(const QString &fieldName, const QVariant &data,
												   AbstractDataForm *dataForm)
{
	Q_UNUSED(fieldName);
	// TODO: it probably works only with the default data form backend
	if (QObject *object = dataForm->findChild<QObject*>("localPorts"))
		object->setProperty("enabled", !data.toBool());
}

} } // namespace qutim_sdk_0_3::oscar

