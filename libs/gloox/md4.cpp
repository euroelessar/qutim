/*
  Copyright (c) 2006-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

/*
  This class is based on a C implementation of the MD5 algorithm written by
  L. Peter Deutsch.
  The full notice as shipped with the original verson is included below.
*/

/*
  Copyright (C) 1999, 2000, 2002 Aladdin Enterprises.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  L. Peter Deutsch
  ghost@aladdin.com

 */
/* $Id: md5.c,v 1.6 2002/04/13 19:20:28 lpd Exp $ */
/*
  Independent implementation of MD5 (RFC 1321).

  This code implements the MD5 Algorithm defined in RFC 1321, whose
  text is available at
        http://www.ietf.org/rfc/rfc1321.txt
  The code is derived from the text of the RFC, including the test suite
  (section A.5) but excluding the rest of Appendix A.  It does not include
  any code or documentation that is identified in the RFC as being
  copyrighted.

  The original and principal author of md5.c is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):

  2002-04-13 lpd Clarified derivation from RFC 1321; now handles byte order
        either statically or dynamically; added missing #include <string.h>
        in library.
  2002-03-11 lpd Corrected argument list for main(), and added int return
        type, in test program and T value program.
  2002-02-21 lpd Added missing #include <stdio.h> in test program.
  2000-07-03 lpd Patched to eliminate warnings about "constant is
        unsigned in ANSI C, signed in traditional"; made test program
        self-checking.
  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5).
  1999-05-03 lpd Original version.
 */

#include "config.h"

#include "md4.h"

#include <cstdio>
#include <string.h>

#include <cstdio> // [s]print[f]

namespace gloox
{
// #undef BYTE_ORDER    /* 1 = big-endian, -1 = little-endian, 0 = unknown */
// #ifdef ARCH_IS_BIG_ENDIAN
// #  define BYTE_ORDER (ARCH_IS_BIG_ENDIAN ? 1 : -1)
// #else
// #  define BYTE_ORDER 0
// #endif

#undef BYTE_ORDER
#define BYTE_ORDER 0

  const unsigned char MD4::pad[64] =
  {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };

  const std::string MD4::md4( const std::string& data )
  {
    MD4 md4;
    md4.feed( data );
    return md4.hex();
  }

  MD4::MD4()
    : m_finished( false )
  {
    init();
  }

  MD4::~MD4()
  {
  }

  void MD4::process( const unsigned char* data /*[64]*/ )
  {
    unsigned int a = m_state.abcd[0];
    unsigned int b = m_state.abcd[1];
    unsigned int c = m_state.abcd[2];
    unsigned int d = m_state.abcd[3];
    unsigned int t;
#if BYTE_ORDER > 0
    /* Define storage only for big-endian CPUs. */
    unsigned int X[16];
#else
    /* Define storage for little-endian or both types of CPUs. */
    unsigned int xbuf[16];
    const unsigned int *X;
#endif

    {
#if BYTE_ORDER == 0
      /*
      * Determine dynamically whether this is a big-endian or
      * little-endian machine, since we can use a more efficient
      * algorithm on the latter.
      */
      static const int w = 1;

      if( *((const unsigned char *)&w) ) /* dynamic little-endian */
#endif
#if BYTE_ORDER <= 0             /* little-endian */
      {
        /*
        * On little-endian machines, we can process properly aligned
        * data without copying it.
        */
        if( !((data - (const unsigned char*)0) & 3) )
        {
          /* data are properly aligned */
          X = (const unsigned int*)data;
        }
        else
        {
          /* not aligned */
          memcpy( xbuf, data, 64 );
          X = xbuf;
        }
      }
#endif
#if BYTE_ORDER == 0
      else // dynamic big-endian
#endif
#if BYTE_ORDER >= 0 // big-endian
      {
        /*
        * On big-endian machines, we must arrange the bytes in the
        * right order.
        */
        const unsigned char* xp = data;
        int i;

#  if BYTE_ORDER == 0
        X = xbuf; // (dynamic only)
#  else
#    define xbuf X  /* (static only) */
#  endif
        for( i = 0; i < 16; ++i, xp += 4 )
          xbuf[i] = xp[0] + ( xp[1] << 8 ) + ( xp[2] << 16 ) + ( xp[3] << 24 );
      }
#endif
    }

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

    /* Round 1. */
    /* Let [abcd k s] denote the operation
       a = (a + F(b,c,d) + X[k] <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s)\
  t = a + F(b,c,d) + X[k];\
  a = ROTATE_LEFT(t, s)
    /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  3);
    SET(d, a, b, c,  1,  7);
    SET(c, d, a, b,  2, 11);
    SET(b, c, d, a,  3, 19);
    SET(a, b, c, d,  4,  3);
    SET(d, a, b, c,  5,  7);
    SET(c, d, a, b,  6, 11);
    SET(b, c, d, a,  7, 19);
    SET(a, b, c, d,  8,  3);
    SET(d, a, b, c,  9,  7);
    SET(c, d, a, b, 10, 11);
    SET(b, c, d, a, 11, 19);
    SET(a, b, c, d, 12,  3);
    SET(d, a, b, c, 13,  7);
    SET(c, d, a, b, 14, 11);
    SET(b, c, d, a, 15, 19);
#undef SET

     /* Round 2. */
     /* Let [abcd k s ] denote the operation
          a = (a + G(b,c,d) + X[k] + 0x5A827999) <<< s. */
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define SET(a, b, c, d, k, s)\
  t = a + G(b,c,d) + X[k] + 0x5A827999;\
  a = ROTATE_LEFT(t, s)
     /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  3);
    SET(d, a, b, c,  4,  5);
    SET(c, d, a, b,  8,  9);
    SET(b, c, d, a, 12, 13);
    SET(a, b, c, d,  1,  3);
    SET(d, a, b, c,  5,  5);
    SET(c, d, a, b,  9,  9);
    SET(b, c, d, a, 13, 13);
    SET(a, b, c, d,  2,  3);
    SET(d, a, b, c,  6,  5);
    SET(c, d, a, b, 10,  9);
    SET(b, c, d, a, 14, 13);
    SET(a, b, c, d,  3,  3);
    SET(d, a, b, c,  7,  5);
    SET(c, d, a, b, 11,  9);
    SET(b, c, d, a, 15, 13);
#undef SET

