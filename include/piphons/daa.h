/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
 * This file is part of the libpiphons Library.
 *
 * The libpiphons Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libpiphons Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libpiphons Library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PIPHONS_DAA_H
#define PIPHONS_DAA_H

#include <piphons/global.h>

namespace Piphons {

  /**
   * @class Daa
   * @brief A telephone line interface, also called a DAA or Data Access Arrangement
   */
  class Daa {
    public:
      typedef void (*DaaHandler) (Daa * line);

      Daa (int ringPin, int offhookPin, int tonePin = -1);
      Daa (int ringPin, int offhookPin, bool ringEnabledLevel, bool offhookEnabledLevel);
      Daa (const Daa & other);
      void swap (Daa & other);
      Daa& operator= (const Daa &other);
      ~Daa();

      void setRingingHandler (DaaHandler handler);
      void setOffhookHandler (DaaHandler handler);
      void setHangupToneHandler (DaaHandler handler);
      bool setHangupToneDetect (bool enable, int tonePulseBeforeHanghup = 10, unsigned long tonePwMin = 450, unsigned long tonePwMax = 550);

      bool open();
      bool isOpen() const;
      void close();

      void offhook (bool oh = true);
      bool isOffhook() const;
      inline void hangup() {
        offhook (false);
      }

      void setRingingBeforeOffhook (int count);
      int ringingBeforeOffhook() const;

      void setHookFlash (bool value);
      bool hookFlash() const;

      int ringingSinceHangup() const;

    protected:
      class Private;
      Daa (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Daa)
  };

}
/* ========================================================================== */
#endif /* PIPHONS_DAA_H defined */
