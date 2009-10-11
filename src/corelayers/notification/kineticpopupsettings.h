/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KINETICPOPUPSETTINGS_H
#define KINETICPOPUPSETTINGS_H
#include <QWidget>

class KineticPopupSettings : public QWidget
{
	Q_OBJECT
public:
    KineticPopupSettings ( QWidget* parent = 0, Qt::WindowFlags f = 0 );
};

#endif // KINETICPOPUPSETTINGS_H
