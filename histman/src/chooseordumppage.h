/****************************************************************************
 *  chooseordumppage.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef CHOOSEORDUMPPAGE_H
#define CHOOSEORDUMPPAGE_H

#include <QtGui/QWizardPage>
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
