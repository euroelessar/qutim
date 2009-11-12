/*
  Copyright (c) 2008-2009 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef JINGLEPLUGIN_H__
#define JINGLEPLUGIN_H__

#include "macros.h"

#include <string>

namespace gloox
{

  class Tag;

  namespace Jingle
  {

    /**
     * @brief An abstraction of a Jingle plugin.
     *
     * This is the base class for Transport and Description. It is also used for
     * session information, such as the 'ringing' info in Jingle Audio, or Jingle DTMF.
     *
     * @author Jakob Schroeter <js@camaya.net>
     * @since 1.0
     */
    class GLOOX_API Plugin
    {
      public:
        /**
         * Virtual destructor.
         */
        virtual ~Plugin() {}

        /**
         * Returns an XPath expression that describes a path to child elements of a
         * jingle element that a plugin handles.
         *
         * @return The plugin's filter string.
         */
        virtual const std::string& filterString() const = 0;

        /**
         * Returns a Tag representation of the plugin.
         * @return A Tag representation of the plugin.
         */
        virtual Tag* tag() const = 0;

        /**
         * Creates an identical deep copy of the current instance.
         * @return An identical deep copy of the current instance.
         */
        virtual Plugin* clone() const = 0;
    };

  }

}

#endif // JINGLEPLUGIN_H__
