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

namespace Ureen
{

namespace Internal
{

template <typename Head, typename Tail>
struct List
{
    typedef Head Car;
    typedef Tail Cdr;
};

template <typename T>
class GenericArgument
{
public:
    GenericArgument(const QVariant &data)
        : m_valid(data.isValid()), m_data(data.value<T>())
    {
    }

    operator QGenericArgument() const
    {
        if (m_valid) {
            const int type = qMetaTypeId<T>();
            return QGenericArgument(QMetaType::typeName(type), &m_data);
        } else {
            return QGenericArgument(0);
        }
    }

private:
    bool m_valid;
    T m_data;
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
}

}

}

#endif // PENDINGREPLY_IMPL_H
