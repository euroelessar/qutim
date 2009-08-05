/*
    PopupWindow
    TrayTextBrowser

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

#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QtGui/QWidget>
#include "ui_popupwindow.h"
#include "defaultnotificationlayer.h"
#include "src/pluginsystem.h"
class DefaultNotificationLayer;
using namespace qutim_sdk_0_2;

class PopupWindow : public QWidget
{
    Q_OBJECT

public:
	PopupWindow(DefaultNotificationLayer *, ChatLayerInterface *chat_layer,const TreeModelItem &, quint8, const QString &, int, int, int, int, int, int,
    		bool f = true, QWidget *parent = 0);
    ~PopupWindow();
    bool firstTrayWindow;
    bool showTray;

    void showTrayMessage();
    void setData(const TreeModelItem &item, const QString &, const QString &, const QString &, const QString &);
    void setSystemData(const QString &, const QString &);
    void setTheme(const QString &, const QString&, const QString&, const QString&, const QString&);
private slots:
	void slideVerticallyUp();
	void slideVerticallyDown();
	void slideHorizontallyRight();
	void slideHorizontallyLeft();
	void startChatSlot();

signals:
	void startChat(const QString &);

protected:
	bool eventFilter(QObject *obj, QEvent *event);
	
private:
	DefaultNotificationLayer *m_notification_layer;
	void setDataHtml(const QString &msg, const QString &picturePath);
	void setSystemDataHtml(const QString &msg);
    Ui::PopupWindowClass ui;
    int position;
    int style;
    int time;
    int positionInStack;
    int moveToPointX;
    int moveToPointY;
    int fromX;
    int fromY;
    void slide();
    bool userMessage;
    QString contactUin;
	TreeModelItem m_item;
	quint8 m_type;
	QString m_message;

    QString themePath;
    QString themeHeader;
    QString themeHeaderCSS;
    QString themeContent;
    QString themeContentCSS;
    TreeModelItem m_contact_item;
    ChatLayerInterface *m_chat_layer;
};

#endif // POPUPWINDOW_H
