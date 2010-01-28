/*
    LoginForm

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QtGui/QWidget>

namespace Ui {
    class LoginForm;
}

class LoginForm : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(LoginForm)
public:
    explicit LoginForm(QWidget *parent = 0);
    virtual ~LoginForm();
    QString getName();
    QString getPass();
    bool getAutoConnect();
protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::LoginForm *m_ui;

};

#endif // LOGINFORM_H
