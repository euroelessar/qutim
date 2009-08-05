/*****************************************************************************
    Contact List Item Delegate

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "contactlistitemdelegate.h"
#include "proxymodelitem.h"
#include <QDir>
#include <QFile>
#include <QScrollBar>
#include <QBrush>
#include <QPalette>
#include <QBitmap>
#include <QLinearGradient>
#include <QItemEditorFactory>
#include "src/qutim.h"
#include "src/abstractlayer.h"

ContactListItemDelegate::ContactListItemDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{
	m_margin=1;
	m_margin_x = QPoint(m_margin,0);
	m_margin_y = QPoint(0,m_margin);
	m_style_type = 2;
	m_show_icons = QVector<bool>(13).toList();
	for (int i = 0; i < 13; ++i)
		m_show_icons[i]=true;
	/*m_style.m_booleans=QVector<bool>(10);
	m_style.m_colors=QVector<QColor>(10);
	m_style.m_fonts=QVector<QFont>(10);
	m_style.m_integers=QVector<int>(10);
	m_style.m_pixmaps=QVector<QPixmap>(10);
	m_style.m_reals=QVector<double>(10);*/
}

ContactListItemDelegate::~ContactListItemDelegate()
{
}

void ContactListItemDelegate::setSettings(QList<bool> show_icons, double opacity)
{
	m_show_icons = show_icons;
	m_opacity = opacity;
}
void ContactListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option2, const QModelIndex &index) const
{
	ProxyModelItem *item = static_cast<ProxyModelItem*>(index.internalPointer());
	QStyleOptionViewItem option = option2;

	QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
							  ? QPalette::Normal : QPalette::Disabled;
	if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
		cg = QPalette::Inactive;

	QBrush selected = option.palette.brush(cg, QPalette::Highlight);  
	
	
	int type = index.data(Qt::UserRole).toInt();
	QFont font=option.font;
	QVariant var_font = index.data(Qt::FontRole);
	if(var_font.type() == QVariant::Font)
		font=qvariant_cast<QFont>(var_font);
	QColor font_color=painter->pen().color();
	QVariant var_color = index.data(Qt::UserRole+10);
	if(var_color.type() == QVariant::Color)
		font_color=qvariant_cast<QColor>(var_color);
	QFont status_font = font;;
	QColor status_color = font_color;
	int x1,x2,y1,y2;
	option.rect.getCoords(&x1,&y1,&x2,&y2);
	option.rect.setCoords(x1,y1,x1+m_tree_view->maximumViewportSize().width()-(m_tree_view->verticalScrollBar()->isVisible()?m_tree_view->verticalScrollBar()->frameSize().width():0),y2);
	// prepare
	painter->save();
	switch(m_style_type)
	{
        case 0:{
//		painter->setOpacity(m_opacity);
                QPixmap background_pixmap(option.rect.size());
                background_pixmap.fill(QColor::fromRgb(0,0,0,0));
                QPainter *background_painter = new QPainter(&background_pixmap);
                // Choose colors and fonts, based on type of item
                QPen pen = painter->pen();
                QBrush brush = painter->brush();
                QPixmap alpha;
                int rect_type=0;
                QRect rect=QRect(QPoint(0,0),option.rect.size());
                background_painter->setPen(QColor(255,255,255,0));
                background_painter->setBrush(QBrush(QColor(255,255,255,0)));
                switch(type)
                {
                case 0:{
                        if(item->childNumber()==item->parent()->childCount()-1)
                                rect_type=2;
                        QString type = index.data(Qt::UserRole+3).toString();
                        alpha = getAlphaMask(painter,rect,rect_type);
                        font = m_style_fonts.value("contact",font);
                        font_color = m_style_colors.value(type+"font",font_color);
                        if (option.state & QStyle::State_Selected) {
                                if(m_style_colors.contains(type+"label"))
                                {
                                        QColor background = m_style_colors.value(type+"label");
                                        drawRect(background_painter,background);
                                }
                                else
                                        background_painter->setBrush(selected);
                        }
                        else
                                drawRect(background_painter,m_style_colors.value("background",painter->background().color()));
                        break;
                }
                case -1:
                        alpha = getAlphaMask(painter,rect,rect_type);
                        drawRect(background_painter,m_style_colors.value("background",painter->background().color()));
                        font = m_style_fonts.value("separator",font);
                        font_color = m_style_colors.value("separatorfont",font_color);
                        if (option.state & QStyle::State_Selected) {
                                if(m_style_colors.contains(type+"label"))
                                {
                                        QColor background = m_style_colors.value("separatorlabel");
                                        drawRect(background_painter,background);
                                }
                                else
                                        background_painter->setBrush(selected);
                        }
                        break;
                case 1:
                case 2:
                        rect_type = type==2?3:1;
                        if(rect_type==1 && !static_cast<QTreeView *>(m_tree_view)->isExpanded(index))
                                rect_type=3;
                        alpha = getAlphaMask(painter,rect,rect_type);
                        drawRect(background_painter,m_style_colors.value("background",painter->background().color()));

                        if(m_style_bools.value("groupgradient",false))
                        {
                                drawGradient(background_painter, m_style_colors["groupup"],m_style_colors["groupdown"],rect);
                        }
                        else
                                drawRect(background_painter,m_style_colors.value("groupbackground",painter->background().color()));

                        if (option.state & QStyle::State_Selected) {
                                if(m_style_colors.contains(type+"label"))
                                {
                                        QColor background = m_style_colors.value("grouplabel");
                                        drawRect(background_painter,background);
                                }
                                else
                                        background_painter->setBrush(selected);
                        }
                        font = m_style_fonts.value("group",option.font);
                        font_color = m_style_colors.value("groupfont",font_color);
                        break;
                }
                drawRect(background_painter,rect);
                background_painter->end();
                background_pixmap.setAlphaChannel(alpha);
                painter->drawPixmap(option.rect,background_pixmap);
                painter->setPen(pen);
                painter->setBrush(brush);

                painter->setPen(font_color);
                painter->setFont(font);
                painter->setOpacity(1);
                option.rect.getCoords(&x1,&y1,&x2,&y2);
                option.rect.setCoords(x1+10,y1,x2-10,y2);
		break;
	}
	case 1:{
		QPen pen = painter->pen();
		QBrush brush = painter->brush();
		int rect_type=0;
		QRect rect = option.rect;
		QString selection = "normal";
		if(option.state & QStyle::State_Selected)
			selection = "down";
		else if(option.state & QStyle::State_MouseOver)
			selection = "hover";
		StyleVar *style_tmp;
		switch(type)
		{
		case 0:{
			QString type = index.data(Qt::UserRole+3).toString();
			style_tmp = &(*const_cast<QHash<QString,StyleVar> *>(&m_styles))[selection+type];
			break;
		}
		case -1:
			style_tmp = &(*const_cast<QHash<QString,StyleVar> *>(&m_styles))[selection+"separator"];
			break;
		case 1:
			style_tmp = &(*const_cast<QHash<QString,StyleVar> *>(&m_styles))[selection+"group"];
			break;
		case 2:
			style_tmp = &(*const_cast<QHash<QString,StyleVar> *>(&m_styles))[selection+"account"];
			break;
		default:
			painter->restore();
			return;
		}
		StyleVar &style = *style_tmp;
		font_color = style.m_text_color;
		font = style.m_font;
		font.setBold(style.m_font_bold);
		font.setItalic(style.m_font_italic);
		font.setOverline(style.m_font_overline);
		font.setUnderline(style.m_font_underline);
		status_font = m_status_font;
		status_color = m_status_color[selection];
		QPen border_pen(style.m_border_color);
		border_pen.setWidth(style.m_border_width);
		painter->setPen(border_pen);
		switch(style.m_background_type)
		{
			case 1:
			case 2:{
				QLinearGradient gradient;
				if(style.m_background_type==1)
					gradient = QLinearGradient(QPoint(0,rect.top()),QPoint(0,rect.bottom()));
				else
					gradient = QLinearGradient(QPoint(0,0),QPoint(rect.width(),0));
				for(int i=0;i<style.m_stops.size();i++)
					gradient.setColorAt(style.m_stops[i],style.m_colors[i]);
				painter->setBrush(gradient);
				break;
			}
			case 0:
				painter->setBrush(style.m_background);
				break;
		}
		QRect background_rect = rect;
		background_rect.setBottomLeft( rect.bottomLeft() - QPoint( style.m_border_width + 1, style.m_border_width + 1 ) );
		drawRect(painter, background_rect);
		
		painter->setBrush(brush);		
		painter->setPen(font_color);
		painter->setFont(font);
//		option.rect.getCoords(&x1,&y1,&x2,&y2);
//		option.rect.setCoords(x1+5,y1,x2-5,y2);
		break;
	}
	default:{
		if (option.state & QStyle::State_Selected)
		{
			QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
			if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
				cg = QPalette::Inactive;
			font_color = option.palette.color(cg, QPalette::HighlightedText);
		}
		QStyleOptionViewItemV4 opt(option2);
		QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
		style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
		status_color = font_color;
		break;}
	}
	QFontMetrics font_metrics(font);
	//qWarning() << font;
	
	// draw selection
/*	if (option.showDecorationSelected && (option.state & QStyle::State_Selected)) {
		QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
								  ? QPalette::Normal : QPalette::Disabled;
		if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
			cg = QPalette::Inactive;

		painter->fillRect(option.rect, option.palette.brush(cg, QPalette::Highlight));
	}*/
	
	// draw

	QPoint point(option.rect.left(),option.rect.top());
	point+=m_margin_x;
	point+=m_margin_y;
	int height=0;
	QList<QVariant> &list = *reinterpret_cast<QList<QVariant> *>(index.data(Qt::DecorationRole).value<qptrdiff>());
	switch (type) {
	case 1:
		if(static_cast<QTreeView *>(m_tree_view)->isExpanded(index))
			list[0]=IconManager::instance().getIcon("expanded");
		else
			list[0]=IconManager::instance().getIcon("collapsed");
		break;
			default:
		break;
	}
	if(m_show_icons[0])
	{
		QVariant icon_variant = index.data(Qt::UserRole+4);
		if( (icon_variant.isValid() && icon_variant.type()==QVariant::Icon) || type==1)
		{
			QIcon icon = qvariant_cast<QIcon>(type==1?list[0]:icon_variant);
			QSize icon_size = size(option, index, type==1?list[0]:icon_variant);
			painter->drawPixmap(QRect(point, icon_size),icon.pixmap(icon_size,QIcon::Normal,QIcon::On));
			point+=QPoint(icon_size.width(),0);
			point+=m_margin_x;
			if(icon_size.height()>height)
				height = icon_size.height();
		}
	}
	for(int i=1;i<3;i++)
	{
		if(m_show_icons[i])
		{
			if(list[i].isValid()&&list[i].type()==QVariant::Icon)
			{
				QIcon icon = qvariant_cast<QIcon>(list[i]);
				if(icon.isNull())
					continue;
				QSize icon_size =  i == 1 ? icon.actualSize(QSize(65535,65535),QIcon::Normal,QIcon::On) : size(option, index, list[i]);
				if(icon_size.width() <= 0)
					continue;
				painter->drawPixmap(QRect(point, icon_size),icon.pixmap(icon_size,QIcon::Normal,QIcon::On));
				point+=QPoint(icon_size.width(),0);
				point+=m_margin_x;
				if(icon_size.height()>height)
					height = icon_size.height();
			}
		}
	}
	QPoint point_r(option.rect.right(),option.rect.top());
	for(int i=12;i>2;i--)
	{
		if(m_show_icons[i])
		if(list[i].isValid()&&list[i].type()==QVariant::Icon)
		{
			QIcon icon = qvariant_cast<QIcon>(list[i]);
			if(icon.isNull())
				continue;
			QSize icon_size = size(option, index, list[i]);
			if(icon_size.width() <= 0)
				continue;
			point_r-=QPoint(icon_size.width(),0);
			point_r-=m_margin_x;
			painter->drawPixmap(QRect(point_r, icon_size),icon.pixmap(icon_size,QIcon::Normal,QIcon::On));
			if(icon_size.height()>height)
				height = icon_size.height(); 
		}
	}
	point_r-=m_margin_x;
	QString text=index.data(Qt::DisplayRole).toString();
	QSize item_size=QSize(point_r.x()-point.x(),font_metrics.height());
	if(font_metrics.height()>height)
		height = font_metrics.height();
	int delta_width = item_size.width() - font_metrics.width(text); 
	if(type==-1 && delta_width>0)
	{
		item_size.setWidth(item_size.width()-delta_width);
		delta_width/=2;
		point+=QPoint(delta_width,0);
	}
	painter->setFont(font);
	painter->setPen(font_color);
	painter->drawText(QRect(point,QSize(item_size.width(),font_metrics.height())),font_metrics.elidedText(text,Qt::ElideRight,item_size.width()));
	QList<QVariant> &row_list = *reinterpret_cast<QList<QVariant> *>(index.data(Qt::UserRole+2).value<qptrdiff>());
	painter->setFont(status_font);
	painter->setPen(status_color);
	font_metrics = QFontMetrics(status_font);
	option.fontMetrics = font_metrics;
	option.font = status_font;
	foreach(const QVariant &row_variant, row_list)
	{
		QList<QVariant> row = row_variant.toList();
		point.setX(option.rect.left());
		point+=m_margin_x;
		point+=QPoint(0,height);
		height=0;
		for(int j=0;j<row.size();j++)
		{
			QRect rect = QRect(point,size(option,index,row[j]));
			draw(painter, option, rect, row[j]);
			point+=m_margin_x;
			point+=QPoint(rect.width(),0);
			if(rect.height()>height)
				height = rect.height(); 
		}
	}
	
	// done
	painter->restore();
}

