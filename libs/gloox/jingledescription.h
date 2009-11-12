/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEDESCRIPTION_H__
#define JINGLEDESCRIPTION_H__


#include "jingleplugin.h"
#include "gloox.h"

#include <list>

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    /**
     * @brief An abstraction of a Jingle Description (Application Format).
     *
     * You should not need to use this class directly. Use derived classes instead
     * (AudioRTP, VideoRTP). See
     * @link gloox::Jingle::Session Jingle::Session @endlink for more info on Jingle.
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.1
     */
    class Description : public Plugin
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~Description() {}

      protected:
        /**
         * Returns the Application Format's namespace.
         * @return The namespace being handled by the Application Format.
         */
        virtual const std::string& xmlns() const = 0;

    };

  }

}

#endif // JINGLEDESCRIPTION_H__
