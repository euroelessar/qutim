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

#ifndef UREEN_PENDINGREPLY_H
#define UREEN_PENDINGREPLY_H

#include <QObject>
#include <QExplicitlySharedDataPointer>
#include <QMetaMethod>
#include "pendingreply_impl.h"
#include "debug.h"

template <>
inline QVariant qVariantFromValue(const Ureen::Internal::UncreatableClass &) { return QVariant(); }

namespace Ureen {

class PendingReplyData;

class PendingReplyBase
{
public:
    class HandlerBase
    {
    public:
        HandlerBase(QObject *object) : m_object(object) {}
        virtual ~HandlerBase() {}

        QObject *object() const { return m_object; }
        virtual void handle(const QVariantList &data) = 0;

    private:
        QObject *m_object;
    };

    PendingReplyBase(int type);
    PendingReplyBase(const PendingReplyBase &);
    PendingReplyBase &operator=(const PendingReplyBase &);
    ~PendingReplyBase();

    void addHandler(HandlerBase *handler);
    
protected:
    template<typename T1, typename T2, typename T3, typename T4,
             typename T5, typename T6, typename T7, typename T8, int Count>
    friend struct PendyngReplyFinisher;
    void setResult_private(const QVariant &arg1 = QVariant(), const QVariant &arg2 = QVariant(),
                           const QVariant &arg3 = QVariant(), const QVariant &arg4 = QVariant(),
                           const QVariant &arg5 = QVariant(), const QVariant &arg6 = QVariant(),
                           const QVariant &arg7 = QVariant(), const QVariant &arg8 = QVariant());

private:
    QExplicitlySharedDataPointer<PendingReplyData> data;
};

template<typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void,
         typename T5 = void, typename T6 = void, typename T7 = void, typename T8 = void>
class PendingReply : protected PendingReplyBase
{
    typedef Internal::List<T1, Internal::List<T2, Internal::List<T3, Internal::List<T4,
            Internal::List<T5, Internal::List<T6, Internal::List<T7, Internal::List<T8,
            void> > > > > > > > Parameters;
    typedef Internal::ParametersInfo<Parameters> ParametersInfo;
//    typedef typename Internal::SafeArgument<T1>::Type T1;
//    typedef typename Internal::SafeArgument<T2>::Type T2;
//    typedef typename Internal::SafeArgument<T3>::Type T3;
//    typedef typename Internal::SafeArgument<T4>::Type T4;
//    typedef typename Internal::SafeArgument<T5>::Type T5;
//    typedef typename Internal::SafeArgument<T6>::Type T6;
//    typedef typename Internal::SafeArgument<T7>::Type T7;
//    typedef typename Internal::SafeArgument<T8>::Type T8;

public:
    enum { Count = ParametersInfo::Count };

    PendingReply() : PendingReplyBase(0) {}
    PendingReply(const PendingReply &other) : PendingReplyBase(other) {}
    PendingReply &operator=(const PendingReply &other) { PendingReplyBase::operator =(other); return *this; }
    ~PendingReply() {}

    template <typename Method>
    void connect(QObject *object, Method method)
    {
        addHandler(new FunctionHandler<Method>(object, method));
    }

    void connect(QObject *object, const char *slotName)
    {
        const QMetaObject * const meta = object->metaObject();
        const char * const methodName = slotName + 1;
        const int index = meta->indexOfMethod(methodName);
        if (index == -1) {
            qWarning("PendingReply::connect: No such method %s::%s", meta->className(), methodName);
            return;
        }
        const QMetaMethod method = meta->method(index);
#ifndef QT_NO_DEBUG
        // This is rather slow operation, disable it for release
        const QList<QByteArray> parameterTypes = method.parameterTypes();
//        const int type = qMetaTypeId<T>();
//        if (parameterTypes.count() != 1 || QMetaType::typeName(type) != parameterTypes[0]) {
//            qWarning("PendingReply::connect: Method %s::%s has to have exactly one argument of type %s",
//                     meta->className(), methodName, QMetaType::typeName(type));
//            return;
//        }
#endif
        addHandler(new MetaHandler(object, method));
    }

//    void setResult() { setResult_private(); }
//    void setResult(const T1 &arg1)
//    { setResult_private(qVariantFromValue(arg1)); }
//    void setResult(const T1 &arg1, const T2 &arg2)
//    { setResult_private(qVariantFromValue(arg1), qVariantFromValue(arg2)); }
//    void setResult(const T1 &arg1, const T2 &arg2, const T3 &arg3)
//    { setResult_private(qVariantFromValue(arg1), qVariantFromValue(arg2), qVariantFromValue(arg3)); }
//    void setResult(const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4)
//    { setResult_private(qVariantFromValue(arg1), qVariantFromValue(arg2), qVariantFromValue(arg3), qVariantFromValue(arg4)); }

private:
    template<typename F1, typename F2, typename F3, typename F4,
             typename F5, typename F6, typename F7, typename F8, int Count>
    friend struct Internal::PendyngReplyFinisher;
    using PendingReplyBase::setResult_private;
    template <typename Method>
    class FunctionHandler : public PendingReplyBase::HandlerBase
    {
    public:
        FunctionHandler(QObject *object, Method method) : HandlerBase(object), m_method(method)
        {
        }

