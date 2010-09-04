/****************************************************************************
*  simplecontactlistitem.cpp
*
*  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
*  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "simplecontactlistdelegate.h"
#include <qutim/tooltip.h>
#include <QToolTip>
#include "simplecontactlistitem.h"
#include <QHelpEvent>
#include <QAbstractItemView>
#include <QApplication>
#include <qutim/debug.h>
#include <QPainter>
#include <qutim/icon.h>
#include "avatarfilter.h"
#include <qutim/config.h>
#include <QStringBuilder>

namespace Core
{
namespace SimpleContactList
{
	bool infoLessThan (const QVariantHash &a, const QVariantHash &b) {
		QString priority = QLatin1String("priorityInContactList");
		int p1 = a.value(priority).toInt();
		int p2 = b.value(priority).toInt();
		return p1 > p2;
	};

	Delegate::Delegate(QTreeView *parent) :
			QAbstractItemDelegate(parent)
	{
		m_horizontal_padding = 5;
		m_vertical_padding = 3;
		m_show_flags = ShowStatusText | ShowExtendedInfoIcons | ShowAvatars;
	}
	
	void Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{			
		QStyleOptionViewItemV4 opt(option);
		painter->save();
		QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
		
		ItemType type = static_cast<ItemType>(index.data(ItemDataType).toInt());
		
		QString name = index.data(Qt::DisplayRole).toString();

		QRect title_rect = option.rect;
		title_rect.setLeft(title_rect.left() + option.decorationSize.width() + 2*m_horizontal_padding);
		title_rect.setTop(title_rect.top() + m_vertical_padding);
		title_rect.setBottom(title_rect.bottom() - m_vertical_padding);
		
		switch (type) {
		case TagType: {
				QStyleOptionButton buttonOption;

				buttonOption.state = option.state;					
#ifdef Q_WS_MAC
				buttonOption.features = QStyleOptionButton::Flat;
				buttonOption.state |= QStyle::State_Raised;
				buttonOption.state &= ~QStyle::State_HasFocus;
#endif

				buttonOption.rect = option.rect;
				buttonOption.palette = option.palette;
				style->drawControl(QStyle::CE_PushButton, &buttonOption, painter, opt.widget);

				QStyleOption branchOption;
				static const int i = 9; // ### hardcoded in qcommonstyle.cpp
				QRect r = option.rect;
				branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
				branchOption.palette = option.palette;
				branchOption.state = QStyle::State_Children;

				QTreeView *view = static_cast<QTreeView *>(parent());

				if (view->isExpanded(index))
					branchOption.state |= QStyle::State_Open;

				style->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, view);
				
				QFont font = opt.font;
				font.setBold(true);
				painter->setFont(font);

				QString count = index.data(ItemContactsCountRole).toString();
				QString online_count = index.data(ItemOnlineContactsCountRole).toString();

				QString txt = name % QLatin1Literal(" (")
							  % online_count
							  % QLatin1Char('/')
							  % count
							  % QLatin1Char(')');
				
				painter->drawText(title_rect,
									Qt::AlignVCenter,
									txt
									);
				break;
			}
		case ContactType: {
				style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
				QRect bounding;
				Status status = index.data(ItemStatusRole).value<Status>();

				if (m_show_flags & ShowExtendedInfoIcons)
				{
					QHash<QString, QVariantHash> extStatuses = status.extendedInfos();

					QList<QVariantHash> list;
					foreach (const QVariantHash &data, extStatuses) {
						QList<QVariantHash>::iterator search_it =
								qLowerBound(list.begin(), list.end(), data, infoLessThan);
						list.insert(search_it,data);
					}

					QString icon = QLatin1String("icon");
					QString showIcon = QLatin1String("showIcon");
					QString id = QLatin1String("id");

					foreach (const QVariantHash &hash, list) {
						QVariant extIconVar = hash.value(icon);
						QIcon icon;
						if (extIconVar.canConvert<ExtensionIcon>())
							icon = extIconVar.value<ExtensionIcon>().toIcon();
						else if (extIconVar.canConvert(QVariant::Icon))
							icon = extIconVar.value<QIcon>();
						if (!hash.value(showIcon,true).toBool() || icon.isNull())
							continue;
						if (!m_statuses.value(hash.value(id).toString(), true))
							continue;
						icon.paint(painter,
									option.rect.left() + m_horizontal_padding,
									option.rect.top() + m_vertical_padding,
									title_rect.right() - m_horizontal_padding,
									option.decorationSize.height(), //FIXME
									Qt::AlignTop |
									Qt::AlignRight);
						title_rect.setWidth(title_rect.width()-option.decorationSize.width()- m_horizontal_padding/2);
					}
				}

				painter->drawText(title_rect,
									Qt::AlignTop,
									name,
									&bounding
									);

				if (m_show_flags & ShowStatusText) {						
					if (!status.text().isEmpty()) {
						QRect status_rect = title_rect;
						status_rect.setTop(status_rect.top() + bounding.height());
#ifdef Q_WS_MAC
						painter->setPen(opt.palette.color(QPalette::Inactive, QPalette::WindowText));
#else
						painter->setPen(opt.palette.color(QPalette::Shadow));
#endif
						QFont font = opt.font;
						font.setPointSize(font.pointSize() - 1);
						painter->setFont(font);
						painter->drawText(status_rect,
											Qt::AlignTop | Qt::TextWordWrap,
											status.text().remove(QLatin1Char('\n'))
											);
					}
				}

				QIcon itemIcon = index.data(Qt::DecorationRole).value<QIcon>();
				bool hasAvatar = false;
				if (m_show_flags & ShowAvatars) {
					QSize avatarSize (option.decorationSize.width()+m_horizontal_padding,
									  option.decorationSize.height()+m_vertical_padding);
					AvatarFilter filter(avatarSize);
					hasAvatar = filter.draw(painter,
											option.rect.left()+m_horizontal_padding/2,
											option.rect.top()+m_vertical_padding/2,
											index.data(ItemAvatarRole).toString(),
											itemIcon);
//					QPixmap avatar = filter.draw(index.data(ItemAvatarRole).toString(),item_icon);
//					if (!avatar.isNull()) {
//						painter->drawPixmap(option.rect.left()+m_horizontal_padding/2,
//											option.rect.top()+m_vertical_padding/2,
//											avatarSize.width(),
//											avatarSize.height(),
//											avatar
//											);
//						hasAvatar = true;
//					}
				} 
				if (!hasAvatar) {
					itemIcon.paint(painter,
									option.rect.left() + m_horizontal_padding,
									option.rect.top() + m_vertical_padding,
									option.decorationSize.width(),
									option.decorationSize.height(),
									Qt::AlignTop);
				}
				
				
				break;
			}
		default:
			break;
		}
		painter->restore();
	}

	
	bool Delegate::helpEvent(QHelpEvent *event,
							 QAbstractItemView *view,
							 const QStyleOptionViewItem &option,
							 const QModelIndex &index)
	{
		Q_UNUSED(option);

		if (!event || !view)
			return false;
		if (event->type() == QEvent::ToolTip) {
			QHelpEvent *he = static_cast<QHelpEvent*>(event);
			if (getItemType(index) == ContactType) {
				ContactItem *item = reinterpret_cast<ContactItem*>(index.internalPointer());
				if (item)
					qutim_sdk_0_3::ToolTip::instance()->showText(he->globalPos(),
																 item->data->contact, view);
				return true;
			}
			QVariant tooltip = index.data(Qt::ToolTipRole);
			if (qVariantCanConvert<QString>(tooltip)) {
				QToolTip::showText(he->globalPos(), tooltip.toString(), view);
				return true;
			}
		}
		return false;
	}
	
	QSize Delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QRect rect = option.rect;
		rect.setLeft(rect.left() + 2*m_horizontal_padding + option.decorationSize.width());
		QFontMetrics metrics = option.fontMetrics;
		int height = metrics.boundingRect(rect, Qt::TextSingleLine,
											index.data(Qt::DisplayRole).toString()).height();

		Status status = index.data(ItemStatusRole).value<Status>();

		ItemType type = static_cast<ItemType>(index.data(ItemDataType).toInt());

		bool isContact = (type == ContactType);

		if (isContact && (m_show_flags & ShowStatusText) && !status.text().isEmpty()) {
			QFont desc_font = option.font;
			desc_font.setPointSize(desc_font.pointSize() -1);
			metrics = QFontMetrics(desc_font);
			height += metrics.boundingRect(rect,
											Qt::TextSingleLine,
											// There is no differ what text to use, but shotter is better
											QLatin1String(".")
//											status.text().remove(QLatin1Char('\n'))
											).height();
		}
		if (isContact)
			height = qMax(option.decorationSize.height(),height);

		height += 2*m_vertical_padding;
		QSize size (option.rect.width(),height);
		return size;
	}
	
void Delegate::setShowFlags(Delegate::ShowFlags flags)
{
	m_show_flags = flags;
}

void Delegate::setExtendedStatuses(const QHash<QString, bool> &statuses)
{
	m_statuses = statuses;
}

}
}
