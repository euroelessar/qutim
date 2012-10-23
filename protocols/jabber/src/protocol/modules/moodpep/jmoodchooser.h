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
#ifndef JMOODCHOOSER_H
#define JMOODCHOOSER_H

#include <QDialog>
#include "jpersonmoodconverter.h"
#include <qutim/actiongenerator.h>

class QListWidgetItem;
namespace Ureen {
class Account;
}

namespace Jabber {

namespace Ui {
class JMoodChooserWindow;
}

class JMoodChooserWindow : public QDialog
{
	Q_OBJECT
public:
	explicit JMoodChooserWindow(Ureen::Account *account, const QString &text,
								const QString &mood, QWidget *parent = 0);
	~JMoodChooserWindow();
private slots:
	void sendMood();
	void onCurrentItemChanged(QListWidgetItem *item);
private:
	Ui::JMoodChooserWindow *ui;
	Ureen::Account *m_account;
	QListWidgetItem *m_noMoodItem;
};

class JMoodChooser : public QObject, public JabberExtension
{
	Q_OBJECT
	Q_INTERFACES(Jabber::JabberExtension)
public:
	JMoodChooser();
	virtual void init(Ureen::Account *account);
private slots:
	void showMoodChooser(QObject *obj);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
private:
	int m_eventId;
	Ureen::Account *m_account;
	QScopedPointer<Ureen::ActionGenerator> m_actionGenerator;
	QString m_currentMood;
	QString m_currentText;
};

class JMoodChooserAction : public Ureen::ActionGenerator
{
public:
	JMoodChooserAction(const QIcon &icon, const Ureen::LocalizedString &text,
					   const QObject *receiver, const char *member);
	JMoodChooserAction(const QIcon &icon, const Ureen::LocalizedString &text,
					   const char *member);
protected:
	virtual void showImpl(QAction *action, QObject *obj);
};

} // namespace Jabber
#endif // JMOODCHOOSER_H