QSize ContactListItemDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	int type = index.data(Qt::UserRole).toInt();
	QVariant value = index.data(Qt::SizeHintRole);
	if (value.isValid())
		return qvariant_cast<QSize>(value);
	QStyleOptionViewItem opt = option;
	QVariant font = index.data(Qt::FontRole);
	if(font.type() == QVariant::Font)
		opt.font=qvariant_cast<QFont>(font);
	//opt.font=m_style.m_fonts[type<1?0:1];
	if(m_style_type==0)
		switch(type)
		{
		case -1:
		case 0:
			opt.font = qvariant_cast<QFont>(getValue("Contact Font",opt.font));
			break;
		case 1:
		case 2:
			opt.font = qvariant_cast<QFont>(getValue("Group Font",opt.font));
			break;
	}
	else if(m_style_type==1)
	{
		switch(type)
		{
		case 0:{
			QString type = index.data(Qt::UserRole+3).toString();
			opt.font = m_styles["normal"+type].m_font;
			break;
		}
		case -1:
			opt.font = m_styles["normalseparator"].m_font;
			break;
		case 1:
			opt.font = m_styles["normalgroup"].m_font;
			break;
		case 2:
			opt.font = m_styles["normalaccount"].m_font;
			break;
		}
	}
	int height=size(opt, index, index.data(Qt::DisplayRole)).height();
        QList<QVariant> &list = *reinterpret_cast<QList<QVariant> *>(index.data(Qt::DecorationRole).value<qptrdiff>());
        int icons_height = 0;
	for(int i=0;i<13;i++)
                if(m_show_icons[i])
                {
					QSize icon_size =  i == 1 ? qvariant_cast<QIcon>(list[i]).actualSize(QSize(65535,65535),QIcon::Normal,QIcon::On) : size(option, index, list[i]);
					icons_height = icon_size.height();
                    if(icons_height > height)
                        height = icons_height;
                }
	height+=m_margin*2;
        QList<QVariant> &row_list = *reinterpret_cast<QList<QVariant> *>(index.data(Qt::UserRole+2).value<qptrdiff>());
        foreach(const QVariant &list, row_list)
	{
		int h = size(option, index, list).height();
		if(h>0)
			height+=h+m_margin;
	}
	int width = m_tree_view->maximumViewportSize().width();
	return QSize(width, height);
}

