/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef OBJECTGENERATOR_H
#define OBJECTGENERATOR_H

#include "libqutim_global.h"
#include <QPointer>
#include <QSharedDataPointer>

namespace qutim_sdk_0_3
{
class ObjectGeneratorPrivate;
class ExtensionInfo;

class ObjectGenerator;
class ActionGenerator;
typedef QList<const ObjectGenerator *> GeneratorList;

class LIBQUTIM_EXPORT ObjectGeneratorHolderData : public QSharedData
{
	Q_DISABLE_COPY(ObjectGeneratorHolderData)
public:
	ObjectGenerator *generator() const;
	ActionGenerator *actionGenerator() const;

private:
	friend class ObjectGenerator;
	ObjectGeneratorHolderData(ObjectGenerator *generator);
	ObjectGenerator *m_generator;
};

/**
* @brief ObjectGenerator is general type for object initiatizations.
*
* It's often needed to automatize process of object creation initialization.
* For example, it's needed to provide flexible menus for different objects,
* but some actions should be shown for every @ref Contact instances, other only
* for every Jabber or may be ICQ contacts. Also it's needed to provide ability
* for customizing this actions at menu showing moment (i.e. to disable it or to
* provide subactions).
*
* Another way for using ObjectGenerator is providing meta info for qutIM modules.
* It's needed to provide QMetaObject for object because it contains most of needed
* information such as parent classes, invokable methods, properties, signals, slots
* and so on.
*/
class LIBQUTIM_EXPORT ObjectGenerator
{
	Q_DECLARE_PRIVATE(ObjectGenerator)
	Q_DISABLE_COPY(ObjectGenerator)
	Q_GADGET
protected:
	/**
	* @brief Constructor
	*/
	ObjectGenerator();
#ifndef Q_QDOC
	ObjectGenerator(ObjectGeneratorPrivate &priv);
#endif
public:
	typedef QExplicitlySharedDataPointer<ObjectGeneratorHolderData> Ptr;

	/**
	* @brief Destructor
	*/
	virtual ~ObjectGenerator();
	/**
	* @brief Set value for property of generating objects.
	*
	* For every new object, created by this generator, property @b name will
	* set to @b value
	*
	* @param name Property's name
	* @param value Property's value
	* @return this
	*
	* @note Properties of already created objects won't be changed to this one
	*/
	ObjectGenerator *addProperty(const QByteArray &name, const QVariant &value);
	/**
	* @brief Generate object
	*/
	inline QObject *generate() const
	{ return generateHelper2(); }
	/**
	* @brief Generate object of type @b T
	*
	* Generator will check by meta info if generator's class can be casted
	* to type @b T and only after succesful result of this check it will
	* be created
	*
	* @return Generated object or null if error
	*/
	template<typename T>
	inline T *generate() const
	{
		QObject *object = generateHelper2();
		T *t = qobject_cast<T *>(object);
		if (!t) delete object;
		return t ? t : 0;
	}
//		/**
//		* @brief Generate object
//		*
//		* @param super Meta info of superiour class
//		* @return Generated object or null if object doesn't extends class
//		* represented by superiour meta info error
//		*/
//		inline QObject *generate(const QMetaObject *super) const
//		{ return extends(super) ? generateHelper2() : 0; }
//		/**
//		* @brief Generate object
//		*
//		* @param id Identification of needed interface
//		* @return Generated object or null if class doesn't implement interface
//		*/
//		inline QObject *generate(const char *id) const
//		{ return extends(id) ? generateHelper2() : 0; }
	/**
	* @brief QMetaObject class, which represents object with meta info
	* of generator's object
	*
	* @return QMetaObject of generator's class
	*/
	virtual const QMetaObject *metaObject() const = 0;
	/**
	* @brief Check if object implements interface
	*
	* @param id Identification of interface
	* @return @b True if object implements interface, else @b false
	*/
	bool hasInterface(const char *id) const;

	virtual QList<QByteArray> interfaces() const;

	// TODO: There should be a way for getting interfaces list
//		virtual QList<const char *> interfaces() const = 0;
	/**
	* @brief Check if object extends class, represented by meta info
	*
	* @param super Meta info of superiour class
	* @return @b True if generator's class extends superiour one, else @b false
	*/
	bool extends(const QMetaObject *super) const;
	/**
	* @brief Check if object implements interface
	*
	* @param id Identification of interface
	* @return @b True if object implements interface, else @b false
	*/
	inline bool extends(const char *id) const
	{ return id && hasInterface(id); }
	/**
	* @brief Check if object extends class @b T
	*
	* @return @b True if generator's class extends @b T, else @b false
	*/
	template<typename T>
	inline bool extends() const
	{ return extends_helper<T>(reinterpret_cast<T *>(0)); }

