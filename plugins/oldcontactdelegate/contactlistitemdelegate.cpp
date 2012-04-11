/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "contactlistitemdelegate.h"
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/extensionicon.h>
#include <qutim/buddy.h>
#include <qutim/tooltip.h>
#include <qutim/servicemanager.h>
#include <qutim/thememanager.h>
#include <qutim/avatarfilter.h>
#include <QDir>
#include <QFile>
#include <QScrollBar>
#include <QBrush>
#include <QPalette>
#include <QBitmap>
#include <QLinearGradient>
#include <QItemEditorFactory>
#include <QApplication>
#include <QHelpEvent>
#include <QLatin1Literal>
#include "settings/olddelegatesettings.h"

bool contactInfoLessThan(const QVariantHash &a, const QVariantHash &b) {
	QString priority = QLatin1String("priorityInContactList");
	int p1 = a.value(priority).toInt();
	int p2 = b.value(priority).toInt();
	return p1 > p2;
}

OldContactDelegatePlugin::OldContactDelegatePlugin()
{
}

void OldContactDelegatePlugin::init()
{
	qutim_sdk_0_3::ExtensionIcon icon(QLatin1String(""));
	qutim_sdk_0_3::LocalizedString name = QT_TRANSLATE_NOOP("Plugin", "Old contact delegate");
	qutim_sdk_0_3::LocalizedString description = QT_TRANSLATE_NOOP("Plugin", "Port of contact list delegate from 0.2");
	setInfo(name, description, QUTIM_VERSION, icon);
	addExtension<ContactListItemDelegate>(name, description, icon);
	addExtension<OldDelegateSettings, Core::ContactListSettingsExtention>(name, description, icon);
}

bool OldContactDelegatePlugin::load()
{
	return true;
}

bool OldContactDelegatePlugin::unload()
{
	return false;
}

ContactListItemDelegate::ContactListItemDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{
//	m_settings.reset(new GeneralSettingsItem<OldDelegateSettings>(
//	                     Settings::General, QIcon(),
//	                     QT_TRANSLATE_NOOP("ContactList", "Contact list")));
//	Settings::registerItem(m_settings.data());
	m_margin = 1;
	m_styleType = LightStyle;
	reloadSettings();
	Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "qutIM 0.2 style"));
}

ContactListItemDelegate::~ContactListItemDelegate()
{
//	Settings::removeItem(m_settings.data());
}

void ContactListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option2, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 option(option2);
	ContactItemType type = static_cast<ContactItemType>(index.data(ItemTypeRole).toInt());
	Status status = index.data(StatusRole).value<Status>();

	const QWidget *widget = getWidget(option);
	const QTreeView *treeView = qobject_cast<const QTreeView*>(widget);

	QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ?
							  QPalette::Normal :
							  QPalette::Disabled;
	if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
		cg = QPalette::Inactive;

	QBrush selected = option.palette.brush(cg, QPalette::Highlight);  
	
	QFont font = option.font;
	QVariant var_font = index.data(Qt::FontRole);
	if(var_font.canConvert(QVariant::Font))
		font = var_font.value<QFont>();

	QColor fontColor = painter->pen().color();
	QVariant varColor = index.data(Color);
	if(varColor.canConvert(QVariant::Font))
		fontColor = varColor.value<QColor>();

	QFont statusFont = font;
	QColor statusColor = fontColor;

	// prepare
	painter->save();
	switch(m_styleType)
	{
	case AdiumStyle: {
		painter->save();
		// painter->setOpacity(m_opacity);
		QPixmap backgroundPixmap(option.rect.size());
		backgroundPixmap.fill(QColor::fromRgb(0,0,0,0));
		QPainter *backgroundPainter = new QPainter(&backgroundPixmap);
		// Choose colors and fonts, based on type of item
		QPen pen = painter->pen();
		QBrush brush = painter->brush();
		QPixmap alpha;
		int rectType = 0;
		QRect rect(QPoint(0,0), option.rect.size());
		QColor backgroundColor = a->backgroundColor.isValid() ? a->backgroundColor : painter->background().color();
		backgroundPainter->setPen(QColor(255,255,255,0));
		backgroundPainter->setBrush(QBrush(QColor(255,255,255,0)));
		switch(type)
		{
		case ContactType: {
			if (index.row() == index.model()->rowCount(index.parent()) - 1)
				rectType = 2;
			if (a->isContactFontValid)
				font = a->contactFont;

			AdiumColors xstyle;
			switch (status.type()) {
			case Status::Online:
			case Status::FreeChat:
				xstyle = a->onlineStyle;
			case Status::Away:
			case Status::DND:
			case Status::NA:
			case Status::Invisible:
				xstyle = a->awayStyle;
			case Status::Connecting:
			case Status::Offline:
				xstyle = a->offlineStyle;
			}

			if (xstyle.fontColor.isValid())
				fontColor = xstyle.fontColor;
			if (option.state & QStyle::State_Selected) {
				if (xstyle.labelColor.isValid())
					drawRect(backgroundPainter, xstyle.labelColor);
				else
					backgroundPainter->setBrush(selected);
			} else {
				drawRect(backgroundPainter, backgroundColor);
			}
			break;
		}
		case InvalidType:
		case SeparatorType:
			drawRect(backgroundPainter, backgroundColor);
			if (option.state & QStyle::State_Selected)
				backgroundPainter->setBrush(selected);
			break;
		case TagType:
		case AccountType: {
			rectType = AccountType == 2 ? 3 : 1;
			if(rectType == 1 && treeView && !treeView->isExpanded(index))
				rectType = 3;
			drawRect(backgroundPainter, a->backgroundColor);

			if(a->isGroupGradient)
				drawGradient(backgroundPainter, a->groupBackground, a->groupGradient, rect);
			else
				drawRect(backgroundPainter, a->groupBackground.isValid() ? a->groupBackground : painter->background().color());

			if (option.state & QStyle::State_Selected) {
				if(a->groupStyle.labelColor.isValid())
					drawRect(backgroundPainter, a->groupStyle.labelColor);
				else
					backgroundPainter->setBrush(selected);
			}
			font = a->isGrouptFontValid ? a->groupFont : option.font;
			fontColor = a->groupStyle.fontColor;
			break;
		}
		}

		alpha = getAlphaMask(painter, rect, rectType);

		drawRect(backgroundPainter,rect);
		backgroundPainter->end();
		backgroundPixmap.setAlphaChannel(alpha);
		painter->drawPixmap(option.rect,backgroundPixmap);
		option.rect.adjust(10, 0, -10, 0);
		painter->restore();
		break;
	}
	case QutimStyle: {
		painter->save();
		QRect rect = option.rect;

		SelectionTypes selection = Normal;
		if(option.state & QStyle::State_Selected)
			selection = Down;
		else if(option.state & QStyle::State_MouseOver)
			selection = Hover;
		int styleId = selection + (type == ContactType ? statusToID(status) : type);
		Q_ASSERT(q->styles.contains(styleId));
		StyleVar style = q->styles.value(styleId);

		fontColor = style.m_text_color;
		font = style.m_font;
		font.setBold(style.m_fontBold);
		font.setItalic(style.m_fontItalic);
		font.setOverline(style.m_fontOverline);
		font.setUnderline(style.m_fontUnderline);
		statusFont = q->status_font;
		statusColor = q->status_color[selection];
		QPen border_pen(style.m_borderColor);
		border_pen.setWidth(style.m_borderWidth);
		painter->setPen(border_pen);

		switch(style.m_backgroundType)
		{
			case 1:
			case 2:{
				QLinearGradient gradient;
				if(style.m_backgroundType==1)
					gradient = QLinearGradient(QPoint(0,rect.top()), QPoint(0,rect.bottom()));
				else
					gradient = QLinearGradient(QPoint(0, 0), QPoint(rect.width(), 0));
				for(int i = 0; i < style.m_stops.size(); ++i)
					gradient.setColorAt(style.m_stops[i], style.m_colors[i]);
				painter->setBrush(gradient);
				break;
			}
			case 0:
				painter->setBrush(style.m_background);
			break;
		}

		QRect backgroundRect = rect;
		backgroundRect.setBottomLeft(rect.bottomLeft() - QPoint(style.m_borderWidth + 1, style.m_borderWidth + 1 ) );
		drawRect(painter, backgroundRect);

		painter->restore();
		break;
	}
	default: {
		if (option.state & QStyle::State_Selected)
		{
			QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
			if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
				cg = QPalette::Inactive;
			fontColor = option.palette.color(cg, QPalette::HighlightedText);
		}
		QStyleOptionViewItemV4 opt(option2);
		QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
		style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
		statusColor = fontColor;
		break;
	}
	}


	// draw
	QFontMetrics fontMetrics(font);
	QRect rect = option.rect;
	rect.adjust(m_margin, m_margin, 0, 0);
	QRect status_rect = rect;
	int height = 0;

	QList<QPixmap> pixmaps;
	if (type == TagType && treeView) {
		bool isExpanded = treeView->isExpanded(index);
		Icon icon(isExpanded ? "expanded" : "collapsed");
		if (icon.pixmap(m_tagIconSize).isNull()) {
			QStyleOptionViewItemV2 branchOption;
			static const int i = 9; // ### hardcoded in qcommonstyle.cpp
			QRect r = option.rect;
			branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
			branchOption.palette = option.palette;
			branchOption.state = QStyle::State_Children;

			if (isExpanded)
				branchOption.state |= QStyle::State_Open;
			if (option.state & QStyle::State_MouseOver)
				 branchOption.state |= QStyle::State_MouseOver;

			getStyle(option)->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, widget);
			rect.adjust(i/2 + i + 2 * m_margin, 0, 0, 0);
		} else {
			pixmaps << icon.pixmap(m_tagIconSize, m_tagIconSize);
		}
	} else if (type == ContactType) {
		QPixmap statusPixmap = index.data(Qt::DecorationRole)
							   .value<QIcon>()
							   .pixmap(m_statusIconSize);
		if (!statusPixmap.isNull()) {
			status_rect.adjust(m_statusIconSize + m_margin, 0, 0, 0);
			pixmaps << statusPixmap;
		}
		if (m_showFlags & ShowAvatars) {
			QString avatar = index.data(AvatarRole).toString();
			if (!avatar.isEmpty()) {
				QPixmap pixmap = AvatarFilter::icon(avatar).pixmap(m_avatarSize);
				pixmaps << pixmap;
				height = pixmap.height();
			}
		}
	} else if (type == AccountType) {
		pixmaps << Icon("qutim").pixmap(m_accountIconSize);
	}
	foreach (const QPixmap &pixmap, pixmaps) {
		if (pixmap.isNull())
			continue;
		painter->drawPixmap(QRect(rect.topLeft(), pixmap.size()), pixmap);
		rect.adjust(pixmap.width() + m_margin, 0, 0, 0);
	}

	QRect title_rect = rect;
	if (m_showFlags & ShowExtendedInfoIcons) {
		QHash<QString, QVariantHash> extStatuses = status.extendedInfos();

		QList<QVariantHash> list;
		foreach (const QVariantHash &data, extStatuses) {
			QList<QVariantHash>::iterator search_it =
					qLowerBound(list.begin(), list.end(), data, contactInfoLessThan);
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
			QPixmap pixmap = icon.pixmap(m_extIconSize, QIcon::Normal, QIcon::On);
			if (!hash.value(showIcon,true).toBool() || pixmap.isNull())
				continue;
			if (!m_extInfo.value(hash.value(id).toString(), true))
				continue;
			title_rect.adjust(0, 0, -m_extIconSize - m_margin * 2, 0);
			painter->drawPixmap(QRect(title_rect.topRight(), QSize(m_extIconSize, m_extIconSize)), pixmap);
		}
		height = qMax(m_extIconSize, height);
	}

	title_rect.adjust(0, 0, -m_margin, 0);
	QString text = index.data(Qt::DisplayRole).toString();
	if (type == TagType) {
		QString count = index.data(ContactsCountRole).toString();
		QString online_count = index.data(OnlineContactsCountRole).toString();

		text = text % QLatin1Literal(" (")
				% online_count
				% QLatin1Char('/')
				% count
				% QLatin1Char(')');
	}
	QSize itemSize = QSize(title_rect.width(), fontMetrics.height());
	text = fontMetrics.elidedText(text, Qt::ElideRight, itemSize.width());
	if (fontMetrics.height() > height)
		height = fontMetrics.height();
	int deltaWidth = itemSize.width() - fontMetrics.width(text);
	if (type == SeparatorType && deltaWidth > 0) {
		itemSize.setWidth(itemSize.width() - deltaWidth);
		deltaWidth /= 2;
		rect.adjust(deltaWidth, 0, 0, 0);
	}
	painter->setFont(font);
	painter->setPen(fontColor);
	painter->drawText(title_rect, text);

	QString statusText = status.text();
	bool isStatusText = (m_showFlags & ShowStatusText) && !statusText.isEmpty();
	if (isStatusText) {
		painter->setFont(statusFont);
		painter->setPen(statusColor);
		fontMetrics = QFontMetrics(statusFont);
		status_rect.adjust(0, height + m_margin, 0, 0);
		statusText = statusText.remove(QLatin1Char('\n'));
		statusText = fontMetrics.elidedText(statusText, Qt::ElideRight, status_rect.width());
		painter->drawText(status_rect, Qt::AlignTop, statusText);
	}
	
	// done
	painter->restore();
}

