/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ketmar // Avalon Group <psyc://ketmar.no-ip.org/~Ketmar>
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
//#include <QtDebug>

#ifdef K8JSON_INCLUDE_GENERATOR
# include <QStringList>
#endif

#include "k8json.h"


namespace K8JSON {

static const quint8 utf8Length[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x00-0x0f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x10-0x1f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x20-0x2f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x30-0x3f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x40-0x4f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x50-0x5f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x60-0x6f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //0x70-0x7f
  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, //0x80-0x8f
  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, //0x90-0x9f
  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, //0xa0-0xaf
  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, //0xb0-0xbf
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, //0xc0-0xcf  c0-c1: overlong encoding: start of a 2-byte sequence, but code point <= 127
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, //0xd0-0xdf
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, //0xe0-0xef
  4,4,4,4,4,8,8,8,8,8,8,8,8,8,8,8  //0xf0-0xff
};


QString quote (const QString &str) {
  int len = str.length(), c;
  QString res('"'); res.reserve(len+128);
  for (int f = 0; f < len; f++) {
    QChar ch(str[f]);
    ushort uc = ch.unicode();
    if (uc < 32) {
      // control char
      switch (uc) {
        case '\b': res += "\\b"; break;
        case '\f': res += "\\f"; break;
        case '\n': res += "\\n"; break;
        case '\r': res += "\\r"; break;
        case '\t': res += "\\t"; break;
        default:
          res += "\\u";
          for (c = 4; c > 0; c--) {
            ushort n = (uc>>12)&0x0f;
            n += '0'+(n>9?7:0);
            res += (uchar)n;
          }
          break;
      }
    } else {
      // normal char
      switch (uc) {
        case '"': res += "\\\""; break;
        case '\\': res += "\\\\"; break;
        default: res += ch; break;
      }
    }
  }
  res += '"';
  return res;
}


/*
 * skip blanks and comments
 * return ptr to first non-blank char or 0 on error
 * 'maxLen' will be changed
 */
const uchar *skipBlanks (const uchar *s, int *maxLength) {
  if (!s) return 0;
  int maxLen = *maxLength;
  if (maxLen < 0) return 0;
  while (maxLen > 0) {
    // skip blanks
    uchar ch = *s++; maxLen--;
    if (ch <= ' ') continue;
    // skip comments
    if (ch == '/') {
      if (maxLen < 2) return 0;
      switch (*s) {
        case '/':
          while (maxLen > 0) {
            s++; maxLen--;
            if (s[-1] == '\n') break;
            if (maxLen < 1) return 0;
          }
          break;
        case '*':
          s++; maxLen--; // skip '*'
          while (maxLen > 0) {
            s++; maxLen--;
            if (s[-1] == '*' && s[0] == '/') {
              s++; maxLen--; // skip '/'
              break;
            }
            if (maxLen < 2) return 0;
          }
          break;
        default: return 0; // error
      }
      continue;
    }
    // it must be a token
    s--; maxLen++;
    break;
  }
  // done
  *maxLength = maxLen;
  return s;
}


//FIXME: table?
static inline bool isValidIdChar (const uchar ch) {
  return (
    ch == '$' || ch == '_' || ch >= 128 ||
    (ch >= '0' && ch <= '9') ||
    (ch >= 'A' && ch <= 'Z') ||
    (ch >= 'a' && ch <= 'z')
  );
}

/*
 * skip one record
 * the 'record' is either one full field ( field: val)
 * or one list/object.
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *skipRec (const uchar *s, int *maxLength) {
  if (!s) return 0;
  int maxLen = *maxLength;
  if (maxLen < 0) return 0;
  int fieldNameSeen = 0;
  while (maxLen > 0) {
    // skip blanks
    if (!(s = skipBlanks(s, &maxLen))) return 0;
    if (!maxLen) break;
    uchar qch, ch = *s++; maxLen--;
    // fieldNameSeen<1: no field name was seen
    // fieldNameSeen=1: waiting for ':'
    // fieldNameSeen=2: field name was seen, ':' was seen too, waiting for value
    // fieldNameSeen=3: everything was seen, waiting for terminator
    if (ch == ':') {
      if (fieldNameSeen != 1) return 0; // wtf?
      fieldNameSeen++;
      continue;
    }
    // it must be a token, skip it
    bool again = false;
    switch (ch) {
      case '{': case '[':
        if (fieldNameSeen == 1) return 0; // waiting for delimiter; error
        fieldNameSeen = 3;
        // recursive skip
        qch = (ch=='{' ? '}' : ']'); // end char
        for (;;) {
          if (!(s = skipRec(s, &maxLen))) return 0;
          if (maxLen < 1) return 0; // no closing char
          ch = *s++; maxLen--;
          if (ch == ',') continue; // skip next field/value pair
          if (ch == qch) break; // end of the list or object
          return 0; // error!
        }
        break;
      case ']': case '}': case ',': // terminator
        if (fieldNameSeen != 3) return 0; // incomplete field
        s--; maxLen++; // back to this char
        break;
      case '"': case '\x27': // string
        if (fieldNameSeen == 1 || fieldNameSeen > 2) return 0; // no delimiter
        fieldNameSeen++;
        qch = ch;
        while (*s && maxLen > 0) {
          ch = *s++; maxLen--;
          if (ch == qch) { s--; maxLen++; break; }
          if (ch != '\\') continue;
          if (maxLen < 2) return 0; // char and quote
          ch = *s++; maxLen--;
          switch (ch) {
            case 'u':
              if (maxLen < 5) return 0;
              if (s[0] == qch || s[0] == '\\' || s[1] == qch || s[1] == '\\') return 0;
              s += 2; maxLen -= 2;
              // fallthru
            case 'x':
              if (maxLen < 3) return 0;
              if (s[0] == qch || s[0] == '\\' || s[1] == qch || s[1] == '\\') return 0;
              s += 2; maxLen -= 2;
              break;
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
              if (maxLen < 4) return 0;
              if (s[0] == qch || s[0] == '\\' || s[1] == qch || s[1] == '\\' || s[2] == qch || s[2] == '\\') return 0;
              s += 3; maxLen -= 3;
              break;
            default: ; // escaped char already skiped
          }
        }
        if (maxLen < 1 || *s != qch) return 0; // error
        s++; maxLen--; // skip quote
        again = true;
        break;
      default: // we can check for punctuation here, but i'm too lazy to do it
        if (fieldNameSeen == 1 || fieldNameSeen > 2) return 0; // no delimiter
        fieldNameSeen++;
        if (isValidIdChar(ch)) {
          // good token, skip it
          again = true; // just a token, skip it and go on
          // check for valid utf8?
          while (*s && maxLen > 0) {
            ch = *s++; maxLen--;
            if (ch != '.' && !isValidIdChar(ch)) {
              s--; maxLen++;
              break;
            }
          }
        } else return 0; // error
    }
    if (!again) break;
  }
  if (fieldNameSeen != 3) return 0;
  // skip blanks
  if (!(s = skipBlanks(s, &maxLen))) return 0;
  // done
  *maxLength = maxLen;
  return s;
}


/*
 * parse json-quoted string. a little relaxed parsing, it allows "'"-quoted strings,
 * whereas json standard does not. also \x and \nnn are allowed.
 * return position after the string or 0
 * 's' should point to the quote char on entry
 */
static const uchar *parseString (QString &str, const uchar *s, int *maxLength) {
  if (!s) return 0;
  int maxLen = *maxLength;
  if (maxLen < 2) return 0;
  uchar ch = 0, qch = *s++; maxLen--;
  if (qch != '"' && qch != '\x27') return 0;
  // calc string length and check string for correctness
  int strLen = 0, tmpLen = maxLen;
  const uchar *tmpS = s;
  while (tmpLen > 0) {
    ch = *tmpS++; tmpLen--; strLen++;
    if (ch == qch) break;
    if (ch != '\\') {
      // ascii or utf-8
      quint8 t = utf8Length[ch];
      if (t&0x08) return 0; // invalid utf-8 sequence
      if (t) {
        // utf-8
        if (tmpLen < t) return 0; // invalid utf-8 sequence
        while (--t) {
          quint8 b = *tmpS++; tmpLen--;
          if (utf8Length[b] != 9) return 0; // invalid utf-8 sequence
        }
      }
      continue;
    }
    // escape sequence
    ch = *tmpS++; tmpLen--; //!strLen++;
    if (tmpLen < 2) return 0;
    int hlen = 0;
    switch (ch) {
      case 'u': hlen = 4;
      case 'x':
        if (!hlen) hlen = 2;
        if (tmpLen < hlen+1) return 0;
        while (hlen-- > 0) {
          ch = *tmpS++; tmpLen--;
          if (ch >= 'a') ch -= 32;
          if (!(ch >= '0' && ch <= '9') && !(ch >= 'A' && ch <= 'F')) return 0;
        }
        hlen = 0;
        break;
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': // octal char
        if (tmpLen < 3) return 0;
        for (hlen = 2; hlen > 0; hlen--) {
          ch = *tmpS++; tmpLen--;
          if (ch < '0' || ch > '7') return 0;
        }
        break;
      case '"': case '\x27': ch = 0; break;
      default: ; // one escaped char; will be checked later
    }
  }
  if (ch != qch) return 0; // no terminating quote
  //
  str.reserve(str.length()+strLen+1);
  ch = 0;
  //fprintf(stderr, "\n");
  while (maxLen > 0) {
    ch = *s++; maxLen--;
    //fprintf(stderr, "[%c] %i\n", ch, maxLen);
    if (ch == qch) break;
    if (ch != '\\') {
      // ascii or utf-8
      quint8 t = utf8Length[ch];
      if (!t) str.append(ch); // ascii
      else {
        // utf-8
        int u = 0; s--; maxLen++;
        while (t--) {
          quint8 b = *s++; maxLen--;
          u = (u<<6)+(b&0x3f);
        }
        if (u > 0x10ffff) u &= 0xffff;
        if ((u >= 0xd800 && u <= 0xdfff) || // utf16/utf32 surrogates
            (u >= 0xfdd0 && u <= 0xfdef) || // just for fun
            (u >= 0xfffe && u <= 0xffff)) continue; // bad unicode, skip it
        QChar zch(u);
        str.append(zch);
      }
      continue;
    }
    ch = *s++; maxLen--; // at least one char left here
    int uu = 0; int escCLen = 0;
    switch (ch) {
      case 'u': // unicode char, 4 hex digits
        //fprintf(stderr, "escape U\n");
        escCLen = 4;
        // fallthru
      case 'x': { // ascii char, 2 hex digits
        if (!escCLen) {
          //fprintf(stderr, "escape X\n");
          escCLen = 2;
        }
        //fprintf(stderr, "escape #%i\n", escCLen);
        while (escCLen-- > 0) {
          ch = *s++; maxLen--;
          if (ch >= 'a') ch -= 32;
          uu = uu*16+ch-'0';
          if (ch >= 'A'/* && ch <= 'F'*/) uu -= 7;
        }
        //fprintf(stderr, " code: %04x\n", uu);
        if (uu > 0x10ffff) uu &= 0xffff;
        if ((uu >= 0xd800 && uu <= 0xdfff) || // utf16/utf32 surrogates
            (uu >= 0xfdd0 && uu <= 0xfdef) || // just for fun
            (uu >= 0xfffe && uu <= 0xffff)) uu = -1; // bad unicode, skip it
        if (uu >= 0) {
          QChar zch(uu);
          str.append(zch);
        }
        } break;
      case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': { // octal char
        //fprintf(stderr, "escape O\n");
        s--; maxLen++;
        uu = 0;
        for (int f = 3; f > 0; f--) {
          ch = *s++; maxLen--;
          uu = uu*8+ch-'0';
        }
        QChar zch(uu);
        str.append(zch);
        } break;
      case '\\': str.append('\\'); break;
      case '/': str.append('/'); break;
      case 'b': str.append('\b'); break;
      case 'f': str.append('\f'); break;
      case 'n': str.append('\n'); break;
      case 'r': str.append('\r'); break;
      case 't': str.append('\t'); break;
      case '"': case '\x27': str.append(ch); /*ch = 0;*/ break;
      default:
        // non-standard!
        if (ch != '\t' && ch != '\r' && ch != '\n') {
          //fprintf(stderr, "escape BAD [%c]\n", ch);
          return 0; // all other chars are BAD
        }
        str.append(ch);
    }
  }
  //fprintf(stderr, "[%c] [%c]\n", ch, qch);
  if (ch != qch) return 0;
  *maxLength = maxLen;
  return s;
}


