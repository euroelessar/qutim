#ifndef OSCARCONNECTION_H
#define OSCARCONNECTION_H

#include <QTcpSocket>
#include <QMap>
#include <QHostAddress>
#include "flap.h"

class SNACHandler;
class SNAC;

struct ClientInfo
{
	QByteArray id_string;
	quint16 id_number;
	quint16 major_version;
	quint16 minor_version;
	quint16 lesser_version;
	quint16 build_number;
	quint32 distribution_number;
	QByteArray language;
	QByteArray country;
};

struct DirectConnectionInfo
{
	QHostAddress internal_ip;
	quint32 port;
	quint8 dc_type;
	quint16 protocol_version;
	quint32 auth_cookie;
	quint32 web_front_port;
	quint32 client_futures;
	quint32 info_utime;         // last info update time (unix time_t)
    quint32 extinfo_utime;      // last ext info update time (i.e. icqphone status)
    quint32 extstatus_utime;    // last ext status update time (i.e. phonebook)
};

class OscarConnection : public QObject
{
	Q_OBJECT
public:
	enum ConnectState { LoginServer, HaveBOSS, BOSS };
    OscarConnection();
	void connectToLoginServer();
	void connectToBOSS(const QByteArray &host, int port, const QByteArray &cookie);
	void approveConnection();
	void md5Login();
	void setSeqNum(quint16 seqnum);
	// max value is 0x7fff, min is 0
	inline quint16 seqNum() { m_seqnum++; return (m_seqnum &= 0x7fff); }
	inline quint32 nextId() { return m_id++; }
	void send(FLAP &flap);
	quint32 send(SNAC &snac);
	void registerHandler(SNACHandler *handler);
	inline void setConnectState(ConnectState state) { m_state = state; }
	inline ConnectState connectState() const { return m_state; }
	inline const ClientInfo &clientInfo() const { return m_client_info; }
	inline const DirectConnectionInfo &dcInfo() const { return m_dc_info; }
	void finishLogin();
	void sendUserInfo();
	void setStatus();
	void setIdle(bool allow);
	void setExternalIP(const QHostAddress &ip) { m_ext_ip = ip; }
	const QHostAddress &externalIP() const { return m_ext_ip; }
private slots:
	void readData();
	void stateChanged(QAbstractSocket::SocketState);
	void error(QAbstractSocket::SocketError);
private:
	void processNewConnection();
	void processSnac();
	void processCloseConnection();
	QTcpSocket *m_socket;
	ConnectState m_state;
	FLAP m_flap;
	quint16 m_seqnum;
	quint32 m_id;
	QMultiMap<quint32, SNACHandler*> m_handlers;
	QHostAddress m_boss_server;
	int m_boss_port;
	QHostAddress m_ext_ip;
	QByteArray m_auth_cookie;
	ClientInfo m_client_info;
	DirectConnectionInfo m_dc_info;
	bool m_is_idle;
};

#endif // OSCARCONNECTION_H
