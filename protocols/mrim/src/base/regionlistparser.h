/*****************************************************************************
    regionlistparser.h

    Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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
