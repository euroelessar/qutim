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

#include <QtGui/QColorDialog>
#include <QtGui/QPainter>
#include <QtGui/QToolTip>
#include <QtGui/QFontMetrics>
#include "qtcolorcombobox.h"

/*! \class QtColorComboBox

    \brief The QtColorComboBox class provides a combobox with colors
    and texts.

    Colors are inserted with insertColor(), which takes a color, a
    text, and an optional index position. The combobox can also be
    populated with a standard set of colors using setStandardColors().
    Users can add their own colors if the color dialog is enabled (see
    setColorDialogEnabled()).

    When a color is highlighted (for example when the mouse moves over
    it after the combobox has been dropped down), the highlighted()
    signal is emitted. When a color is activated (for example,
    clicked), the activated() signal is emitted. If the color dialog
    is enabled and the item clicked is the "More..." item at the end,
    the color dialog is popped up and if the user chooses a color the
    activated signal is emitted after the dialog closes.

    Use color() to get the color at a certain index position, and
    currentColor() to get the color that is currently selected.

    \img qtcolorcombobox.png
*/

/*! \fn QtColorComboBox::activated(const QColor &color)

    This signal is emitted when the color \a color has been activated.
*/

/*! \fn QtColorComboBox::highlighted(const QColor &color)

    This signal is emitted when the color \a color has been highlighted
    on the popup color grid.
*/

/*
    A class representing a color item in the combo box. The most
    interesting function here is paint().
*/

/*!
    Constructs a QtColorComboBox with the color dialog disabled. The
    \a parent and \a name arguments are passed to the QComboBox's
    constructor.
*/
QtColorComboBox::QtColorComboBox(QWidget *parent, const char * /*name*/)
    : QComboBox(parent), numUserColors(0)
{
    colorDialogEnabled = false;

    // QtColorComboBox uses QColor based signals with the same name.
    // We intercept the int signals emitted by QtComboBox in able to
    // do our logics.
    connect(this, SIGNAL(activated(int)), SLOT(emitActivatedColor(int)));
    connect(this, SIGNAL(highlighted(int)), SLOT(emitHighlightedColor(int)));
}

/*!
    Enables the color dialog if \a enabled is true; otherwise disables
    it.

    When the color dialog is enabled, an extra item is available at
    the bottom of the list of colors in the combobox with the
    translatable text "More...". Selecting this item will pop up the
    color dialog, allowing the user to add new colors to the list. The
    new colors will be called "User color 1", "User color 2", and so
    on; the names can be changed by translating "User color %1".

    \sa insertColor()
*/
void QtColorComboBox::setColorDialogEnabled(bool enabled)
{
    if (colorDialogEnabled == enabled)
	return;

    if ((colorDialogEnabled = enabled) == true) {
	// Add the color dialog item if it's disabled from before.
	addItem(tr("More..."));
    } else {
	// Remove the color dialog item if it's enabled from before.
	// It's always the last item in the list.
        removeItem(colorCount());
    }
}

/*!
  \fn void QtColorComboBox::addColor(const QColor &color, const QString &name)

    Appends the color \a color with the text \a name to the combobox.
*/

/*!
    Inserts the color \a color with the text \a name at position \a
    index.

    If \a index is -1, then color is prepended to the beginning end of the list.
*/
void QtColorComboBox::insertColor(int index, const QColor &color, const QString &name)
{
    QPixmap pix(12, 12);
    QPainter painter(&pix);
    if (color.isValid()) {
	painter.setPen(Qt::gray);
	painter.setBrush(QBrush(color));
	painter.drawRect(0, 0, 12, 12);
    }
    QIcon icon;
    icon.addPixmap(pix);

    // Prevent adding of colors after the color dialog item.
    if (colorDialogEnabled && index > colorCount()) index = colorCount() - 1;
    insertItem(index, icon, name, color);
}

/*!
    This function returns the number of colours in the combobox. (excluding the "More...")
*/
int QtColorComboBox::colorCount() const
{
    return QComboBox::count() - (colorDialogEnabled ? 1 : 0);
}