QSize ContactListItemDelegate::sizeHint(const QStyleOptionViewItem &option2, const QModelIndex &index) const
{
	QStyleOptionViewItemV4 option(option2);
	ContactItemType type = static_cast<ContactItemType>(index.data(ItemTypeRole).toInt());
	Status status = index.data(StatusRole).value<Status>();

	QVariant value = index.data(Qt::SizeHintRole);
	if (value.isValid())
		return value.value<QSize>();
	QStyleOptionViewItem opt = option;

	QVariant font = index.data(Qt::FontRole);
	if (font.canConvert(QVariant::Font))
		opt.font = font.value<QFont>();
	QFont optFont = opt.font;
	if (m_styleType == AdiumStyle) {
		switch(type)
		{
		case InvalidType:
		case SeparatorType:
		case ContactType:
			if (a->isContactFontValid)
				opt.font = a->contactFont;
			break;
		case TagType:
		case AccountType:
			if (a->isGrouptFontValid)
				opt.font = a->groupFont;
			break;
		}
	} else if (m_styleType == QutimStyle) {
		int styleId = Normal + (type == ContactType ? statusToID(status) : type);
		Q_ASSERT(q->styles.contains(styleId));
		StyleVar style = q->styles.value(styleId);
		opt.font = style.m_font;
	}

	int height = size(opt, index, index.data(Qt::DisplayRole)).height();

	int iconSize = 0;
	if (type == ContactType)
		iconSize = m_statusIconSize;
	else if (type == TagType)
		iconSize = m_tagIconSize;
	else if (type == AccountType)
		iconSize = m_accountIconSize;
	height = qMax(height, iconSize);

	if (m_showFlags & ShowAvatars && QFileInfo(index.data(AvatarRole).toString()).exists())
		height = qMax(height, m_avatarSize);

	if (m_showFlags & ShowExtendedInfoIcons &&
		m_extIconSize > height &&
		!status.extendedInfos().isEmpty())
	{
		height = m_extIconSize;
	}

	if (m_showFlags & ShowStatusText && !status.text().isEmpty()) {
		QFontMetrics metrics = QFontMetrics(m_styleType == QutimStyle ? q->status_font : optFont);
		height += metrics.height();
	}

	height += 2 * m_margin;

	return QSize(opt.rect.width(), height);
}

