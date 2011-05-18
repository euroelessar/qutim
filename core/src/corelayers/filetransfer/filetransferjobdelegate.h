/****************************************************************************
 *  filetransferjobdelegate.h
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef FILETRANSFERJOBDELEGATE_H
#define FILETRANSFERJOBDELEGATE_H

#include "itemdelegate.h"

using namespace qutim_sdk_0_3;
namespace Core {

class FileTransferJobDelegate : public ItemDelegate
{
public:
	FileTransferJobDelegate(QObject* parent = 0);
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

} // namespace Core

#endif // FILETRANSFERJOBDELEGATE_H