/*
 * parse identifier
 */
static const uchar *parseId (QString &str, const uchar *s, int *maxLength) {
  if (!s) return 0;
  int maxLen = *maxLength;
  if (maxLen < 1) return 0;
  uchar ch = 0;
  // calc string length and check string for correctness
  int strLen = 0, tmpLen = maxLen;
  const uchar *tmpS = s;
  while (tmpLen > 0) {
    ch = *tmpS++; tmpLen--;
    if (!isValidIdChar(ch)) {
      if (!strLen) return 0;
      break;
    }
    strLen++;
    // ascii or utf-8
    quint8 t = utf8Length[ch];
    if (t&0x08) return 0; // invalid utf-8 sequence
    if (t) {
      // utf-8
      if (tmpLen < t) return 0; // invalid utf-8 sequence
      while (--t) {
        quint8 b = *tmpS++; tmpLen--;
        if (utf8Length[b] != 9) return 0; // invalid utf-8 sequence
      }
    }
    continue;
  }
/*
  str = "true";
  while (isValidIdChar(*s)) { s++; (*maxLength)--; }
  return s;
*/
  //
  str.reserve(str.length()+strLen+1);
  ch = 0;
  while (maxLen > 0) {
    ch = *s++; maxLen--;
    if (!isValidIdChar(ch)) { s--; maxLen++; break; }
    // ascii or utf-8
    quint8 t = utf8Length[ch];
    if (!t) str.append(ch); // ascii
    else {
      // utf-8
      int u = 0; s--; maxLen++;
      while (t--) {
        quint8 ch = *s++; maxLen--;
        u = (u<<6)+(ch&0x3f);
      }
      if (u > 0x10ffff) u &= 0xffff;
      if ((u >= 0xd800 && u <= 0xdfff) || // utf16/utf32 surrogates
          (u >= 0xfdd0 && u <= 0xfdef) || // just for fun
          (u >= 0xfffe && u <= 0xffff)) continue; // bad unicode, skip it
      QChar zch(u);
      str.append(zch);
    }
    continue;
  }
  *maxLength = maxLen;
  return s;
}


