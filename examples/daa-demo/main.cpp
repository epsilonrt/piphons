// libpiphons DAA example
//
// This example shows how to manage the phone line.
// The program waits for a call, counts the number of rings and picks up 
// the phone after 3 rings.
// The program hangs up the phone if the local user presses Enter or the 
// other party hangs up the phone.
//
// This example code is in the public domain.
#include <iostream>
#include <piphons.h>
#include <Arduino.h> // Piduino Arduino API

using namespace std;
using namespace Piphons;

const int ringPin = 6;  // Header Pin 22: GPIO6 for RPi, GPIOA1 for NanoPi
const int offhookPin = 5; // Header Pin 18: GPIO7 for RPi, GPIOG9 for NanoPi
const int tonePin = 11; // Header Pin 26: GPIO24 for RPi, GPIOA17 for NanoPi

// -----------------------------------------------------------------------------
void ringIsr (Daa * daa) { // this function is executed at each ring

  // displays the number of rings since the last call
  cout << daa->ringingSinceHangup() << flush;
}

// -----------------------------------------------------------------------------
void offhookIsr (Daa * daa) { // this function is executed after hookoff

  cout << endl << "Off-hook !" << endl;
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  Daa daa (ringPin, offhookPin, tonePin);

  daa.setRingingHandler (ringIsr);
  daa.setOffhookHandler (offhookIsr);
  daa.setHookFlash (true);
  daa.setRingingBeforeOffhook (3);
  daa.open();

  cout << "Piphons DAA Example" << endl
       << "Waiting for a call..." << endl 
       << "Press Enter to quit !" << endl;
  cin.get(); // wait Enter to quit

  daa.close();
  cout << endl
       << "I hung up the phone." << endl
       << "Have a nice day !" << endl;

  return 0;
}
/* ========================================================================== */