QWidget *ContactListItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	qDebug() << index.isValid() << index.data( Qt::DisplayRole ).toString() << index.data( Qt::EditRole );
	if (!index.isValid())
		return 0;
//	QVariant::Type t = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());
	return QItemEditorFactory::defaultFactory()->createEditor(QVariant::String, parent);
}

void ContactListItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	qDebug() << Q_FUNC_INFO;
}

void ContactListItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	qDebug() << Q_FUNC_INFO;
}

void ContactListItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	qDebug() << Q_FUNC_INFO;
}

void ContactListItemDelegate::drawList(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex & index, const QList<QVariant> & list) const
{
	QFontMetrics font_metrics(option.font);
	int string_num=-1,separator=-1;
	int length=0;
	for (int i = 0; i < list.size(); ++i)
	{
		bool alive=true;
		if(!list[i].isValid() || list[i].isNull())
		{
			//list.removeAt(i);
			i--;
			alive=false;
		}
		/*else if(list[i].isNull())
		{
			if(separator<0)
				separator=i;
			list.removeAt(i);
			i--;
			alive=false;
		}
		else if(list[i].type()==QVariant::String)
		{
			if(string_num<0)
				string_num=i;
			else
			{
				list.removeAt(i);
				i--;
				alive=false;
			}
		}*/
		if(alive)
			length+=size(option, index, list.at(i)).width();
	}
	if(separator==-1)
		separator=list.size();
	QPoint point(option.rect.left(),option.rect.top());
	point+=m_margin_x;
	point+=m_margin_y;
	int height=0;
	
	if(length>option.rect.width())
	{
		
	}
	else
	{
		for(int i=0;i<separator;i++)
		{
			if(list[i].isValid()&&list[i].type()==QVariant::Icon)
			{
				QIcon icon = qvariant_cast<QIcon>(list[i]);
				QSize icon_size = size(option, index, list[i]);
				painter->drawPixmap(QRect(point, icon_size),icon.pixmap(icon_size,QIcon::Normal,QIcon::On));
				point+=QPoint(icon_size.width(),0);
				point+=m_margin_x;
				if(icon_size.height()>height)
					height = icon_size.height(); 
			}
		}
		QPoint point_r(option.rect.right(),option.rect.top());
		for(int i=list.size()-1;i>separator;i--)
		{
			if(list[i].isValid()&&list[i].type()==QVariant::Icon)
			{
				QIcon icon = qvariant_cast<QIcon>(list[i]);
				QSize icon_size = size(option, index, list[i]);
				point_r-=QPoint(icon_size.width(),0);
				point_r-=m_margin_x;
				painter->drawPixmap(QRect(point_r, icon_size),icon.pixmap(icon_size,QIcon::Normal,QIcon::On));
				if(icon_size.height()>height)
					height = icon_size.height(); 
			}
		}
		point_r-=m_margin_x;QString text=index.data(Qt::DisplayRole).toString();
		QSize item_size=QSize(point_r.x()-point.x(),font_metrics.height());
		if(font_metrics.height()>height)
			height = font_metrics.height();
		painter->drawText(QRect(point,QSize(item_size.width(),font_metrics.height())),font_metrics.elidedText(text,Qt::ElideRight,item_size.width()));
	}
}

