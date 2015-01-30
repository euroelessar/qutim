#include "quickmactoolbar.h"
#include <qqml.h>
#include <QWindow>
#include <qutim/icon.h>
#include <QDebug>

#import <Cocoa/Cocoa.h>

@interface QuickMacSearchField : NSSearchField
{
@public
    Core::QuickMacToolBar *toolBar;
    NSString *itemIdentifier;
}
@end

@implementation QuickMacSearchField

- (void)dealloc
{
    [super dealloc];
}

- (NSString *)itemIdentifier
{
    return self->itemIdentifier;
}

- (void)textDidChange:(NSNotification *)obj
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    NSString *nsText = [self stringValue];
    QString result = QString::fromNSString(nsText);
    toolBar->onSearchTextChanged(result);
    [pool release];
    [super textDidChange: obj];
}

@end

namespace Core {

using namespace qutim_sdk_0_3;

QuickMacToolBar::QuickMacToolBar(QObject *parent) :
    QObject(parent)
{
    [m_toolBar.nativeToolbar() setDisplayMode: NSToolbarDisplayModeIconOnly];
    [m_toolBar.nativeToolbar() setSizeMode: NSToolbarSizeModeSmall];
    [m_toolBar.nativeToolbar() setAllowsUserCustomization: NO];

    QMacToolBarItem *spaceItem = new QMacToolBarItem(this);
    spaceItem->setStandardItem(QMacToolBarItem::FlexibleSpace);
    m_items.append(spaceItem);

    // Always keep at least one item in the toolbar to avoid unnescary resizes
    QMacToolBarItem *item = new QMacToolBarItem(this);
    item->setText(QStringLiteral(" "));
    m_items.append(item);

    // Search field
    QMacToolBarItem *searchItem = new QMacToolBarItem(this);
    searchItem->setText(QStringLiteral(""));
    QuickMacSearchField *searchField = [[QuickMacSearchField alloc] init];
    searchField->itemIdentifier = [searchItem->nativeToolBarItem() itemIdentifier];
    searchField->toolBar = this;
    [searchItem->nativeToolBarItem() setView: searchField];
    m_items.append(searchItem);

    reloadItems();

    [m_toolBar.nativeToolbar() insertItemWithItemIdentifier: spaceItem->nativeToolBarItem().itemIdentifier atIndex: 0];
    [m_toolBar.nativeToolbar() insertItemWithItemIdentifier: item->nativeToolBarItem().itemIdentifier atIndex: 0];
    [m_toolBar.nativeToolbar() insertItemWithItemIdentifier: searchItem->nativeToolBarItem().itemIdentifier atIndex: 0];
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

QString QuickMacToolBar::searchText() const
{
    return m_searchText;
}

void QuickMacToolBar::onSearchTextChanged(const QString &searchText)
{
    if (m_searchText == searchText)
        return;

    m_searchText = searchText;
    emit searchTextChanged(m_searchText);
}

void QuickMacToolBar::insert(int index, QuickAction *action)
{
    fixIndex(index);

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
    fixIndex(index);

    QMacToolBarItem *item = m_items.takeAt(index);
    [m_toolBar.nativeToolbar() removeItemAtIndex: index];

    reloadItems();
    delete item;
}

void QuickMacToolBar::fixIndex(int &index)
{
    Q_UNUSED(index);
}

void QuickMacToolBar::reloadItems()
{
    QList<QMacToolBarItem *> items = m_items;
    items.removeAll(nullptr);
    m_toolBar.setItems(items);
    m_toolBar.setAllowedItems(items);
}

} // namespace Core
