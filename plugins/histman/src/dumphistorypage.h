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

#ifndef DUMPHISTORYPAGE_H
#define DUMPHISTORYPAGE_H

#include <QWizardPage>
#include <QThread>
#include "historymanagerwindow.h"

namespace Ui {
    class DumpHistoryPage;
}

namespace HistoryManager {

class DumpHistoryPage;

class DumpHistoryPageHelper : public QThread
{
public:
	DumpHistoryPageHelper(DumpHistoryPage *parent);
	virtual void run();
private:
	DumpHistoryPage *m_parent;
};

class DumpHistoryPage : public QWizardPage {
    Q_OBJECT
    Q_DISABLE_COPY(DumpHistoryPage)
public:
	enum State
	{
		PreInit,
		LoadingHistory,
		SavingHistory,
		Finished
	};
	explicit DumpHistoryPage(HistoryManagerWindow *parent = 0);
    virtual ~DumpHistoryPage();

protected:
    virtual void changeEvent(QEvent *e);
	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool isComplete() const;
	virtual bool validatePage();
	virtual int nextId() const;

protected slots:
	void completed();

private:
    Ui::DumpHistoryPage *m_ui;
	HistoryManagerWindow *m_parent;
	State m_state;
	char m_format;
	friend class DumpHistoryPageHelper;
	DumpHistoryPageHelper *m_helper;
//	QList<HistoryExporter *> m_clients_list;
};

}

#endif // DUMPHISTORYPAGE_H

