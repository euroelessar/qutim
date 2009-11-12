/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLERAWUDP_H__
#define JINGLERAWUDP_H__

#include "jingletransport.h"
#include "gloox.h"
#include "tag.h"

#include <string>

namespace gloox
{

  namespace Jingle
  {

    /**
     * @brief An abstraction of the signaling part of Jingle Raw UDP Transport Method (XEP-0177).
     *
     * XEP Version: 0.15
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class GLOOX_API RawUDP : public Transport
    {
      public:
        /**
         *
         */
        RawUDP();

        /**
         * Virtual destructor.
         */
        virtual ~RawUDP() {}

        // reimplemented from Plugin
        virtual const std::string& filterString() const;

        // reimplemented from Plugin
        virtual Tag* tag() const;

        // reimplemented from Plugin
        virtual Plugin* clone() const
        {
          return new RawUDP( *this );
        }
    };

  }

}

#endif // JINGLERAWUDP_H__
