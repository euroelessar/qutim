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

#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H

#include <QWidget>
#include <QTimer>

namespace Ui {
    class ProgressWindow;
}

class ProgressWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressWindow(QWidget *parent = 0);
    ~ProgressWindow();
    void setStatus(QString status);
    void setFileName(QString file_name);

private:
    qint64 m_last_progress;
    qint32 m_time_passed;
    QTimer m_timer;
    Ui::ProgressWindow *ui;
    QString getSize(qint64 bytes);
    QString getTime(qint32 sec);
    void setTimePassed();
    void setTimeLeft(qint32 speed, qint64 file_size, qint64 progress);
    void setSpeed(qint64 last, qint64 cur);

signals:
    void canselUpload();
private slots:
    void time();
public slots:
    void processStarted();
    void progressChanged(qint64, qint64);
};

#endif // PROGRESSWINDOW_H
