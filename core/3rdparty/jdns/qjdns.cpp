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

#include "qjdns_p.h"

// for fprintf
#include <stdio.h>

static jdns_string_t *qt2str(const QByteArray &in)
{
	jdns_string_t *out = jdns_string_new();
	jdns_string_set(out, (const unsigned char *)in.data(), in.size());
	return out;
}

static jdns_rr_t *export_record(const QJDns::Record &in)
{
	jdns_rr_t *out = jdns_rr_new();

	jdns_rr_set_owner(out, (const unsigned char *)in.owner.data());
	out->ttl = in.ttl;

	// if we have known, use that
	if(in.haveKnown)
	{
		int type = in.type;

		if(type == QJDns::A)
		{
			jdns_address_t *addr = qt2addr(in.address);
			jdns_rr_set_A(out, addr);
			jdns_address_delete(addr);
		}
		else if(type == QJDns::Aaaa)
		{
			jdns_address_t *addr = qt2addr(in.address);
			jdns_rr_set_AAAA(out, addr);
			jdns_address_delete(addr);
		}
		else if(type == QJDns::Mx)
		{
			jdns_rr_set_MX(out, (const unsigned char *)in.name.data(), in.priority);
		}
		else if(type == QJDns::Srv)
		{
			jdns_rr_set_SRV(out, (const unsigned char *)in.name.data(), in.port, in.priority, in.weight);
		}
		else if(type == QJDns::Cname)
		{
			jdns_rr_set_CNAME(out, (const unsigned char *)in.name.data());
		}
		else if(type == QJDns::Ptr)
		{
			jdns_rr_set_PTR(out, (const unsigned char *)in.name.data());
		}
		else if(type == QJDns::Txt)
		{
			jdns_stringlist_t *list = jdns_stringlist_new();
			for(int n = 0; n < in.texts.count(); ++n)
			{
				jdns_string_t *str = qt2str(in.texts[n]);
				jdns_stringlist_append(list, str);
				jdns_string_delete(str);
			}
			jdns_rr_set_TXT(out, list);
			jdns_stringlist_delete(list);
		}
		else if(type == QJDns::Hinfo)
		{
			jdns_string_t *cpu = qt2str(in.cpu);
			jdns_string_t *os = qt2str(in.os);
			jdns_rr_set_HINFO(out, cpu, os);
			jdns_string_delete(cpu);
			jdns_string_delete(os);
		}
		else if(type == QJDns::Ns)
		{
			jdns_rr_set_NS(out, (const unsigned char *)in.name.data());
		}
	}
	else
		jdns_rr_set_record(out, in.type, (const unsigned char *)in.rdata.data(), in.rdata.size());

	return out;
}

//----------------------------------------------------------------------------
// QJDns::NameServer
//----------------------------------------------------------------------------
QJDns::NameServer::NameServer()
{
	port = JDNS_UNICAST_PORT;
}

//----------------------------------------------------------------------------
// QJDns::Record
//----------------------------------------------------------------------------
QJDns::Record::Record()
{
	ttl = 0;
	type = -1;
	haveKnown = false;
}

bool QJDns::Record::verify() const
{
	jdns_rr_t *rr = export_record(*this);
	int ok = jdns_rr_verify(rr);
	jdns_rr_delete(rr);
	return (ok ? true : false);
}

//----------------------------------------------------------------------------
// QJDns
//----------------------------------------------------------------------------
static int my_srand_done = 0;

void my_srand()
{
	if(my_srand_done)
		return;

	// lame attempt at randomizing without srand
	int count = ::time(NULL) % 128;
	for(int n = 0; n < count; ++n)
		rand();

	my_srand_done = 1;
}

QJDns::QJDns(QObject *parent)
:QObject(parent)
{
	d = new Private(this);
}

QJDns::~QJDns()
{
	delete d;
}

bool QJDns::init(Mode mode, const QHostAddress &address)
{
	return d->init(mode, address);
}

void QJDns::shutdown()
{
	d->shutting_down = true;
	d->pending_wait = false;
	d->complete_shutdown = false;
	jdns_shutdown(d->sess);
	d->process();
}

QStringList QJDns::debugLines()
{
	QStringList tmp = d->debug_strings;
	d->debug_strings.clear();
	return tmp;
}

