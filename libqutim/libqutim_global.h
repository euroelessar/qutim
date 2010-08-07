/****************************************************************************
 *  libqutim_global.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef LIBQUTIM_GLOBAL_H
#define LIBQUTIM_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore/QObject>

#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
# error Incompatible Qt library, need 4.6.0 at least
#endif

#ifdef QT_VISIBILITY_AVAILABLE
# define LIBQUTIM_NO_EXPORT __attribute__ ((visibility("hidden")))
#else
# define LIBQUTIM_NO_EXPORT
#endif

#if defined(LIBQUTIM_LIBRARY)
#  define LIBQUTIM_EXPORT Q_DECL_EXPORT
#else
#  define LIBQUTIM_EXPORT Q_DECL_IMPORT
#endif

#ifdef Q_OS_SYMBIAN
#	define QUTIM_SOFTKEYS_SUPPORT
#	define QUTIM_MOBILE_UI
#endif

#define QUTIM_VERSION_STR "0.2.60.0"
#define QUTIM_VERSION 0x00023c00
#define QUTIM_VERSION_CHECK(major, minor, secminor, patch) ((major<<24)|(minor<<16)|(secminor<<8)|(patch))

template <typename Key, typename T>
class QHash;
class QDateTime;

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT StartupModule
	{
		inline StartupModule() {}
		virtual ~StartupModule() {}
	};

//	Q_ENUMS(Status)
//	enum Status
//	{
//		Offline = 0,
//		Online,
//		Away,
//		DND,
//		NA,
//		Occupied,
//		FreeChat,
//		Evil,       // ?
//		Depression, // ?
//		Invisible,
//		AtHome,     // ?
//		AtWork,     // ?
//		OutToLunch,
//		OnThePhone,
//		Connecting  = 100,
//		ConnectingStart = Connecting,
//		ConnectingStop  = 1000
//	};
	enum ChatState
	{
		ChatStateActive = 0,    // User is actively participating in the chat session.
		ChatStateInActive,      // User has not been actively participating in the chat session.
		ChatStateGone,          // User has effectively ended their participation in the chat session.
		ChatStateComposing,     // User is composing a message.
		ChatStatePaused         // User had been composing but now has stopped.
	};
	class ObjectGenerator;
	class Protocol;
	class ModuleManager;
	typedef QList<const ObjectGenerator *> GeneratorList;
	typedef QHash<QString, Protocol *> ProtocolMap;
	LIBQUTIM_EXPORT const char *qutimVersionStr();
	LIBQUTIM_EXPORT quint32 qutimVersion();
	LIBQUTIM_EXPORT const char *metaInfo(const QMetaObject *meta, const char *name);
	LIBQUTIM_EXPORT bool isCoreInited();
	LIBQUTIM_EXPORT GeneratorList moduleGenerators(const QMetaObject *module);
	LIBQUTIM_EXPORT GeneratorList moduleGenerators(const char *iid);
	template<typename T> inline GeneratorList moduleGenerators_helper(const QObject *)
	{ return moduleGenerators(&T::staticMetaObject); }
	template<typename T> inline GeneratorList moduleGenerators_helper(const void *)
	{ return moduleGenerators(qobject_interface_iid<T *>()); }
	template<typename T> inline GeneratorList moduleGenerators()
	{ return moduleGenerators_helper<T>(reinterpret_cast<T *>(0)); }
	LIBQUTIM_EXPORT ProtocolMap allProtocols();
	LIBQUTIM_EXPORT QString convertTimeDate(const QString &mac_format, const QDateTime &datetime);
	LIBQUTIM_EXPORT QString &validateCpp(QString &text );
	LIBQUTIM_EXPORT QString getThemePath (const QString& category, const QString &themeName ); //search theme in user and share dir
	LIBQUTIM_EXPORT QStringList listThemes( const QString &category); //list all themes for category, will seach in user and share fir
	//NOTE it's return themename not themepath!
	LIBQUTIM_EXPORT void centerizeWidget(QWidget *widget); //move widget to center of current screen
	LIBQUTIM_EXPORT QObject *getService(const QByteArray &name);
	template<typename T>
	inline T getService(const QByteArray &name)
	{return qobject_cast<T>(getService(name));}
	LIBQUTIM_EXPORT QList<QByteArray> getServiceNames();


	template <typename ForwardIterator>
	Q_OUTOFLINE_TEMPLATE void qDeleteAllLater(ForwardIterator begin, ForwardIterator end)
	{
		while (begin != end) {
			begin->deleteLater();
			++begin;
		}
	}

	template <typename Container>
	inline void qDeleteAllLater(const Container &c)
	{
		qDeleteAll(c.begin(), c.end());
	}
}

Q_DECLARE_INTERFACE(qutim_sdk_0_3::StartupModule, "org.qutim.StartupModule")

#endif // LIBQUTIM_GLOBAL_H
