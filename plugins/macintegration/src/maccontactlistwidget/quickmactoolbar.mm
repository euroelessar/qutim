#include "quickmactoolbar.h"
#include <qqml.h>
#include <QWindow>
#include <qutim/icon.h>
#include <QDebug>

#import <Cocoa/Cocoa.h>

namespace Core {

using namespace qutim_sdk_0_3;

QuickMacToolBar::QuickMacToolBar(QObject *parent) :
    QObject(parent)
{
    [m_toolBar.nativeToolbar() setDisplayMode: NSToolbarDisplayModeIconOnly];
    [m_toolBar.nativeToolbar() setSizeMode: NSToolbarSizeModeSmall];
    [m_toolBar.nativeToolbar() setAllowsUserCustomization: NO];

    // Always keep at least one item in the toolbar to avoid unnescary resizes
    QMacToolBarItem *item = new QMacToolBarItem(this);
    item->setText(QStringLiteral(" "));
    m_items.append(item);

    reloadItems();

    [m_toolBar.nativeToolbar() insertItemWithItemIdentifier: item->nativeToolBarItem().itemIdentifier atIndex: 0];
}

void QuickMacToolBar::registerTypes()
{
    qmlRegisterType<QuickMacToolBar>("org.qutim.mac", 0, 4, "MacToolBar");
}

QWindow *QuickMacToolBar::window() const
{
    return m_window;
}

void QuickMacToolBar::setWindow(QWindow *window)
{
    if (m_window == window)
        return;

    if (m_window)
        m_toolBar.detachFromWindow();

    if (window) {
        window->winId();
        m_toolBar.attachToWindow(window);
    }

    m_window = window;
    emit windowChanged(window);
}

void QuickMacToolBar::insert(int index, QuickAction *action)
{
    Q_ASSERT(action);
    qDebug() << "|||||| insert" << index << action;

    QMacToolBarItem *item = new QMacToolBarItem(this);

    if (action->isSeparator()) {
        item->setStandardItem(QMacToolBarItem::Space);
    } else {
        item->setIcon(QIcon::fromTheme(action->iconName()));
        connect(action, &QuickAction::iconNameChanged, item, [item, action] () {
            item->setIcon(QIcon::fromTheme(action->iconName()));
        });

        item->setText(action->text());
        connect(action, &QuickAction::textChanged, item, [item, action] () {
            item->setText(action->text());
        });

        connect(item, &QMacToolBarItem::activated, action, &QuickAction::trigger);
    }

    qDebug() << item->icon().name() << item->text();

    m_items.insert(index, item);
    reloadItems();

    [m_toolBar.nativeToolbar() insertItemWithItemIdentifier: item->nativeToolBarItem().itemIdentifier atIndex: index];
}

void QuickMacToolBar::replace(int index, QuickAction *action)
{
    remove(index);
    insert(index, action);
}

void QuickMacToolBar::remove(int index)
{
    qDebug() << "|||||| remove" << index;

    QMacToolBarItem *item = m_items.takeAt(index);

    [m_toolBar.nativeToolbar() removeItemAtIndex: index];

    reloadItems();
    delete item;
}

void QuickMacToolBar::reloadItems()
{
    QList<QMacToolBarItem *> items = m_items;
    items.removeAll(nullptr);
    m_toolBar.setItems(items);
    m_toolBar.setAllowedItems(items);
}

} // namespace Core