QJDns::SystemInfo QJDns::systemInfo()
{
	SystemInfo out;
	jdns_dnsparams_t *params = jdns_system_dnsparams();
	for(int n = 0; n < params->nameservers->count; ++n)
	{
		NameServer ns;
		ns.address = addr2qt(params->nameservers->item[n]->address);
		out.nameServers += ns;
	}
	for(int n = 0; n < params->domains->count; ++n)
		out.domains += str2qt(params->domains->item[n]);
	for(int n = 0; n < params->hosts->count; ++n)
	{
		DnsHost h;
		h.name = str2qt(params->hosts->item[n]->name);
		h.address = addr2qt(params->hosts->item[n]->address);
		out.hosts += h;
	}
	jdns_dnsparams_delete(params);
	return out;
}

#define PROBE_BASE  20000
#define PROBE_RANGE   100

QHostAddress QJDns::detectPrimaryMulticast(const QHostAddress &address)
{
	my_srand();

	QUdpSocket *sock = new QUdpSocket;
	QUdpSocket::BindMode mode;
	mode |= QUdpSocket::ShareAddress;
	mode |= QUdpSocket::ReuseAddressHint;
	int port = -1;
	for(int n = 0; n < PROBE_RANGE; ++n)
	{
		if(sock->bind(address, PROBE_BASE + n, mode))
		{
			port = PROBE_BASE + n;
			break;
		}
	}
	if(port == -1)
	{
		delete sock;
		return QHostAddress();
	}

	jdns_address_t *a;
	if(address.protocol() == QAbstractSocket::IPv6Protocol)
		a = jdns_address_multicast6_new();
	else
		a = jdns_address_multicast4_new();
	QHostAddress maddr = addr2qt(a);
	jdns_address_delete(a);

	if(address.protocol() == QAbstractSocket::IPv6Protocol)
	{
		int x;
		if(!qjdns_sock_setMulticast6(sock->socketDescriptor(), maddr.toIPv6Address().c, &x))
		{
			delete sock;
			return QHostAddress();
		}
		qjdns_sock_setTTL6(sock->socketDescriptor(), 0);
	}
	else
	{
		int x;
		if(!qjdns_sock_setMulticast4(sock->socketDescriptor(), maddr.toIPv4Address(), &x))
		{
			delete sock;
			return QHostAddress();
		}
		qjdns_sock_setTTL4(sock->socketDescriptor(), 0);
	}

	QHostAddress result;
	QByteArray out(128, 0);
	for(int n = 0; n < out.size(); ++n)
		out[n] = rand();
	if(sock->writeDatagram(out.data(), out.size(), maddr, port) == -1)
	{
		delete sock;
		return QHostAddress();
	}
	while(1)
	{
		if(!sock->waitForReadyRead(1000))
		{
			fprintf(stderr, "QJDns::detectPrimaryMulticast: timeout while checking %s\n", qPrintable(address.toString()));
			delete sock;
			return QHostAddress();
		}
		QByteArray in(128, 0);
		QHostAddress from_addr;
		quint16 from_port;
		int ret = sock->readDatagram(in.data(), in.size(), &from_addr, &from_port);
		if(ret == -1)
		{
			delete sock;
			return QHostAddress();
		}

		if(from_port != port)
			continue;
		in.resize(ret);
		if(in != out)
			continue;

		result = from_addr;
		break;
	}
	delete sock;

	return result;
}

void QJDns::setNameServers(const QList<NameServer> &list)
{
	d->setNameServers(list);
}

int QJDns::queryStart(const QByteArray &name, int type)
{
	int id = jdns_query(d->sess, (const unsigned char *)name.data(), type);
	d->process();
	return id;
}

void QJDns::queryCancel(int id)
{
	jdns_cancel_query(d->sess, id);
	d->removeCancelled(id);
	d->process();
}

int QJDns::publishStart(PublishMode m, const Record &record)
{
	jdns_rr_t *rr = export_record(record);

	int pubmode;
	if(m == QJDns::Unique)
		pubmode = JDNS_PUBLISH_UNIQUE;
	else
		pubmode = JDNS_PUBLISH_SHARED;

	int id = jdns_publish(d->sess, pubmode, rr);
	jdns_rr_delete(rr);
	d->process();
	return id;
}

void QJDns::publishUpdate(int id, const Record &record)
{
	jdns_rr_t *rr = export_record(record);

	jdns_update_publish(d->sess, id, rr);
	jdns_rr_delete(rr);
	d->process();
}

void QJDns::publishCancel(int id)
{
	jdns_cancel_publish(d->sess, id);
	d->removeCancelled(id);
	d->process();
}
