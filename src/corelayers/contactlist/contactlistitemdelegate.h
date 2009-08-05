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

#ifndef CONTACTLISTITEMDELEGATE_H_
#define CONTACTLISTITEMDELEGATE_H_
#include <QItemDelegate>
#include <QAbstractItemDelegate>
#include <QPainter>
#include <QDebug>
#include <QDomElement>
#include <QDomNode>
#include <QDomDocument>
#include <QTreeView>
#include <QFont>
#include <QVector>
#include <QHash>
#include <QVariant>
//#include <phonon>

/*struct ItemDelegateStyle
{
	QVector<QFont> m_fonts;
	QVector<QColor> m_colors;
	QVector<int> m_integers;
	QVector<QPixmap> m_pixmaps;
	QVector<double> m_reals;
	QVector<bool> m_booleans;
};*/
class ContactListItemDelegate : public QAbstractItemDelegate
{
	Q_OBJECT
public:
	explicit ContactListItemDelegate(QObject *parent = 0);
	virtual ~ContactListItemDelegate();
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setTreeView(QAbstractItemView *tree);
	bool setThemeStyle(QString path=QString(""));
	void setSettings(QList<bool> show_icons, double opacity);
	QMap<QString,QVariant> m_style_hash;
	//void setDefaultStyle();
private:
	struct StyleVar
	{
                StyleVar() : m_font_bold(false), m_font_italic(false),
                        m_font_overline(false), m_font_underline(false),
                        m_border_width(0)
                {}
		QFont m_font;
		int m_background_type;
		QVector<double> m_stops;
                QVector<QColor> m_colors;
                QColor m_text_color;
                bool m_font_bold;
                bool m_font_italic;
                bool m_font_overline;
                bool m_font_underline;
		QColor m_background;
		int m_border_width;
		QColor m_border_color;
		QColor m_status_color;
		QFont m_status_font;
	};
	QPixmap getAlphaMask(QPainter *painter, QRect rect, int type) const;
	void drawList(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index, const QList<QVariant> & list) const;
	QMap<QString,QVariant> appendStyleFile(QString path);
	QMap<QString,QVariant> parse(const QDomNode & root_element);
        QSize size(const QStyleOptionViewItem &option, const QModelIndex & index, const QVariant &item) const;
	void draw(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QVariant &item) const;
	QSize uniPaint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index, const bool &paint=false) const;
	QVariant getValue(const QString &name, const QVariant &def) const;
	QColor getColor(QVariant var) const;
	QFont getFont(QVariant var) const;
	void drawGradient(QPainter *painter, const QColor & up, const QColor & down, const QRect & rect/*, int type*/) const;
	void drawRect(QPainter *painter, const QColor & color/*, const QRect & rect, int type*/) const;
	void drawRect(QPainter *painter, QRect rect) const;
	QList<bool> m_show_icons;
	int m_margin;
	int m_style_type; // 0 - AdiumX, 1 - qutIM, 2 - light
	double m_opacity;
	QPoint m_margin_x;
	QPoint m_margin_y;
	QAbstractItemView *m_tree_view;
	
	QHash<QString,StyleVar> m_styles;	
	QFont m_status_font;
	QHash<QString,QColor> m_status_color;
	
	QHash<QString,QColor> m_style_colors;
	QHash<QString,QFont> m_style_fonts;
	QHash<QString,bool> m_style_bools;
	QHash<QString,int> m_style_ints;
	QHash<QString,double> m_style_reals;
	QHash<QString,QString> m_style_strings;
	//ItemDelegateStyle m_style;
};

#endif /*CONTACTLISTITEMDELEGATE_H_*/
