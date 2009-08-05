/*
	DefaultStatusLayer

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
                  2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "defaultstatuslayer.h"
#include "statusdialog.h"

bool DefaultStatusLayer::setStatusMessage(const TreeModelItem &item, const QString &status_type, QString &status_message, bool &dshow)
{
	Q_UNUSED(item);
	Q_UNUSED(status_type);
    StatusDialog status_dialog(m_profile_name);
    status_dialog.setStatusMessage(status_message);
    if ( status_dialog.exec() )
    {
        status_message = status_dialog.getStatusMessage();
        dshow = status_dialog.getDshowFlag();
        return true;
    }
    else
    {
        return false;
    }
}
