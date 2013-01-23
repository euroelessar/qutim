/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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

#ifndef SHOTER_H
#define SHOTER_H

#include <QMainWindow>
#include <QtGui>
#include <QPixmap>
#include <QNetworkReply>
#include "ui_screenshoter.h"

class Shoter :public QMainWindow
{
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent *event);
public:
    explicit Shoter(QWidget *parent = 0);
    ~Shoter();

    QPixmap pix;
    QLabel label;
    QProgressBar progressBar;
    void writeSettings();
    void readSettings();

private slots:
    void on_Button_send_clicked();
    void on_Button_cancel_clicked();
    void finishedSlot(QNetworkReply* reply);
    void up_progress(qint64 recieved, qint64 total);
    void on_push_save_clicked();
    void on_ShotButton_clicked();
    void reShot();
    void setScreenShot();

private:
    Ui::Screenshoter *ui;
    void upload(QString host_url, QHttpMultiPart *multipart);

signals:
    void link();
};

#endif 
