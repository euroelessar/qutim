/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "messagingdialog.h"
#include "manager.h"
#include <QDebug>
#include <qutim/plugininterface.h>
#include "buddylistmanager.h"
#include <QFileDialog>
#include <qutim/iconmanagerinterface.h>
#include <QMenu>

MessagingDialog::MessagingDialog(Manager *manager)
{
    m_manager = manager;
    setupUi(this);
    qutim_sdk_0_2::SystemsCity::PluginSystem()->centerizeWidget(this);
    progressBar->setVisible(false);
    progressHint->setVisible(false);
    QFont font;
    font.setBold(true);
    progressHint->setFont(font);
    //treeWidget->setIconSize(baseSize());
    treeWidget->insertTopLevelItem(0,m_manager->getRootItem());
    treeWidget->sortItems(0,Qt::AscendingOrder);
    m_manager->getRootItem()->setExpanded(true);  

    connect(sendButton,SIGNAL(clicked(bool)),SLOT(onSendButtonClicked()));
    connect(stopButton,SIGNAL(clicked(bool)),SLOT(onStopButtonClicked()));
    connect(stopButton,SIGNAL(clicked(bool)),m_manager,SLOT(endSending()));
    sendButton->setIcon(qutim_sdk_0_2::SystemsCity::IconManager()->getIcon("multiple"));
    stopButton->setIcon(qutim_sdk_0_2::SystemsCity::IconManager()->getIcon("stop"));

    QMenu *menu = new QMenu(tr("Actions"),this);
    QAction *loadAction = new QAction(qutim_sdk_0_2::SystemsCity::IconManager()->getIcon("folder"),
                                      tr("Load buddy list"),
                                      this);
    connect(loadAction,SIGNAL(triggered(bool)),SLOT(onLoadButtonClicked()));
    menu->addAction(loadAction);
    QAction *saveAction = new QAction(qutim_sdk_0_2::SystemsCity::IconManager()->getIcon("save_all"),
                                      tr("Save buddy list"),
                                      this);
    connect(saveAction,SIGNAL(triggered(bool)),SLOT(onSaveButtonClicked()));
    menu->addAction(saveAction);
    actionsButton->setMenu(menu);
    actionsButton->setIcon(qutim_sdk_0_2::SystemsCity::IconManager()->getIcon("advanced"));

    connect(m_manager,SIGNAL(finished(bool)),SLOT(onStopButtonClicked()));
    connect(m_manager,SIGNAL(updateProgressBar(uint,uint,QString)),SLOT(updateProgressBar(uint,uint,QString)));
    connect(treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),SLOT(onTreeWidgetItemChanged(QTreeWidgetItem*,int)));
}

MessagingDialog::~MessagingDialog()
{

}

void MessagingDialog::onSendButtonClicked()
{
    progressBar->setVisible(true);
    messageEdit->setEnabled(false);
    intervalEdit->setEnabled(false);
    m_manager->beginSending(messageEdit->toPlainText(),intervalEdit->text().toInt());
    //progressHint->setVisible(true);
}

void MessagingDialog::onStopButtonClicked()
{
    progressBar->setVisible(false);
    messageEdit->setEnabled(true);
    intervalEdit->setEnabled(true);
    //progressHint->setVisible(false);
    setWindowTitle(tr("Multiply sending: all jobs finished"));
}


void MessagingDialog::onLoadButtonClicked()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Load custom buddy list"),
                                                QDir::homePath(),
                                                "Json files (*.json)");
    BuddyListManager buddyListManager (path);
    m_manager->addCustomBuddyList(buddyListManager.Load());
}


void MessagingDialog::onSaveButtonClicked()
{
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save buddy list"),
                                                QDir::homePath(),
                                                "Json files (*.json)");
    BuddyListManager buddyListManager (path);
    buddyListManager.Save(m_manager->getBuddyList());
}


void MessagingDialog::onTreeWidgetItemChanged(QTreeWidgetItem* item, int )
{
    if ( item->childCount() )
    {
        Qt::CheckState checkState = item->checkState(0);
        for ( int i = 0; i < item->childCount(); i++)
        {
            item->child(i)->setCheckState(0,checkState);
        }
    }
}

void MessagingDialog::updateProgressBar(const uint& completed, const uint& total, const QString& message)
{
    progressBar->setMaximum(total);
    progressBar->setValue(completed);
    progressBar->setFormat(tr("Sending message to %1: %v/%m").arg(message));
    progressBar->setToolTip(tr("Sending message to %1").arg(message));
    //progressHint->setText(tr("Sending messages: (%2/%3)").arg(completed).arg(total));
    int secs = (total-completed)*intervalEdit->text().toInt();
    QTime time (secs/1440, secs/60, secs%60);
    setWindowTitle(tr("Sending message to %1 (%2/%3), time remains: %4").arg(message).arg(completed).arg(total).arg(time.toString()));
}

