/****************************************************************************
 *  k8json.h
 *
 *  Copyright(c) 2009 by Ketmar // Avalon Group <psyc://ketmar.no-ip.org/~Ketmar>
 *                    by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef K8JSON_QUTIM_H
#define K8JSON_QUTIM_H

#include "libqutim_global.h"

#include <QString>
#include <QVariant>
#include <QByteArray>
class QObject;

namespace qutim_sdk_0_3
{
	namespace Json
	{
		// High level api
		/**
		  * @brief This function quotes string to Json friendly format
		  * and surronds it by \"
		  *
		  * @param str String which would be quoted
		  */
		LIBQUTIM_EXPORT QString quote(const QString &str);
		/**
		  *
		  *
		  *
		  *
		  */
		LIBQUTIM_EXPORT QVariant parse(const QByteArray &data);

		LIBQUTIM_EXPORT void parseToProperties(const QByteArray &json, QObject *obj);

		LIBQUTIM_EXPORT QByteArray generate(const QVariant &data, bool indent = 0);

		// Low level api
		LIBQUTIM_EXPORT bool isValidUtf8(const QByteArray &data, bool zeroInvalid = false);
		LIBQUTIM_EXPORT bool isValidUtf8(const uchar *s, int maxLen, bool zeroInvalid = false);
		LIBQUTIM_EXPORT bool isValidUtf8(const  char *s, int maxLen, bool zeroInvalid = false);

		LIBQUTIM_EXPORT const uchar *skipBlanks(const uchar *s, int *maxLength);
		LIBQUTIM_EXPORT const  char *skipBlanks(const  char *s, int *maxLength);

		LIBQUTIM_EXPORT const uchar *skipRecord(const uchar *s, int *maxLength);
		LIBQUTIM_EXPORT const  char *skipRecord(const  char *s, int *maxLength);

		LIBQUTIM_EXPORT const uchar *parseValue(QVariant &fvalue, const uchar *s, int *maxLength);
		LIBQUTIM_EXPORT const  char *parseValue(QVariant &fvalue, const  char *s, int *maxLength);

		LIBQUTIM_EXPORT const uchar *parseField(QString &fname, QVariant &fvalue, const uchar *s,
												int *maxLength);
		LIBQUTIM_EXPORT const  char *parseField(QString &fname, QVariant &fvalue, const  char *s,
												int *maxLength);

		LIBQUTIM_EXPORT const uchar *parseRecord(QVariant &res, const uchar *s, int *maxLength);
		LIBQUTIM_EXPORT const  char *parseRecord(QVariant &res, const  char *s, int *maxLength);

		typedef bool(*generatorExt)(QString &err, QByteArray &res, const QVariant &val, int indent);

		LIBQUTIM_EXPORT bool generate(QByteArray &res, const QVariant &val, int indent = 0,
									  generatorExt cb = 0, QString *err = 0);

		LIBQUTIM_EXPORT bool generate(QByteArray &res, const QVariant &val, int indent, QString *err);
	}
}

#endif // K8JSON_QUTIM_H