     /* Round 3. */
     /* Let [abcd k s t] denote the operation
          a = (a + H(b,c,d) + X[k] + 0x6ED9EBA1) <<< s. */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s)\
  t = a + H(b,c,d) + X[k] + 0x6ED9EBA1;\
  a = ROTATE_LEFT(t, s)
     /* Do the following 16 operations. */
    SET(a, b, c, d,  0,  3);
    SET(d, a, b, c,  8,  9);
    SET(c, d, a, b,  4, 11);
    SET(b, c, d, a, 12, 15);
    SET(a, b, c, d,  2,  3);
    SET(d, a, b, c, 10,  9);
    SET(c, d, a, b,  6, 11);
    SET(b, c, d, a, 14, 15);
    SET(a, b, c, d,  1,  3);
    SET(d, a, b, c,  9,  9);
    SET(c, d, a, b,  5, 11);
    SET(b, c, d, a, 13, 15);
    SET(a, b, c, d,  3,  3);
    SET(d, a, b, c, 11,  9);
    SET(c, d, a, b,  7, 11);
    SET(b, c, d, a, 15, 15);
#undef SET


     /* Then perform the following additions. (That is increment each
        of the four registers by the value it had before this block
        was started.) */
    m_state.abcd[0] += a;
    m_state.abcd[1] += b;
    m_state.abcd[2] += c;
    m_state.abcd[3] += d;
  }

  void MD4::init()
  {
    m_finished = false;
    m_state.count[0] = 0;
    m_state.count[1] = 0;
    m_state.abcd[0] = 0x67452301;
    m_state.abcd[1] = 0xefcdab89;
    m_state.abcd[2] = 0x98badcfe;
    m_state.abcd[3] = 0x10325476;
  }

  void MD4::feed( const std::string& data )
  {
    feed( (const unsigned char*)data.c_str(), (int)data.length() );
  }

  void MD4::feed( const unsigned char* data, int bytes )
  {
    const unsigned char* p = data;
    int left = bytes;
    int offset = ( m_state.count[0] >> 3 ) & 63;
    unsigned int nbits = (unsigned int)( bytes << 3 );

    if( bytes <= 0 )
      return;

    /* Update the message length. */
    m_state.count[1] += bytes >> 29;
    m_state.count[0] += nbits;
    if( m_state.count[0] < nbits )
      m_state.count[1]++;

    /* Process an initial partial block. */
    if( offset )
    {
      int copy = ( offset + bytes > 64 ? 64 - offset : bytes );

      memcpy( m_state.buf + offset, p, copy );
      if( offset + copy < 64 )
        return;
      p += copy;
      left -= copy;
      process( m_state.buf );
    }

    /* Process full blocks. */
    for( ; left >= 64; p += 64, left -= 64 )
      process( p );

    /* Process a final partial block. */
    if( left )
      memcpy( m_state.buf, p, left );
  }

  void MD4::finalize()
  {
    if( m_finished )
      return;

    unsigned char data[8];

    /* Save the length before padding. */
    for( int i = 0; i < 8; ++i )
      data[i] = (unsigned char)( m_state.count[i >> 2] >> ( ( i & 3 ) << 3 ) );

    /* Pad to 56 bytes mod 64. */
    feed( pad, ( ( 55 - ( m_state.count[0] >> 3 ) ) & 63 ) + 1 );

    /* Append the length. */
    feed( data, 8 );

    m_finished = true;
  }

  const std::string MD4::hex()
  {
    if( !m_finished )
      finalize();

    char buf[33];

    for( int i = 0; i < 16; ++i )
      sprintf( buf + i * 2, "%02x", (unsigned char)( m_state.abcd[i >> 2] >> ( ( i & 3 ) << 3 ) ) );

    return std::string( buf, 32 );
  }

  const std::string MD4::binary()
  {
    if( !m_finished )
      finalize();

    unsigned char digest[16];
    for( int i = 0; i < 16; ++i )
      digest[i] = (unsigned char)( m_state.abcd[i >> 2] >> ( ( i & 3 ) << 3 ) );

    return std::string( (char*)digest, 16 );
  }

  void MD4::reset()
  {
    init();
  }

}