/*
 * parse number
 */
static const uchar *parseNumber (QVariant &num, const uchar *s, int *maxLength) {
  if (!s) return 0;
  int maxLen = *maxLength;
  if (maxLen < 1) return 0;
  uchar ch = *s++; maxLen--;
  // check for negative number
  bool negative = false, fr = false;
  if (ch == '-') {
    if (maxLen < 1) return 0;
    ch = *s++; maxLen--;
    negative = true;
  }
  if (ch < '0' || ch > '9') return 0; // invalid integer part
  double fnum = 0.0;
  // parse integer part
  while (ch >= '0' && ch <= '9') {
    ch -= '0';
    fnum = fnum*10+ch;
    if (!maxLen) goto done;
    ch = *s++; maxLen--;
  }
  // check for fractional part
  if (ch == '.') {
    // parse fractional part
    if (maxLen < 1) return 0;
    ch = *s++; maxLen--;
    double frac = 0.1; fr = true;
    if (ch < '0' || ch > '9') return 0; // invalid fractional part
    while (ch >= '0' && ch <= '9') {
      ch -= '0';
      fnum += frac*ch;
      if (!maxLen) goto done;
      frac /= 10;
      ch = *s++; maxLen--;
    }
  }
  // check for exp part
  if (ch == 'e' || ch == 'E') {
    if (maxLen < 1) return 0;
    // check for exp sign
    bool expNeg = false;
    ch = *s++; maxLen--;
    if (ch == '+' || ch == '-') {
      if (maxLen < 1) return 0;
      expNeg = (ch == '-');
      ch = *s++; maxLen--;
    }
    // check for exp digits
    if (ch < '0' || ch > '9') return 0; // invalid exp
    quint32 exp = 0; // 64? %-)
    while (ch >= '0' && ch <= '9') {
      exp = exp*10+ch-'0';
      if (!maxLen) { s++; maxLen--; break; }
      ch = *s++; maxLen--;
    }
    while (exp--) {
      if (expNeg) fnum /= 10; else fnum *= 10;
    }
    if (expNeg && !fr) {
      if (fnum > 2147483647.0 || ((qint64)fnum)*1.0 != fnum) fr = true;
    }
  }
  s--; maxLen++;
done:
  if (!fr && fnum > 2147483647.0) fr = true;
  if (negative) fnum = -fnum;
  if (fr) num = fnum; else num = (qint32)fnum;
  *maxLength = maxLen;
  return s;
}


