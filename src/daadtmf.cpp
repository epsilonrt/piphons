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
#include "daadtmf_p.h"
#include <piduino/clock.h>

// -----------------------------------------------------------------------------
//
//                          Daa::Private Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Daa::Private::Private (Daa * q, int rp, int ohp, bool rel, bool ohel) :
  q_ptr (q),
  ringPin (gpio.pin (rp)),
  offhookPin (gpio.pin (ohp)) ,
  ringEnabledLevel (rel),
  offhookEnabledLevel (ohel),
  ringingBeforeOffhook (8),
  ringingSinceHangup (0),
  userRingingHandler (0),
  userOffhookHandler (0),
  lastRinging (0),
  hookFlash (false),
  isOpen (false) {}

// -----------------------------------------------------------------------------
Daa::Private::Private (const Daa::Private & other) :
  q_ptr (other.q_ptr),
  ringPin (other.ringPin),
  offhookPin (other.offhookPin) ,
  ringEnabledLevel (other.ringEnabledLevel),
  offhookEnabledLevel (other.offhookEnabledLevel) {

  // requires exclusive ownership to write to *this
  std::unique_lock<std::shared_timed_mutex> lhs (mut, std::defer_lock);
  // requires shared ownership to read from other
  std::shared_lock<std::shared_timed_mutex> rhs (other.mut, std::defer_lock);
  std::lock (lhs, rhs);

  // assign data
  ringingBeforeOffhook = other.ringingBeforeOffhook;
  ringingSinceHangup = other.ringingSinceHangup;
  userRingingHandler = other.userRingingHandler;
  userOffhookHandler = other.userOffhookHandler;
  lastRinging = other.lastRinging;
  hookFlash = other.hookFlash;
  isOpen = other.isOpen;
}

// -----------------------------------------------------------------------------
Daa::Private::~Private() = default;

// -----------------------------------------------------------------------------
void Daa::Private::offhook (bool value) {

  offhookPin.write (value == offhookEnabledLevel);
}

// -----------------------------------------------------------------------------
bool Daa::Private::isOffhook () const {

  return offhookPin.read () == offhookEnabledLevel;
}

// -----------------------------------------------------------------------------
bool Daa::Private::open() {

  if (gpio.open()) {

    offhookPin.setMode (Pin::ModeOutput);
    offhook (false);

    ringPin.setMode (Pin::ModeInput);
    ringPin.setPull (ringEnabledLevel ? Pin::PullDown : Pin::PullUp);
    ringPin.attachInterrupt (Private::ringIsr, ringEnabledLevel ? Pin::EdgeRising : Pin::EdgeFalling, this);
    isOpen = true;
  }
  return isOpen;
}

// -----------------------------------------------------------------------------
void Daa::Private::close() {

  ringPin.detachInterrupt();
  isOpen = false;
}

/*
 *                      Isr   User
  q_ptr                 r     r
  ringPin               r     r
  offhookPin            rw    rw
  //
  ringingSinceHangup    rw    r
  //
  hookFlash             r     rw
  userRingingHandler    r     w
  userOffhookHandler    r     w
  ringingBeforeOffhook  r     rw
 */

// -----------------------------------------------------------------------------
void Daa::Private::ringIsr (void * data) {
  Private * d;
  bool hookFlash;
  int ringingSinceHangup;
  int ringingBeforeOffhook;
  DaaHandler userRingingHandler;
  DaaHandler userOffhookHandler;
  bool offhookQuickly;
  unsigned long now;
  unsigned long dt;

  now = clk.millis();
  d = reinterpret_cast<Daa::Private *> (data);
  dt = now - d->lastRinging;
  d->lastRinging = now;
  ringingSinceHangup = d->ringingSinceHangup;
  offhookQuickly = false;

  {
    // requires shared ownership to read from other this data
    std::shared_lock<std::shared_timed_mutex> lk (d->mut);
    hookFlash = d->hookFlash;
    ringingBeforeOffhook = d->ringingBeforeOffhook;
    userRingingHandler = d->userRingingHandler;
    userOffhookHandler = d->userOffhookHandler;
  }

  if ( (ringingSinceHangup > 0) && (dt > 7000)) {

    if ( (hookFlash) && (ringingSinceHangup <= 3) && (dt <= 60000)) {

      offhookQuickly = true;
    }
    else {

      ringingSinceHangup = 0;
    }
  }
  ringingSinceHangup++;
  d->ringPin.read();  // phony reading, to erase irq !

  {
    // requires exclusive ownership to write to ringingSinceHangup
    std::lock_guard<std::shared_timed_mutex> lk (d->mut);
    d->ringingSinceHangup = ringingSinceHangup;
  }

  if (userRingingHandler) {

    userRingingHandler (d->q_ptr);
  }

  if (!d->isOffhook()) {

    if ( (ringingSinceHangup >= ringingBeforeOffhook) || offhookQuickly) {

      d->offhook (true);
      if (userOffhookHandler) {

        userOffhookHandler (d->q_ptr);
      }
    }
  }
  else {

    d->offhook (false);
  }
}

