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

#ifndef PROFILE_H
#define PROFILE_H

#include "libqutim_global.h"
#include <QVariantMap>

namespace qutim_sdk_0_3
{
class Config;
class ProfilePrivate;

class LIBQUTIM_EXPORT Profile : public QObject
{
    Q_OBJECT
public:
	virtual ~Profile();
	
	static Profile *instance();

	Config config();
	QVariant value(const QString &key) const;
	void setValue(const QString &key, const QVariant &value);
	
public slots:
	QString configPath();
	QString configPath(bool *isSystem);
	bool acceptData(const QVariantMap &profilesData, const QString &password, QString *error = NULL);
	bool acceptData(const QVariantMap &profilesData, const QString &password, bool checkHash, QString *error = NULL);

signals:
	void error(const QString &text);

private:
    explicit Profile();
	QScopedPointer<ProfilePrivate> d_ptr;
};
}

#endif // PROFILE_H