static const QString sTrue("true");
static const QString sFalse("false");
static const QString sNull("null");


/*
 * parse one simple record (f-v pair)
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *parseSimple (QString &fname, QVariant &fvalue, const uchar *s, int *maxLength) {
  if (!s) return 0;
  //int maxLen = *maxLength;
  fname.clear();
  fvalue.clear();
  if (!(s = skipBlanks(s, maxLength))) return 0;
  if (*maxLength < 1) return 0;
  uchar ch = *s;
  // field name
  if (isValidIdChar(ch)) {
    // id
    if (!(s = parseId(fname, s, maxLength))) return 0;
  } else if (ch == '"' || ch == '\x27') {
    // string
    if (!(s = parseString(fname, s, maxLength))) return 0;
    //if (fname.isEmpty()) return 0;
  }
  // ':'
  if (!(s = skipBlanks(s, maxLength))) return 0;
  if (*maxLength < 2 || *s != ':') return 0;
  s++; (*maxLength)--;
  // field value
  if (!(s = skipBlanks(s, maxLength))) return 0;
  if (*maxLength < 1) return 0;
  ch = *s;
  if (ch == '-' || (ch >= '0' && ch <= '9')) {
    // number
    if (!(s = parseNumber(fvalue, s, maxLength))) return 0;
  } else if (isValidIdChar(ch)) {
    // identifier (true/false/null)
    QString tmp;
    //s--; (*maxLength)++;
    //while (isValidIdChar(*s)) { s++; (*maxLength)--; }
    //tmp = "true";
    if (!(s = parseId(tmp, s, maxLength))) return 0;
    if (tmp == sTrue) fvalue = true;
    else if (tmp == sFalse) fvalue = false;
    else if (tmp != sNull) return 0; // invalid id
  } else if (ch == '"' || ch == '\x27') {
    // string
    QString tmp;
    if (!(s = parseString(tmp, s, maxLength))) return 0;
    fvalue = tmp;
  } else if (ch == '{' || ch == '[') {
    // object or list
    if (!(s = parseRec(fvalue, s, maxLength))) return 0;
  } else return 0; // unknown
  if (!(s = skipBlanks(s, maxLength))) return 0;
  return s;
}


/*
 * parse one record (list or object)
 * return ptr to the first non-blank char after the record (or 0)
 * 'maxLen' will be changed
 */