// -----------------------------------------------------------------------------
//
//                             Daa Class
//
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
Daa::Daa (Daa::Private &dd) : d_ptr (&dd) {}

// -----------------------------------------------------------------------------
Daa::Daa (int rp, int ohp, bool rel, bool ohel) :
  d_ptr (new Private (this, rp, ohp, rel, ohel)) {}

// ---------------------------------------------------------------------------
Daa::Daa (const Daa & other) :
  d_ptr (new Private (*other.d_ptr)) {}

// ---------------------------------------------------------------------------
void Daa::swap (Daa &other) {

  d_ptr.swap (other.d_ptr);
}

// ---------------------------------------------------------------------------
Daa& Daa::operator= (const Daa &other) {

  Daa (other).swap (*this);
  return *this;
}

// -----------------------------------------------------------------------------
Daa::~Daa() {

  close();
}

// -----------------------------------------------------------------------------
bool Daa::open() {

  if (!isOpen()) {
    PIMP_D (Daa);

    return d->open();
  }
  return true;
}

// -----------------------------------------------------------------------------
void Daa::close() {

  if (isOpen()) {
    PIMP_D (Daa);

    offhook (false);
    d->close();
  }
}

// -----------------------------------------------------------------------------
void Daa::setRingingHandler (DaaHandler handler) {
  PIMP_D (Daa);

  std::lock_guard<std::shared_timed_mutex> lk (d->mut);
  d->userRingingHandler = handler;
}

// -----------------------------------------------------------------------------
void Daa::setOffhookHandler (DaaHandler handler) {
  PIMP_D (Daa);

  std::lock_guard<std::shared_timed_mutex> lk (d->mut);
  d->userOffhookHandler = handler;
}

// -----------------------------------------------------------------------------
void Daa::offhook (bool value) {
  PIMP_D (Daa);

  if (isOpen()) {

    d->offhook (value);
  }
  if (value == false) {

    d->ringingSinceHangup = 0;
  }
}

// -----------------------------------------------------------------------------
bool Daa::isOffhook() const {

  if (isOpen()) {
    PIMP_D (const Daa);

    return d->isOffhook();
  }
  return false;
}

// -----------------------------------------------------------------------------
void Daa::setRingingBeforeOffhook (int count) {
  PIMP_D (Daa);

  std::lock_guard<std::shared_timed_mutex> lk (d->mut);
  d->ringingBeforeOffhook = count;
}

// -----------------------------------------------------------------------------
int Daa::ringingBeforeOffhook() const {
  PIMP_D (const Daa);

  return d->ringingBeforeOffhook;
}

// -----------------------------------------------------------------------------
int Daa::ringingSinceHangup() const {
  PIMP_D (const Daa);

  std::shared_lock<std::shared_timed_mutex> lk (d->mut);
  return d->ringingSinceHangup;
}

// -----------------------------------------------------------------------------
void Daa::setHookFlash (bool value) {
  PIMP_D (Daa);

  std::lock_guard<std::shared_timed_mutex> lk (d->mut);
  d->hookFlash = value;
}

// -----------------------------------------------------------------------------
bool Daa::hookFlash() const {
  PIMP_D (const Daa);

  return d->ringingSinceHangup;
}

// -----------------------------------------------------------------------------
bool Daa::isOpen() const {
  PIMP_D (const Daa);

  return d->isOpen;
}

/* ========================================================================== */
