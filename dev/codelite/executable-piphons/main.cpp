// libpiphons DTMF example
//
// This example shows how to manage a DTMF key decoder.
// The program waits for a call, counts the number of rings and picks up 
// the phone after 3 rings.
// Then the program waits for the other party to press keys and display them.
// If the other party presses 2 # keys successively, the program hangs up the phone.
//
// This example code is in the public domain.
#include <iostream>
#include <atomic>
#include <piphons.h>
#include <Arduino.h> // Piduino Arduino API

using namespace std;
using namespace Piphons;

const int ringPin = 6;  // Header Pin 22: GPIO6 for RPi, GPIOA1 for NanoPi
const int offhookPin = 5; // Header Pin 18: GPIO7 for RPi, GPIOG9 for NanoPi
const int tonePin = 11; // Header Pin 26: GPIO24 for RPi, GPIOA17 for NanoPi
const std::array<int, 5> dtmfPins = {0, 1, 2, 3, 4}; // D0, D1, D2, D3, DV

atomic<bool> loop (true); // Atomic Boolean variable, needed because the handlers run in different threads...

// -----------------------------------------------------------------------------
void ringIsr (Daa * daa) { // this function is executed at each ring

  // displays the number of rings since the last call
  cout << daa->ringingSinceHangup() << flush;
}

// -----------------------------------------------------------------------------
void offhookIsr (Daa * daa) { // this function is executed after hookoff

  cout << endl
       << "Off-hook !" << endl
       << "Waiting for DTMF key... " << endl
       << "Send ## to hang up" << endl;
}

// -----------------------------------------------------------------------------
void keyIsr (Dtmf * dtmf) {
  static char prev;
  char c;

  c = dtmf->read(); // get the key
  cout << c << flush; // print the key to stdout
  
  if ( (c == '#') && (prev == '#')) {
    // 2 # sent consecutively, this is the signal to stop
    loop = false; // drop the flag atomically to tell the main thread to stop
  }

  prev = c; // memorizes the previous character
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  Daa daa (ringPin, offhookPin, tonePin);
  Dtmf dtmf (dtmfPins);

  daa.setRingingHandler (ringIsr);
  daa.setOffhookHandler (offhookIsr);
  daa.setHookFlash (true);
  daa.setRingingBeforeOffhook (3);
  daa.open();

  dtmf.setKeyHandler (keyIsr);
  dtmf.open();

  cout << "Piphons DTMF Example" << endl
       << "Waiting for a call..." << endl
       << "Press CTRL+C to quit !" << endl;
  while (loop.load()) {

    delay (100);
  }

  daa.close();
  dtmf.close();
  cout << endl
       << "I hung up the phone." << endl
       << "Have a nice day !" << endl;

  return 0;
}
/* ========================================================================== */