QWidget *ContactListItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option);
	if (!index.isValid())
		return 0;
//	QVariant::Type t = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());
	return QItemEditorFactory::defaultFactory()->createEditor(QVariant::String, parent);
}

void ContactListItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	debug() << Q_FUNC_INFO;
	QAbstractItemDelegate::setEditorData(editor, index);
}

void ContactListItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	debug() << Q_FUNC_INFO;
	QAbstractItemDelegate::setModelData(editor, model, index);
}

void ContactListItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	debug() << Q_FUNC_INFO;
	QAbstractItemDelegate::updateEditorGeometry(editor, option, index);
}

QSize ContactListItemDelegate::size(const QStyleOptionViewItem &option, const QModelIndex &index, const QVariant &value) const
{
	static QSize maxsize(16,16);
	if (value.isValid() && !value.isNull()) {
		switch (value.type())
		{
		case QVariant::Invalid:
			break;
		case QVariant::Pixmap:
			return qvariant_cast<QPixmap>(value).size();
		case QVariant::Image:
			return qvariant_cast<QImage>(value).size();
		case QVariant::Icon:{
			//QIcon::Mode mode = QIcon::Normal;//d->iconMode(option.state);
			//QIcon::State state = QIcon::On;//option.state&QStyle::State_On==QStyle::State_On?QIcon::On:QIcon::Off;
			QIcon icon = qvariant_cast<QIcon>(value);
			QSize size = icon.actualSize(maxsize);//, mode, state);
			return size;
		}
		case QVariant::Color:
			return option.decorationSize;
		case QVariant::List: {
			int maxh=0;
			int width=0;
			foreach(QVariant item, value.toList())
			{
				QSize item_size = size(option, index, item);
				width+=item_size.width();
				if(item_size.height()>maxh)
				maxh=item_size.height();
			}
			return QSize(width,maxh);
		}
		case QVariant::String: {
			QFont fnt = option.font;
			QFontMetrics fm(fnt);
			return QSize(fm.width(value.toString()),fm.height());
		}
		default:
			break;
		}
	}
	return QSize(0,0);
}
void ContactListItemDelegate::drawGradient(QPainter *painter, const QColor &up, const QColor &down, const QRect &rect/*, int type*/) const
{
	painter->setPen(QColor(255,255,255,0));
	QLinearGradient gradient(QPoint(0,0), QPoint(0,rect.height()));
	gradient.setColorAt(0, up);
	gradient.setColorAt(1, down);
	painter->setBrush(gradient);
}

void ContactListItemDelegate::drawRect(QPainter *painter, const QColor & color/*, const QRect & rect, int type*/) const
{
	if (!m_styleType) {
        QColor color2 = color;
        color2.setAlpha(50);
        painter->setBrush(color2);
    }
    else
        painter->setBrush(color);
	//QPen pen = painter->pen();
	//QBrush brush = painter->brush();
	painter->setPen(QColor(255,255,255,0));
	//drawRect(painter,rect,type);
	//painter->setPen(pen);
	//painter->setBrush(brush);
}

void ContactListItemDelegate::drawRect(QPainter *painter, QRect rect) const
{
//	int x1=rect.left(),y1=rect.top(),x2=rect.width(),y2=rect.height();
	painter->drawRect(rect);
/*	QPixmap buffer(rect.size());
	buffer.fill(QColor(255,255,255,0));
	QPainter buffer_painter(&buffer);
	//buffer_painter.setRenderHints(painter->renderHints());
	buffer_painter.setBrush(painter->brush());
	buffer_painter.setPen(painter->pen());
	buffer_painter.drawRect(QRect(QPoint(0,0),QSize(x2-1,y2)));
	buffer_painter.end();
	buffer.setAlphaChannel(alpha);
	painter->drawPixmap(rect,buffer);*/
}

QPixmap ContactListItemDelegate::getAlphaMask(QPainter *painter, QRect rect, int type) const
{
	Q_UNUSED(painter);
	int /*x1 = rect.left(), */y1 = rect.top(), x2 = rect.width(),y2 = rect.height();
	QPixmap alpha_buffer(rect.size());
	alpha_buffer.fill(QColor(0,0,0));
	QPainter alpha_painter(&alpha_buffer);
	alpha_painter.setRenderHints(QPainter::Antialiasing);
	alpha_painter.setBrush(QColor(255,255,255));
	alpha_painter.setPen(QColor(255,255,255));
	if(type!=0)
	alpha_painter.drawRoundedRect(QRect(QPoint(0,0),QSize(x2-1,y2-1)),10,50);
	switch(type)
	{
	case 0: //draw rectangle
		y1 = 0;
		break;
	case 1: //draw rectangle rounded at top
		y2 /= 2;
		y1 = y2;
		break;
	case 2: //draw rectangle rounded at bottom
		y2 /= 2;
		y1 = 0;
		break;
	default: //draw rounded rectangle
		break;
	}
	if(type < 3)
		alpha_painter.drawRect(QRect(QPoint(0,y1),QSize(x2,y2)));
	alpha_painter.end();
	return alpha_buffer;
	
}

