/**************************************************************************
*  Rghost Uploader. qutIM 0.3 plugin, that allows users to send files via *
*  rghost.ru serviсe.                                                     *
*  Copyright (C) 2010  Ian Kazlauskas <nayzak90@googlemail.com>           *
*                                                                         *
*  This library is free software: you can redistribute it and/or modify   *
*  it under the terms of the GNU General Public License as published by   *
*  the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                    *
*                                                                         *
*  This library is distributed in the hope that it will be useful,        *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*  GNU General Public License for more details.                           *
*                                                                         *
*  You should have received a copy of the GNU General Public License      *
*  along with this library.  If not, see <http://www.gnu.org/licenses/>.  *
***************************************************************************/

#include "progresswindow.h"
#include "ui_progresswindow.h"

ProgressWindow::ProgressWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressWindow)
{
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(canselUpload()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(close()));
}

ProgressWindow::~ProgressWindow()
{
    delete ui;
}

void ProgressWindow::progressChanged(qint64 value, qint64 max){
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(value);
    ui->progressBar->setFormat(getSize(value) + " / " + getSize(max));
}

QString ProgressWindow::getSize(qint64 bytes){
    if (bytes < 1024){
        return (QString::number(bytes) + " B");
    } else if (bytes < 1048576){
        bytes /= 1024;
        return (QString::number(bytes) + " Kb");
    } else if (bytes < 1073741824){
        bytes /= 1048576;
        return (QString::number(bytes) + " Mb");
    } else{
        bytes /= 1073741824;
        return (QString::number(bytes) + " Gb");
    }
}

void ProgressWindow::setStatus(QString status){
    ui->label_status->setText(status);
}

void ProgressWindow::setFileName(QString file_name){
    ui->label_file->setText(tr("File: ") + file_name);
}

void ProgressWindow::setSpeed(qint64 last, qint64 cur){
    qint64 delta = cur - last;
    setTimeLeft(delta, ui->progressBar->maximum(), ui->progressBar->value());
    if (delta < 1024){
        ui->label_speed->setText(tr("Speed: ") + QString::number(delta) + " B/s");
    } else if (delta < 1048576){
        delta /= 1024;
        ui->label_speed->setText(tr("Speed: ") + QString::number(delta) + " Kb/s");
    } else{
        delta /= 1048576;
        ui->label_speed->setText(tr("Speed: ") + QString::number(delta) + " Mb/s");
    }
}

void ProgressWindow::setTimeLeft(qint32 speed, qint64 file_size, qint64 progress){
    if (speed == 0){
        speed += 1;
    }
    qint32 sec_left = (file_size - progress)/speed;
    ui->label_time_left->setText(tr("Time left: ") + getTime(sec_left));
}

void ProgressWindow::processStarted(){
    m_timer.setInterval(1000);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(time()));
    m_last_progress = 0;
    m_time_passed = 0;
    m_timer.start();
}

void ProgressWindow::time(){
    qint64 cur = ui->progressBar->value();
    setSpeed(m_last_progress, cur);
    m_last_progress = cur;
    m_time_passed++;
    setTimePassed();
}

void ProgressWindow::setTimePassed(){
    ui->label_time_passed->setText(tr("Time passed: ") + getTime(m_time_passed));
}

QString ProgressWindow::getTime(qint32 secs){
    QString time;
    if (secs < 60){
        time = QString::number(secs) + "s";
    } else if (secs < 360){
        qint32 min = secs/60;
        qint32 sec = secs - min*60;
        time = QString::number(min) + "m " + QString::number(sec) + "s";
    } else if (secs < 23040){
        qint32 h = secs/360;
        qint32 min = (secs - h*360) / 60;
        qint32 sec = secs - (h*360 + min*60);
        time = QString::number(h) + "h " + QString::number(min) + "m " + QString::number(sec) + "s";
    } else {
        qint32 d = secs/23040;
        qint32 h = (secs - d*23040) / 360;
        qint32 min = (secs - d*23040 - h*360) / 60;
        qint32 sec = secs - (d*23040 + h*360 + min*60);
        time = QString::number(d) + "d " + QString::number(h) + "h " + QString::number(min) + "m " + QString::number(sec) + "s";
    }
    return time;
}
