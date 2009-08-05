/*
    emoticonMenu

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
				  2009 by Ruslan Nigmatullin <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef CHATEMOTICONMENU_H
#define CHATEMOTICONMENU_H

#include <QtGui>
#include <cmath>
#include "movielabel.h"

class ChatEmoticonMenu : public QScrollArea
{
    Q_OBJECT

public:
	ChatEmoticonMenu(QWidget *parent = 0);
    ~ChatEmoticonMenu();
	void setEmoticons(QHash<QString, QStringList>);
public slots:
	void ensureGeometry();

signals:
	void insertSmile(const QString &);
	
protected:

	void hideEvent ( QHideEvent * );
	void showEvent ( QShowEvent * );
    
private:
	QList<QSize> sizeList;
    QList<movieLabel *> labelList;
    QList<QMovie *> movieList;
	QHash<QString, QString>  _emotList;
    QGridLayout *m_grid_layout;
	QWidget *m_widget;
	QSize m_desktop_geometry;
	void clearList();
};

#endif // CHATEMOTICONMENU_H
