/*
 * Copyright (C) 2005-2008  Justin Karneges
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef QJDNS_P_H
#define QJDNS_P_H

#include "qjdns.h"

#include <QTimer>
#include <QTime>
#include <QUdpSocket>
#include <QPointer>
#include <QStringList>
#include <QMetaType>

#include <time.h>
#include "qjdns_sock.h"
#include "jdns.h"


namespace JDns {

// safeobj stuff, from qca

inline void releaseAndDeleteLater(QObject *owner, QObject *obj)
{
	obj->disconnect(owner);
	obj->setParent(0);
	obj->deleteLater();
}

class SafeTimer : public QObject
{
	Q_OBJECT
public:
	SafeTimer(QObject *parent = 0) :
		QObject(parent)
	{
		t = new QTimer(this);
		connect(t, SIGNAL(timeout()), SIGNAL(timeout()));
	}

	~SafeTimer()
	{
		releaseAndDeleteLater(this, t);
	}

	int interval() const                { return t->interval(); }
	bool isActive() const               { return t->isActive(); }
	bool isSingleShot() const           { return t->isSingleShot(); }
	void setInterval(int msec)          { t->setInterval(msec); }
	void setSingleShot(bool singleShot) { t->setSingleShot(singleShot); }
	int timerId() const                 { return t->timerId(); }

public slots:
	void start(int msec)                { t->start(msec); }
	void start()                        { t->start(); }
	void stop()                         { t->stop(); }

signals:
	void timeout();

private:
	QTimer *t;
};

}

static jdns_string_t *qt2str(const QByteArray &in)
{
	jdns_string_t *out = jdns_string_new();
	jdns_string_set(out, (const unsigned char *)in.data(), in.size());
	return out;
}

static QByteArray str2qt(const jdns_string_t *in)
{
	return QByteArray((const char *)in->data, in->size);
}

static void qt2addr_set(jdns_address_t *addr, const QHostAddress &host)
{
	if(host.protocol() == QAbstractSocket::IPv6Protocol)
		jdns_address_set_ipv6(addr, host.toIPv6Address().c);
	else
		jdns_address_set_ipv4(addr, host.toIPv4Address());
}

static jdns_address_t *qt2addr(const QHostAddress &host)
{
	jdns_address_t *addr = jdns_address_new();
	qt2addr_set(addr, host);
	return addr;
}

static QHostAddress addr2qt(const jdns_address_t *addr)
{
	if(addr->isIpv6)
		return QHostAddress(addr->addr.v6);
	else
		return QHostAddress(addr->addr.v4);
}

static QJDns::Record import_record(const jdns_rr_t *in)
{
	QJDns::Record out;

	out.owner = QByteArray((const char *)in->owner);
	out.ttl = in->ttl;
	out.type = in->type;
	out.rdata = QByteArray((const char *)in->rdata, in->rdlength);

	// known
	if(in->haveKnown)
	{
		int type = in->type;

		if(type == QJDns::A || type == QJDns::Aaaa)
		{
			out.haveKnown = true;
			out.address = addr2qt(in->data.address);
		}
		else if(type == QJDns::Mx)
		{
			out.haveKnown = true;
			out.name = QByteArray((const char *)in->data.server->name);
			out.priority = in->data.server->priority;
		}
		else if(type == QJDns::Srv)
		{
			out.haveKnown = true;
			out.name = QByteArray((const char *)in->data.server->name);
			out.priority = in->data.server->priority;
			out.weight = in->data.server->weight;
			out.port = in->data.server->port;
		}
		else if(type == QJDns::Cname || type == QJDns::Ptr || type == QJDns::Ns)
		{
			out.haveKnown = true;
			out.name = QByteArray((const char *)in->data.name);
		}
		else if(type == QJDns::Txt)
		{
			out.haveKnown = true;
			out.texts.clear();
			for(int n = 0; n < in->data.texts->count; ++n)
				out.texts += str2qt(in->data.texts->item[n]);
		}
		else if(type == QJDns::Hinfo)
		{
			out.haveKnown = true;
			out.cpu = str2qt(in->data.hinfo.cpu);
			out.os = str2qt(in->data.hinfo.os);
		}
	}

	return out;
}

extern void my_srand();

class QJDns::Private : public QObject
{
	Q_OBJECT
public:
	class LateError
	{
	public:
		int source_type; // 0 for query, 1 for publish
		int id;
		Error error;
	};

	class LateResponse
	{
	public:
		int id;
		QJDns::Response response;
		bool do_cancel;
	};

	QJDns *q;
	QJDns::Mode mode;
	jdns_session_t *sess;
	bool shutting_down;
	JDns::SafeTimer stepTrigger, debugTrigger;
	JDns::SafeTimer stepTimeout;
	QTime clock;
	QStringList debug_strings;
	bool new_debug_strings;
	int next_handle;
	bool need_handle;
	QHash<int,QUdpSocket*> socketForHandle;
	QHash<QUdpSocket*,int> handleForSocket;
	int pending;
	bool pending_wait;
	bool complete_shutdown;

	// pointers that will point to things we are currently signalling
	//   about.  when a query or publish is cancelled, we can use these
	//   pointers to extract anything we shouldn't signal.
	QList<LateError> *pErrors;
	QList<int> *pPublished;
	QList<LateResponse> *pResponses;

	Private(QJDns *_q) :
		QObject(_q),
		q(_q),
		stepTrigger(this),
		debugTrigger(this),
		stepTimeout(this),
		pErrors(0),
		pPublished(0),
		pResponses(0)
	{
		sess = 0;
		shutting_down = false;
		new_debug_strings = false;
		pending = 0;

		connect(&stepTrigger, SIGNAL(timeout()), SLOT(doNextStepSlot()));
		stepTrigger.setSingleShot(true);

		connect(&debugTrigger, SIGNAL(timeout()), SLOT(doDebug()));
		debugTrigger.setSingleShot(true);

		connect(&stepTimeout, SIGNAL(timeout()), SLOT(st_timeout()));
		stepTimeout.setSingleShot(true);

		my_srand();

		clock.start();
	}

	~Private()
	{
		cleanup();
	}

	void cleanup()
	{
		if(sess)
		{
			jdns_session_delete(sess);
			sess = 0;
		}

		shutting_down = false;
		pending = 0;

		// it is safe to delete the QUdpSocket objects here without
		//   deleteLater, since this code path never occurs when
		//   a signal from those objects is on the stack
		qDeleteAll(socketForHandle);
		socketForHandle.clear();
		handleForSocket.clear();

		stepTrigger.stop();
		stepTimeout.stop();
		need_handle = 0;
	}

	bool init(QJDns::Mode _mode, const QHostAddress &address)
	{
		mode = _mode;

		jdns_callbacks_t callbacks;
		callbacks.app = this;
		callbacks.time_now = cb_time_now;
		callbacks.rand_int = cb_rand_int;
		callbacks.debug_line = cb_debug_line;
		callbacks.udp_bind = cb_udp_bind;
		callbacks.udp_unbind = cb_udp_unbind;
		callbacks.udp_read = cb_udp_read;
		callbacks.udp_write = cb_udp_write;
		sess = jdns_session_new(&callbacks);
		jdns_set_hold_ids_enabled(sess, 1);
		next_handle = 1;
		need_handle = false;

		int ret;

		jdns_address_t *baddr = qt2addr(address);
		if(mode == Unicast)
		{
			ret = jdns_init_unicast(sess, baddr, 0);
		}
		else
		{
			jdns_address_t *maddr;
			if(address.protocol() == QAbstractSocket::IPv6Protocol)
				maddr = jdns_address_multicast6_new();
			else
				maddr = jdns_address_multicast4_new();
			ret = jdns_init_multicast(sess, baddr, JDNS_MULTICAST_PORT, maddr);
			jdns_address_delete(maddr);
		}
		jdns_address_delete(baddr);

		if(!ret)
		{
			jdns_session_delete(sess);
			sess = 0;
			return false;
		}
		return true;
	}

	void setNameServers(const QList<NameServer> &nslist)
	{
		jdns_nameserverlist_t *addrs = jdns_nameserverlist_new();
		for(int n = 0; n < nslist.count(); ++n)
		{
			jdns_address_t *addr = qt2addr(nslist[n].address);
			jdns_nameserverlist_append(addrs, addr, nslist[n].port);
			jdns_address_delete(addr);
		}
		jdns_set_nameservers(sess, addrs);
		jdns_nameserverlist_delete(addrs);
	}

	void process()
	{
		if(!stepTrigger.isActive())
		{
			stepTimeout.stop();
			stepTrigger.start();
		}
	}

	void processDebug()
	{
		new_debug_strings = true;
		if(!debugTrigger.isActive())
			debugTrigger.start();
	}

	void doNextStep()
	{
		if(shutting_down && complete_shutdown)
		{
			cleanup();
			emit q->shutdownFinished();
			return;
		}

		QPointer<QObject> self = this;

		int ret = jdns_step(sess);

		QList<LateError> errors;
		QList<int> published;
		QList<LateResponse> responses;
		bool finish_shutdown = false;

		pErrors = &errors;
		pPublished = &published;
		pResponses = &responses;

		while(1)
		{
			jdns_event_t *e = jdns_next_event(sess);
			if(!e)
				break;

			if(e->type == JDNS_EVENT_SHUTDOWN)
			{
				finish_shutdown = true;
			}
			else if(e->type == JDNS_EVENT_PUBLISH)
			{
				if(e->status != JDNS_STATUS_SUCCESS)
				{
					QJDns::Error error;
					if(e->status == JDNS_STATUS_CONFLICT)
						error = QJDns::ErrorConflict;
					else
						error = QJDns::ErrorGeneric;
					LateError le;
					le.source_type = 1;
					le.id = e->id;
					le.error = error;
					errors += le;
				}
				else
				{
					published += e->id;
				}
			}
			else if(e->type == JDNS_EVENT_RESPONSE)
			{
				if(e->status != JDNS_STATUS_SUCCESS)
				{
					QJDns::Error error;
					if(e->status == JDNS_STATUS_NXDOMAIN)
						error = QJDns::ErrorNXDomain;
					else if(e->status == JDNS_STATUS_TIMEOUT)
						error = QJDns::ErrorTimeout;
					else
						error = QJDns::ErrorGeneric;
					LateError le;
					le.source_type = 0;
					le.id = e->id;
					le.error = error;
					errors += le;
				}
				else
				{
					QJDns::Response out_response;
					for(int n = 0; n < e->response->answerCount; ++n)
						out_response.answerRecords += import_record(e->response->answerRecords[n]);
					LateResponse lr;
					lr.id = e->id;
					lr.response = out_response;
					if(mode == Unicast)
						lr.do_cancel = true;
					else
						lr.do_cancel = false;
					responses += lr;
				}
			}

			jdns_event_delete(e);
		}

		if(ret & JDNS_STEP_TIMER)
			stepTimeout.start(jdns_next_timer(sess));
		else
			stepTimeout.stop();

		need_handle = (ret & JDNS_STEP_HANDLE);

		// read the lists safely enough so that items can be deleted
		//   behind our back

		while(!errors.isEmpty())
		{
			LateError i = errors.takeFirst();
			if(i.source_type == 0)
				jdns_cancel_query(sess, i.id);
			else
				jdns_cancel_publish(sess, i.id);
			emit q->error(i.id, i.error);
			if(!self)
				return;
		}

		while(!published.isEmpty())
		{
			int i = published.takeFirst();
			emit q->published(i);
			if(!self)
				return;
		}

		while(!responses.isEmpty())
		{
			LateResponse i = responses.takeFirst();
			if(i.do_cancel)
				jdns_cancel_query(sess, i.id);
			emit q->resultsReady(i.id, i.response);
			if(!self)
				return;
		}

		if(finish_shutdown)
		{
			// if we have pending udp packets to write, stick around
			if(pending > 0)
			{
				pending_wait = true;
			}
			else
			{
				complete_shutdown = true;
				process();
			}
		}

		pErrors = 0;
		pPublished = 0;
		pResponses = 0;
	}

	void removeCancelled(int id)
	{
		if(pErrors)
		{
			for(int n = 0; n < pErrors->count(); ++n)
			{
				if(pErrors->at(n).id == id)
				{
					pErrors->removeAt(n);
					--n; // adjust position
				}
			}
		}

		if(pPublished)
		{
			for(int n = 0; n < pPublished->count(); ++n)
			{
				if(pPublished->at(n) == id)
				{
					pPublished->removeAt(n);
					--n; // adjust position
				}
			}
		}

		if(pResponses)
		{
			for(int n = 0; n < pResponses->count(); ++n)
			{
				if(pResponses->at(n).id == id)
				{
					pResponses->removeAt(n);
					--n; // adjust position
				}
			}
		}
	}

private slots:
	void udp_readyRead()
	{
		QUdpSocket *sock = (QUdpSocket *)sender();
		int handle = handleForSocket.value(sock);

		if(need_handle)
		{
			jdns_set_handle_readable(sess, handle);
			process();
		}
		else
		{
			// eat packet
			QByteArray buf(4096, 0);
			QHostAddress from_addr;
			quint16 from_port;
			sock->readDatagram(buf.data(), buf.size(), &from_addr, &from_port);
		}
	}

	void udp_bytesWritten(qint64)
	{
		if(pending > 0)
		{
			--pending;
			if(shutting_down && pending_wait && pending == 0)
			{
				pending_wait = false;
				complete_shutdown = true;
				process();
			}
		}
	}

	void st_timeout()
	{
		doNextStep();
	}

	void doNextStepSlot()
	{
		doNextStep();
	}

	void doDebug()
	{
		if(new_debug_strings)
		{
			new_debug_strings = false;
			if(!debug_strings.isEmpty())
				emit q->debugLinesReady();
		}
	}

private:
	// jdns callbacks
	static int cb_time_now(jdns_session_t *, void *app)
	{
		QJDns::Private *self = (QJDns::Private *)app;

		return self->clock.elapsed();
	}

	static int cb_rand_int(jdns_session_t *, void *)
	{
		return rand() % 65536;
	}

	static void cb_debug_line(jdns_session_t *, void *app, const char *str)
	{
		QJDns::Private *self = (QJDns::Private *)app;

		self->debug_strings += QString::fromLatin1(str);
		self->processDebug();
	}

	static int cb_udp_bind(jdns_session_t *, void *app, const jdns_address_t *addr, int port, const jdns_address_t *maddr)
	{
		QJDns::Private *self = (QJDns::Private *)app;

		// we always pass non-null to jdns_init, so this should be a valid address
		QHostAddress host = addr2qt(addr);

		QUdpSocket *sock = new QUdpSocket(self);
		self->connect(sock, SIGNAL(readyRead()), SLOT(udp_readyRead()));

		// use queued for bytesWritten, since qt is evil and emits before writeDatagram returns
		qRegisterMetaType<qint64>("qint64");
		self->connect(sock, SIGNAL(bytesWritten(qint64)), SLOT(udp_bytesWritten(qint64)), Qt::QueuedConnection);

		QUdpSocket::BindMode mode;
		mode |= QUdpSocket::ShareAddress;
		mode |= QUdpSocket::ReuseAddressHint;
		if(!sock->bind(host, port, mode))
		{
			delete sock;
			return 0;
		}

		if(maddr)
		{
			int sd = sock->socketDescriptor();
			bool ok;
			int errorCode;
			if(maddr->isIpv6)
				ok = qjdns_sock_setMulticast6(sd, maddr->addr.v6, &errorCode);
			else
				ok = qjdns_sock_setMulticast4(sd, maddr->addr.v4, &errorCode);

			if(!ok)
			{
				delete sock;

				self->debug_strings += QString("failed to setup multicast on the socket (errorCode=%1)").arg(errorCode);
				self->processDebug();
				return 0;
			}

			if(maddr->isIpv6)
			{
				qjdns_sock_setTTL6(sd, 255);
				qjdns_sock_setIPv6Only(sd);
			}
			else
				qjdns_sock_setTTL4(sd, 255);
		}

		int handle = self->next_handle++;
		self->socketForHandle.insert(handle, sock);
		self->handleForSocket.insert(sock, handle);
		return handle;
	}

	static void cb_udp_unbind(jdns_session_t *, void *app, int handle)
	{
		QJDns::Private *self = (QJDns::Private *)app;

		QUdpSocket *sock = self->socketForHandle.value(handle);
		if(!sock)
			return;

		self->socketForHandle.remove(handle);
		self->handleForSocket.remove(sock);
		delete sock;
	}

	static int cb_udp_read(jdns_session_t *, void *app, int handle, jdns_address_t *addr, int *port, unsigned char *buf, int *bufsize)
	{
		QJDns::Private *self = (QJDns::Private *)app;

		QUdpSocket *sock = self->socketForHandle.value(handle);
		if(!sock)
			return 0;

		// nothing to read?
		if(!sock->hasPendingDatagrams())
			return 0;

		QHostAddress from_addr;
		quint16 from_port;
		int ret = sock->readDatagram((char *)buf, *bufsize, &from_addr, &from_port);
		if(ret == -1)
			return 0;

		qt2addr_set(addr, from_addr);
		*port = (int)from_port;
		*bufsize = ret;
		return 1;
	}

	static int cb_udp_write(jdns_session_t *, void *app, int handle, const jdns_address_t *addr, int port, unsigned char *buf, int bufsize)
	{
		QJDns::Private *self = (QJDns::Private *)app;

		QUdpSocket *sock = self->socketForHandle.value(handle);
		if(!sock)
			return 0;

		QHostAddress host = addr2qt(addr);
		int ret = sock->writeDatagram((const char *)buf, bufsize, host, port);
		if(ret == -1)
		{
			// this can happen if the datagram to send is too big.  i'm not sure what else
			//   may cause this.  if we return 0, then jdns may try to resend the packet,
			//   which might not work if it is too large (causing the same error over and
			//   over).  we'll return success to jdns, so the result is as if the packet
			//   was dropped.
			return 1;
		}

		++self->pending;
		return 1;
	}
};


#endif // QJDNS_P_H
