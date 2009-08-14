#ifndef CONFIGBACKEND_H
#define CONFIGBACKEND_H

#include "libqutim_global.h"
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QSharedPointer>

namespace qutim_sdk_0_3
{
	struct LIBQUTIM_EXPORT ConfigEntry
	{
		typedef QSharedPointer<ConfigEntry>    Ptr;
		typedef QWeakPointer<ConfigEntry>      WeakPtr;
		typedef QMap<QString,ConfigEntry::Ptr> EntryMap;
		typedef QVector<ConfigEntry::Ptr>      EntryArray;
		enum Type { Invalid = 0x00, Root = 0x01, Map = 0x02,
					Array = 0x04, AnyGroup = Map | Array, Value = 0x08 };
		inline ConfigEntry() : type(AnyGroup), deleted(false), dirty(false) {}
		inline ~ConfigEntry() { remove(); }
		bool remove();
		Type type;
		bool deleted;
		bool dirty;
		QAtomicInt ref;
		EntryMap map;
		EntryArray array;
		QVariant value;
	};

	class LIBQUTIM_EXPORT ConfigBackend : public QObject
	{
		Q_OBJECT
	public:
		virtual ConfigEntry::Ptr parse(const QString &file) = 0;
		virtual void generate(const QString &file, const ConfigEntry::Ptr &entry) = 0;
	protected:
		QIODevice *openDefault(const QString &file, QIODevice::OpenMode mode);
		ConfigBackend();
	};
}

#endif // CONFIGBACKEND_H
