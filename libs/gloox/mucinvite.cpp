/*
  Copyright (c) 2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/



#include "mucinvite.h"
#include "tag.h"

#include <cstdlib>

namespace gloox
{

  MUCInvite::MUCInvite( const Tag* tag )
    : StanzaExtension( ExtMUCInvite )
  {
    if( !tag || tag->name() != "x" || tag->xmlns() != XMLNS_X_CONFERENCE )
      return;

    m_room.setJID( tag->findAttribute( "jid" ) );
    m_pwd = tag->findAttribute( "password" );
  }

  MUCInvite::MUCInvite( const JID& room, const std::string& password)
    : StanzaExtension( ExtMUCInvite ), m_room( room ), m_pwd( password )
  {
  }

  MUCInvite::~MUCInvite()
  {
  }

  const std::string& MUCInvite::filterString() const
  {
    static const std::string filter = "/message/x[@xmlns='" + XMLNS_X_CONFERENCE + "']";
    return filter;
  }

  Tag* MUCInvite::tag() const
  {
    if( !m_room )
      return 0;

    Tag* t = new Tag( "x" );
    t->setXmlns( XMLNS_X_CONFERENCE );
    t->addAttribute( "jid", m_room.bare() );
    t->addAttribute( "password", m_pwd );

    return t;
  }

}
