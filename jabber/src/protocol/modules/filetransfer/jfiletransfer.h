#ifndef JFILETRANSFER_H
#define JFILETRANSFER_H

#include "../../account/jaccount.h"
#include <qutim/filetransfer.h>
#include <gloox/siprofileft.h>
#include <gloox/siprofilefthandler.h>
#include <gloox/bytestreamdatahandler.h>
#include <QFile>
#include <QThread>

using namespace qutim_sdk_0_3;
using namespace gloox;

namespace Jabber
{
class JFileTransferFactory;
class JFileTransferHandler;

class ByteStreamConnector : public QThread
{
	Q_OBJECT
public:
	inline ByteStreamConnector(Bytestream *bs, QObject *parent) : m_bs(bs) {}
	virtual void run();
signals:
	void resultReady(bool);
private:
	Bytestream *m_bs;
};

class JFileTransfer : public FileTransferEngine, public BytestreamDataHandler
{
	Q_OBJECT
public:
	JFileTransfer(ChatUnit *unit, Direction direction, JFileTransferHandler *handler);
	JFileTransfer(const JID &from, const std::string &sid, const std::string &name, long size,
				  const std::string &desc, int stype, JFileTransferHandler *handler);
	virtual int progress() const;
	virtual int currentFile() const;
	virtual QStringList files() const;
	virtual QStringList remoteFiles() const;
	virtual void setFiles(const QStringList &);
	virtual qint64 totalSize() const;
	virtual qint64 fileSize() const;
//	virtual int localPort() const;
//	virtual int remotePort() const;
//	virtual QHostAddress remoteAddress() const;
	virtual State state() const;
	virtual void start();
	virtual void cancel();
	virtual void handleBytestreamData(Bytestream *bs, const std::string &data);
	virtual void handleBytestreamError(Bytestream *bs, const IQ &iq);
	virtual void handleBytestreamOpen(Bytestream *bs);
	virtual void handleBytestreamClose(Bytestream *bs);
private:
	Bytestream *m_bs;
	QFile *m_file;
	int m_fileSize;
	int m_progress;
	JFileTransferHandler *m_handler;
	JID m_from;
	std::string m_sid;
	QString m_fileName;
	QString m_description;
	State m_state;
	int m_stypes;
	friend class JFileTransferHandler;
};

class JFileTransferHandler : public QObject, public SIProfileFTHandler, public BytestreamHandler, public ConnectionListener
{
	Q_OBJECT
public:
	JFileTransferHandler(JAccount *account, JFileTransferFactory *factory);
	// SIProfileFTHandler
	virtual void handleFTRequest(const JID &from, const JID &to, const std::string &sid,
								  const std::string &name, long size, const std::string &hash,
								  const std::string &date, const std::string &mimetype,
								  const std::string &desc, int stypes);
	virtual void handleFTRequestError(const IQ &iq, const std::string &sid);
	virtual void handleFTBytestream(Bytestream* bs);
	virtual const std::string handleOOBRequestResult(const JID &from, const JID &to, const std::string &sid);
	// BytestreamHandler
	virtual void handleIncomingBytestreamRequest(const std::string &sid, const JID &from);
	virtual void handleIncomingBytestream(Bytestream *bs);
	virtual void handleOutgoingBytestream(Bytestream *bs);
	virtual void handleBytestreamError(const IQ &iq, const std::string &sid);
	// ConnectionListener
	virtual void onConnect();
	virtual void onDisconnect(ConnectionError) {}
	virtual bool onTLSConnect(const CertInfo &) { return true; }
	void registerTransfer(const std::string &sid, JFileTransfer *transfer);
	inline SIProfileFT *profileFT() { return m_profileFT; }
	inline JAccount *account() { return m_account; }
	inline JFileTransferFactory *factory() { return m_factory; }
private:
	SIProfileFT *m_profileFT;
	SOCKS5BytestreamManager *m_socks5Manager;
	JAccount *m_account;
	JFileTransferFactory *m_factory;
	QHash<QByteArray, JFileTransfer *> m_transfers;
};

class JFileTransferFactory : public FileTransferFactory
{
	Q_OBJECT
public:
	JFileTransferFactory();
	virtual bool check(ChatUnit *unit);
	virtual FileTransferEngine *create(ChatUnit *unit);
	inline SOCKS5BytestreamServer *server() { return m_server; }
protected slots:
	void onAccountCreated(qutim_sdk_0_3::Account *account);
private:
	SOCKS5BytestreamServer *m_server;
	QList<QPointer<JFileTransferHandler> > m_handlers;
};
}
#endif // JFILETRANSFER_H
