/****************************************************************************
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
** 
** This file is part of a Qt Solutions component.
**
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
** 
****************************************************************************/

#ifndef COLORCOMBOBOX_H
#define COLORCOMBOBOX_H
#include <QtGui/QComboBox>
#include <QtGui/QColor>

#if defined(Q_WS_WIN)
#  if !defined(QT_QTCOLORCOMBOBOX_EXPORT) && !defined(QT_QTCOLORCOMBOBOX_IMPORT)
#    define QT_QTCOLORCOMBOBOX_EXPORT
#  elif defined(QT_QTCOLORCOMBOBOX_IMPORT)
#    if defined(QT_QTCOLORCOMBOBOX_EXPORT)
#      undef QT_QTCOLORCOMBOBOX_EXPORT
#    endif
#    define QT_QTCOLORCOMBOBOX_EXPORT __declspec(dllimport)
#  elif defined(QT_QTCOLORCOMBOBOX_EXPORT)
#    undef QT_QTCOLORCOMBOBOX_EXPORT
#    define QT_QTCOLORCOMBOBOX_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTCOLORCOMBOBOX_EXPORT
#endif

class QT_QTCOLORCOMBOBOX_EXPORT QtColorComboBox : public QComboBox
{
    Q_OBJECT
public:
    QtColorComboBox(QWidget *parent = 0, const char *name = 0);
    void insertColor(int index, const QColor &color, const QString &name);
    inline void addColor(const QColor &color, const QString &name)
                { insertColor(colorCount(), color, name); }

    int colorCount() const;

    void setCurrentColor(const QColor &color);
    QColor currentColor() const;

    QColor color(int index) const;

    void setColorDialogEnabled(bool enabled = true);
    bool isColorDialogEnabled() const;

    void setStandardColors();

    QSize sizeHint() const;

Q_SIGNALS:
    void activated(const QColor &color);
    void highlighted(const QColor &color);

private Q_SLOTS:
    void emitActivatedColor(int index);
    void emitHighlightedColor(int index);

private:
    QColor lastActivated;
    int numUserColors;
    bool colorDialogEnabled;
};

#endif

