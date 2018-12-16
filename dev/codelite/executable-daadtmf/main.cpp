// libdaadtmf template
// This example code is in the public domain.
#include <iostream>
#include <daadtmf.h>
#include <Arduino.h> // Piduino Arduino API

using namespace std;

const int ringPin = 6;  // Header Pin 22: GPIO6 for RPi, GPIOA1 for NanoPi
// <DANGER> Be careful !!! Before launching this program :
//    -> Check that the pin below is well connected to an LED ! <-
const int offhookPin = 22; // Header Pin 31: GPIO22 for RPi, GPIOA21 for NanoPi

// -----------------------------------------------------------------------------
void ringIsr (Daa * daa) {

  cout << daa->ringingSinceHangup() << flush;
}

// -----------------------------------------------------------------------------
void offhookIsr (Daa * daa) {

  cout << endl << "off-hook !" << endl << "Waiting DTMF key..." << endl;
}

// -----------------------------------------------------------------------------
int main (int argc, char **argv) {
  Daa daa (ringPin, offhookPin, false, true);

  daa.setRingingHandler (ringIsr);
  daa.setOffhookHandler (offhookIsr);

  while (!daa.isOffhook()) {

    delay (1000);
  }

  daa.hangup();
  return 0;
}
/* ========================================================================== */
