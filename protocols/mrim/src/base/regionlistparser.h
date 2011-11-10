/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#ifndef REGIONLISTPARSER_H
#define REGIONLISTPARSER_H

#include <QStringList>

struct LiveRegion
{
	quint32 id;
	quint32 cityId;
	quint32 countryId;
	QString name;
};

class RegionListParser
{
public:
	RegionListParser(QString relPath);
	~RegionListParser(void);
	inline const QList<LiveRegion>* getRegionsList() { return m_regionsList; }
	
private:
	void addRegion(QString aStr);

	QList<LiveRegion>* m_regionsList;
};

#endif //REGIONLISTPARSER_H

