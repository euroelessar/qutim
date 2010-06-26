/****************************************************************************
 * wmanager.h
 *
 *  Copyright (c) 2009-2010 by Belov Nikita <null@deltaz.org>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef WMANAGER_H
#define WMANAGER_H

#include <QObject>
#include <QtNetwork>
#include <QtXml>

class WManager : public QObject
{
	Q_OBJECT

public:
	WManager();
	WManager( QString cityid, QString unit = "m" );
	virtual ~WManager();

	void update( int dayf = 0 );

	const QHash< QString, QString > *getUnit();
	const QHash< QString, QString > *getLoc();
	const QHash< QString, QString > *getCC();
	const QHash< QString, QString > *getDayF( int day );
	QString getUnit( QString key );
	QString getLoc( QString key );
	QString getCC( QString key );
	QString getDayF( int day, QString key );

private slots:
	void finished( QNetworkReply *reply );

signals:
	void finished();

private:
	void init();
	void fillData( QDomNodeList elements, QHash< QString, QString > &hash, QString prefix = "" );

	QNetworkAccessManager *m_netman;

	QString m_cityid;
	QString m_unit;

	QHash< QString, QString > m_units; // set of units ( key, value )
	QHash< QString, QString > m_loc; // location info ( key, value )
	QHash< QString, QString > m_cc; // current conditions ( key, value )
	QHash< int, QHash< QString, QString > > m_dayf; // multi-day forecast information ( day, ( day part _ key, value ) )
};

#endif // WMANAGER_H
