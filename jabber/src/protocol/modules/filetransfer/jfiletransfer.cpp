#include "jfiletransfer.h"
#include "../../jprotocol.h"
#include "../../account/roster/jcontact.h"
#include "../../account/roster/jcontactresource.h"
#include <QFileInfo>
#include "../../../jabber_global.h"
#include "../../account/connection/jconnectionbase.h"
#include "../../account/connection/jconnectiontcpserver.h"
#include <qutim/debug.h>
#include <gloox/socks5bytestreamserver.h>

namespace Jabber
{
void ByteStreamConnector::run()
{
	emit resultReady(m_bs->connect());
	deleteLater();
}

inline bool resource_priority_less(JContactResource *r1, JContactResource *r2)
{ return r1->priority() > r2->priority(); }

JFileTransfer::JFileTransfer(ChatUnit *unit, Direction direction, JFileTransferHandler *handler) :
		FileTransferEngine(unit, direction, handler->factory())
{
	m_bs = 0;
	m_fileSize = 0;
	m_stypes = SIProfileFT::FTTypeAll;
	m_handler = handler;
	m_state = StateNotStarted;
	if (JContact *contact = qobject_cast<JContact*>(unit)) {
		QString cap = QString::fromStdString(gloox::XMLNS_SI);
		QList<JContactResource*> resources = contact->resources();
		qSort(resources.begin(), resources.end(), resource_priority_less);
		JContactResource *current = 0;
		foreach (JContactResource *resource, resources) {
			if (resource->checkFeature(cap)) {
				current = resource;
				break;
			}
		}
		Q_ASSERT(current);
		unit = current;
	}
	m_from = unit->id().toStdString();
}

JFileTransfer::JFileTransfer(const JID &from, const std::string &sid, const std::string &name,
							 long size, const std::string &desc, int stype, JFileTransferHandler *handler)
			: FileTransferEngine(handler->account()->getUnit(QString::fromStdString(from.full())),
								 Receive, handler->factory())
{
	m_bs = 0;
	m_sid = sid;
	m_fileName = QString::fromStdString(name);
	m_fileSize = size;
	m_description = QString::fromStdString(desc);
	m_stypes = stype;
	m_handler = handler;
	m_state = StateNotStarted;
}

int JFileTransfer::progress() const
{
	return m_progress;
}

int JFileTransfer::currentFile() const
{
	return 0;
}

QStringList JFileTransfer::files() const
{
	return m_file ? QStringList(m_file->fileName()) : QStringList();
}

QStringList JFileTransfer::remoteFiles() const
{
	return m_fileName.isEmpty() ? QStringList() : QStringList(m_fileName);
}

void JFileTransfer::setFiles(const QStringList &files)
{
	debug() << Q_FUNC_INFO << files << direction() << m_state;
	if (files.isEmpty())
		return;
	if (direction() == Send && m_state == StateNotStarted) {
		m_fileName = files.first();
		m_file = new QFile(m_fileName, this);
	}
}

qint64 JFileTransfer::totalSize() const
{
	return fileSize();
}

qint64 JFileTransfer::fileSize() const
{
	return m_fileSize;
}

//int JFileTransfer::localPort() const
//{
//}
//
//int JFileTransfer::remotePort() const
//{
//}
//
//QHostAddress JFileTransfer::remoteAddress() const
//{
//}

FileTransferEngine::State JFileTransfer::state() const
{
	return m_state;
}

void JFileTransfer::start()
{
	if (m_state != StateNotStarted)
		return;
	debug() << Q_FUNC_INFO;
	if (direction() == Receive) {
		SIProfileFT::StreamType type = SIProfileFT::FTTypeS5B;
		m_handler->profileFT()->acceptFT(m_from, m_sid, type);
	} else {
		SIProfileFT *profile = m_handler->profileFT();
		QFileInfo info(m_fileName);
		m_sid = profile->requestFT(m_from, info.fileName().toStdString(), info.size(), gloox::EmptyString,
								   gloox::EmptyString, date2stamp(info.lastModified()), gloox::EmptyString,
								   SIProfileFT::FTTypeAll);
		qDebug() << QString::fromStdString(m_sid) << m_fileName << info.fileName() << info.size();
		m_handler->registerTransfer(m_sid, this);
	}
}

void JFileTransfer::cancel()
{
	debug() << Q_FUNC_INFO;
	m_handler->profileFT()->declineFT(m_from, m_sid, SIManager::RequestRejected);
}

void JFileTransfer::handleBytestreamData(Bytestream *bs, const std::string &data)
{
	debug() << Q_FUNC_INFO;
}

void JFileTransfer::handleBytestreamError(Bytestream *bs, const IQ &iq)
{
	debug() << Q_FUNC_INFO;
}

void JFileTransfer::handleBytestreamOpen(Bytestream *bs)
{
	debug() << Q_FUNC_INFO;
}

void JFileTransfer::handleBytestreamClose(Bytestream *bs)
{
	debug() << Q_FUNC_INFO;
}

JFileTransferHandler::JFileTransferHandler(JAccount *account, JFileTransferFactory *factory) :
		QObject(account)
{
	debug() << Q_FUNC_INFO;
	m_account = account;
	m_factory = factory;
	m_account->client()->registerConnectionListener(this);
	m_socks5Manager = new SOCKS5BytestreamManager(account->client(), this);
	m_profileFT = new SIProfileFT(account->client(), this,
								  account->connection()->siManager(), m_socks5Manager);
}

void JFileTransferHandler::handleFTRequest(const JID &from, const JID &to, const std::string &sid,
										   const std::string &name, long size, const std::string &hash,
										   const std::string &date, const std::string &mimetype,
										   const std::string &desc, int stypes)
{
	debug() << Q_FUNC_INFO;
	Q_UNUSED(date);
	Q_UNUSED(hash);
	Q_UNUSED(mimetype);
	if (QString::fromStdString(to.bare()) != m_account->id()) {
		m_profileFT->declineFT(to, sid, SIManager::RequestRejected);
		return;
	}
	JFileTransfer *transfer = new JFileTransfer(from, sid, name, size, desc, stypes, this);
	m_transfers.insert(sid.c_str(), transfer);
}

void JFileTransferHandler::handleFTRequestError(const IQ &iq, const std::string &sid)
{
	debug() << Q_FUNC_INFO;
}

void JFileTransferHandler::handleFTBytestream(Bytestream* bs)
{
	debug() << Q_FUNC_INFO;
}

const std::string JFileTransferHandler::handleOOBRequestResult(const JID &from, const JID &to,
															   const std::string &sid)
{
	debug() << Q_FUNC_INFO;
	return gloox::EmptyString;
}

void JFileTransferHandler::handleIncomingBytestreamRequest(const std::string &sid, const JID &from)
{
	debug() << Q_FUNC_INFO;
}

void JFileTransferHandler::handleIncomingBytestream(Bytestream *bs)
{
	debug() << Q_FUNC_INFO;
}

void JFileTransferHandler::handleOutgoingBytestream(Bytestream *bs)
{
	debug() << Q_FUNC_INFO;
}

void JFileTransferHandler::handleBytestreamError(const IQ &iq, const std::string &sid)
{
	debug() << Q_FUNC_INFO;
}

void JFileTransferHandler::onConnect()
{
	ConnectionBase *connection = m_account->connection()->connection();
	debug() << Q_FUNC_INFO << connection; //static_cast<JConnectionBase*>(connection);
//	debug() << static_cast<JTC*>(connection);
	StreamHost sh = {
		m_account->client()->jid(),
		connection->localInterface(),
		8010
	};
	StreamHostList hosts(1, sh);
	qDebug() << sh.host.c_str() << sh.port << hosts.size();
	m_socks5Manager->setStreamHosts(hosts);
}

void JFileTransferHandler::registerTransfer(const std::string &sid, JFileTransfer *transfer)
{
	m_transfers.insert(sid.c_str(), transfer);
}

JFileTransferFactory::JFileTransferFactory()
{
	m_server = 0;
	Protocol *proto = Protocol::all().value("jabber");
	if (qobject_cast<JProtocol*>(proto)) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach (Account *acc, proto->accounts())
			onAccountCreated(acc);
	}
}

