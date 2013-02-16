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
#ifndef CONTACTDELEGATE_H
#define CONTACTDELEGATE_H

#include <QAbstractItemDelegate>
#include <qutim/libqutim_global.h>
#include <qutim/plugin.h>

namespace Core
{

using namespace qutim_sdk_0_3;

enum ContactItemRole
{
    BuddyRole = Qt::UserRole,
    StatusRole,
    ContactsCountRole,
    OnlineContactsCountRole,
    AvatarRole,
    ItemTypeRole,
    AccountRole,
    Color
};
Q_DECLARE_FLAGS(ContactItemRoles,ContactItemRole)

enum ContactItemType
{
    InvalidType = 0,
    TagType = 100,
    ContactType = 101,
    AccountType = 102
};

struct ContactDelegatePrivate;

class ContactDelegatePlugin: public Plugin
{
    Q_OBJECT
    Q_CLASSINFO("DebugName", "ContactDelegate")
public:
    ContactDelegatePlugin();
    void init();
    bool load();
    bool unload();
};

class ContactDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
    Q_CLASSINFO("Service","ContactDelegate")
    Q_CLASSINFO("Uses", "IconLoader")
    Q_CLASSINFO("SettingsDescription", "Default style")
public:
    enum ShowFlag
    {
        ShowStatusText = 0x1,
        ShowExtendedInfoIcons = 0x2,
        ShowAvatars = 0x4
    };
    Q_DECLARE_FLAGS(ShowFlags, ShowFlag)
    explicit ContactDelegate(QObject *parent = 0);
    ~ContactDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
public slots:
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index);
    void reloadSettings();
protected:
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
                     const QRect &rect, const QString &text) const;
    void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
                        const QRect &rect, const QPixmap &pixmap) const;
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option,
                   const QRect &rect) const;
    void setExtInfo(const QHash<QString, bool> &info);
    void setFlag(ContactDelegate::ShowFlags flag, bool on);
private:
    QScopedPointer<ContactDelegatePrivate> p;
    QFont m_headerFont;
    QFont m_contactFont;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::ContactItemRoles)
Q_DECLARE_OPERATORS_FOR_FLAGS(Core::ContactDelegate::ShowFlags)

#endif // CONTACTDELEGATE_H

