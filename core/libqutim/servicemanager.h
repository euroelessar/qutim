/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "libqutim_global.h"
#include "extensioninfo.h"

namespace qutim_sdk_0_3
{

class ServiceManagerPrivate;

class LIBQUTIM_EXPORT ServicePointerData : public QSharedData
{
	Q_DISABLE_COPY(ServicePointerData)
public:
    typedef QWeakPointer<ServicePointerData> Ptr;
	ServicePointerData();

	QByteArray name;
	QObject *object;
};

class LIBQUTIM_EXPORT ServiceManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ServiceManager)

	Q_PROPERTY(bool inited READ isInited NOTIFY initedChanged)
public:
	static bool isInited();
	static QObject *getByName(const QByteArray &name);
	template<typename T>
	static inline T getByName(const QByteArray &name)
	{ return qobject_cast<T>(ServiceManager::getByName(name)); }
	static QList<QByteArray> names();
	static ServiceManager *instance();
	/*! List info about all implementations of service with \param name
	  */
	static ExtensionInfoList listImplementations(const QByteArray &name);
	/*! Set an implementation with \param info for service with \param name.
	  * When \param info is empty, service is turned off.
	  * Returns true, if the service has been successfully switched. Otherwise,
	  * return false.
	  */
	static bool setImplementation(const QByteArray &name, const ExtensionInfo &info);

signals:
	/*! Notify a change of service.
	  */
	void serviceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject);
	void initedChanged(bool inited = true);
private:
	ServiceManager();
	~ServiceManager();
	template <typename T>
	static ServicePointerData::Ptr getData();
	static ServicePointerData::Ptr getData(const QMetaObject *meta);
	static ServicePointerData::Ptr getData(const QByteArray &name);
	template <typename T>
	static ServicePointerData::Ptr getDataHelper(QObject *);
	template <typename T>
	static ServicePointerData::Ptr getDataHelper(void *);
	
	template<typename T> friend class ServicePointer;
	QScopedPointer<ServiceManagerPrivate> d_ptr;
};

template<typename T>
class ServicePointer
{
public:
	inline ServicePointer() : d(ServiceManager::getData<T>()) {}
	inline ServicePointer(Qt::Initialization) {}
	inline ServicePointer(const QByteArray &name) : d(ServiceManager::getData(name)) {}
	inline ServicePointer(const ServicePointer &o) : d(o.d) {}
	inline ServicePointer &operator =(const ServicePointer &o) { d = o.d; return *this; }
	inline ~ServicePointer() {}

	inline QByteArray name() const { return d ? d.data()->name : QByteArray(); }
	inline T *data() const { return qobject_cast<T*>(d ? d.data()->object : 0); }
	inline T *operator ->() const { Q_ASSERT(d); return data(); }
	inline T &operator *() const { Q_ASSERT(d); return *data(); }
	inline operator bool() const { return d && d.data()->object; }
	inline operator T*() const { return data(); }
	inline bool operator !() const { return !operator bool(); }
	inline bool isNull() const { return !data(); }

private:
	ServicePointerData::Ptr d;
};

template <typename T>
Q_INLINE_TEMPLATE ServicePointerData::Ptr ServiceManager::getData()
{
	return getDataHelper<T>(reinterpret_cast<T*>(0));
}

template <typename T>
Q_INLINE_TEMPLATE ServicePointerData::Ptr ServiceManager::getDataHelper(QObject *)
{
	return getData(&T::staticMetaObject);
}

template <typename T>
Q_INLINE_TEMPLATE ServicePointerData::Ptr ServiceManager::getDataHelper(void *)
{
	return getData(qobject_interface_iid<T*>());
}

} // namespace qutim_sdk_0_3

#endif // SERVICEMANAGER_H

