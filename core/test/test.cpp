/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include <QtCore>
#include <QDebug>
#include <QTextCodec>


#include "k8json.h"


static void testCounter () {
  do {

  QFile fl("tc_hist.json");
  if (!fl.open(QIODevice::ReadOnly)) {
    fprintf(stderr, "ERROR: can't open input file!\n");
    return;
  }

  const uchar *fmap = (const uchar *)(fl.map(0, fl.size()));
  int len = fl.size();
  const uchar *sj = K8JSON::skipBlanks(fmap, &len);
  if (!sj) {
    fprintf(stderr, "ERROR: invalid JSON file!\n");
    break;
  }
  if (*sj == '[') sj++;
  if (len < 1) {
    fprintf(stderr, "ERROR: empty JSON file!\n");
    break;
  }
  int cnt = 0;
  QTime st;
  st.start();
  while (len > 0) {
    //int ofs = sj-fmap;
    //fprintf(stderr, "0x%08x  %i\n", ofs, cnt);
    sj = K8JSON::skipRec(sj, &len);
    if (!sj) {
      fprintf(stderr, "ERROR: invalid JSON file!\n");
      break;
    }
    cnt++;
    switch (*sj) {
      case ',': sj++; len--; break;
      case ']': case '}': len = 0; break;
    }
  }
  qDebug() << "time taken (ms):" << st.elapsed();
  fprintf(stdout, "%i messages found.\n", cnt);

  } while (0);
}


static void testReader () {
  do {

  QFile fl("one.json");
  if (!fl.open(QIODevice::ReadOnly)) {
    fprintf(stderr, "ERROR: can't open input file!\n");
    return;
  }

  const uchar *fmap = (const uchar *)(fl.map(0, fl.size()));
  int len = fl.size();
  const uchar *sj = K8JSON::skipBlanks(fmap, &len);
  if (!sj) {
    fprintf(stderr, "ERROR: invalid JSON file!\n");
    break;
  }
  uchar qch = *sj;
  if (qch != '[' && qch != '{') {
    fprintf(stderr, "ERROR: invalid JSON file!\n");
    break;
  }

  int cnt = 0;
  QTime st;
  st.start();

  QVariant val;
  while (sj) {
    //int ofs = sj-fmap;
    //fprintf(stderr, "0x%08x  %i\n", ofs, cnt);
    if (!(sj = K8JSON::skipBlanks(sj, &len))) break;
    val.clear();
    sj = K8JSON::parseRecord(val, sj, &len);
    if (!sj) {
      //fprintf(stderr, "ERROR: invalid JSON file!\n");
      break;
    }
    qDebug() << val;
    QByteArray bo;
    if (!K8JSON::generate(bo, val)) {
      fprintf(stderr, "ERROR: can't generate JSON!\n");
      break;
    }
    qDebug() << bo;
    cnt++;
  }
  qDebug() << "time taken (ms):" << st.elapsed();
  fprintf(stdout, "%i messages found.\n", cnt);


  } while (0);
}


static void testReaderAll () {
  do {

  QFile fl("tc_hist.json");
  if (!fl.open(QIODevice::ReadOnly)) {
    fprintf(stderr, "ERROR: can't open input file!\n");
    return;
  }

  const uchar *fmap = (const uchar *)(fl.map(0, fl.size()));
  int len = fl.size();
  const uchar *sj = K8JSON::skipBlanks(fmap, &len);
  if (!sj) {
    fprintf(stderr, "ERROR: invalid JSON file!\n");
    break;
  }
  uchar qch = *sj;
  if (qch != '[' && qch != '{') {
    fprintf(stderr, "ERROR: invalid JSON file!\n");
    break;
  }
  sj++; len--;
  qch = (qch=='{' ? '}' : ']');

  int cnt = 0;
  QTime st;
  st.start();

  QVariant val;
  while (sj) {
    //int ofs = sj-fmap;
    //fprintf(stderr, "0x%08x  %i\n", ofs, cnt);
    sj = K8JSON::skipBlanks(sj, &len);
    if (len < 2 || (sj && *sj == qch)) break;
    if ((cnt && *sj != ',') || (!cnt && *sj == ',')) {
      fprintf(stderr, "ERROR: invalid JSON file (delimiter)!\n");
      break;
    }
    if (*sj == ',') { sj++; len--; }
    val.clear();
    sj = K8JSON::parseRecord(val, sj, &len);
    if (!sj) {
      fprintf(stderr, "ERROR: invalid JSON file!\n");
      break;
    }
    //qDebug() << val;
    cnt++;
  }
  qDebug() << "time taken (ms):" << st.elapsed();
  fprintf(stdout, "%i messages found.\n", cnt);

  } while (0);
}


static void testReaderAll2 () {
  do {

  QFile fl("tc_hist.json");
  if (!fl.open(QIODevice::ReadOnly)) {
    fprintf(stderr, "ERROR: can't open input file!\n");
    return;
  }

  const uchar *fmap = (const uchar *)(fl.map(0, fl.size()));
  int len = fl.size();
  const uchar *sj = K8JSON::skipBlanks(fmap, &len);
  if (!sj) {
    fprintf(stderr, "ERROR: invalid JSON file!\n");
    break;
  }
  uchar qch = *sj;
  if (qch != '[' && qch != '{') {
    fprintf(stderr, "ERROR: invalid JSON file!\n");
    break;
  }

  int cnt = 0;
  QTime st;
  st.start();

  QVariant val;
  sj = K8JSON::parseRecord(val, sj, &len);
  if (!sj) {
    fprintf(stderr, "ERROR: invalid JSON file!\n");
    break;
  }
  qDebug() << "time taken (ms):" << st.elapsed();
  //qDebug() << val;

  QByteArray bo;
  if (!K8JSON::generate(bo, val)) {
    fprintf(stderr, "ERROR: can't generate JSON file!\n");
    break;
  }
  QFile fo("000.json");
  if (fo.open(QIODevice::WriteOnly)) {
    fo.write(bo);
    fo.write("\n");
    fo.close();
  }

  } while (0);
}


int main (/*int argc, char *argv[]*/) {
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("koi8-u"));
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("koi8-u"));

/*
  const char *str = "\"this is a \\u04e3 test \\x21 string \\120!\\n\"";
  int bp = 0;
  qDebug() << "strlen:" << strlen(str);
  qDebug() << "str:" << str;
  qDebug() << K8JSON::stringLength(str, strlen(str), &bp);
  qDebug() << " " << bp;
//bool parseString (QString &str, const char *s, int charCount, int *bytesProcessed=0);
*/

  //testCounter();
  testReader();
  //testReaderAll();
  //testReaderAll2();

  return 0;
}

