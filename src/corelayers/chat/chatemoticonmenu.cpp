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


#include "chatemoticonmenu.h"
#include <QVector>

ChatEmoticonMenu::ChatEmoticonMenu(QWidget *parent)
	: QScrollArea(parent)
{
	m_widget = 0;
	m_grid_layout = 0;
	setFrameStyle(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ChatEmoticonMenu::~ChatEmoticonMenu()
{
	m_widget = 0;
	clearList();
}

void ChatEmoticonMenu::setEmoticons(QHash<QString, QStringList> list)
{
	clearList();
	m_widget = new QWidget;
	m_grid_layout = new QGridLayout(m_widget);
	m_grid_layout->setSpacing(1);
	m_widget->setLayout(m_grid_layout);
	int max_len_size = 0;
	m_desktop_geometry = QSize();
	QStringList emotList = list.keys();
	emotList.sort();

	foreach(const QString &path, emotList)
	{
		QStringList values = list.value(path);
		if(!values.size())
			continue;
		movieLabel *label = new movieLabel;
		labelList << label;
		QMovie *movie = new QMovie(path);
		movieList << movie;
		label->setMovie(movie);
		movie->setCacheMode(QMovie::CacheAll);
		movie->start();
		QSize size = movie->currentPixmap().size();
		label->setMinimumSize(size);
		sizeList << size;
		label->setToolTip(values.first());
		connect(label, SIGNAL(sendMovieTip(const QString &)), this, SIGNAL(insertSmile(const QString &)));
		movie->stop();
	}
//
//	int sq  = std::ceil(std::sqrt((float)list.count()));
//
//	int i = 0, j = 0;
//
//	foreach(const QString &path, emotList)
//	{
//		QStringList values = list.value(path);
//		if(!values.size())
//			continue;
//		movieLabel *l = new movieLabel;
////		QMovie *movie = new QMovie(path + "/" + list.key(name));
//		QMovie *movie = new QMovie(path);
//		movieList.append(movie);
//		l->setMovie(movie);
//		movie->setCacheMode(QMovie::CacheAll);
//		movie->start();
//		QSize movie_size = movie->currentPixmap().size();
//		l->setMinimumSize(movie_size);
//		labelList.append(l);
//		l->setToolTip(values.first());
//		connect(l, SIGNAL(sendMovieTip(const QString &)), this, SIGNAL(insertSmile(const QString &)));
//		m_grid_layout->addWidget(l,i,j);
//		if ( j < sq )
//			j++;
//		else
//		{
//			i++;
//			j = 0;
//		}
//		movie->stop();
//	}
	setWidget(m_widget);
}

void ChatEmoticonMenu::clearList()
{
//	foreach(movieLabel *l, labelList)
//		delete l;
	delete m_widget;
	m_widget = 0;
	
	sizeList.clear();
	qDeleteAll(labelList);
	labelList.clear();
		
//	foreach(QMovie *m, movieList)
//		delete m;
	qDeleteAll(movieList);
	movieList.clear();
}


void ChatEmoticonMenu::hideEvent(QHideEvent *e)
{
	foreach(QMovie *m, movieList)
		m->stop();
//	clearList();
	QWidget::hideEvent(e);
}

void ChatEmoticonMenu::showEvent(QShowEvent *e)
{
//	ensureGeometry();
//	setEmoticons(emotList, emotPath);
	foreach(QMovie *m, movieList)
	{
		m->setCacheMode(QMovie::CacheAll);
		m->start();
	}
	QWidget::showEvent(e);
}

void ChatEmoticonMenu::ensureGeometry()
{
	if(sizeList.isEmpty())
		return;
	QSize geom = QApplication::desktop()->availableGeometry(QCursor::pos()).size();
	if(m_desktop_geometry == geom)
		return;
	m_desktop_geometry = geom;
	foreach(movieLabel *label, labelList)
		m_grid_layout->removeWidget(label);
	int sq  = std::ceil(std::sqrt((float)sizeList.count()));
	int width = 0;
	int height = 0;
	{
		for(int i = 0; i < sq; i++)
		{
			width += sizeList.at(i).width();
			if(width > geom.width())
			{
				sq = i ? i : 1;
				break;
			}
		}
//		for(int i = 0; i < sizeList.size(); i += sq)
//		{
//			height += sizeList.at(i).height();
//			if(height > geom.height())
//			{
//				sq = i ? i : 1;
//				break;
//			}
//		}
	}
	QVector<int> rows((sizeList.size() / sq) + 1, 0);
	QVector<int> columns(sq, 0);
	for(; sq > 1; sq--)
	{
		width = m_grid_layout->margin();
		height = m_grid_layout->margin();
		for(int i = 0; i < sizeList.size(); i++)
		{
			const QSize &size = sizeList[i];
			rows[i / sq] = qMax(rows[i / sq], size.height());
			columns[i % sq] = qMax(columns[i % sq], size.width());
		}
		for(int i = 0; i < (sizeList.size() / sq) + (sizeList.size() % sq > 0 ? 1 : 0); i++)
			height += rows[i] + 1;
		for(int i = 0; i < sq; i++)
			width += columns[i] + 1;
		if(width <= geom.width())
			break;
	}
	for(int i = 0; i < labelList.size(); i++)
	{
		movieList[i]->start();
		m_grid_layout->addWidget(labelList[i], i / sq, i % sq, Qt::AlignHCenter | Qt::AlignTop);
		movieList[i]->stop();
	}
//	parentWidget()->resize(width, qMin(height, geom.height()));
	resize(width/* + 10*/, qMin(height, geom.height()));
	setMinimumSize(size());
	m_widget->resize(width, qMin(height, geom.height()));
}