	/**
	 * Function to detect if ModuleManager and it's inner data had been initialized.
	 */
	static bool isInited();
	/**
	 * Returns list of ObjectGenerator's for extensions that match QMetaObject criterion
	 */
	static GeneratorList module(const QMetaObject *module);

	Ptr pointerHolder();
	/**
	 * Returns list of ObjectGenerator's for extensions that match char* interfaceID
	 */
	static GeneratorList module(const char *iid);
	template<typename T> static inline GeneratorList module()
	{ return module_helper<T>(reinterpret_cast<T *>(0)); }
#ifndef Q_QDOC
	ExtensionInfo info() const;
protected:
	template<typename T> static inline GeneratorList module_helper(const QObject *)
	{ return ObjectGenerator::module(&T::staticMetaObject); }
	template<typename T> static inline GeneratorList module_helper(const void *)
	{ return ObjectGenerator::module(qobject_interface_iid<T *>()); }
	template<typename T>
	inline bool extends_helper(const QObject *) const
	{ return extends(&T::staticMetaObject); }
	template<typename T>
	inline bool extends_helper(const void *) const
	{ return extends(qobject_interface_iid<T *>()); }
	QObject *generateHelper2() const;
#endif
protected:
	/**
	* @brief Generate object.
	*
	* This method have to be implemented to enable object generation
	*
	* @return Generated object
	*/
	virtual QObject *generateHelper() const = 0;
	virtual void virtual_hook(int id, void *data);
	QScopedPointer<ObjectGeneratorPrivate> d_ptr;
#ifndef Q_QDOC
public:
	typedef ObjectGeneratorPrivate Data;
	inline Data *data() { return d_ptr.data(); }
#endif
};

template<typename T, typename I0 = void,
typename I1 = void, typename I2 = void, typename I3 = void,
typename I4 = void, typename I5 = void, typename I6 = void,
typename I7 = void, typename I8 = void, typename I9 = void>
class GeneralGenerator : public ObjectGenerator
{
	Q_DISABLE_COPY(GeneralGenerator)
public:
	/**
	* @brief Constructor
	*/
	inline GeneralGenerator() {}
protected:
	/**
	* @brief Generate object of type @b T
	* @return Generated object
	*/
	virtual QObject *generateHelper() const
	{
		return new T();
	}
	/**
	* @brief Get meta info of class @b T
	*
	* @return QMetaObject of class @b T
	*/
	virtual const QMetaObject *metaObject() const
	{
		return &T::staticMetaObject;
	}
	/**
	* @brief Check if class any interface of I[0-9] has identification @b id
	*
	* @param id Identification of interface
	* @return @b True if any interface of I[0-9] has identification @b id
	*/
	virtual QList<QByteArray> interfaces() const
	{
		QList<QByteArray> result;
		addInterface<I0>(result);
		addInterface<I1>(result);
		addInterface<I2>(result);
		addInterface<I3>(result);
		addInterface<I4>(result);
		addInterface<I5>(result);
		addInterface<I6>(result);
		addInterface<I7>(result);
		addInterface<I8>(result);
		addInterface<I9>(result);
		return result;
	}
private:
	template<typename Interface>
	Q_INLINE_TEMPLATE void addInterface(QList<QByteArray> &result, T *pointer = 0) const
	{
        Interface *i = pointer;
		Q_UNUSED(i);
		if (qobject_interface_iid<Interface*>())
			result << qobject_interface_iid<Interface*>();
	}
};


template<typename T, typename I0 = void,
typename I1 = void, typename I2 = void, typename I3 = void,
typename I4 = void, typename I5 = void, typename I6 = void,
typename I7 = void, typename I8 = void, typename I9 = void>
class SingletonGenerator : public GeneralGenerator<T, I0, I1, I2, I3, I4, I5, I6, I7, I8, I9>
{
	Q_DISABLE_COPY(SingletonGenerator)
public:
	/**
	* @brief Constructor
	*/
	inline SingletonGenerator() {}
protected:
	/**
	* @brief Generate object of type @b T
	* @return Generated object
	*
	* @note There can be only one instance of object in the same time.
	* So if it has already been created it will be returned pointer to
	* already created object.
	*/
	virtual QObject *generateHelper() const
	{
		if(m_object.isNull())
			m_object = new T;
		return m_object.data();
	}
	/**
	* @brief Pointer to instance of object
	*/
	mutable QPointer<QObject> m_object;
};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ObjectGenerator*)

#endif // OBJECTGENERATOR_H

