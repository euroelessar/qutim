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

#ifndef IMPORTHISTORYPAGE_H
#define IMPORTHISTORYPAGE_H

#include <QWizardPage>
#include "historymanagerwindow.h"
#include <QThread>

namespace Ui {
    class ImportHistoryPage;
}

namespace HistoryManager {

class ImportHistoryPage;

class ImportHistoryPageHepler : public QThread
{
	Q_OBJECT
public:
	ImportHistoryPageHepler(ImportHistoryPage *parent);
	virtual void run();
	inline void setPath(const QString &path) { m_path = path; }
	inline int getTime() { return m_time; }
private:
	ImportHistoryPage *m_parent;
	QString m_path;
	int m_time;
};

class ImportHistoryPage : public QWizardPage {
    Q_OBJECT
    Q_DISABLE_COPY(ImportHistoryPage)
public:
	explicit ImportHistoryPage(HistoryManagerWindow *parent = 0);
    virtual ~ImportHistoryPage();

protected:
    virtual void changeEvent(QEvent *e);
	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool isComplete() const;
	virtual int nextId() const;
public slots:
	void completed();

private:
	friend class ImportHistoryPageHepler;
	ImportHistoryPageHepler *m_helper;
	HistoryManagerWindow *m_parent;
    Ui::ImportHistoryPage *m_ui;
	bool m_completed;
};

}

#endif // IMPORTHISTORYPAGE_H