void ContactListItemDelegate::draw(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QVariant &value) const
{
	if (value.isValid() && !value.isNull()) {
	switch (value.type()) {
	case QVariant::Invalid:
		break;
	case QVariant::Pixmap:
		painter->drawPixmap(rect,qvariant_cast<QPixmap>(value));
		break;
	case QVariant::Image:
		painter->drawImage(rect,qvariant_cast<QImage>(value));
		break;
	case QVariant::Icon: {
		//QIcon::Mode mode = QIcon::Normal;//d->iconMode(option.state);
		//QIcon::State state = QIcon::On;//option.state&QStyle::State_On==QStyle::State_On?QIcon::On:QIcon::Off;
		QIcon icon = qvariant_cast<QIcon>(value);
		painter->drawPixmap(rect, icon.pixmap(icon.actualSize(QSize(16,16)), QIcon::Normal, QIcon::On));
		break;
	}
	case QVariant::String:{
		QFont fnt = option.font;
		QFontMetrics font_metrics(fnt);
		painter->drawText(rect, font_metrics.elidedText(value.toString(), Qt::ElideRight,rect.width()));
		break;
	}
	default:
		break;
	}
	}
}

bool ContactListItemDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view,
										const QStyleOptionViewItem &option,
										const QModelIndex &index)
{
	if (event->type() == QEvent::ToolTip) {
#ifdef Q_WS_MAEMO_5
		return true;
#else
		Buddy *buddy = index.data(BuddyRole).value<Buddy*>();
		if (buddy)
			ToolTip::instance()->showText(event->globalPos(), buddy, view);
		return true;
#endif
	} else {
		return QAbstractItemDelegate::helpEvent(event, view, option, index);
	}
}

QMap<QString,QVariant> ContactListItemDelegate::appendStyleFile(QString path)
{
	if (QFileInfo(path).isDir())
		path += "/Contents/Resources/Data.plist";

	QFile themeFile(path);
	if (themeFile.exists() && themeFile.open(QIODevice::ReadOnly)) {
		QDomDocument themeDom;
		if (themeDom.setContent(&themeFile)) {
			QDomElement rootElement = themeDom.documentElement();
			if (!rootElement.isNull())
				return parse(rootElement.firstChild());
		}
	}

	return QMap<QString, QVariant>();
}

QMap<QString,QVariant> ContactListItemDelegate::parse(const QDomNode &rootElement)
{
	QMap<QString,QVariant> styleHash;
	if (rootElement.isNull())
		return styleHash;
	QDomNode subelement = rootElement;
	QString key="";
	for (QDomNode node = subelement.firstChild(); !node.isNull(); node = node.nextSibling()) {
		QDomElement element = node.toElement();
		if (element.nodeName() == "key") {
			key = element.text();
		} else {
			QVariant value;
			if (element.nodeName() == "true")
				value = QVariant(true);
			else if (element.nodeName() == "false")
				value = QVariant(false);
			else if (element.nodeName() == "real")
				value = QVariant(element.text().toDouble());
			else if (element.nodeName() == "string") {
				QString text = element.text();
				if(key.indexOf("Font",Qt::CaseInsensitive)>-1)
					value = getFont(text);
				else if(key.indexOf("Color", Qt::CaseInsensitive) >-1 || key.endsWith("Gradient") || key.endsWith("Background"))
					value = getColor(text);
				else
					value = QVariant(text);
			}
			else if (element.nodeName() == "integer") {
				value = QVariant(element.text().toInt());
			} else if (element.nodeName() == "dict") {
				value = parse(node);
			}
			styleHash.insert(key,value);
		}
	}
	return styleHash;
}

