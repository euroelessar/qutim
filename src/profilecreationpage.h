/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#ifndef PROFILECREATIONPAGE_H
#define PROFILECREATIONPAGE_H

#include <QWizardPage>
#include <QDir>

class QLineEdit;
namespace Ui {
	class ProfileCreationPage;
}

namespace Core
{
class ProfileCreationPage : public QWizardPage
{
	Q_OBJECT
public:
	ProfileCreationPage(QWidget *parent = 0);
	~ProfileCreationPage();
	virtual void initializePage();
	virtual bool validatePage();
	inline QByteArray cryptoName() const { return m_cryptoName; }

protected slots:
	void on_portableBox_toggled(bool portable);
	void on_idEdit_textChanged(const QString &text);
	void on_cryptoBox_currentIndexChanged(int index);
	void on_configBox_currentIndexChanged(int index);
	void onPathSelectTriggered();
protected:
	void rebaseDirs();
	bool createDirs(QString &);
	void changeEvent(QEvent *e);

private:
	QString m_password;
	QByteArray m_cryptoName;
	Ui::ProfileCreationPage *ui;
	bool m_singleProfile;
	bool m_is_valid;
};
}

#endif // PROFILECREATIONPAGE_H
