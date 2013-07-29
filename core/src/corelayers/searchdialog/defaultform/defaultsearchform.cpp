/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "defaultsearchform.h"
#include <qutim/itemdelegate.h>
#include <qutim/icon.h>
#include <QToolButton>

namespace Core
{
DefaultSearchForm::DefaultSearchForm(const QList<AbstractSearchFactory*> &factories,
                                     const QString &title,
                                     const QIcon &icon,
                                     QWidget *parent) :
    AbstractSearchForm(factories, title, icon, parent)
{
    ui.setupUi(this);
    setTitle(title, icon);
    ui.splitter->setStretchFactor(1, 2);
    ui.updateServiceButton->setIcon(Icon("view-refresh"));
    ui.serviceBox->setVisible(false);
    ui.updateServiceButton->setVisible(false);
    ui.progressBar->setVisible(false);
    ui.resultView->setModel(resultModel());
    ui.resultView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.requestBox->setModel(requestsModel());
    connect(ui.searchButton, SIGNAL(clicked()), SLOT(startSearch()));
    connect(ui.cancelButton, SIGNAL(clicked()), SLOT(cancelSearch()));
    connect(ui.requestBox, SIGNAL(currentIndexChanged(int)), SLOT(updateRequest(int)));
    connect(ui.updateServiceButton, SIGNAL(clicked()), SLOT(updateService()));
    if (requestsModel()->rowCount() > 0)
        updateRequest(0);
}

void DefaultSearchForm::startSearch()
{
    if (AbstractSearchForm::startSearch()) {
        setState(true);
    }
}

void DefaultSearchForm::cancelSearch()
{
    if (AbstractSearchForm::cancelSearch()) {
        setState(false);
    }
}

void DefaultSearchForm::updateRequest(int row)
{
    setCurrentRequest(requestsModel()->request(row));
    if (currentRequest()) {
        ui.searchButton->setEnabled(true);
        connect(currentRequest().data(), SIGNAL(done(bool)), SLOT(done(bool)));
        connect(currentRequest().data(), SIGNAL(fieldsUpdated()), SLOT(updateFields()));
        connect(currentRequest().data(), SIGNAL(servicesUpdated()), SLOT(updateServiceBox()));
        connect(currentRequest().data(), SIGNAL(actionsUpdated()), SLOT(updateActionButtons()));
        updateFields();
        updateServiceBox();
        updateActionButtons();
    } else {
        ui.searchButton->setEnabled(false);
        if (searchFieldsWidget())
            searchFieldsWidget()->deleteLater();
        clearActionButtons();
    }
}

void DefaultSearchForm::updateService()
{
    setService(ui.serviceBox->currentText());
}

void DefaultSearchForm::done(bool ok)
{
    Q_UNUSED(ok);
    setState(false);
}

void DefaultSearchForm::updateFields()
{
    updateSearchFieldsWidget();
    if (searchFieldsWidget())
        ui.generalLayout->insertWidget(2, searchFieldsWidget());
}

void DefaultSearchForm::updateServiceBox()
{
    AbstractSearchForm::updateServiceBox(ui.serviceBox, ui.updateServiceButton);
}

void DefaultSearchForm::updateActionButtons()
{
    Q_ASSERT(currentRequest());
    clearActionButtons();
    for (int i = 0, c = currentRequest()->actionCount(); i < c; ++i) {
        QAction *action = actionAt(i);
        QToolButton *button = new QToolButton(this);
        button->setDefaultAction(action);
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        ui.actionLayout->addWidget(button);
        connect(action, SIGNAL(triggered()), SLOT(actionButtonClicked()));
    }
}

void DefaultSearchForm::actionButtonClicked()
{
    QAction *action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action);
    AbstractSearchForm::actionButtonClicked(action,
                                            ui.resultView->selectionModel()->selectedRows());
}

void DefaultSearchForm::setState(bool search)
{
    ui.searchButton->setEnabled(!search);
    ui.cancelButton->setEnabled(search);
    ui.requestBox->setEnabled(!search);
    ui.progressBar->setVisible(search);
    if (searchFieldsWidget())
        searchFieldsWidget()->setEnabled(!search);
}

AbstractSearchForm *DefaultSearchFormFactory::createForm(const QList<AbstractSearchFactory*> &factories,
                                                         const QString &title,
                                                         const QIcon &icon,
                                                         QWidget *parent)
{
    return new DefaultSearchForm(factories, title, icon, parent);
}

}