bool ContactListItemDelegate::setThemeStyle(const QString &name)
{
	if (m_styleType == AdiumStyle)
		delete a;
	else if (m_styleType == QutimStyle)
		delete q;

	m_styleType	= LightStyle;

	if(name.isEmpty())
		return false;

	QString path = getThemePath(name);
	QFileInfo dir(path);
	if(!dir.exists())
		return false;
	QMap<QString,QVariant> styleHash = appendStyleFile(path);
	m_styleHash = styleHash;
	if(styleHash.isEmpty())
		return false;
	if (path.endsWith(".ListTheme")) {
		m_styleType = AdiumStyle;
		a = new AdiumStyleData;
		a->backgroundColor = styleHash["Background Color"].value<QColor>();
		a->backgroundColor.setAlpha(0);

		m_css = "QAbstractItemView { ";
		m_css.append(QString("background-color: rgba(%1, %2, %3, %4);").
					   arg(a->backgroundColor.red()).
					   arg(a->backgroundColor.green()).
					   arg(a->backgroundColor.blue()).
					   arg(a->backgroundColor.alpha()));
		m_css+=" }";

		//if (styleHash.value("Grid Enabled",false).toBool())
		//    gridColor = styleHash["Grid Color"].value<QColor>();

		a->isContactFontValid = styleHash.contains("Contact Font");
		if (a->isContactFontValid)
			a->contactFont = styleHash["Contact Font"].value<QFont>();

		if (styleHash.value("Online Enabled",false).toBool()) {
			  a->onlineStyle.fontColor = styleHash["Online Color"].value<QColor>();
			  a->onlineStyle.labelColor = styleHash["Online Label Color"].value<QColor>();
		} else {
			  a->onlineStyle.fontColor = styleHash["Contact Text Color"].value<QColor>();
			  a->onlineStyle.labelColor = a->backgroundColor;
		}

		if (styleHash.value("Offline Enabled",false).toBool()) {
			  a->offlineStyle.fontColor = styleHash["Offline Color"].value<QColor>();
			  a->offlineStyle.labelColor = styleHash["Offline Label Color"].value<QColor>();
		} else {
			  a->offlineStyle.fontColor = styleHash["Contact Text Color"].value<QColor>();
			  a->offlineStyle.labelColor = a->backgroundColor;
		}

		if (styleHash.value("Away Enabled",false).toBool()) {
			  a->awayStyle.fontColor = styleHash["Away Color"].value<QColor>();
			  a->awayStyle.labelColor = styleHash["Away Label Color"].value<QColor>();
		} else {
			  a->awayStyle.fontColor = styleHash["Contact Text Color"].value<QColor>();
			  a->awayStyle.labelColor = a->backgroundColor;
		}

		a->isGroupGradient = styleHash.value("Group Gradient",false).toBool();
		a->groupBackground = styleHash["Group Background"].value<QColor>();
		if (a->isGroupGradient)
			  a->groupGradient = styleHash["Group Background Gradient"].value<QColor>();
		a->groupStyle.labelColor = styleHash["Group Label Color"].value<QColor>();
		a->groupStyle.fontColor = styleHash["Group Text Color"].value<QColor>();
		a->isGrouptFontValid = styleHash.contains("Group Font");
		if (a->isGrouptFontValid)
			a->groupFont = styleHash["Group Font"].value<QFont>();

		// It's important that the calls stay after theme loading
		emit styleSheetChanged(m_css);
		QAbstractItemView *view = getContactListView();
		if (view)
			view->setStyleSheet(m_css);

		return true;
	} else {
		m_styleType = QutimStyle;
		q = new QutimStyleData;

		QString theme_path;
		if(dir.isFile()) {
			theme_path = dir.absolutePath();
		} else {
			QDir theme_dir(path);
			theme_dir.cdUp();
			theme_path = theme_dir.absolutePath();
		}

		m_css = "QAbstractItemView { ";
		int full_background_type = styleHash.value("Background Type", 0).toInt();
		if (styleHash.contains("Gradients") && (full_background_type==1 || full_background_type==2)) {
			QString gradient = QString("background: qlineargradient(x1:0, y1:0, x2:%1, y2:%2")
							   .arg(full_background_type == 1 ? 0 : 1)
							   .arg(full_background_type == 1 ? 1 : 0);
			QMap<QString,QVariant> gradients = styleHash.value("Gradients").toMap();
			int n=1;
			while(gradients.contains("Color " + QString::number(n)) && gradients.contains("Stop " + QString::number(n))) {
				QColor color = qvariant_cast<QColor>(gradients.value("Color "+QString::number(n)));
				gradient.append(QString(", stop: %1 rgba(%2, %3, %4, %5)")
								.arg(gradients.value("Stop "+QString::number(n)).toDouble())
								.arg(color.red())
								.arg(color.green())
								.arg(color.blue())
								.arg(color.alpha()));
				n++;
			}
			gradient.append(");");
			m_css.append(gradient);
		} else if (styleHash.contains("Background")) {
			QColor background = qvariant_cast<QColor>(styleHash.value("Background"));
			m_css.append(QString("background-color: rgba(%1, %2, %3, %4);")
						 .arg(background.red())
						 .arg(background.green())
						 .arg(background.blue())
						 .arg(background.alpha()));
		}

		if (styleHash.contains("Alternate Background")) {
			QColor background = qvariant_cast<QColor>(styleHash.value("Alternate Background"));
			m_css.append(QString("alternate-background-color: rgba(%1, %2, %3, %4);")
						 .arg(background.red())
						 .arg(background.green())
						 .arg(background.blue())
						 .arg(background.alpha()));
		}

		//int background_picture_type = style_hash.value("Background Picture Type",0).toInt();
		if(styleHash.contains("Background Image")) {
			m_css.append(QString("background-image: url(\"%1\");")
						 .arg(theme_path+"/"+styleHash.value("Background Image").toString()));
			if(styleHash.contains("Background Repeat"))
				m_css.append(QString("background-repeat: repeat-%1;")
							 .arg(styleHash.value("Background Repeat").toString().toLower()));
			if(styleHash.contains("Background Position"))
				m_css.append(QString("background-repeat: position-%1;")
							 .arg(styleHash.value("Background Position").toString().toLower()));
			if(styleHash.contains("Background Attachment")) {
				if(styleHash.value("Background Attachment")=="Fixed")
					m_css.append(QString("background-attachment: fixed;"));
			}
		}
		m_css.append(" }");

		static QStringList types = QStringList()
			<< "Separator"
			<< "Group"
			<< "Account"
			<< "Contact"
			<< "Online"
			<< "FreeForChat"
			<< "AtHome"
			<< "AtWork"
			<< "Lunch"
			<< "Evil"
			<< "Depression"
			<< "Offline"
			<< "Away"
			<< "NA"
			<< "DND"
			<< "Occupied"
			<< "Invisible";
		static int short_types[] =
		{
			SeparatorType,
			TagType,
			AccountType,
			ContactType,

			Status::Online,
			Status::FreeChat,
			OscarHome,
			OscarWork,
			OscarLunch,
			OscarEvil,
			OscarDepress,
			Status::Offline,
			Status::Away,
			Status::NA,
			Status::DND,
			OscarOccupied,
			Status::Invisible
		};

		static QStringList selection_types = QStringList() << "Normal" << "Hover" << "Down";
		static int selection_short_types [] =  { Normal, Hover, Down };

		QFont systemFont = QApplication::font();
		QColor systemColor = QApplication::palette().text().color();

		QColor normalColor = styleHash.value("Status Text Color", systemColor).value<QColor>();
		StyleVar normalStyle;
		normalStyle.m_backgroundType = 0;
		normalStyle.m_background = styleHash.value("Background", QColor(0, 0, 0, 0)).value<QColor>();
		normalStyle.m_font = qvariant_cast<QFont>(styleHash.value("Font", systemFont));
		normalStyle.m_fontBold = styleHash.value("Font Bold", false).toBool();
		normalStyle.m_fontItalic = styleHash.value("Font Italic", false).toBool();
		normalStyle.m_fontOverline = styleHash.value("Font Overline", false).toBool();
		normalStyle.m_fontUnderline = styleHash.value("Font Underline", false).toBool();
		normalStyle.m_borderWidth = 0;
		normalStyle.m_borderColor = QColor(0, 0, 0, 0);
		normalStyle.m_text_color = styleHash.value("Text Color",systemColor).value<QColor>();
		q->styles[Normal] = normalStyle;
		q->status_font = styleHash.value("Status Font", systemFont).value<QFont>();
		q->status_color[Normal] = normalColor;

		for (int i = 1; i < selection_types.size(); ++i) {
			QString colorString = selection_types[i] + "Status Text Color";
			q->status_color[selection_short_types[i]] = styleHash.value(colorString, normalColor).value<QColor>();
		}

		for(int i = 1; i < selection_types.size(); ++i) {
			QString currentSelection = selection_types[i];
			StyleVar selectionStyle = normalStyle;
			if(styleHash.contains(currentSelection + "Background"))
				selectionStyle.m_background = styleHash.value(currentSelection + "Background").value<QColor>();
			if(styleHash.contains(currentSelection + "Font"))
				selectionStyle.m_font = styleHash.value(currentSelection + "Font", systemFont).value<QFont>();
			if(styleHash.contains(currentSelection + "Text Color"))
				selectionStyle.m_text_color = styleHash.value(currentSelection + "Text Color", systemColor).value<QColor>();
			q->styles[selection_short_types[i]] = selectionStyle;
		}

		QMap<QString,QVariant> root = m_styleHash;
		for (int i = 0; i < types.size(); i++) {
			StyleVar current = normalStyle;
			if (root.contains(types[i])) {
				QMap<QString,QVariant> item = root.value(types[i]).toMap();
				if (item.contains("Font"))
					current.m_font = item.value("Font").value<QFont>();
				if (item.contains("Font Bold"))
					current.m_fontBold = item.value("Font Bold").toBool();
				if (item.contains("Font Italic"))
					current.m_fontItalic = item.value("Font Italic").toBool();
				if (item.contains("Font Overline"))
					current.m_fontOverline = item.value("Font Overline").toBool();
				if (item.contains("Font Underline"))
					current.m_fontUnderline = item.value("Font Underline").toBool();
				q->styles[Normal + short_types[i]] = current;

				for (int j = 0; j < selection_types.size(); j++) {
					StyleVar selectionStyle = i > 3 ?
											  q->styles[selection_short_types[j] + ContactType] :
											  q->styles[Normal + short_types[i]];
					if (item.contains(selection_types[j])) {
						QMap<QString,QVariant> selection = item.value(selection_types[j]).toMap();
						if (selection.contains("Text Color"))
							selectionStyle.m_text_color = selection.value("Text Color").value<QColor>();
						if (selection.contains("Background Type"))
							selectionStyle.m_backgroundType = selection.value("Background Type").toInt();
						int backgroundType = selectionStyle.m_backgroundType;
						switch (backgroundType) {
							case 1:
							case 2: {
								selectionStyle.m_stops.clear();
								selectionStyle.m_colors.clear();
								if (selection.contains("Gradients")) {
									QMap<QString,QVariant> gradients = selection.value("Gradients").toMap();
									int n = 1;
									forever {
										QString nstr = QString::number(n);
										if (!gradients.contains("Color " + nstr) ||
											!gradients.contains("Stop " + nstr))
										{
											break;
										}
										selectionStyle.m_stops.append(gradients.value("Stop "+ nstr).toDouble());
										selectionStyle.m_colors.append(gradients.value("Color " + nstr).value<QColor>());
										n++;
									}
									n--;
									break;
								}
							}
							default:{
								selectionStyle.m_backgroundType = 0;
								if(selection.contains("Background"))
									selectionStyle.m_background = selection.value("Background").value<QColor>();
								break;
							}
						}
						if (selection.contains("Border Width"))
							selectionStyle.m_borderWidth = selection.value("Border Width").toInt();
						if (selection.contains("Border Color"))
							selectionStyle.m_borderColor = selection.value("Border Color").value<QColor>();
					}
					q->styles[selection_short_types[j] + short_types[i]] = selectionStyle;
				}

				if (i == 3) {
					root = item;
					for(int j = 0; j < selection_types.size(); ++j)
						q->styles[selection_short_types[j]] = q->styles[selection_short_types[j] + short_types[i]];
				}
			} else {
				for (int j = 0; j < selection_types.size(); ++j) {
					StyleVar style = i > 3 ?
									 q->styles[selection_short_types[j] + ContactType] :
									 current;
					q->styles[selection_short_types[j] + short_types[i]] = style;
				}
			}
		}

		// It's important that the calls stay after theme loading
		QAbstractItemView *view = getContactListView();
		if (view) {
			view->setStyleSheet(m_css);
			if (styleHash.contains("ScrollBar Policy")) {
				switch (styleHash.value("ScrollBar Policy",0).toInt())
				{
				case 1:
					view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
					break;
				case 2:
					view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
					break;
				default:
					view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
					break;
				}
			}
		}
		emit styleSheetChanged(m_css);

		return true;
	}
}

