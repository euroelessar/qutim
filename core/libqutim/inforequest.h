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

#ifndef INFOREQUEST_H
#define INFOREQUEST_H

#include "dataforms.h"
#include "config.h"
#include <QEvent>
#include <QSet>
#include <QHash>

namespace qutim_sdk_0_3
{
class InfoRequest;
class InfoRequestPrivate;
class InfoRequestFactoryPrivate;
class InfoObserverPrivate;
class Account;
class Buddy;

class LIBQUTIM_EXPORT InfoRequestFactory : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Interface", "InfoRequestFactory")
public:
	enum SupportLevel {
		NotSupported,
		Unavailable,
		ReadOnly,
		ReadWrite
	};

	virtual ~InfoRequestFactory();
	virtual SupportLevel supportLevel(QObject *object) = 0;
	virtual InfoRequest *createrDataFormRequest(QObject *object) = 0;
	static InfoRequest *dataFormRequest(QObject *object);
	static InfoRequestFactory *factory(QObject *object);

protected:
	friend class InfoObserver;
	InfoRequestFactory();
	void setSupportLevel(QObject *object, SupportLevel level);
	virtual bool startObserve(QObject *object) = 0;
	virtual bool stopObserve(QObject *object) = 0;
	virtual void virtual_hook(int id, void *data);
};

class LIBQUTIM_EXPORT InfoRequest : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(InfoRequest)
public:
	enum State {
		Initialized,
		Requesting,
		RequestDone,
		Updating,
		Updated,
		Canceled,
		LoadedFromCache,
		Error
	};

	virtual ~InfoRequest();
	QObject *object() const;
	State state() const;
	DataItem dataItem() const;
	LocalizedString errorString() const;
	void requestData(const QSet<QString> &hints = QSet<QString>());
	void updateData(const DataItem &dataItem);
	void cancel();
	QVariant value(const QString &name, const QVariant &def = QVariant()) const;
	template <typename T>
	T value(const QString &name, const T &def = T()) const;
signals:
	void stateChanged(qutim_sdk_0_3::InfoRequest::State state);
	void errorStringChanged(const qutim_sdk_0_3::LocalizedString &error);
protected:
	InfoRequest(QObject *object);
	void setState(State state);
	void setErrorString(const LocalizedString &errorString);
	virtual DataItem createDataItem() const = 0;
	virtual QVariant getValue(const QString &name) const;
	virtual void doRequest(const QSet<QString> &hints) = 0;
	virtual void doUpdate(const DataItem &dataItem) = 0;
	virtual void doCancel() = 0;
	virtual void virtual_hook(int id, void *data);
private:
	QScopedPointer<InfoRequestPrivate> d_ptr;
};

template <typename T>
T InfoRequest::value(const QString &name, const T &def) const
{
	QVariant val = value(name);
    return val.isNull() ? def : val.value<T>();
}

class LIBQUTIM_EXPORT InfoObserver : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(InfoObserver)
public:
	InfoObserver(QObject *object);
	virtual ~InfoObserver();
	QObject *object() const;
	InfoRequestFactory::SupportLevel supportLevel() const;

signals:
	void supportLevelChanged(qutim_sdk_0_3::InfoRequestFactory::SupportLevel level);
	
private slots:
	void onObjectDestroyed(QObject *object);

private:
	friend class InfoRequestFactory;
	QScopedPointer<InfoObserverPrivate> d_ptr;
};

}

Q_DECLARE_INTERFACE(qutim_sdk_0_3::InfoRequestFactory, "org.qutim.InfoRequestFactory")

#endif // INFOREQUEST_H

