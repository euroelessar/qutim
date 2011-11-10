/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "domaininfo_p.h"

namespace qutim_sdk_0_3
{	
	Q_GLOBAL_STATIC(DomainInfoHelper, helper)
	Q_GLOBAL_STATIC(QJDns, qJDns)
	
	class DomainInfoPrivate
	{
		Q_DECLARE_PUBLIC(DomainInfo)
	public:
		DomainInfoPrivate(DomainInfo *q) : q_ptr(q) {}
		QList<DomainInfo::Record> result;
		DomainInfo *q_ptr;
		
		static DomainInfoPrivate *get(DomainInfo *info) { return info->d_func(); }
		void resultsReady() { emit q_func()->resultReady(); }
	};
	
	DomainInfoHelper::DomainInfoHelper()
	{
		QJDns *qjdns = qJDns();
		qjdns->init(QJDns::Unicast, QHostAddress::Any);
		connect(qjdns, SIGNAL(resultsReady(int,QJDns::Response)), this, SLOT(onResultsReady(int,QJDns::Response)));
		connect(qjdns, SIGNAL(error(int,QJDns::Error)), this, SLOT(onError(int,QJDns::Error)));

		QJDns::SystemInfo info = QJDns::systemInfo();
		qjdns->setNameServers( info.nameServers );		
	}
	
	DomainInfoHelper::~DomainInfoHelper()
	{
	}
	
	void DomainInfoHelper::lookup(const QString &service, const QString &proto,
								  const QString &domain, DomainInfo *handler)
	{
		QByteArray request = "_";
		request += service.toUtf8();
		request += "._";
		request += proto.toUtf8();
		request += ".";
		request += domain.toUtf8();
		int id = qJDns()->queryStart(request, QJDns::Srv);
		m_handlers.insert(id, handler);
	}

	void DomainInfoHelper::onResultsReady(int id, const QJDns::Response &results)
	{
		DomainInfo *handler = m_handlers.take(id);
		if (!handler)
			return;
		DomainInfoPrivate *p = DomainInfoPrivate::get(handler);
		p->result.clear();
		DomainInfo::Record infoRecord;
		foreach (const QJDns::Record &record, results.answerRecords) {
			infoRecord.name = QString::fromUtf8(record.name);
			//Remove root dot
			if (infoRecord.name.endsWith('.'))
				infoRecord.name.chop(1);
			infoRecord.port = record.port;
			infoRecord.weight = record.weight;
			p->result.append(infoRecord);
		}
		p->resultsReady();
	}
	
	void DomainInfoHelper::onError(int id, QJDns::Error error)
	{
		Q_UNUSED(error);
		DomainInfo *handler = m_handlers.take(id);
		if (handler)
			emit DomainInfoPrivate::get(handler)->resultsReady();
	}
	
	DomainInfo::DomainInfo(QObject *parent) : QObject(parent), d_ptr(new DomainInfoPrivate(this))
	{
	}

	DomainInfo::~DomainInfo()
	{
	}
	
	void DomainInfo::lookupSrvRecord(const QString &service, const QString &proto, const QString &domain)
	{
		Q_D(DomainInfo);
		d->result.clear();
		Record record = { domain, 5222, 1 };
		d->result.append(record);
		helper()->lookup(service, proto, domain, this);
	}

	QList<DomainInfo::Record> DomainInfo::resultRecords()
	{
		return d_func()->result;
	}
}

