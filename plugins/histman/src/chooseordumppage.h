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

#ifndef CHOOSEORDUMPPAGE_H
#define CHOOSEORDUMPPAGE_H

#include <QWizardPage>
#include "historymanagerwindow.h"

namespace Ui {
    class ChooseOrDumpPage;
}

namespace HistoryManager {

class ChooseOrDumpPage : public QWizardPage {
    Q_OBJECT
    Q_DISABLE_COPY(ChooseOrDumpPage)
public:
	explicit ChooseOrDumpPage(HistoryManagerWindow *parent = 0);
    virtual ~ChooseOrDumpPage();

protected:
    virtual void changeEvent(QEvent *e);
	virtual void initializePage();
	virtual void cleanupPage();
	virtual bool validatePage();
	virtual int nextId() const;

private:
    Ui::ChooseOrDumpPage *m_ui;
	HistoryManagerWindow *m_parent;
};

}

#endif // CHOOSEORDUMPPAGE_H