QSize ContactListItemDelegate::size(const QStyleOptionViewItem &option, const QModelIndex & index, const QVariant &value) const
{
	static QSize maxsize(16,16);
	if (value.isValid() && !value.isNull()) {
			switch (value.type()) {
			case QVariant::Invalid:
				break;
			case QVariant::Pixmap:
				return qvariant_cast<QPixmap>(value).size();
			case QVariant::Image:
				return qvariant_cast<QImage>(value).size();
			case QVariant::Icon:{
				QIcon::Mode mode = QIcon::Normal;//d->iconMode(option.state);
				QIcon::State state = QIcon::On;//option.state&QStyle::State_On==QStyle::State_On?QIcon::On:QIcon::Off;
				QIcon icon = qvariant_cast<QIcon>(value);
				QSize size = icon.actualSize(maxsize);//, mode, state);
				return size;}
			case QVariant::Color:
				return option.decorationSize;
			case QVariant::List:{
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
			case QVariant::String:{
				QFont fnt = option.font;
				QFontMetrics fm(fnt);
				return QSize(fm.width(value.toString()),fm.height());}
			default:
				break;
			}
		}
		return QSize(0,0); 
}
void ContactListItemDelegate::drawGradient(QPainter *painter, const QColor & up, const QColor & down, const QRect & rect/*, int type*/) const
{
	painter->setPen(QColor(255,255,255,0));
	QLinearGradient gradient(QPoint(0,0),QPoint(0,rect.height()));
    if(!m_style_type)
    {
        gradient.setColorAt(0, up);
        gradient.setColorAt(1, down);
    }
    else
    {
	gradient.setColorAt(0, up);
	gradient.setColorAt(1, down);
    }
	painter->setBrush(gradient);
}
void ContactListItemDelegate::drawRect(QPainter *painter, const QColor & color/*, const QRect & rect, int type*/) const
{
    if(!m_style_type)
    {
        QColor color2 = color;
        color2.setAlpha(50);
        painter->setBrush(color2);
    }
    else
        painter->setBrush(color);
	/*QPen pen = painter->pen();
	QBrush brush = painter->brush();*/
        painter->setPen(QColor(255,255,255,0));
	/*drawRect(painter,rect,type);
	painter->setPen(pen);
	painter->setBrush(brush);*/
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
	int x1=rect.left(),y1=rect.top(),x2=rect.width(),y2=rect.height();
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
		y1=0;
		break;
	case 1: //draw rectangle rounded at top
		y2/=2;
		y1=y2;
		break;
	case 2: //draw rectangle rounded at bottom
		y2/=2;
		y1=0;
		break;
	default: //draw rounded rectangle
		break;
	}
	if(type<3)
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
		case QVariant::Icon:{
			//QIcon::Mode mode = QIcon::Normal;//d->iconMode(option.state);
			//QIcon::State state = QIcon::On;//option.state&QStyle::State_On==QStyle::State_On?QIcon::On:QIcon::Off;
			QIcon icon = qvariant_cast<QIcon>(value);
			painter->drawPixmap(rect,icon.pixmap(icon.actualSize(QSize(16,16)),QIcon::Normal,QIcon::On));
			break;}
		case QVariant::String:{
			QFont fnt = option.font;
			QFontMetrics font_metrics(fnt);
			painter->drawText(rect,font_metrics.elidedText(value.toString(),Qt::ElideRight,rect.width()));
			break;}
		default:
			break;
		}
	}
}
QMap<QString,QVariant> ContactListItemDelegate::appendStyleFile(QString path)
{
	QMap<QString,QVariant> style_hash;
	QDir dir(path);
	QFileInfo theme_file_info(path);//dir.filePath(dir.dirName()+file));
	QFile *theme_file;
	if(theme_file_info.isDir())
		theme_file = new QFile(path+"/Contents/Resources/Data.plist");//QDir(QDir(QDir(theme_file_info.absoluteDir().filePath(dir.dirName()+file)).filePath("Contents")).filePath("Resources")).filePath("Data.plist"));
	else
		theme_file = new QFile(path);//dir.absoluteFilePath(dir.dirName()+file));
	if (!theme_file->exists())
	{
		delete theme_file;
		return QMap<QString,QVariant>();
	}
	if (theme_file->open(QIODevice::ReadOnly))
	{
		QDomDocument theme_dom;

		if (theme_dom.setContent(theme_file))
		{
			QDomElement root_element = theme_dom.documentElement();
			if (root_element.isNull())
			{
				delete theme_file;
				return QMap<QString,QVariant>();
			}
			style_hash = parse(root_element.firstChild());
		}
	}
	delete theme_file;	
	return style_hash;
}
QMap<QString,QVariant> ContactListItemDelegate::parse(const QDomNode & root_element)
{
	QMap<QString,QVariant> style_hash;
	if (root_element.isNull())
		return style_hash;
	QDomNode subelement =root_element;//.firstChild();
	QString key="";
	for (QDomNode node = subelement.firstChild(); !node.isNull(); node = node.nextSibling())
	{
		QDomElement element = node.toElement();
		if(element.nodeName()=="key")
			key=element.text();
		else
		{
			QVariant value;
			if(element.nodeName()=="true")
				value=QVariant(true);
			else if(element.nodeName()=="false")
				value=QVariant(false);
			else if(element.nodeName()=="real")
				value=QVariant(element.text().toDouble());
			else if(element.nodeName()=="string")
			{
				QString text = element.text();
				if(key.indexOf("Font",Qt::CaseInsensitive)>-1)
					value=getFont(text);
				else if(key.indexOf("Color",Qt::CaseInsensitive)>-1||key.endsWith("Gradient")||key.endsWith("Background"))
					value=getColor(text);
				else
					value=QVariant(text);
			}
			else if(element.nodeName()=="integer")
				value=QVariant(element.text().toInt());
			else if(element.nodeName()=="dict")
			{
				value = parse(node);
			}
			style_hash.insert(key,value);
		}
	}
	return style_hash;
}
bool ContactListItemDelegate::setThemeStyle(QString path)
{
	m_style_type=2;
	m_style_hash.clear();
	m_styles.clear();
	if(path.isEmpty())
		return false;
	QFileInfo dir(path);
	if(!dir.exists())
		return false;
	QMap<QString,QVariant> style_hash = appendStyleFile(path);
	if(style_hash.isEmpty())
		return false;
	if(path.endsWith(".ListTheme"))
	{
		m_style_type=0;
		m_style_hash = style_hash;
                QColor &background = m_style_colors["background"] = qvariant_cast<QColor>(style_hash["Background Color"]);
                background.setAlpha(0);
                QString css = "QTreeView { ";
                css.append(QString("background-color: rgba(%1, %2, %3, %4);").arg(background.red()).arg(background.green()).arg(background.blue()).arg(background.alpha()));
                css+=" }";
                m_tree_view->setStyleSheet(css);
		m_style_fonts["contact"] = qvariant_cast<QFont>(style_hash["Contact Font"]);
		if(style_hash.value("Grid Enabled",false).toBool())
			m_style_colors["grid"] = qvariant_cast<QColor>(style_hash["Grid Color"]);
		if(style_hash.value("Online Enabled",false).toBool())
		{
			m_style_colors["onlinefont"] = qvariant_cast<QColor>(style_hash["Online Color"]);
			m_style_colors["onlinelabel"] = qvariant_cast<QColor>(style_hash["Online Label Color"]);
		}
		else
		{
			m_style_colors["onlinefont"] = qvariant_cast<QColor>(style_hash["Contact Text Color"]);
			m_style_colors["onlinelabel"] = m_style_colors["background"];
		}
		m_style_colors["ffcfont"] = m_style_colors["onlinefont"];
		m_style_colors["ffclabel"] = m_style_colors["onlinelabel"];
		m_style_colors["athomefont"] = m_style_colors["onlinefont"];
		m_style_colors["athomelabel"] = m_style_colors["onlinelabel"];
		m_style_colors["atworkfont"] = m_style_colors["onlinefont"];
		m_style_colors["atworklabel"] = m_style_colors["onlinelabel"];
		m_style_colors["lunchfont"] = m_style_colors["onlinefont"];
		m_style_colors["lunchlabel"] = m_style_colors["onlinelabel"];
		m_style_colors["evilfont"] = m_style_colors["onlinefont"];
		m_style_colors["evillabel"] = m_style_colors["onlinelabel"];
		m_style_colors["depressionfont"] = m_style_colors["onlinefont"];
		m_style_colors["depressionlabel"] = m_style_colors["onlinelabel"];
		if(style_hash.value("Offline Enabled",false).toBool())
		{
			m_style_colors["offlinefont"] = qvariant_cast<QColor>(style_hash["Offline Color"]);
			m_style_colors["offlinelabel"] = qvariant_cast<QColor>(style_hash["Offline Label Color"]);
		}
		else
		{
			m_style_colors["offlinefont"] = qvariant_cast<QColor>(style_hash["Contact Text Color"]);
			m_style_colors["offlinelabel"] = m_style_colors["background"];
		}
		if(style_hash.value("Away Enabled",false).toBool())
		{
				m_style_colors["awayfont"] = qvariant_cast<QColor>(style_hash["Away Color"]);
			m_style_colors["awaylabel"] = qvariant_cast<QColor>(style_hash["Away Label Color"]);
		}
		else
		{
			m_style_colors["awayfont"] = qvariant_cast<QColor>(style_hash["Contact Text Color"]);
			m_style_colors["awaylabel"] = m_style_colors["background"];
		}
		m_style_colors["nafont"] = m_style_colors["awayfont"];
		m_style_colors["nalabel"] = m_style_colors["awaylabel"];
		m_style_colors["occupiedfont"] = m_style_colors["awayfont"];
		m_style_colors["occupiedlabel"] = m_style_colors["awaylabel"];
		m_style_colors["dndfont"] = m_style_colors["awayfont"];
		m_style_colors["dndlabel"] = m_style_colors["awaylabel"];
		m_style_colors["invisiblefont"] = m_style_colors["awayfont"];
		m_style_colors["invisiblelabel"] = m_style_colors["awaylabel"];
		
		if(style_hash.value("Group Gradient",false).toBool())
		{
			m_style_bools["groupgradient"]=true;
			m_style_colors["groupup"] = qvariant_cast<QColor>(style_hash["Group Background"]);
			m_style_colors["groupdown"] = qvariant_cast<QColor>(style_hash["Group Background Gradient"]);
		}
		else
			m_style_colors["groupbackground"] = qvariant_cast<QColor>(style_hash["Group Background"]);
		m_style_colors["grouplabel"] = qvariant_cast<QColor>(style_hash["Group Label Color"]);
		m_style_colors["groupfont"] = qvariant_cast<QColor>(style_hash["Group Text Color"]);
		m_style_fonts["group"] = qvariant_cast<QFont>(style_hash["Group Font"]);
		return true;
	}
	else
	{
		m_style_type = 1;
		m_style_hash = style_hash;
		QString theme_path;
		if(dir.isFile())
			theme_path = dir.absolutePath();
		else
		{
			QDir theme_dir(path);
			theme_dir.cdUp();
			theme_path = theme_dir.absolutePath();
		}
		QStringList types;
		QStringList short_types;
		types << "Separator" << "Group" << "Account" << "Contact";
		short_types << "separator" << "group" << "account" << "contact";
		types << "Online" << "FreeForChat" << "AtHome" << "AtWork" << "Lunch" << "Evil" << "Depression";
		types << "Offline" << "Away" << "NA" << "DND" << "Occupied" << "Invisible";
		short_types << "online" << "ffc" << "athome" << "atwork" << "lunch" << "evil" << "depression";
		short_types << "offline" << "away" << "na" << "dnd" << "occupied" << "invisible";
		QStringList selection_types;
		QStringList selection_short_types;
		selection_short_types << "normal" << "hover" << "down";
		selection_types << "Normal" << "Hover" << "Down";
		//QString css = "QAbstractItemView { ";
		QString css = "QTreeView { ";
		int full_background_type = style_hash.value("Background Type",0).toInt();
		if(style_hash.contains("Gradients") && (full_background_type==1 || full_background_type==2))
		{
			QString gradient = QString("background: qlineargradient(x1:0, y1:0, x2:%1, y2:%2").arg(full_background_type==1?0:1).arg(full_background_type==1?1:0);
			QMap<QString,QVariant> gradients = style_hash.value("Gradients").toMap();
			int n=1;
			while(gradients.contains("Color "+QString::number(n)) && gradients.contains("Stop "+QString::number(n)))
			{
				QColor color = qvariant_cast<QColor>(gradients.value("Color "+QString::number(n)));
				gradient.append(QString(", stop: %1 rgba(%2, %3, %4, %5)").arg(gradients.value("Stop "+QString::number(n)).toDouble()).arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
				n++;
			}
			gradient.append(");");
			css.append(gradient);
		}
		else if(style_hash.contains("Background"))
		{
			QColor background = qvariant_cast<QColor>(style_hash.value("Background"));
			css.append(QString("background-color: rgba(%1, %2, %3, %4);").arg(background.red()).arg(background.green()).arg(background.blue()).arg(background.alpha()));
		}
		if(style_hash.contains("Alternate Background"))
		{
			QColor background = qvariant_cast<QColor>(style_hash.value("Alternate Background"));
			css.append(QString("alternate-background-color: rgba(%1, %2, %3, %4);").arg(background.red()).arg(background.green()).arg(background.blue()).arg(background.alpha()));
		}
		//int background_picture_type = style_hash.value("Background Picture Type",0).toInt();
		if(style_hash.contains("Background Image"))
		{
			css.append(QString("background-image: url(\"%1\");").arg(theme_path+"/"+style_hash.value("Background Image").toString()));
			if(style_hash.contains("Background Repeat"))
				css.append(QString("background-repeat: repeat-%1;").arg(style_hash.value("Background Repeat").toString().toLower()));
			if(style_hash.contains("Background Position"))
				css.append(QString("background-repeat: position-%1;").arg(style_hash.value("Background Position").toString().toLower()));
			if(style_hash.contains("Background Attachment"))
			{
				if(style_hash.value("Background Attachment")=="Fixed")
					css.append(QString("background-attachment: fixed;"));
			}
		}
		css.append(" }");
		qDebug() << css;
		m_tree_view->setStyleSheet(css);
		if(style_hash.contains("ScrollBar Policy"))
		{
			switch(style_hash.value("ScrollBar Policy",0).toInt())
			{
				case 1:
					m_tree_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
					break;
				case 2:
					m_tree_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
					break;
				default:
					m_tree_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
					break;
			}
		}
		m_styles["normal"].m_background_type = 0;
		m_styles["normal"].m_background = qvariant_cast<QColor>(style_hash.value("Background",QColor(0,0,0,0)));
                m_styles["normal"].m_font = qvariant_cast<QFont>(style_hash.value("Font",QApplication::font()));
                m_styles["normal"].m_font_bold = style_hash.value("Font Bold", false).toBool();
                m_styles["normal"].m_font_italic = style_hash.value("Font Italic", false).toBool();
                m_styles["normal"].m_font_overline = style_hash.value("Font Overline", false).toBool();
                m_styles["normal"].m_font_underline = style_hash.value("Font Underline", false).toBool();
		m_styles["normal"].m_border_width = 0;
		m_styles["normal"].m_border_color = QColor(0,0,0,0);
		m_styles["normal"].m_text_color = qvariant_cast<QColor>(style_hash.value("Text Color",QApplication::palette().text().color()));
		m_status_font = qvariant_cast<QFont>(style_hash.value("Status Font",QApplication::font()));
		m_status_color["normal"] = qvariant_cast<QColor>(style_hash.value("Status Text Color",QApplication::palette().text().color()));
		for(int i=1;i<selection_types.size();i++)
			m_status_color[selection_short_types[i]] = qvariant_cast<QColor>(style_hash.value(selection_types[i]+"Status Text Color",m_status_color["normal"]));
		for(int i=1;i<selection_types.size();i++)
		{
			m_styles[selection_short_types[i]] = m_styles["normal"];
			if(style_hash.contains(selection_types[i]+"Background"))
				m_styles[selection_short_types[i]].m_background = qvariant_cast<QColor>(style_hash.value(selection_types[i]+"Background"));
			if(style_hash.contains(selection_types[i]+"Font"))
				m_styles[selection_short_types[i]].m_font = qvariant_cast<QFont>(style_hash.value(selection_types[i]+"Font",QApplication::font()));
			if(style_hash.contains(selection_types[i]+"Text Color"))
				m_styles[selection_short_types[i]].m_text_color = qvariant_cast<QColor>(style_hash.value(selection_types[i]+"Text Color",QApplication::palette().text().color()));
		}
		QMap<QString,QVariant> root = m_style_hash;
		for(int i=0;i<types.size();i++)
		{
			m_styles["normal"+short_types[i]] = m_styles["normal"];
			if(root.contains(types[i]))
			{
                                QMap<QString,QVariant> item = root.value(types[i]).toMap();
                                if(item.contains("Font"))
                                        m_styles["normal"+short_types[i]].m_font = qvariant_cast<QFont>(item.value("Font"));
                                if(item.contains("Font Bold"))
                                        m_styles["normal"+short_types[i]].m_font_bold = item.value("Font Bold").toBool();
                                if(item.contains("Font Italic"))
                                        m_styles["normal"+short_types[i]].m_font_italic = item.value("Font Italic").toBool();
                                if(item.contains("Font Overline"))
                                        m_styles["normal"+short_types[i]].m_font_overline = item.value("Font Overline").toBool();
                                if(item.contains("Font Underline"))
                                        m_styles["normal"+short_types[i]].m_font_underline = item.value("Font Underline").toBool();
				for(int j=0;j<selection_types.size();j++)
				{
					m_styles[selection_short_types[j]+short_types[i]] = m_styles[i>3?(selection_short_types[j]+"contact"):("normal"+short_types[i])];
					if(item.contains(selection_types[j]))
					{
						QMap<QString,QVariant> selection = item.value(selection_types[j]).toMap();
						if(selection.contains("Text Color"))
							m_styles[selection_short_types[j]+short_types[i]].m_text_color = qvariant_cast<QColor>(selection.value("Text Color"));
						if(selection.contains("Background Type"))
							m_styles[selection_short_types[j]+short_types[i]].m_background_type = selection.value("Background Type").toInt();
						int background_type = m_styles[selection_short_types[j]+short_types[i]].m_background_type;
						switch(background_type)
						{
							case 1:
							case 2:{
								m_styles[selection_short_types[j]+short_types[i]].m_stops.clear();
								m_styles[selection_short_types[j]+short_types[i]].m_colors.clear();
								if(selection.contains("Gradients"))
								{
									QMap<QString,QVariant> gradients = selection.value("Gradients").toMap();
									int n=1;
									while(gradients.contains("Color "+QString::number(n)) && gradients.contains("Stop "+QString::number(n)))
									{
										m_styles[selection_short_types[j]+short_types[i]].m_stops.append(gradients.value("Stop "+QString::number(n)).toDouble());
										m_styles[selection_short_types[j]+short_types[i]].m_colors.append(qvariant_cast<QColor>(gradients.value("Color "+QString::number(n))));
										n++;
									}
									n--;
									break;
								}
							}
							default:{
								m_styles[selection_short_types[j]+short_types[i]].m_background_type=0;
								if(selection.contains("Background"))
									m_styles[selection_short_types[j]+short_types[i]].m_background = qvariant_cast<QColor>(selection.value("Background"));
								break;
							}
						}
						if(selection.contains("Border Width"))
							m_styles[selection_short_types[j]+short_types[i]].m_border_width = selection.value("Border Width").toInt();
						if(selection.contains("Border Color"))
							m_styles[selection_short_types[j]+short_types[i]].m_border_color = qvariant_cast<QColor>(selection.value("Border Color"));
					}
				}
				if(i==3)
				{
					root = item;
					for(int j=0;j<selection_types.size();j++)
						m_styles[selection_short_types[j]] = m_styles[selection_short_types[j]+short_types[i]];
				}
			}
			else
				for(int j=0;j<selection_types.size();j++)
					m_styles[selection_short_types[j]+short_types[i]] = m_styles[i>3?(selection_short_types[j]+"contact"):("normal"+short_types[i])];
		}
		return true;
	}
}
QVariant ContactListItemDelegate::getValue(const QString &name, const QVariant &def) const
{
	QVariant var = m_style_hash.value(name);
	if(var.isValid())
		return var;
	return def;
	
}
QColor ContactListItemDelegate::getColor(QVariant var) const
{
	QStringList list = var.toString().split(",");
	if(list.size()>3)
		return QColor(list[0].toInt(),list[1].toInt(),list[2].toInt(),list[3].toInt());
	else if(list.size() == 3)
		return QColor(list[0].toInt(),list[1].toInt(),list[2].toInt());
	else
		return QColor(var.toString());
}
QFont ContactListItemDelegate::getFont(QVariant var) const
{
	QStringList list = var.toString().split(",");
	if(list.size() < 2)
		return QFont(var.toString(), 12);
//	qWarning() << var.toString() << QFont(list[0],list[1].toInt());
	QFont font(list[0]);
	font.setPixelSize(	list[1].toInt());
	return font;
	//return QFont(list[0],list[1].toInt());
}
/*void ContactListItemDelegate::setDefaultStyle()
{
	//var = hash.value("Background Color");
	m_style.m_colors[0]=QColor(0,0,0,0);
	//var = hash.value("Background Fade");
	m_style.m_reals[0]=1.0;
	//var = hash.value("Background Image Style");
	m_style.m_integers[0]=0;
	//var = hash.value("Contact Font");
	m_style.m_fonts[0]=QFont();
	//var = hash.value("Contact Text Alignment");
	m_style.m_integers[1]=0;
	//var = hash.value("Contact Status Text Color");
	m_style.m_colors[1]=QColor(0,0,0,255);
	//var = hash.value("Contact Left Indent");
	m_style.m_integers[2]=5;
	//var = hash.value("Contact Right Indent");
	m_style.m_integers[3]=5;
	//var = hash.value("Group Background");
	m_style.m_colors[2]=QColor(0,0,0,0);
	//var = hash.value("Group Font");
	m_style.m_fonts[1]=QFont();
	//var = hash.value("Group Inverted Text Color");
	m_style.m_colors[3]=QColor(255,255,255,255);
	//var = hash.value("Group Text Alignment");
	m_style.m_integers[4]=0;
	//var = hash.value("Group Text Color");
	m_style.m_colors[4]=QColor(0,0,0,255);	
}*/
void ContactListItemDelegate::setTreeView(QAbstractItemView *tree)
{
	m_tree_view = tree;
}
