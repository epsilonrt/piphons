/* Copyright © 2018 Pascal JEAN, All rights reserved.
 * This file is part of the libdaadtmf Library.
 *
 * The libdaadtmf Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libdaadtmf Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libdaadtmf Library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DAA_DTMF_H
#define DAA_DTMF_H

#include "global.h"

/**
 * @class Daa
 * @brief A telephone line interface, also called a DAA or Data Access Arrangement
 */
class Daa {
  public:
    typedef void (*DaaHandler) (Daa * line);

    Daa (int ringPin, int offhookPin, bool ringEnabledLevel = false, bool offhookEnabledLevel = false);
    ~Daa();

    void setRingingHandler (DaaHandler handler);
    void setOffhookHandler (DaaHandler handler);

    bool open();
    bool isOpen() const;
    void close();

    void offhook (bool oh = true);
    bool isOffhook() const;
    inline void hangup() {
      offhook (false);
    }

    void setRingingBeforeOffhook (int count);
    void setHookFlash (bool value);
    
    bool hookFlash() const;
    int ringingBeforeOffhook() const;
    int ringingSinceHangup() const;

  protected:
    class Private;
    Daa (Private &dd);
    std::unique_ptr<Private> d_ptr;

  private:
    PIMP_DECLARE_PRIVATE (Daa)
};

/* ========================================================================== */
#endif /* DAA_DTMF_H defined */