bool JFileTransferFactory::check(ChatUnit *unit)
{
	debug() << Q_FUNC_INFO << unit->metaObject()->className();
	if (JContact *contact = qobject_cast<JContact*>(unit)) {
		QString cap = QString::fromStdString(gloox::XMLNS_SI);
		foreach (JContactResource *resource, contact->resources()) {
			if (resource->checkFeature(cap))
				return true;
		}
	} else if (JContactResource *resource = qobject_cast<JContactResource*>(unit)) {
		return resource->checkFeature(gloox::XMLNS_SI);
	}
	return false;
}

FileTransferEngine *JFileTransferFactory::create(ChatUnit *unit)
{
	foreach (JFileTransferHandler *handler, m_handlers) {
		if (handler && handler->account() == unit->account())
			return new JFileTransfer(unit, FileTransferEngine::Send, handler);
	}
	return 0;
}

void JFileTransferFactory::onAccountCreated(Account *qutimAccount)
{
	JAccount *account = qobject_cast<JAccount*>(qutimAccount);
	if (account) {
		if (!m_server) {
			m_server = new SOCKS5BytestreamServer(account->client()->logInstance(), 8010);
			ConnectionBase *server = new JConnectionTcpServer(m_server);
			m_server->setServerImpl(server);
			m_server->listen();
		}
		m_handlers << new JFileTransferHandler(account, this);
	}
}
}