QString ContactListItemDelegate::getThemePath(const QString &themeName)
{
	foreach (QDir dir, ThemeManager::categoryDirs("olddelegate")) {
		QFileInfo file(dir.filePath(themeName));
		if (file.exists())
			return file.absoluteFilePath();
	}
	return QString();
}

QVariant ContactListItemDelegate::getValue(const QString &name, const QVariant &def) const
{
	QVariant var = m_styleHash.value(name);
	if (var.isValid())
		return var;
	return def;
	
}

QColor ContactListItemDelegate::getColor(QVariant var) const
{
	QStringList list = var.toString().split(",");
	if(list.size() > 3)
		return QColor(list[0].toInt(), list[1].toInt(), list[2].toInt(), list[3].toInt());
	else if(list.size() == 3)
		return QColor(list[0].toInt(), list[1].toInt(), list[2].toInt());
	else
		return QColor(var.toString());
}

QFont ContactListItemDelegate::getFont(QVariant var) const
{
	QStringList list = var.toString().split(",");
	if(list.size() < 2)
		return QFont(var.toString(), 12);
	QFont font(list[0]);
	font.setPixelSize(list[1].toInt());
	return font;
}

const QWidget *ContactListItemDelegate::getWidget(const QStyleOptionViewItem &option)
{
	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
		return v3->widget;

	return 0;
}

