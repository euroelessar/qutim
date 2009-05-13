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

#include "plugitemdelegate.h"
#include "plugpackagemodel.h"
#include <QPainter>
#include <QEvent>
#include <QDebug>

#define UNIVERSAL_PADDING 6
#define FADE_LENGTH 32
#define MAIN_ICON_SIZE 48

plugItemDelegate::plugItemDelegate(QObject* parent) {
	
}

void plugItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {

//  	QItemDelegate::paint(painter,option,index);
	
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
	switch (installedRole) {
                case isUpgradable:
			attribute = tr("isUpgradable");
                        break;
                case isInstallable:
			attribute = tr("isInstallable");
                        break;
                case isDowngradable:
                        attribute = tr("isDowngradable");
                        break;
                case installed:
			attribute = tr("installed");
                        break;
                default:
                        attribute = tr("Unknown");
                        break;
	}
        int checkedRole = index.model()->data(index,plugPackageModel::CheckedRole).toInt();
        switch (checkedRole) {
                case markedForInstall:
                        attribute = tr ("Install");
                        color = Qt::blue;
                        break;
                case markedForRemove:
                        attribute = tr ("Remove");
                        color = Qt::red;
                        break;
                case markedForUpgrade:
                        attribute = tr ("Upgrade");
                        color = Qt::green;
                        break;
        }
        QIcon icon = index.model()->data(index, Qt::DecorationRole).value<QIcon>();
        QIcon::Mode iconMode = QIcon::Normal;

	// Painting main column
	QStyleOptionViewItem local_option_title(option);
	QStyleOptionViewItem local_option_normal(option);
	
	local_option_title.font.setBold(true);
	local_option_title.font.setPointSize(local_option_title.font.pointSize() + 2);
	
	QPixmap pixmap(option.rect.size());
	pixmap.fill(Qt::transparent);
	QPainter p(&pixmap);
	p.translate(-option.rect.topLeft());
	
	
	// Painting
        if (option.state & QStyle::State_Selected)	{
            p.fillRect(option.rect,option.palette.highlight());
        }

	// Text
	int textInner = 2 * UNIVERSAL_PADDING + MAIN_ICON_SIZE;
	const int itemHeight = calcItemHeight(option);
	
	p.setPen(foregroundColor);
        if (group) {
                p.setFont(local_option_title.font);
                p.drawText(
                                        left + (leftToRight ? textInner : 0),
                                        top,
                                        width - textInner, itemHeight,
                                        Qt::AlignVCenter | Qt::AlignLeft, title);
        }
        else {
		p.setFont(local_option_title.font);
		p.drawText(
					left + (leftToRight ? textInner : 0),
					top,
					width - textInner, itemHeight / 2,
					Qt::AlignBottom | Qt::AlignLeft, title);
		p.setFont(local_option_normal.font);
		p.drawText(
					left + (leftToRight ? textInner : 0),
					top + itemHeight / 2,
					width - textInner, itemHeight / 2,
					Qt::AlignTop | Qt::AlignLeft, description);
                p.setPen(color);
                p.drawText(
					left + (leftToRight ? textInner : 0),
					top + itemHeight/2,
					width - textInner, itemHeight / 2,
					Qt::AlignTop | Qt::AlignRight, attribute);
        }
	p.end();

        painter->drawPixmap(option.rect,pixmap);

        icon.paint(	painter,
                                leftToRight ? left + UNIVERSAL_PADDING : left + width - UNIVERSAL_PADDING - MAIN_ICON_SIZE,
                                top + UNIVERSAL_PADDING,
                                MAIN_ICON_SIZE, MAIN_ICON_SIZE, Qt::AlignCenter, iconMode
                                );
}

QWidget* plugItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
return QAbstractItemDelegate::createEditor(parent,option,index);
}

int plugItemDelegate::calcItemHeight(const QStyleOptionViewItem &option) const
{
	// Painting main column
	QStyleOptionViewItem local_option_title(option);
	QStyleOptionViewItem local_option_normal(option);
	
	local_option_title.font.setBold(true);
	local_option_title.font.setPointSize(local_option_title.font.pointSize() + 2);
	
	int textHeight = QFontInfo(local_option_title.font).pixelSize() + QFontInfo(local_option_normal.font).pixelSize();
	return qMax(textHeight, MAIN_ICON_SIZE) + 2 * UNIVERSAL_PADDING;
}

QSize plugItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	int width = index.model()->data(index, Qt::SizeHintRole).toSize().width();
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
        qDebug () << model->data(index, plugPackageModel::CheckedRole).toInt();
        return model->setData(index, model->data(index, plugPackageModel::CheckedRole), plugPackageModel::CheckedRole);
    }
    else
        return QAbstractItemDelegate::editorEvent(event, model, option, index);
}
