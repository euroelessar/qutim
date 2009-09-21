/*
    W7 integration plugin

    Copyright (c) 2009 by Belov Nikita <null@deltaz.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "wTaskBar.h"

#include <QDebug>

w7itaskbar::w7itaskbar()
{
	m_taskBarList = 0;

	tabWindowType = 0;
}

w7itaskbar::~w7itaskbar()
{
	if ( !m_taskBarList )
		return;

	m_taskBarList->Release();
	m_taskBarList = 0;
}

bool w7itaskbar::init()
{
	if ( m_taskBarList )
		return 0;

	HRESULT hr = CoCreateInstance( CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &m_taskBarList ) );
	if ( FAILED( hr ) )
		return 0;

	hr = m_taskBarList->HrInit();
	if ( FAILED( hr ) )
	{
		m_taskBarList->Release();
		m_taskBarList = 0;

		return 0;
	}

	return 1;
}

void w7itaskbar::setHWND( HWND id, int type )
{
	windowsId.insert( type, id );
	if ( id == 0 )
		alerts.insert( type, 0 );
}

void w7itaskbar::tabAlert( TreeModelItem *item, bool state )
{
	int type;
	if ( windowsId.value( 1, 0 ) != 0 )
		type = 1;
	else
	{
		if ( item->m_item_type == 2 )
			type = 3;
		else if ( item->m_item_type == 34)
			type = 2;
	}

	QString iname = QString("%1.%2.%3").arg(item->m_protocol_name).arg(item->m_account_name).arg(item->m_item_name);
	if ( state )
	{
		if ( !activeAlerts.contains( iname ) )
		{
			activeAlerts.append( iname );
			alerts.insert( type, alerts.value( type, 0 )+1 );
		}
		setOverlayIcon( windowsId.value( type, 0 ), "message" );
	}
	else
	{
		if ( activeAlerts.contains( iname ) )
		{
			alerts.insert( type, alerts.value( type, 0 )-1 );
			activeAlerts.removeAll( iname );
		}
		if ( alerts.value( type, 0 ) == 0 )
			setOverlayIcon( windowsId.value( type, 0 ) );
	}
}

void w7itaskbar::setVisibleStatus( int type, bool status )
{
	HWND id = windowsId.value( type, 0 );

	if ( !m_taskBarList || id == 0 )
		return;

	if ( status )
		m_taskBarList->AddTab( id );
	else
		m_taskBarList->DeleteTab( id );
}

void w7itaskbar::registerTab( HWND /* id */ )
{
}

void w7itaskbar::setOverlayIcon( HWND wid, QString name )
{
	if ( !m_taskBarList )
		return;

	HICON icon = (name.isEmpty() ? NULL : w7iicon( name ).getIcon( 16, 16 ));
	m_taskBarList->SetOverlayIcon( wid, icon, NULL );
}
