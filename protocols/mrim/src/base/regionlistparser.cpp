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

#include "regionlistparser.h"
#include <QFile>
#include <QTextCodec>
#include <QTextStream>

RegionListParser::RegionListParser(QString relPath)
{//usually will be - Resources/region.txt	
	QFile regionsFile(relPath);
	QTextCodec* codec = QTextCodec::codecForName("utf-8");
	m_regionsList = new QList<LiveRegion>();

	if (codec == NULL)
		return;

	if (!regionsFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	
	QTextStream fileStream(&regionsFile);
	fileStream.setCodec(codec);
	
	while (!fileStream.atEnd()) 
	{
		addRegion(fileStream.readLine());
	}
	regionsFile.close();
}

RegionListParser::~RegionListParser(void)
{
	delete m_regionsList;
}

void RegionListParser::addRegion(QString aStr)
{
	QStringList strList = aStr.split(';');
	LiveRegion reg;
	
	if (strList.count() > 0)
	{
		reg.id = strList[0].toUInt();		
	}

	if (strList.count() > 1)
	{
		reg.cityId = strList[1].toUInt();		
	}

	if (strList.count() > 2)
	{
		reg.countryId = strList[2].toUInt();		
	}

	if (strList.count() > 3)
	{
		reg.name = strList[3];		
	}
	m_regionsList->append(reg);
}

