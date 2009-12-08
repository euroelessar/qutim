/****************************************************************************
 *  k8json.h
 *
 *  Copyright (c) 2009 by Ketmar // Avalon Group <psyc://ketmar.no-ip.org/~Ketmar>
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

#ifndef K8JSON_QUTIM_H
#define K8JSON_QUTIM_H

#include <QHash>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QByteArray>
#include "libqutim_global.h"

// Low level api for parsing json files

namespace K8JSON
{
	/*
	 * quote string to JSON-friendly format, add '"'
	 */
	LIBQUTIM_EXPORT QString quote(const QString &str);

	/*
	 * skip blanks and comments
	 * return ptr to first non-blank char or 0 on error
	 * 'maxLen' will be changed
	 */
	LIBQUTIM_EXPORT const uchar *skipBlanks(const uchar *s, int *maxLength);

	/*
	 * skip one record
	 * the 'record' is either one full field ( field: val)
	 * or one list/object.
	 * return ptr to the first non-blank char after the record (or 0)
	 * 'maxLen' will be changed
	 */
	LIBQUTIM_EXPORT const uchar *skipRec(const uchar *s, int *maxLength);

	/*
	 * parse one simple record (f-v pair)
	 * return ptr to the first non-blank char after the record (or 0)
	 * 'maxLen' will be changed
	 */
	LIBQUTIM_EXPORT const uchar *parseSimple(QString &fname, QVariant &fvalue, const uchar *s, int *maxLength);

	/*
	 * parse one record (list or object)
	 * return ptr to the first non-blank char after the record (or 0)
	 * 'maxLen' will be changed
	 */
	LIBQUTIM_EXPORT const uchar *parseRecord(QVariant &res, const uchar *s, int *maxLength);

	LIBQUTIM_EXPORT bool generate(QByteArray &res, const QVariant &val, int indent=0);
}

#endif // K8JSON_QUTIM_H
