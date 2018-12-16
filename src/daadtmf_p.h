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
#ifndef DAADTMF_PRIVATE_H
#define DAADTMF_PRIVATE_H

#include <mutex>
#include <cerrno>
#include <piduino/gpio.h>
#include <piduino/gpiopin.h>
#include <daadtmf.h>

using namespace Piduino;

class Daa::Private {
  public:
    Private (Daa * q, int ringPin, int offhookPin, bool ringEnabledLevel, bool offhookEnabledLevel);
    virtual ~Private();

    Daa * const q_ptr;
    Pin & ringPin;
    Pin & offhookPin;
    bool ringEnabledLevel;
    bool offhookEnabledLevel;
    int ringingBeforeOffhook;
    int ringingSinceHangup;
    DaaHandler userRingingHandler;
    DaaHandler userOffhookHandler;
    unsigned long lastRinging;
    bool hookFlash;
    bool isOpen;
    std::mutex mutex;

    virtual bool isOffhook () const;
    virtual void offhook (bool value);
    virtual bool open ();
    virtual void close ();

    static void ringIsr (void * daa);

    PIMP_DECLARE_PUBLIC (Daa)
};

/* ========================================================================== */
#endif /* DAADTMF_PRIVATE_H defined */
