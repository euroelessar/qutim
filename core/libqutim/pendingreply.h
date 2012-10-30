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
        virtual void handle(int count, void **data) = 0;

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
    void setResult_private(const Internal::Variable::PtrList &args);

private:
    QExplicitlySharedDataPointer<PendingReplyData> data;
};

template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
class PendingReplyTemplateBase : protected PendingReplyBase
{
    typedef Internal::List<T1, Internal::List<T2, Internal::List<T3, Internal::List<T4,
            Internal::List<T5, Internal::List<T6, Internal::List<T7, Internal::List<T8,
            void> > > > > > > > Parameters;
    typedef Internal::ParametersInfo<Parameters> ParametersInfo;

public:
    enum { Count = ParametersInfo::Count };

    PendingReplyTemplateBase() : PendingReplyBase(0) {}
    PendingReplyTemplateBase(const PendingReplyTemplateBase &other) : PendingReplyBase(other) {}
    PendingReplyTemplateBase &operator=(const PendingReplyTemplateBase &other) { PendingReplyBase::operator =(other); return *this; }
    ~PendingReplyTemplateBase() {}

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
//        // This is rather slow operation, disable it for release
//        const QList<QByteArray> expectedParameterTypes = method.parameterTypes();
//        QList<QByteArray> parameterTypes;
//        if (expectedParameterTypes.size() > Count) {
//            qWarning("PendingReply::connect: Method %s::%s has to have exactly one argument of type %s",
//                     meta->className(), methodName, QMetaType::typeName(type));
//        }
//        const int type = qMetaTypeId<T>();
//        if (parameterTypes.count() != 1 || QMetaType::typeName(type) != parameterTypes[0]) {
//            qWarning("PendingReply::connect: Method %s::%s has to have exactly one argument of type %s",
//                     meta->className(), methodName, QMetaType::typeName(type));
//            return;
//        }
#endif
        addHandler(new MetaHandler(object, method));
    }

protected:
    template <typename F1, typename F2, typename F3, typename F4,
              typename F5, typename F6, typename F7, typename F8>
    void setResult_impl(const F1 &arg1, const F2 &arg2, const F3 &arg3, const F4 &arg4,
                        const F5 &arg5, const F6 &arg6, const F7 &arg7, const F8 &arg8 )
    {
        Internal::Variable::PtrList args;
        if (Count >= 1) args << Internal::Variable::create(arg1);
        if (Count >= 2) args << Internal::Variable::create(arg2);
        if (Count >= 3) args << Internal::Variable::create(arg3);
        if (Count >= 4) args << Internal::Variable::create(arg4);
        if (Count >= 5) args << Internal::Variable::create(arg5);
        if (Count >= 6) args << Internal::Variable::create(arg6);
        if (Count >= 7) args << Internal::Variable::create(arg7);
        if (Count >= 8) args << Internal::Variable::create(arg8);
        setResult_private(args);
    }

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

        void handle(int count, void **data)
        {
            Q_ASSERT(count == Count);
            Internal::CallHelper<Method, Count>::template call<Parameters>(m_method, data);
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

        void handle(int count, void **data)
        {
            Q_ASSERT(count == Count);
            Internal::GenericArgument<T1> arg1(data[0]);
            Internal::GenericArgument<T2> arg2(data[1]);
            Internal::GenericArgument<T3> arg3(data[2]);
            Internal::GenericArgument<T4> arg4(data[3]);
            Internal::GenericArgument<T5> arg5(data[4]);
            Internal::GenericArgument<T6> arg6(data[5]);
            Internal::GenericArgument<T7> arg7(data[6]);
            Internal::GenericArgument<T8> arg8(data[7]);
            m_method.invoke(object(), arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        }

    private:
        const QMetaMethod m_method;
    };
};

template<typename T1, typename T2, typename T3, typename T4,
         typename T5, typename T6, typename T7, typename T8>
class PendingReply : public PendingReplyTemplateBase<T1, T2, T3, T4, T5, T6, T7, T8>
{
public:
    typedef typename Internal::SafeArgument<T1, true>::Type S1;
    typedef typename Internal::SafeArgument<T2, true>::Type S2;
    typedef typename Internal::SafeArgument<T3, true>::Type S3;
    typedef typename Internal::SafeArgument<T4, true>::Type S4;
    typedef typename Internal::SafeArgument<T5, true>::Type S5;
    typedef typename Internal::SafeArgument<T6, true>::Type S6;
    typedef typename Internal::SafeArgument<T7, true>::Type S7;
    typedef typename Internal::SafeArgument<T8, true>::Type S8;
    typedef PendingReplyTemplateBase<T1, T2, T3, T4, T5, T6, T7, T8> Base;
    enum { Count = Base::Count };

