/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef KB_QWERTY_H
#define KB_QWERTY_H

#include <QWidget>


namespace Ui {
    class kb_Qwerty;
}
namespace Core
{

namespace AdiumChat
{



class kb_Qwerty : public QWidget
{
    Q_OBJECT

public:
    explicit kb_Qwerty(QWidget *parent = 0);
    ~kb_Qwerty();
    static const QString DELETE;
    static const QString NEWLINE;
    static const QString SPACE;

private:
    Ui::kb_Qwerty *ui;
    int m_nActiveModifiers;
    void setButtonsLabel();

public slots:
    void directInputClicked();
           // void settingsChanged(int);
signals:
    void input(QString);

private slots:
    void maiuscClicked(/*int nForceCheck = 0*/);
    void symClicked();
    void newLineClicked();
    void spaceClicked();
    void deleteClicked();
    void accentClicked();

};
}
}

#endif // KB_QWERTY_H

