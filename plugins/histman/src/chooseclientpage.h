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

#ifndef CHOOSECLIENTPAGE_H
#define CHOOSECLIENTPAGE_H

#include <QWizardPage>
#include <QListWidgetItem>
#include "historymanagerwindow.h"

namespace Ui {
    class ChooseClientPage;
}

namespace HistoryManager {

class ChooseClientPage : public QWizardPage {
    Q_OBJECT
    Q_DISABLE_COPY(ChooseClientPage)
public:
	explicit ChooseClientPage(HistoryManagerWindow *parent = 0);
    virtual ~ChooseClientPage();

protected:
    virtual void changeEvent(QEvent *e);
	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool isComplete() const;
	virtual int nextId() const;

protected slots:
	void clientChanged(QListWidgetItem *current = 0, QListWidgetItem *previous = 0);

private:
    Ui::ChooseClientPage *m_ui;
	HistoryManagerWindow *m_parent;
	bool m_valid;
	QList<HistoryImporter *> m_clients_list;
};

}

#endif // CHOOSECLIENTPAGE_H

