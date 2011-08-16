/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

#include "kb_qwerty.h"
#include "ui_kb_qwerty.h"
#include <qutim/icon.h>

#define MODIFIERS_NONE 0
#define MODIFIERS_SHIFT 1
#define MODIFIERS_SYM 3
#define MODIFIERS_ACCENT 5
#define MODIFIERS_SHIFT_SYM 4
#define MODIFIERS_SHIFT_ACCENT 6
#define MODIFIERS_SYM_ACCENT 8
#define MODIFIERS_SHIFT_SYM_ACCENT 9


namespace Core
{

namespace AdiumChat
{
const QString kb_Qwerty::DELETE = "\b";
const QString kb_Qwerty::NEWLINE = "\n";
const QString kb_Qwerty::SPACE = " ";

kb_Qwerty::kb_Qwerty(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::kb_Qwerty)
{
    ui->setupUi(this);

	ui->btShift->setIcon(qutim_sdk_0_3::Icon("top"));
	ui->btNewLine->setIcon(qutim_sdk_0_3::Icon("undo"));
	ui->btDel->setIcon(qutim_sdk_0_3::Icon("back"));

    //Setup all connections
    foreach(QAbstractButton *button, ui->bgDirectInput->buttons())
    {
        connect(button, SIGNAL(clicked()), this, SLOT(directInputClicked()));
    }

    connect(ui->btNewLine,SIGNAL(clicked()), this, SLOT(newLineClicked()));
    connect(ui->btDel,SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->btSpace,SIGNAL(clicked()), this, SLOT(spaceClicked()));
    connect(ui->btShift, SIGNAL(clicked()), this, SLOT(maiuscClicked()));
    connect(ui->btAccent, SIGNAL(clicked()), this, SLOT(accentClicked()));
    connect(ui->btSymbol, SIGNAL(clicked()), this, SLOT(symClicked()));
    //

    m_nActiveModifiers = 0;
    setButtonsLabel();


}

kb_Qwerty::~kb_Qwerty()
{
    delete ui;
}

void kb_Qwerty::setButtonsLabel()
{
    foreach(QAbstractButton *button, ui->bgDirectInput->buttons())
    {
        QString sCharset = button->property("charset").toString();

        if (m_nActiveModifiers == MODIFIERS_NONE)
        {
            //qDebug() << sCharset.at(0);
            button->setText(sCharset.at(0));
        }
        else if(m_nActiveModifiers == MODIFIERS_SHIFT)
        {
            //qDebug() << sCharset.at(1);
            button->setText(sCharset.at(1));
        }
        else if(m_nActiveModifiers == MODIFIERS_SYM)
        {
            //qDebug() << sCharset.at(2);
            button->setText(sCharset.at(2));
        }
        else if (m_nActiveModifiers == MODIFIERS_SHIFT_SYM)
        {
            //qDebug() << sCharset.at(3);
            button->setText(sCharset.at(3));
        }
        else if(m_nActiveModifiers == MODIFIERS_ACCENT)
        {
            //qDebug() << sCharset.at(4);
            button->setText(sCharset.at(4));
        }
        else if(m_nActiveModifiers == MODIFIERS_SHIFT_ACCENT)
        {
            //qDebug() << sCharset.at(4);
            button->setText(sCharset.at(4).toUpper());
        }
        else if(m_nActiveModifiers == MODIFIERS_SHIFT_SYM_ACCENT)
        {
            //qDebug() << sCharset.at(4);
            button->setText(sCharset.at(3).toUpper());
        }
        else if(m_nActiveModifiers == MODIFIERS_SYM_ACCENT)
        {
            //qDebug() << sCharset.at(4);
            button->setText(sCharset.at(2));
        }
    }
}

void kb_Qwerty::maiuscClicked(/*int nForceCheck*/)
{
        QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());

        if(clickedButton->isChecked())
        {
            m_nActiveModifiers = m_nActiveModifiers + MODIFIERS_SHIFT;
        }

        else
            m_nActiveModifiers = m_nActiveModifiers - MODIFIERS_SHIFT;


    setButtonsLabel();
}

void kb_Qwerty::symClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());

    if(clickedButton->isChecked())
        m_nActiveModifiers = m_nActiveModifiers + MODIFIERS_SYM;
    else
        m_nActiveModifiers = m_nActiveModifiers - MODIFIERS_SYM;

    setButtonsLabel();
}
void kb_Qwerty::accentClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());

    if(clickedButton->isChecked())
        m_nActiveModifiers = m_nActiveModifiers + MODIFIERS_ACCENT;
    else
        m_nActiveModifiers = m_nActiveModifiers - MODIFIERS_ACCENT;

    setButtonsLabel();
}

void kb_Qwerty::newLineClicked()
{
    emit input(kb_Qwerty::NEWLINE);
}

void kb_Qwerty::spaceClicked()
{
    emit input(kb_Qwerty::SPACE);
}

void kb_Qwerty::deleteClicked()
{
    emit input(kb_Qwerty::DELETE);
}

void kb_Qwerty::directInputClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString lastPress = clickedButton->text();

    emit input(lastPress);
}

}
}