QStyle *ContactListItemDelegate::getStyle(const QStyleOptionViewItem &option)
{
	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
	return v3->widget ? v3->widget->style() : QApplication::style();

	return QApplication::style();
}

int ContactListItemDelegate::statusToID(const Status &status)
{
	switch (status.type()) {
	case Status::Online:
		switch (status.subtype()) {
		case OscarEvil:
		case OscarDepress:
		case OscarHome:
		case OscarWork:
			return status.subtype();
		default:
			return status.type();
		}
	case Status::Away:
		if (status.subtype() == OscarLunch)
			return OscarLunch;
		return status.type();
	case Status::DND:
		if (status.subtype() == OscarOccupied)
			return OscarOccupied;
		return status.type();
	case Status::Connecting:
		return Status::Offline;
	case Status::FreeChat:
	case Status::NA:
	case Status::Invisible:
	case Status::Offline:
		return status.type();
	}
	return 0;
}

void ContactListItemDelegate::reloadSettings()
{
	Config cfg("appearance");
	cfg = cfg.group("contactList");

	int smallIconSize = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize);
#ifdef QUTIM_MOBILE_UI
	m_statusIconSize = cfg.value("statusIconSize",
								 qApp->style()->pixelMetric(QStyle::PM_ListViewIconSize));
#else
	m_statusIconSize = cfg.value("statusIconSize", smallIconSize);
#endif
	m_extIconSize = cfg.value("extIconSize", smallIconSize);
	m_avatarSize = cfg.value("avatarIconSize", smallIconSize);
	m_accountIconSize = cfg.value("accountIconSize", smallIconSize);
	m_tagIconSize = cfg.value("tagIconSize", smallIconSize);

	setFlag(ShowStatusText, cfg.value("showStatusText", true));
	setFlag(ShowExtendedInfoIcons, cfg.value("showExtendedInfoIcons", true));
	setFlag(ShowAvatars, cfg.value("showAvatars", true));
	// Load extended statuses.
	cfg.beginGroup("extendedStatuses");
	m_extInfo.clear();
	foreach (const QString &name, cfg.childKeys())
		m_extInfo.insert(name, cfg.value(name, true));
	cfg.endGroup();

	setThemeStyle(cfg.value("oldDelegateTheme", "default.ListQutim"));
	emit sizeHintChanged(QModelIndex());
}

QAbstractItemView *ContactListItemDelegate::getContactListView()
{
	QWidget *widget = ServiceManager::getByName<QWidget*>("ContactListWidget");
	return widget ? widget->findChild<QAbstractItemView*>() : 0;
}

void ContactListItemDelegate::setFlag(ShowFlags flag, bool on)
{
	if (on)
		m_showFlags |= flag;
	else
		m_showFlags &= ~flag;
}

QUTIM_EXPORT_PLUGIN(OldContactDelegatePlugin)

