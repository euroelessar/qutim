/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef CONTROL_SETTINGSWIDGET_H
#define CONTROL_SETTINGSWIDGET_H

#include <qutim/settingswidget.h>
#include <QListWidget>
#include <QPlainTextEdit>

namespace Control {

namespace Ui {
class SettingsWidget;
}

class AnswerLine : public QWidget
{
	Q_OBJECT
public:
	typedef QSharedPointer<QObject> Guard;
	AnswerLine(const QString &text, QListWidget *list);
	
	QString text() const;
	
protected slots:
	void onButtonClicked();
	
signals:
	void modified();
	
private:
	QPlainTextEdit *m_textEdit;
	QListWidgetItem *m_item;
};

class SettingsWidget : public qutim_sdk_0_3::SettingsWidget
{
	Q_OBJECT
	
public:
	explicit SettingsWidget();
	~SettingsWidget();
	
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
	
	void addAnswer(const QString &text = QString());
	
private slots:
	void onModified();
	void on_addButton_clicked();
	
private:
	Ui::SettingsWidget *ui;
};


} // namespace Control
#endif // CONTROL_SETTINGSWIDGET_H
