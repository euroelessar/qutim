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
 *   (at your option) any later version.                                   *
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
	/**
	* @brief Namepsace provides both low and high levels for parsing and
	* generating JavaScript Object Notations.
	*
	* @note Parser is able to work only with utf-8 strings for better perfomance
	*/
	namespace Json
	{
		// High level api
		/**
		* @brief This function quotes string to Json friendly format
		* and surronds it by quotes;
		* @param str String which would be quoted
		*/
		LIBQUTIM_EXPORT QString quote(const QString &str);
		/**
		* @brief Parse JSON data to QVariant
		* @param data String with JSON data
		* @return Result of parsing, QVariant::Null if there was an error
		*/
		LIBQUTIM_EXPORT QVariant parse(const QByteArray &data);
		/**
		* @brief Parse JSON data and put it's values to object as properties
		* @param json String with JSON data
		* @param obj Object which properties will be changed to JSON's object
		*/
		LIBQUTIM_EXPORT void parseToProperties(const QByteArray &json, QObject *obj);
		/**
		* @brief Generate JSON string from QVariant
		* @param data QVariant with data
		* @param indent Identation of new lines
		* @return JSON string with data
		*/
		LIBQUTIM_EXPORT QByteArray generate(const QVariant &data, int indent = 0);

		// Low level api
		/**
		* @brief Check if given string represents valid UTF-8 sequence
		*
		* @param s String to be checked
		* @param maxLen Maximum length of string
		* @param zeroInvalid If @b true check will be continued after finding null-symbol
		* @return @b True if input is not empty and is valid UTF-8 sequence
		*/
		LIBQUTIM_EXPORT bool isValidUtf8(const uchar *s, int maxLen, bool zeroInvalid = false);
		/**
		* @brief Convience fucntion for @ref isValidUtf8
		*/
		LIBQUTIM_EXPORT bool isValidUtf8(const QByteArray &data, bool zeroInvalid = false);
		/**
		* @brief Convience fucntion for @ref isValidUtf8
		*/
		LIBQUTIM_EXPORT bool isValidUtf8(const  char *s, int maxLen, bool zeroInvalid = false);

		/**
		* @brief Skip blanks and comments
		*
		* @param s String with JSON data
		* @param maxLength String length, will be changed after skiping
		* @return Pointer to first non-blak char or null-pointer if error
		* @note Blank is any character with ansii-code lower or equal to space
		*/
		LIBQUTIM_EXPORT const uchar *skipBlanks(const uchar *s, int *maxLength);
		/**
		* @brief Convience fucntion for @ref skipBlanks
		*/
		LIBQUTIM_EXPORT const  char *skipBlanks(const  char *s, int *maxLength);

		/**
		* @brief Skip one record.
		* The 'record' is either one full field (field: val) or one list/object.
		* @param s String with JSON data
		* @param maxLength String length, will be changed after skiping
		* @return Pointer to first non-blak char after the record or null-pointer if error
		*/
		LIBQUTIM_EXPORT const uchar *skipRecord(const uchar *s, int *maxLength);
		/**
		* @brief Convience fucntion for @ref skipRecord
		*/
		LIBQUTIM_EXPORT const  char *skipRecord(const  char *s, int *maxLength);

		/**
		* @brief Parse field value
		* @param fvalue Result of parsing
		* @param s String with JSON data
		* @param maxLength String length, will be changed after skiping
		* @return Pointer to first non-blak char after the value or null-pointer if error
		*/
		LIBQUTIM_EXPORT const uchar *parseValue(QVariant &fvalue, const uchar *s, int *maxLength);
		/**
		* @brief Convience fucntion for @ref parseValue
		*/
		LIBQUTIM_EXPORT const  char *parseValue(QVariant &fvalue, const  char *s, int *maxLength);

		/**
		* @brief Parse one field (field-value pair)
		* @param fname Field's name
		* @param fvalue Field's value
		* @param s String with JSON data
		* @param maxLength String length, will be changed after skiping
		* @return Pointer to first non-blak char or null-pointer if error
		*/
		LIBQUTIM_EXPORT const uchar *parseField(QString &fname, QVariant &fvalue, const uchar *s,
												int *maxLength);
		/**
		* @brief Convience fucntion for @ref parseField
		*/
		LIBQUTIM_EXPORT const  char *parseField(QString &fname, QVariant &fvalue, const  char *s,
												int *maxLength);

		/**
		* @brief Parse one record (list or object)
		* @param res Result of parsing
		* @param s String with JSON data
		* @param maxLength String length, will be changed after skiping
		* @return Pointer to first non-blak char or null-pointer if error
		*/
		LIBQUTIM_EXPORT const uchar *parseRecord(QVariant &res, const uchar *s, int *maxLength);
		/**
		* @brief Convience fucntion for @ref parseRecord
		*/
		LIBQUTIM_EXPORT const  char *parseRecord(QVariant &res, const  char *s, int *maxLength);

		typedef bool(*generatorExt)(QString &err, QByteArray &res, const QVariant &val, int indent);

		LIBQUTIM_EXPORT bool generate(QByteArray &res, const QVariant &val, int indent = 0,
									  generatorExt cb = 0, QString *err = 0);

		LIBQUTIM_EXPORT bool generate(QByteArray &res, const QVariant &val, int indent, QString *err);
	}
}

#endif // K8JSON_QUTIM_H
