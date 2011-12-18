/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ketmar // Avalon Group <psyc://ketmar.no-ip.org/~Ketmar>
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
#ifndef K8JSON_H
#define K8JSON_H

//#define K8JSON_INCLUDE_GENERATOR


#include <QHash>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>

#ifdef K8JSON_INCLUDE_GENERATOR
# include <QByteArray>
#endif


namespace K8JSON {

/*
 * quote string to JSON-friendly format, add '"'
 */
QString quote (const QString &str);


/*
 * skip blanks and comments
 * return ptr to first non-blank char or 0 on error
 * 'maxLen' will be changed
 */
const uchar *skipBlanks (const uchar *s, int *maxLength);

/*
 * skip one record
 * the 'record' is either one full field ( field: val)
 * or one list/object.
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *skipRec (const uchar *s, int *maxLength);

/*
 * parse one simple record (f-v pair)
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *parseSimple (QString &fname, QVariant &fvalue, const uchar *s, int *maxLength);

/*
 * parse one record (list or object)
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *parseRec (QVariant &res, const uchar *s, int *maxLength);


#ifdef K8JSON_INCLUDE_GENERATOR
bool generate (QByteArray &res, const QVariant &val, int indent=0);
#endif


}


#endif

