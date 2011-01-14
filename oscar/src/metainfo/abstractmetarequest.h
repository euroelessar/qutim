/****************************************************************************
 *  abstractmetarequest.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/

#ifndef ABSTRACTMETAREQUEST_H
#define ABSTRACTMETAREQUEST_H

#include "metafield.h"
#include "dataunit.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;
class AbstractMetaRequestPrivate;

class LIBOSCAR_EXPORT AbstractMetaRequest : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(AbstractMetaRequest)
	Q_DISABLE_COPY(AbstractMetaRequest)
public:
	enum ErrorType {
		NoError,
		Canceled,
		Timeout,
		ProtocolError
	};

	AbstractMetaRequest();
	virtual ~AbstractMetaRequest();
	quint16 id() const;
	IcqAccount *account() const;
	bool isDone() const;
	void setTimeout(int msec);
	ErrorType errorType();
	QString errorString();
	static QString readSString(const DataUnit &data);
signals:
	void done(bool ok);
public slots:
	virtual void send() const = 0;
	void cancel();
private slots:
	void timeout();
protected:
	friend class MetaInfo;
	AbstractMetaRequest(IcqAccount *account, AbstractMetaRequestPrivate *d);
	virtual bool handleData(quint16 type, const DataUnit &data) = 0;
	void sendRequest(quint16 type, const DataUnit &data) const;
	void close(bool ok, ErrorType error = NoError, const QString &errorString = QString());
protected:
	QScopedPointer<AbstractMetaRequestPrivate> d_ptr;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // ABSTRACTMETAREQUEST_H
