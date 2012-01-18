/****************************************************************************
**
** qutIM instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef CONSTANTS_H
#define CONSTANTS_H


namespace MeegoIntegration
{
#define STYLE " \
    QPushButton { \
     color: white; \
     border: 1px solid black; \
     border-radius: 3px; \
     padding: 1px; \
     background: qlineargradient( \
       x1:0, y1:0, x2:0, y2:1, \
       stop:0 #bec1d2, \
       stop: 0.4 #717990, \
       stop: 0.5 #5c637d \
       stop:1 #68778e \
     ); \
     min-height: 36px; \
    } \
\
    QPushButton:pressed { \
    background: qlineargradient( \
      x1:0, y1:0, x2:0, y2:1, \
      stop:0 #68778e, \
      stop: 0.4 #5c637d \
      stop: 0.5 #717990, \
      stop:1 #bec1d2 \
     ); \
     color: black; \
    } \
\
    QLineEdit { \
     background: qlineargradient( \
      x1:0, y1:0, x2:0, y2:1, \
      stop:0 gray, \
      stop: 0.2 white \
      stop:1 white \
     ); \
     border-radius: 1px; \
     border: 1px solid black; \
     min-height: 24px; \
     color: black; \
    } \
\
	QTextEdit { \
	 background: qlineargradient( \
	  x1:0, y1:0, x2:0, y2:1, \
	  stop:0 gray, \
	  stop: 0.2 white \
	  stop:1 white \
	 ); \
	 border-radius: 1px; \
	 border: 1px solid black; \
	 min-height: 24px; \
	 color: black; \
	} \
	\
	QComboBox { border: 3px solid gray; border-radius: 18px; padding: 8px 20px 6px 8px; margin-top: 2px; border-color: lightgray; border-bottom-color: white; background: white; selection-background-color: rgb(70, 140, 250) }\
QComboBox:focus { border-color: rgb(70, 140, 250) }\
					      QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; width: 40px; border-style: none; padding-right 10px; selection-background-color: rgb(70, 140, 250) }\
					      QComboBox::down-arrow { image: url(/usr/share/themes/blanco/meegotouch/icons/icon-m-toolbar-down.png); }\
					      QListView { selection-background-color: rgb(70, 140, 250); }\
"



}

#endif // CONSTANTS_H
