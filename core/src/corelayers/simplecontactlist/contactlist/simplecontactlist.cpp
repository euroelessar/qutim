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
#include "simplecontactlist.h"

#include <qutim/simplecontactlist/simplecontactlistitem.h>
#include <qutim/simplecontactlist/abstractcontactmodel.h>
#include <qutim/simplecontactlist/simplestatusdialog.h>
#include <qutim/simplecontactlist/abstractcontactlist.h>
#include <qutim/simplecontactlist/simplecontactlistview.h>
#include "simplecontactlistsettings.h"

#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/icon.h>
#include <QTreeView>
#include <qutim/debug.h>
#include <QStringBuilder>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QCoreApplication>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <qutim/qtwin.h>
#include <qutim/shortcut.h>
#include <QMainWindow>
#include <qutim/servicemanager.h>
#include <qutim/settingslayer.h>
#include <QAbstractItemDelegate>
#include <qutim/servicemanager.h>
#include <qutim/systemintegration.h>
#include <qutim/metacontactmanager.h>
#include <QMenuBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QWidgetAction>
#include <qutim/utils.h>
#include "tagsfilterdialog.h"
#include <qutim/contact.h>

namespace Core
{
namespace SimpleContactList
{

struct ModulePrivate
{
    ~ModulePrivate() {}
    ServicePointer<QWidget> widget;
    ServicePointer<AbstractContactModel> model;
    QScopedPointer<ActionGenerator> tagsGenerator;
    QList<ActionGenerator*> toolBarButtons;
};

Module::Module() : p(new ModulePrivate)
{
    Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ContactList"));
    Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ContactListWidget"));
    Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ContactModel"));
    Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ContactDelegate"));

    // init shortcuts
    Shortcut::registerSequence("contactListGlobalStatus",
                               QT_TRANSLATE_NOOP("ContactList", "Change global status"),
                               QT_TRANSLATE_NOOP("ChatLayer", "ContactList"),
                               QKeySequence("Ctrl+S")
                               );
    Shortcut::registerSequence("contactListActivateMainMenu",
                               QT_TRANSLATE_NOOP("ContactList", "Activate main menu"),
                               QT_TRANSLATE_NOOP("ChatLayer", "ContactList"),
                               QKeySequence("Ctrl+M")
                               );

    p->widget = ServicePointer<QWidget>("ContactListWidget");

    ActionGenerator *gen = new ActionGenerator(Icon("configure"),
                                               QT_TRANSLATE_NOOP("ContactList", "&Settings..."),
                                               this,
                                               SLOT(onConfigureClicked(QObject*))
                                               );
    gen->setMenuRole(QAction::PreferencesRole);
    gen->setPriority(1);
    gen->setType(ActionTypeAdditional);
    gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Main menu"));
    addAction(gen);

    gen = new ActionGenerator(Icon("application-exit"),
                              QT_TRANSLATE_NOOP("ContactList","&Quit"),
                              this,
                              SLOT(onQuitTriggered(QObject*)));
    gen->setMenuRole(QAction::QuitRole);
    gen->setPriority(-127);
    gen->setType(512);
    addAction(gen);

    connect(ServiceManager::instance(), SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
            SLOT(onServiceChanged(QByteArray,QObject*,QObject*)));

    Settings::registerItem(new GeneralSettingsItem<ContactListSettings>(Settings::General,
                                                                        Icon("preferences-contact-list"),
                                                                        QT_TRANSLATE_NOOP("ContactList", "Contact list")));

    QTimer::singleShot(0, this, SLOT(init()));
}

Module::~Module()
{
    AbstractContactListWidget *widget = qobject_cast<AbstractContactListWidget*>(p->widget);
    if (widget) {
        foreach (ActionGenerator *gen, p->toolBarButtons)
            widget->removeButton(gen);
    }
}

QWidget *Module::widget()
{
    return p->widget;
}

void Module::addButton(ActionGenerator *generator)
{
    if (!p->toolBarButtons.contains(generator))
        p->toolBarButtons << generator;
    AbstractContactListWidget *w = qobject_cast<AbstractContactListWidget*>(p->widget);
    if (w)
        w->addButton(generator);
}

void Module::show()
{
    QWidget *w = p->widget->window();
    SystemIntegration::show(w);
    w->setWindowState(w->windowState() & ~Qt::WindowMinimized);
    w->activateWindow();
    w->raise();
}

void Module::hide()
{
    p->widget->window()->hide();
}

void Module::changeVisibility()
{
    QWidget *w = p->widget->window();
    if (w->isActiveWindow()) {
        QTimer::singleShot( 0, w, SLOT(hide()) );
    } else {
        show();
    }
}

void Module::onConfigureClicked(QObject*)
{
    Settings::showWidget();
}

void Module::onQuitTriggered(QObject *)
{
    qApp->quit();
}

bool Module::event(QEvent *ev)
{
    if (ev->type() == ActionCreatedEvent::eventType()) {
        ActionCreatedEvent *event = static_cast<ActionCreatedEvent*>(ev);
        if (event->generator() == p->tagsGenerator.data()) {
            QAction *action = event->action();
            QMenu *menu = new QMenu(p->widget);
            QAction *act = menu->addAction(tr("Select tags"));
            connect(act, SIGNAL(triggered()), this, SLOT(onSelectTagsTriggered()));
            act = menu->addAction(tr("Reset"));
            connect(act, SIGNAL(triggered()), this, SLOT(onResetTagsTriggered()));
            action->setMenu(menu);
        }
    }
    return QObject::event(ev);
}

void Module::init()
{
    p->tagsGenerator.reset(new ActionGenerator(Icon("feed-subscribe"), QT_TRANSLATE_NOOP("ContactList", "Select tags"), 0));
    p->tagsGenerator->addHandler(ActionCreatedHandler,this);
    p->tagsGenerator->setPriority(-127);
    addButton(p->tagsGenerator.data());

    // TODO: choose another, non-kopete icon
    ActionGenerator *gen = new ActionGenerator(Icon("view-user-offline-kopete"), QT_TRANSLATE_NOOP("ContactList","Show/Hide offline"), this, SLOT(onHideShowOffline()));
    gen->setCheckable(true);
    gen->setChecked(!p->model->showOffline());
    gen->setToolTip(QT_TRANSLATE_NOOP("ContactList", "Hide offline"));
    addButton(gen);
}

void Module::onResetTagsTriggered()
{
    p->model->filterList(QStringList());
}

void Module::onSelectTagsTriggered()
{
    QStringList tags = p->model->tags();
    TagsFilterDialog *dialog = new TagsFilterDialog(tags, p->widget);
    if (!p->model->selectedTags().isEmpty())
        tags = p->model->selectedTags().toList();
    dialog->setSelectedTags(tags);
    SystemIntegration::show(dialog);
    centerizeWidget(dialog);
    if (dialog->exec()) {
        p->model->filterList(dialog->selectedTags());
    }
    dialog->deleteLater();
}

void Module::addContact(qutim_sdk_0_3::Contact *contact)
{
#if 1
    p->model->metaObject()->invokeMethod(p->model, "addContact",
                                         Q_ARG(qutim_sdk_0_3::Contact*, contact));
#else
    p->model->addContact(contact);
#endif
}

void Module::onServiceChanged(const QByteArray &name, QObject *now, QObject *old)
{
    Q_UNUSED(old);
    if (name == "ContactModel") {
        AbstractContactListWidget *widget = qobject_cast<AbstractContactListWidget*>(p->widget);
        if (!widget)
            return;
        widget->contactView()->setContactModel(p->model);
        AbstractContactModel *oldModel = qobject_cast<AbstractContactModel*>(old);
        if (oldModel)
            p->model->setContacts(oldModel->contacts());
    } else if (name == "ContactListWidget") {
        AbstractContactListWidget *w = qobject_cast<AbstractContactListWidget*>(now);
        if (w) {
            foreach (ActionGenerator *gen, p->toolBarButtons)
                w->addButton(gen);
        }
    }
}

void Module::onHideShowOffline()
{
    p->model->hideShowOffline();
}

}
}

