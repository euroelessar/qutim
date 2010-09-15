#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3 {

	class LIBQUTIM_EXPORT ServiceManager
	{
	public:
		static QObject *getByName(const QByteArray &name);
		template<typename T>
		static inline T getByName(const QByteArray &name)
		{ return qobject_cast<T>(ServiceManager::getByName(name)); }
		static QList<QByteArray> names();
	private:
		ServiceManager();
		~ServiceManager();
		Q_DISABLE_COPY(ServiceManager)
	};

} // namespace qutim_sdk_0_3

#endif // SERVICEMANAGER_H
