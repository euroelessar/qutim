/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <QPointer>
#include <TelepathyQt/PendingOperation>

namespace Detail {

template <typename Functor, typename ReturnType, typename... Args>
class LambdaFunctor
{
public:
    LambdaFunctor(QObject *object, Functor functor)
        : m_guard(object), m_functor(functor)
    {
    }

    ReturnType operator() (Args... args) const
    {
        if (!m_guard)
            return ReturnType();
        return m_functor(args...);
    }

private:
    QPointer<QObject> m_guard;
    Functor m_functor;
};

template <typename... Args>
struct LambdaFunctorTail
{
};

template <typename Functor>
struct LambdaFunctorHelper2;

template <typename ReturnType, typename... Args>
struct LambdaFunctorHelper2<ReturnType (*) (Args...)>
{
    typedef LambdaFunctorTail<ReturnType, Args...> FunctorTail;
};

template <typename Object, typename ReturnType, typename... Args>
struct LambdaFunctorHelper2<ReturnType (Object::*) (Args...) const>
{
    typedef LambdaFunctorTail<ReturnType, Args...> FunctorTail;
};

template <typename Functor, typename Tail>
struct LambdaFunctorHelper;

template <typename Functor, typename... Args>
struct LambdaFunctorHelper<Functor, LambdaFunctorTail<Args...> >
{
    typedef LambdaFunctor<Functor, Args...> FunctorType;
};

} // Detail

/*!
 * Connect signal to functor slot.
 * Makes sure, that functor won't be called if receiver is dead.
 */
template <typename Sender, typename Signal, typename Slot>
QMetaObject::Connection lconnect(Sender *sender, Signal signal, QObject *receiver, Slot slot)
{
    typedef decltype(&Slot::operator()) SlotFunction;
    typedef typename Detail::LambdaFunctorHelper2<SlotFunction>::FunctorTail FunctorTail;
    typedef typename Detail::LambdaFunctorHelper<Slot, FunctorTail>::FunctorType FunctorType;

    return QObject::connect(sender, signal, FunctorType(receiver, slot));
}

/*!
 * Connect finished signal of Tp::PendingOperation-derived class to functor slot.
 * Makes sure that slot won't be called if receiver is dead.
 */
template <typename Sender, typename Slot>
QMetaObject::Connection lconnect(Sender *sender, QObject *receiver, Slot slot)
{
    return lconnect(sender, &Tp::PendingOperation::finished,
                    receiver, [sender, slot] () { return slot(sender); });
}

/*!
 * Connect finished signal of Tp::PendingOperation-derived class to functor slot.
 */
template <typename Sender, typename Slot>
QMetaObject::Connection lconnect(Sender *sender, Slot slot)
{
    return QObject::connect(sender, &Tp::PendingOperation::finished,
                            [sender, slot] () { return slot(sender); });
}

#endif // FUNCTIONAL_H
