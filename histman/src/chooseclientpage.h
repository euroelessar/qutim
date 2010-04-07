/****************************************************************************
 *  chooseclientpage.h
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

#ifndef CHOOSECLIENTPAGE_H
#define CHOOSECLIENTPAGE_H

#include <QtGui/QWizardPage>
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
