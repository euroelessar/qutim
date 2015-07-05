/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PROFILELISTWIDGET_H
#define PROFILELISTWIDGET_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace qutim_sdk_0_3
{
	class Config;
}

namespace Core
{
class ProfileListWidget : public QWidget
{
	Q_OBJECT

public:
	ProfileListWidget(const QString &id, const QString &configDir);
	~ProfileListWidget();

	void activate(bool a);

signals:
	void submit(const QString &password);

private slots:
	void wantLogin();

private:
	QLabel *photoLabel;
	QLineEdit *passwordEdit;
	QPushButton *loginButton;
};
}

#endif // PROFILELISTWIDGET_H

