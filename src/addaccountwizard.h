/*
    AddAccountWizard, ProtocolPage, LastLoginPage  

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef ADDACCOUNTWIZARD_H_
#define ADDACCOUNTWIZARD_H_

#include <QWizard>
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include "pluginsystem.h"
class ProtocolPage;
class LastLoginPage;

class AddAccountWizard : public QWizard
{
    Q_OBJECT
public:
    enum { Page_Protocol, Page_Login};
    AddAccountWizard(QWidget *parent = 0);
    ~AddAccountWizard();
    void addProtocolsToWizardList(const PluginInfoList &);
    QString getChosenProtocol() const;

private slots:
	void on_currentIdChanged(int);
private:
	ProtocolPage *m_protocol_page;
	LastLoginPage *m_login_page;
	QPoint desktopCenter();
};

class ProtocolPage : public QWizardPage
{
    Q_OBJECT

public:
	
	ProtocolPage(QWidget *parent = 0);
	void addItemToList(const PluginInfo &);
	QString getChosenProtocol() const;
    int nextId() const;

private:
	QLabel *m_top_label;
    QListWidget *m_protocol_list;
};

class LastLoginPage : public QWizardPage
{
    Q_OBJECT

public:
	LastLoginPage(QWidget *parent = 0);
	void setLoginForm(QWidget *);
    int nextId() const;

private:
	QVBoxLayout *layout;
};

#endif /*ADDACCOUNTWIZARD_H_*/
