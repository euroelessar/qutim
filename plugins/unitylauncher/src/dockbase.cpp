/****************************************************************************
 * dockbase.cpp
 *  Copyright © 2011, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv2 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#include "dockbase.h"
#include <QApplication>

DockBase::DockBase(QObject *p) : QObject(p) {}

DockBase::~DockBase() {}

void DockBase::setIcon(QIcon &) {}

void DockBase::setOverlayIcon(QIcon &) {}

void DockBase::setMenu(QMenu *) {}

void DockBase::setProgress(int) {}

void DockBase::setBadge(QString &) {}

void DockBase::setAlert(bool) {}
