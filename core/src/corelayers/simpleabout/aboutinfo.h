/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2014 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CORE_SIMPLEABOUT_ABOUTINFO_H
#define CORE_SIMPLEABOUT_ABOUTINFO_H

#include <QObject>
#include <qutim/personinfo.h>

namespace Core {
namespace SimpleAbout {

class AboutInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString qutimVersion READ qutimVersion NOTIFY languageChanged)
    Q_PROPERTY(QString qtVersion READ qtVersion NOTIFY languageChanged)
    Q_PROPERTY(int wordSize READ wordSize NOTIFY languageChanged)
    Q_PROPERTY(QString license READ license NOTIFY languageChanged)
    Q_PROPERTY(QString developers READ developers NOTIFY languageChanged)
    Q_PROPERTY(QString translators READ translators NOTIFY languageChanged)
public:
    explicit AboutInfo(QObject *parent = 0);
    
    QString qutimVersion() const;
    QString qtVersion() const;
    int wordSize() const;
    QString license() const;
    QString developers() const;
    QString translators() const;

protected:
	QString toHtml(const QList<qutim_sdk_0_3::PersonInfo> &persons, bool useTask) const;
    
signals:
    void languageChanged();

private:
	QString m_license;
};

void registerTypes();

} // namespace SimpleAbout
} // namespace Core

#endif // CORE_SIMPLEABOUT_ABOUTINFO_H