const uchar *parseRec (QVariant &res, const uchar *s, int *maxLength) {
  if (!s) return 0;
  //int maxLen = *maxLength;
  res.clear();
  if (!(s = skipBlanks(s, maxLength))) return 0;
  if (*maxLength < 1) return 0;
  QString str;
  QVariant val;
  // field name or list/object start
  uchar ch = *s;
  switch (ch) {
    case '{': { // object
      if (*maxLength < 2) return 0;
      s++; (*maxLength)--;
      QVariantMap obj;
      for (;;) {
        str.clear();
        if (!(s = parseSimple(str, val, s, maxLength))) return 0;
        obj[str] = val;
        if (*maxLength > 0) {
          ch = *s++; (*maxLength)--;
          if (ch == ',') continue; // next field/value pair
          if (ch == '}') break; // end of the object
        }
        // error
        s = 0;
        break;
      }
      res = obj;
      return s;
      } // it will never comes here
    case '[': { // list
      if (*maxLength < 2) return 0;
      s++; (*maxLength)--;
      QVariantList lst;
      for (;;) {
        if (!(s = skipBlanks(s, maxLength))) return 0;
        if (*maxLength > 0) {
          // value
          bool err = false;
          ch = *s;
          if (ch == '[' || ch == '{') {
            // list/object
            const uchar *tmp = s;
            if (!(s = parseRec(val, s, maxLength))) {
              QString st(QByteArray((const char *)tmp, 64));
              err = true;
            } else lst << val;
          } else if (isValidIdChar(ch)) {
            // identifier
            str.clear();
            if (!(s = parseId(str, s, maxLength))) {
              err = true;
            } else {
              if (str == sTrue) lst << true;
              else if (str == sFalse) lst << false;
              else if (str == sNull) lst << QVariant();
              else {
                err = true;
              }
            }
          } else if (ch == '"' || ch == '\x27') {
            // string
            str.clear();
            if (!(s = parseString(str, s, maxLength))) {
              err = true;
            } else lst << str;
          } else if (ch == '-' || (ch >= '0' && ch <= '9')) {
            // number
            if (!(s = parseNumber(val, s, maxLength))) {
              err = true;
            } else lst << val;
          } else {
            err = true;
          }
          //
          if (!err) {
            if ((s = skipBlanks(s, maxLength))) {
              if (*maxLength > 0) {
                ch = *s++; (*maxLength)--;
                if (ch == ',') continue; // next value
                if (ch == ']') break; // end of the list
              }
            }
          }
        }
        // error
        s = 0;
        break;
      }
      res = lst;
      return s;
      } // it will never comes here
  }
  if (!(s = parseSimple(str, val, s, maxLength))) return 0;
  QVariantMap obj;
  obj[str] = val;
  res = obj;
  return s;
}


