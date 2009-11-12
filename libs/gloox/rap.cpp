/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "rap.h"
#include "tag.h"

#include <cstdlib>

namespace gloox
{

  RAP::RAP( const Tag* tag )
    : StanzaExtension( ExtRAP ), m_primary( false )
  {
    if( !tag || tag->name() != "rap" || tag->xmlns() != XMLNS_RAP )
      return;

    m_ns = tag->findAttribute( "ns" );
    m_num = strtol( tag->findAttribute( "num" ).c_str(), 0, 10 );

    m_primary = tag->hasChild( "primary" );
  }

  RAP::RAP( const std::string& ns, int num )
    : StanzaExtension( ExtRAP ), m_ns( ns ), m_num( num ), m_primary( false )
  {
  }

  RAP::~RAP()
  {
  }

  const std::string& RAP::filterString() const
  {
    static const std::string filter = "/presence/rap[@xmlns='" + XMLNS_RAP + "']";
    return filter;
  }

  Tag* RAP::tag() const
  {
    Tag* t = new Tag( "rap" );
    t->setXmlns( XMLNS_RAP );
    t->addAttribute( "ns", m_ns );
    t->addAttribute( "num", m_num );
    if( m_primary )
      new Tag( t, "primary" );

    return t;
  }

}
