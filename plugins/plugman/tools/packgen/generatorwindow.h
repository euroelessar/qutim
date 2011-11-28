/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef GENERATORWINDOW_H
#define GENERATORWINDOW_H

#include <QtGui/QWizard>
#include "plugpackage.h"

#define qGen m_parent

class GeneratorWindow : public QWizard
{
    Q_OBJECT

public:
    GeneratorWindow(QWidget *parent = 0);
    ~GeneratorWindow();
	enum State { ChooseType, ChoosePath, Config, Save };
	enum Type { Art, Core, Lib, Plugin };
	inline void setType( Type type ) { m_type = type; }
	inline Type type() const { return m_type; }
private:
	Type m_type;
};

#endif // GENERATORWINDOW_H