#ifdef K8JSON_INCLUDE_GENERATOR
bool generate (QByteArray &res, const QVariant &val, int indent) {
  switch (val.type()) {
    case QVariant::Invalid: res += "null"; break;
    case QVariant::Bool: res += (val.toBool() ? "true" : "false"); break;
    case QVariant::Char: res += quote(QString(val.toChar())).toUtf8(); break;
    case QVariant::Double: res += QString::number(val.toDouble()).toAscii(); break; //CHECKME: is '.' always '.'?
    case QVariant::Int: res += QString::number(val.toInt()).toAscii(); break;
    case QVariant::String: res += quote(val.toString()).toUtf8(); break;
    case QVariant::UInt: res += QString::number(val.toUInt()).toAscii(); break;
    case QVariant::ULongLong: res += QString::number(val.toULongLong()).toAscii(); break;
    case QVariant::Map: {
      //for (int c = indent; c > 0; c--) res += ' ';
      res += "{";
      indent++; bool comma = false;
      QVariantMap m(val.toMap());
      QVariantMap::const_iterator i;
      for (i = m.constBegin(); i != m.constEnd(); ++i) {
        if (comma) res += ",\n"; else { res += '\n'; comma = true; }
        for (int c = indent; c > 0; c--) res += ' ';
        res += quote(i.key()).toUtf8();
        res += ": ";
        if (!generate(res, i.value(), indent)) return false;
      }
      indent--;
      if (comma) {
        res += '\n';
        for (int c = indent; c > 0; c--) res += ' ';
      }
      res += '}';
      indent--;
      } break;
    case QVariant::List: {
      //for (int c = indent; c > 0; c--) res += ' ';
      res += "[";
      indent++; bool comma = false;
      QVariantList m(val.toList());
      foreach (const QVariant &v, m) {
        if (comma) res += ",\n"; else { res += '\n'; comma = true; }
        for (int c = indent; c > 0; c--) res += ' ';
        if (!generate(res, v, indent)) return false;
      }
      indent--;
      if (comma) {
        res += '\n';
        for (int c = indent; c > 0; c--) res += ' ';
      }
      res += ']';
      indent--;
      } break;
    case QVariant::StringList: {
      //for (int c = indent; c > 0; c--) res += ' ';
      res += "[";
      indent++; bool comma = false;
      QStringList m(val.toStringList());
      foreach (const QString &v, m) {
        if (comma) res += ",\n"; else { res += '\n'; comma = true; }
        for (int c = indent; c > 0; c--) res += ' ';
        res += quote(v).toUtf8();
      }
      indent--;
      if (comma) {
        res += '\n';
        for (int c = indent; c > 0; c--) res += ' ';
      }
      res += ']';
      indent--;
      } break;
    default: return false;
  }
  return true;
}
#endif


}

