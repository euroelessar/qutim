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

#include "libqutim_global.h"

#define K8JSON_INCLUDE_COMPLEX_GENERATOR
#define K8JSON_INCLUDE_GENERATOR

// Low level api for parsing json files
#include <QHash>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>

#if defined(K8JSON_INCLUDE_COMPLEX_GENERATOR) || defined(K8JSON_INCLUDE_GENERATOR)
# include <QByteArray>
#endif


namespace K8JSON
{
   /*
	* quote string to JSON-friendly format, add '"'
	*/
	LIBQUTIM_EXPORT QString quote (const QString &str);

   /*
	* check if given (const uchar *) represents valid UTF-8 sequence
	* NULL (or empty) s is not valid
	* sequence ends on '\0' if zeroInvalid==false
	*/
	LIBQUTIM_EXPORT bool isValidUtf8 (const uchar *s, int maxLen, bool zeroInvalid=false);


   /*
	* skip blanks and comments
	* return ptr to first non-blank char or 0 on error
	* 'maxLen' will be changed
	*/
	LIBQUTIM_EXPORT const uchar *skipBlanks (const uchar *s, int *maxLength);

	/*
	 * skip one record
	 * the 'record' is either one full field ( field: val)
	 * or one list/object.
	 * return ptr to the first non-blank char after the record (or 0)
	 * 'maxLen' will be changed
	 */
	LIBQUTIM_EXPORT const uchar *skipRec (const uchar *s, int *maxLength);

	/*
	 * parse field value
	 * return ptr to the first non-blank char after the value (or 0)
	 * 'maxLen' will be changed
	 */
	LIBQUTIM_EXPORT const uchar *parseValue (QVariant &fvalue, const uchar *s, int *maxLength);


	/*
	 * parse one field (f-v pair)
	 * return ptr to the first non-blank char after the record (or 0)
	 * 'maxLen' will be changed
	 */
	LIBQUTIM_EXPORT const uchar *parseField (QString &fname, QVariant &fvalue, const uchar *s, int *maxLength);

	/*
	 * parse one record (list or object)
	 * return ptr to the first non-blank char after the record (or 0)
	 * 'maxLen' will be changed
	 */
	LIBQUTIM_EXPORT const uchar *parseRecord (QVariant &res, const uchar *s, int *maxLength);


#ifdef K8JSON_INCLUDE_GENERATOR
	/*
	 * generate JSON text from variant
	 * 'err' must be empty (generateEx() will not clear it)
	 * return false on error
	 */
	LIBQUTIM_EXPORT bool generateEx (QString &err, QByteArray &res, const QVariant &val, int indent=0);

	/*
	 * same as above, but without error message
	 */
	LIBQUTIM_EXPORT bool generate (QByteArray &res, const QVariant &val, int indent=0);
#endif


#ifdef K8JSON_INCLUDE_COMPLEX_GENERATOR
	/*
	 * callback for unknown variant type
	 * return false and set 'err' on error
	 * or return true and *add* converted value (valid sequence of utf-8 bytes) to res
	 */
	typedef bool (*generatorCB) (void *udata, QString &err, QByteArray &res, const QVariant &val, int indent);

	/*
	 * generate JSON text from variant
	 * 'err' must be empty (generateEx() will not clear it)
	 * return false on error
	 */
	LIBQUTIM_EXPORT bool generateExCB (void *udata, generatorCB cb, QString &err, QByteArray &res, const QVariant &val, int indent=0);

	/*
	 * same as above, but without error message
	 */
	LIBQUTIM_EXPORT bool generateCB (void *udata, generatorCB cb, QByteArray &res, const QVariant &val, int indent=0);
#endif
}

#endif // K8JSON_QUTIM_H
