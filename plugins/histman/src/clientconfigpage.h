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

#ifndef CLIENTCONFIGPAGE_H
#define CLIENTCONFIGPAGE_H

#include <QWizardPage>
#include "historymanagerwindow.h"

namespace Ui {
    class ClientConfigPage;
}

namespace HistoryManager {

class ClientConfigPage : public QWizardPage {
    Q_OBJECT
    Q_DISABLE_COPY(ClientConfigPage)
public:
	explicit ClientConfigPage(HistoryManagerWindow *parent = 0);
    virtual ~ClientConfigPage();
	static QString getAppropriatePath(const QString &path);
	static QString getAppropriateFilePath(const QString &filename);

protected:
    virtual void changeEvent(QEvent *e);
	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool validatePage();
	virtual bool isComplete() const;
	virtual int nextId() const;

protected slots:
	void on_browseButton_clicked();
	void onTextChanged(const QString &filename);

private:
    Ui::ClientConfigPage *m_ui;
	bool m_valid;
	HistoryManagerWindow *m_parent;
	QPixmap m_valid_pixmap;
	QPixmap m_invalid_pixmap;
	QList<ConfigWidget> m_config_list;
};

}

#endif // CLIENTCONFIGPAGE_H