        void handle(const QVariantList &data)
        {
            Internal::CallHelper<Method, ParametersInfo::Count>::template call<Parameters>(m_method, data);
        }

    private:
        Method m_method;
    };
    class MetaHandler : public PendingReplyBase::HandlerBase
    {
    public:
        MetaHandler(QObject *object, const QMetaMethod &method) : HandlerBase(object), m_method(method)
        {
        }

        void handle(const QVariantList &data)
        {
            const int attributesCount = m_method.attributes();
            Q_ASSERT(data.size() >= attributesCount);
            Internal::GenericArgument<T1> arg1(0 < attributesCount ? data[0] : QVariant());
            Internal::GenericArgument<T2> arg2(1 < attributesCount ? data[1] : QVariant());
            Internal::GenericArgument<T3> arg3(2 < attributesCount ? data[2] : QVariant());
            Internal::GenericArgument<T4> arg4(3 < attributesCount ? data[3] : QVariant());
            Internal::GenericArgument<T5> arg5(4 < attributesCount ? data[4] : QVariant());
            Internal::GenericArgument<T6> arg6(5 < attributesCount ? data[5] : QVariant());
            Internal::GenericArgument<T7> arg7(6 < attributesCount ? data[6] : QVariant());
            Internal::GenericArgument<T8> arg8(7 < attributesCount ? data[7] : QVariant());
            m_method.invoke(object(), arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        }

    private:
        const QMetaMethod m_method;
    };
};

namespace Internal
{

template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8, int Count>
struct PendyngReplyFinisher;

template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8>
struct PendyngReplyFinisher<T1, T2, T3, T4, T5, T6, T7, T8, 0>
{
    typedef PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> PR;
    static typename Internal::EnableIf<PR::Count == 0>::Type finish(PR &reply)
    { reply.setResult_private(); }
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8>
struct PendyngReplyFinisher<T1, T2, T3, T4, T5, T6, T7, T8, 1>
{
    typedef PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> PR;
    static typename Internal::EnableIf<PR::Count == 1>::Type finish(PR &reply, const T1 &arg1)
    { reply.setResult_private(qVariantFromValue(arg1)); }
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8>
struct PendyngReplyFinisher<T1, T2, T3, T4, T5, T6, T7, T8, 2>
{
    typedef PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> PR;
    static typename Internal::EnableIf<PR::Count == 2>::Type finish(PR &reply, const T1 &arg1, const T2 &arg2)
    { reply.setResult_private(qVariantFromValue(arg1), qVariantFromValue(arg2)); }
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8>
struct PendyngReplyFinisher<T1, T2, T3, T4, T5, T6, T7, T8, 3>
{
    typedef PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> PR;
    static typename Internal::EnableIf<PR::Count == 3>::Type finish(PR &reply, const T1 &arg1, const T2 &arg2, const T3 &arg3)
    { reply.setResult_private(qVariantFromValue(arg1), qVariantFromValue(arg2), qVariantFromValue(arg3)); }
};
template<typename T1, typename T2, typename T3, typename T4, typename T5,
         typename T6, typename T7, typename T8>
struct PendyngReplyFinisher<T1, T2, T3, T4, T5, T6, T7, T8, 4>
{
    typedef PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> PR;
    static typename Internal::EnableIf<PR::Count == 4>::Type finish(PR &reply, const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4)
    { reply.setResult_private(qVariantFromValue(arg1), qVariantFromValue(arg2), qVariantFromValue(arg3), qVariantFromValue(arg4)); }
};


template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
struct PendyngReplyFinisherHelper : public PendyngReplyFinisher<T1, T2, T3, T4, T5, T6, T7, T8,
        PendingReply<T1, T2, T3, T4, T5, T6, T7, T8>::Count>
{
};

}

template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
void finishPendingReply(PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> &reply)
{
    Internal::PendyngReplyFinisherHelper<T1, T2, T3, T4, T5, T6, T7, T8>::finish(reply);
}
template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
void finishPendingReply(PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> &reply,
                        const T1 &arg1)
{
    Internal::PendyngReplyFinisherHelper<T1, T2, T3, T4, T5, T6, T7, T8>::finish(reply, arg1);
}
template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
void finishPendingReply(PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> &reply,
                        const T1 &arg1, const T2 &arg2)
{
    Internal::PendyngReplyFinisherHelper<T1, T2, T3, T4, T5, T6, T7, T8>::finish(reply, arg1, arg2);
}
template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
void finishPendingReply(PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> &reply,
                        const T1 &arg1, const T2 &arg2, const T3 &arg3)
{
    Internal::PendyngReplyFinisherHelper<T1, T2, T3, T4, T5, T6, T7, T8>::finish(reply, arg1, arg2, arg3);
}
template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
void finishPendingReply(PendingReply<T1, T2, T3, T4, T5, T6, T7, T8> &reply,
                        const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4)
{
    Internal::PendyngReplyFinisherHelper<T1, T2, T3, T4, T5, T6, T7, T8>::finish(reply, arg1, arg2, arg3, arg4);
}

} // namespace Ureen

#endif // UREEN_PENDINGREPLY_H