    void setResult(const typename Internal::SafeArgument<T1, Count == 1>::Type &arg1)
    { this->setResult_impl(arg1, S2(), S3(), S4(), S5(), S6(), S7(), S8()); }
    void setResult(const typename Internal::SafeArgument<T1, Count == 2>::Type &arg1,
                   const typename Internal::SafeArgument<T2, Count == 2>::Type &arg2)
    { this->setResult_impl(arg1, arg2, S3(), S4(), S5(), S6(), S7(), S8()); }
    void setResult(const typename Internal::SafeArgument<T1, Count == 3>::Type &arg1,
                   const typename Internal::SafeArgument<T2, Count == 3>::Type &arg2,
                   const typename Internal::SafeArgument<T3, Count == 3>::Type &arg3)
    { this->setResult_impl(arg1, arg2, arg3, S4(), S5(), S6(), S7(), S8()); }
    void setResult(const typename Internal::SafeArgument<T1, Count == 4>::Type &arg1,
                   const typename Internal::SafeArgument<T2, Count == 4>::Type &arg2,
                   const typename Internal::SafeArgument<T3, Count == 4>::Type &arg3,
                   const typename Internal::SafeArgument<T4, Count == 4>::Type &arg4)
    { this->setResult_impl(arg1, arg2, arg3, arg4, S5(), S6(), S7(), S8()); }
    void setResult(const typename Internal::SafeArgument<T1, Count == 5>::Type &arg1,
                   const typename Internal::SafeArgument<T2, Count == 5>::Type &arg2,
                   const typename Internal::SafeArgument<T3, Count == 5>::Type &arg3,
                   const typename Internal::SafeArgument<T4, Count == 5>::Type &arg4,
                   const typename Internal::SafeArgument<T5, Count == 5>::Type &arg5)
    { this->setResult_impl(arg1, arg2, arg3, arg4, arg5, S6(), S7(), S8()); }
    void setResult(const typename Internal::SafeArgument<T1, Count == 6>::Type &arg1,
                   const typename Internal::SafeArgument<T2, Count == 6>::Type &arg2,
                   const typename Internal::SafeArgument<T3, Count == 6>::Type &arg3,
                   const typename Internal::SafeArgument<T4, Count == 6>::Type &arg4,
                   const typename Internal::SafeArgument<T5, Count == 6>::Type &arg5,
                   const typename Internal::SafeArgument<T6, Count == 6>::Type &arg6)
    { this->setResult_impl(arg1, arg2, arg3, arg4, arg5, arg6, S7(), S8()); }
    void setResult(const typename Internal::SafeArgument<T1, Count == 7>::Type &arg1,
                   const typename Internal::SafeArgument<T2, Count == 7>::Type &arg2,
                   const typename Internal::SafeArgument<T3, Count == 7>::Type &arg3,
                   const typename Internal::SafeArgument<T4, Count == 7>::Type &arg4,
                   const typename Internal::SafeArgument<T5, Count == 7>::Type &arg5,
                   const typename Internal::SafeArgument<T6, Count == 7>::Type &arg6,
                   const typename Internal::SafeArgument<T7, Count == 7>::Type &arg7)
    { this->setResult_impl(arg1, arg2, arg3, arg4, arg5, arg6, arg7, S8()); }
    void setResult(const typename Internal::SafeArgument<T1, Count == 8>::Type &arg1,
                   const typename Internal::SafeArgument<T2, Count == 8>::Type &arg2,
                   const typename Internal::SafeArgument<T3, Count == 8>::Type &arg3,
                   const typename Internal::SafeArgument<T4, Count == 8>::Type &arg4,
                   const typename Internal::SafeArgument<T5, Count == 8>::Type &arg5,
                   const typename Internal::SafeArgument<T6, Count == 8>::Type &arg6,
                   const typename Internal::SafeArgument<T7, Count == 8>::Type &arg7,
                   const typename Internal::SafeArgument<T8, Count == 8>::Type &arg8)
    { this->setResult_impl(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); }
};

template<>
class PendingReply<void, void, void, void, void, void, void> : public PendingReplyTemplateBase<void, void, void, void, void, void, void, void>
{
    typedef Internal::UncreatableClass S;

public:
    void setResult() { this->setResult_impl(S(), S(), S(), S(), S(), S(), S(), S()); }
};

} // namespace Ureen

#endif // UREEN_PENDINGREPLY_H