/*!
    Returns the current color.
*/
QColor QtColorComboBox::currentColor() const
{
    return qVariantValue<QColor>(itemData(currentIndex()) );
}

/*!
    Sets \a color to be the current color if \a color is one of the
    combobox's colors; otherwise does nothing.

    \sa insertColor()
*/
void QtColorComboBox::setCurrentColor(const QColor &color)
{
    for (int i = 0; i < colorCount(); i++) {
	if (QtColorComboBox::color(i) == color) {
	    setCurrentIndex(i);
	    break;
	}
    }
}

/*!
    Returns the color at position \a index.
*/
QColor QtColorComboBox::color(int index) const
{
    QVariant var = itemData(index);
    // discard out of bounds requests
    if (!var.isValid())
	return QColor();

    // discard requests for the "more" item
    if (colorDialogEnabled && index == colorCount())
	return QColor();

    return qVariantValue<QColor>(var);
}

/*! \internal

    This slot is triggered by the combobox' activated() signal. If the
    color dialog item was activated, shows the color dialog and
    inserts the new color; otherwise, emits activated() with the color
    that was selected.
*/
void QtColorComboBox::emitActivatedColor(int index)
{
    if (colorDialogEnabled && index == colorCount()) {
	// Get a new color from the color dialog.
	QColor col = QColorDialog::getColor();
	if (col.isValid()) {
	    // Unless the user pressed cancel, insert the new color at
	    // the end of the list.
	    addColor(col, tr("User color %1").arg(++numUserColors));
	    setCurrentIndex(index);
	} else {
	    // The user pressed cancel - reset the current color to
	    // what it was before the color dialog was shown.
	    setCurrentColor(lastActivated);
	    col = lastActivated;
	}

	update();
	lastActivated = col;
	emit activated(col);
    } else {

        // If any other item than the color dialog item was activated,
        // pull out the color of that item.
        QColor col = qVariantValue<QColor>(itemData(index) );
        update();
        lastActivated = col;
        emit activated(col);

    }
}

/*! \internal

    This slot is triggered by QComboBox' highlighted() signal. If the
    highlighted item is a color, emits the highlighted() signal with
    that color.
*/
void QtColorComboBox::emitHighlightedColor(int index)
{
    if (!colorDialogEnabled || index != colorCount())
	emit highlighted(color(index));
}

/*! \reimp
 */
QSize QtColorComboBox::sizeHint() const
{
    QFontMetrics fm = fontMetrics();
    QStyleOptionComboBox box;
    box.init(this);
    return style()->sizeFromContents(QStyle::CT_ComboBox, &box,
                                     QSize(fm.width(tr("User Color 99")) + 16 + 4, fm.height() + 4),
                                     this);
}

/*!
    Adds the 17 predefined colors from the Qt namespace.

    (The names given to the colors, "Black", "White", "Red", etc., are
    all translatable.)

    \sa insertColor()
*/
void QtColorComboBox::setStandardColors()
{
    addColor(Qt::black, tr("Black"));
    addColor(Qt::white, tr("White"));
    addColor(Qt::red, tr("Red"));
    addColor(Qt::darkRed, tr("Dark red"));
    addColor(Qt::green, tr("Green"));
    addColor(Qt::darkGreen, tr("Dark green"));
    addColor(Qt::blue, tr("Blue"));
    addColor(Qt::darkBlue, tr("Dark blue"));
    addColor(Qt::cyan, tr("Cyan"));
    addColor(Qt::darkCyan, tr("Dark cyan"));
    addColor(Qt::magenta, tr("Magenta"));
    addColor(Qt::darkMagenta, tr("Dark magenta"));
    addColor(Qt::yellow, tr("Yellow"));
    addColor(Qt::darkYellow, tr("Dark yellow"));
    addColor(Qt::gray, tr("Gray"));
    addColor(Qt::darkGray, tr("Dark gray"));
    addColor(Qt::lightGray, tr("Light gray"));
}

/*!
    Returns true if the color dialog button is enabled;
    otherwise returns false.
*/
bool QtColorComboBox::isColorDialogEnabled() const
{
    return colorDialogEnabled;
}
