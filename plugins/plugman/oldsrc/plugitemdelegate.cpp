/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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

#include "plugitemdelegate.h"
#include "plugpackagemodel.h"
#include <QPainter>
#include <QEvent>
#include <QDebug>
#include <QApplication>
#include <qutim/iconmanagerinterface.h>

#define UNIVERSAL_PADDING 6
#define FADE_LENGTH 32
#define ACTION_ICON_SIZE 22
#define DESC_TRIM 150
#define FAV_ICON_SIZE 24

using namespace qutim_sdk_0_2;

plugItemDelegate::plugItemDelegate(QObject* parent)
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/plugman", "plugman"); //FIXME на Элесаровской либе переделать
    settings.beginGroup("interface");
    mainIconSize = settings.value("mainIconSize",32).toInt();
    settings.endGroup();
}

void plugItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {

    QStyleOptionViewItemV4 opt(option);
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    int left = option.rect.left();
    int top = option.rect.top();
    int width = option.rect.width();
    int right = option.rect.right();

    bool leftToRight = (painter->layoutDirection() == Qt::LeftToRight);

    bool group = (index.model()->data(index,plugPackageModel::CategoryRole).toBool());
    QString title = index.model()->data(index, Qt::DisplayRole).toString();
    QString description = index.model()->data(index, plugPackageModel::SummaryRole).toString();
    int installedRole = index.model()->data(index,plugPackageModel::InstalledRole).toInt();

    QString attribute;
    QColor foregroundColor = (option.state.testFlag(QStyle::State_Selected))?
                             option.palette.color(QPalette::HighlightedText):option.palette.color(QPalette::Text);
    QColor color = foregroundColor;
    QIcon actionIcon;
	QIcon::Mode actionIconMode = option.state & QStyle::State_MouseOver ? QIcon::Active : QIcon::Disabled;
    if (!group) {
        switch (installedRole) {
        case isUpgradable:
            attribute = tr("isUpgradable");
			actionIcon = SystemsCity::IconManager()->getIcon("up");
            break;
        case isInstallable:
            attribute = tr("isInstallable");
			actionIcon = SystemsCity::IconManager()->getIcon("add");
            break;
        case isDowngradable:
            attribute = tr("isDowngradable");
			actionIcon = SystemsCity::IconManager()->getIcon("down");
            break;
        case installed:
            attribute = tr("installed");
			actionIcon = SystemsCity::IconManager()->getIcon("remove");
            break;
        default:
            attribute = tr("Unknown");
            break;
        }
        int checkedRole = index.model()->data(index,plugPackageModel::CheckedRole).toInt();
        switch (checkedRole) {
        case markedForInstall:
            attribute = tr ("Install");
			actionIcon = SystemsCity::IconManager()->getIcon("add");
            color = Qt::blue;
            break;
        case markedForRemove:
            attribute = tr ("Remove");
            color = Qt::red;
			actionIcon = SystemsCity::IconManager()->getIcon("remove");
            break;
        case markedForUpgrade:
            attribute = tr ("Upgrade");
            color = Qt::green;
			actionIcon = SystemsCity::IconManager()->getIcon("up");
            break;
        }
        if (checkedRole!=unchecked) {
			qDebug() << "Checked";
            actionIconMode = QIcon::Selected;
        }
    }
    QIcon itemIcon = index.model()->data(index, Qt::DecorationRole).value<QIcon>();
    QIcon::Mode itemIconMode = QIcon::Normal;

    // Painting main column
    QStyleOptionViewItem local_option_title(option);
    QStyleOptionViewItem local_option_normal(option);

    local_option_title.font.setBold(true);
    local_option_title.font.setPointSize(local_option_title.font.pointSize() + 2);

    QPixmap pixmap(option.rect.size());
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.translate(-option.rect.topLeft());

    // Text
    int textInner = 2 * UNIVERSAL_PADDING + mainIconSize;
    const int itemHeight = calcItemHeight(option);

    p.setPen(foregroundColor);
    p.setFont(local_option_title.font);
    p.drawText(
        left + (leftToRight ? textInner : 0),
        top,
        width - textInner, itemHeight / 2,
        Qt::AlignBottom | Qt::AlignLeft, title);
    p.setFont(local_option_normal.font);
    description.truncate(DESC_TRIM);
    p.drawText(
        left + (leftToRight ? textInner : 0),
        top + itemHeight / 2,
        width - textInner, itemHeight / 2,
        Qt::AlignTop | Qt::AlignLeft, description);
    p.setPen(color);
//      if (!group) {
//          p.drawText(
//              left + (leftToRight ? textInner : 0),
//              top + itemHeight/2,
//              width - textInner, itemHeight / 2,
//              Qt::AlignTop | Qt::AlignRight, attribute);
//      }
    p.end();
    painter->drawPixmap(option.rect,pixmap);

    itemIcon.paint(	painter,
                    leftToRight ? left + UNIVERSAL_PADDING : left + width - UNIVERSAL_PADDING - mainIconSize,
                    top + UNIVERSAL_PADDING,
                    mainIconSize, mainIconSize, Qt::AlignCenter, itemIconMode
                  );
    if (!group) {
        actionIcon.paint(painter,
                         right - (leftToRight ? textInner : 0),
                         top + itemHeight/2 - ACTION_ICON_SIZE/2 ,
                         ACTION_ICON_SIZE, ACTION_ICON_SIZE,Qt::AlignRight, actionIconMode
                        );
    }
}

int plugItemDelegate::calcItemHeight(const QStyleOptionViewItem &option) const
{
    // Painting main column
    QStyleOptionViewItem local_option_title(option);
    QStyleOptionViewItem local_option_normal(option);

    local_option_title.font.setBold(true);
    local_option_title.font.setPointSize(local_option_title.font.pointSize() + 2);

    int textHeight = QFontInfo(local_option_title.font).pixelSize() + QFontInfo(local_option_normal.font).pixelSize();
    return qMax(textHeight, mainIconSize) + 2 * UNIVERSAL_PADDING;
}

QSize plugItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    int width = (index.column() == 0) ? index.model()->data(index, Qt::SizeHintRole).toSize().width() : FAV_ICON_SIZE + 2 * UNIVERSAL_PADDING;
    QSize ret;
    ret.rheight() += calcItemHeight(option);
    ret.rwidth()  += width;
    return ret;
}

bool plugItemDelegate::editorEvent(QEvent *event,
                                   QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index)
{
    Q_UNUSED(option)
//    if (!(index.flags() & Qt::ItemIsUserCheckable)) {
//        return false;
//    }
    if (event->type() == QEvent::MouseButtonPress) {
        return model->setData(index, model->data(index, plugPackageModel::CheckedRole), plugPackageModel::CheckedRole);
	}
    else
        return QAbstractItemDelegate::editorEvent(event, model, option, index);
}


