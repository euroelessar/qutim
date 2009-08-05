#ifndef PLUGINPOINTER_H
#define PLUGINPOINTER_H

#include "include/qutim/plugininterface.h"

namespace qutim_sdk_0_2 {

// TODO: SDK 0.3 Fuck this cast off and use normal qobject_cast
#if !defined(PLUGIN_DEVELS) || (QUTIM_BUILD_VERSION_MINOR >= 2)
inline static const char *plugin_cast_helper( PluginInterface* ) { return "PluginInterface"; }
inline static const char *plugin_cast_helper( PluginContainerInterface* ) { return "PluginContainerInterface"; }
inline static const char *plugin_cast_helper( ProtocolInterface* ) { return "ProtocolInterface"; }
inline static const char *plugin_cast_helper( SimplePluginInterface* ) { return "SimplePluginInterface"; }
inline static const char *plugin_cast_helper( LayerPluginInterface* ) { return "LayerPluginInterface"; }
inline static const char *plugin_cast_helper( DeprecatedSimplePluginInterface* ) { return "DeprecatedSimplePluginInterface"; }
inline static const char *plugin_cast_helper( void* ) { return 0; }

template< typename T >
inline static T plugin_cast( QObject *plugin )
{
	if( !plugin )
		return 0;
	if( T t = qobject_cast<T>( plugin ) )
		return t;
	return static_cast<T>( plugin->qt_metacast( plugin_cast_helper( static_cast<T>(0) ) ) );
}

template< typename T >
inline static T plugin_cast( const QObject *plugin )
{
	return plugin_cast<T>( const_cast<QObject *>( plugin ) );
}
#else
#define plugin_cast qobject_cast
#endif

// TODO: SDK 0.3 May be this class should be public?..
template <typename T>
class AbstractPluginPointer
{
	T *p;
	QObject *o;
public:
	inline AbstractPluginPointer() : p(0), o(0) {}
	// I don't know efficient ways for casts from PluginInterface to QObject
	inline AbstractPluginPointer( QObject *t1, T *t2 ) : p(t2), o(t1)
		{ QMetaObject::addGuard(&o); }
	inline AbstractPluginPointer( QObject *t ) : p(plugin_cast<T *>(t)), o(p?t:0)
		{ QMetaObject::addGuard(&o); }
	inline AbstractPluginPointer( const AbstractPluginPointer<T> &t ) : p(t.p), o(t.o)
		{ QMetaObject::addGuard(&o); }
	inline ~AbstractPluginPointer()
		{ QMetaObject::removeGuard(&o); }
	inline AbstractPluginPointer<T> &operator=(const AbstractPluginPointer<T> &t)
		{ if (this != &t) { QMetaObject::changeGuard(&o, t.o); p = t.p; } return *this; }
	inline AbstractPluginPointer<T> &operator=(QObject* t)
		{ if (o != t) { t = const_cast<QObject *>(p = plugin_cast<T *>(t)); QMetaObject::changeGuard(&o, t); } return *this; }

	inline bool isNull() const
		{ return !o; }

	inline T* operator->() const
		{ return  o ? const_cast<T *>(p) : 0; }
	inline T& operator*() const
		{ return  o ? *const_cast<T *>(p) : *static_cast<T *>(0); }
	inline operator T*() const
		{ return  o ? const_cast<T *>(p) : 0; }
	inline T* data() const
		{ return  o ? const_cast<T *>(p) : 0; }
	inline QObject* object() const
		{ return  o; }

	inline operator QObject*() const
		{ return const_cast<QObject*>(o); }

};

template <class T>
inline bool operator==(T *o, const AbstractPluginPointer<T> &p)
{ return o == p.operator->(); }

template<class T>
inline bool operator==(const AbstractPluginPointer<T> &p, T *o)
{ return p.operator->() == o; }

template<class T>
inline bool operator==(const AbstractPluginPointer<T> &p1, const AbstractPluginPointer<T> &p2)
{ return p1.operator->() == p2.operator->(); }

typedef AbstractPluginPointer<PluginInterface> PluginPointer;
typedef AbstractPluginPointer<PluginContainerInterface> PluginContainerPointer;
typedef AbstractPluginPointer<ProtocolInterface> ProtocolPointer;
typedef AbstractPluginPointer<SimplePluginInterface> SimplePluginPointer;
typedef AbstractPluginPointer<LayerPluginInterface> LayerPluginPointer;
typedef AbstractPluginPointer<DeprecatedSimplePluginInterface> DeprecatedSimplePluginPointer;

}

#endif // PLUGINPOINTER_H
