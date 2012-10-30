/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PENDINGREPLY_IMPL_H
#define PENDINGREPLY_IMPL_H

#include <QMetaType>
#include <QVariant>
#include <QSharedPointer>

namespace Ureen
{

template<typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void,
         typename T5 = void, typename T6 = void, typename T7 = void, typename T8 = void>
class PendingReply;

namespace Internal
{

template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8, int Count>
struct PendyngReplyFinisher;

class UncreatableClass
{
private:
    template<typename T1, typename T2, typename T3, typename T4,
             typename T5, typename T6, typename T7, typename T8>
    friend class Ureen::PendingReply;
    UncreatableClass() {}

public:
    ~UncreatableClass() {}
};

template <typename T, bool condition>
struct SafeArgument
{
    typedef T Type;
};
template <bool condition>
struct SafeArgument<void, condition>
{
    typedef UncreatableClass Type;
};
template <typename T>
struct SafeArgument<T, false>
{
    typedef UncreatableClass Type;
};
template <>
struct SafeArgument<void, false>
{
    typedef UncreatableClass Type;
};

typedef void *(*InitFunction)(void *object);
typedef void (*DestroyFunction)(void *object);

template <typename T>
struct TypeFunctions
{
    static void *init(void *data)
    {
        if (!data)
            return new T();
        return new T(*reinterpret_cast<T*>(data));
    }

    static void destory(void *data)
    {
        delete reinterpret_cast<T*>(data);
    }
};

template <>
struct TypeFunctions<UncreatableClass>
{
    static void *init(void *)
    {
        return 0;
    }

    static void destory(void *)
    {
    }
};

class Variable
{
public:
    typedef QSharedPointer<Variable> Ptr;
    typedef QList<Ptr> PtrList;

    template <typename T>
    static Ptr create(const T &data)
    {
        InitFunction init = TypeFunctions<T>::init;
        DestroyFunction destroy = TypeFunctions<T>::destory;
        return Ptr(new Variable(destroy, init(const_cast<T*>(&data))));
    }

    ~Variable()
    {
        m_destroy(m_data);
    }

    void *data() const
    {
        return m_data;
    }

    template <typename T>
    const T *data() const
    {
        return reinterpret_cast<T*>(m_data);
    }

private:
    Variable(DestroyFunction destroy, void *data)
        : m_destroy(destroy), m_data(data)
    {
    }

    DestroyFunction m_destroy;
    void *m_data;
};

template<bool Condition, class T = void> struct EnableIf {};
template<class T> struct EnableIf<true, T> { typedef T Type; };

template <typename Head, typename Tail>
struct List
{
    typedef Head Car;
    typedef Tail Cdr;
};

template <typename L, int index>
struct ListElement
{
    typedef typename ListElement<typename L::Cdr, index - 1>::Car Value;
};

template <typename L>
struct ListElement<L, 0>
{
    typedef typename L::Car Value;
};

template <typename Head, typename Tail>
struct ParametersCounter
{
    enum {
        Value = 1 + ParametersCounter<typename Tail::Car, typename Tail::Cdr>::Value
    };
};

template <typename Head>
struct ParametersCounter<Head, void>
{
    enum { Value = 1 };
};

template <typename Tail>
struct ParametersCounter<void, Tail>
{
    enum { Value = 0 };
};

template <>
struct ParametersCounter<void, void>
{
    enum { Value = 0 };
};

template <typename Parameters>
struct ParametersInfo
{
    enum {
        Count = ParametersCounter<typename Parameters::Car, typename Parameters::Cdr>::Value
    };
};

template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
struct HighParametersCounter
{
    typedef List<T1, List<T2, List<T3, List<T4, List<T5,
        List<T6, List<T7, List<T8, void> > > > > > > > Parameters;
    enum {
        Value = ParametersInfo<Parameters>::Count
    };
};

template <typename Method, int Count> struct CallHelper;

template <typename Method> struct CallHelper<Method, 0>
{
    static void call(Method &method, const QVariantList &) { method(); }
};
template <typename Method> struct CallHelper<Method, 1>
{
    template <typename Args>
    static void call(Method &method, const QVariantList &args) {
        method(args[0].value<ListElement<Args, 0> >());
    }
};
template <typename Method> struct CallHelper<Method, 2>
{
    template <typename Args>
    static void call(Method &method, const QVariantList &args) {
        method(args[0].value<ListElement<Args, 0> >(),
                args[1].value<ListElement<Args, 1> >());
    }
};
template <typename Method> struct CallHelper<Method, 3>
{
    template <typename Args>
    static void call(Method &method, const QVariantList &args) {
        method(args[0].value<ListElement<Args, 0> >(),
                args[1].value<ListElement<Args, 1> >(),
                args[2].value<ListElement<Args, 2> >());
    }
};
template <typename Method> struct CallHelper<Method, 4>
{
    template <typename Args>
    static void call(Method &method, const QVariantList &args) {
        method(args[0].value<ListElement<Args, 0> >(),
                args[1].value<ListElement<Args, 1> >(),
                args[2].value<ListElement<Args, 2> >(),
                args[3].value<ListElement<Args, 3> >());
    }
};

template <typename T>
class GenericArgument
{
public:
    GenericArgument(void *data)
        : m_data(data)
    {
    }

    operator QGenericArgument() const
    {
        if (m_data) {
            const int type = qMetaTypeId<T>();
            return QGenericArgument(QMetaType::typeName(type), m_data);
        } else {
            return QGenericArgument(0);
        }
    }

private:
    void *m_data;
};

template <>
class GenericArgument<void>
{
public:
    GenericArgument(const QVariant &)
    {
    }

    operator QGenericArgument() const
    {
        return QGenericArgument(0);
    }
};

}

}

#endif // PENDINGREPLY_IMPL_H
