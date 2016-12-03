/*
  SfxTime

  This sketch request the time and date from the SigFox network on startup and every 24 hours.

  It then output time every second over the Serial port.


  The function demonstrates how to request the 8 bytes of downlink message (AckMessage).
  The sketch is blocked during the update request. This choice has been made to ease understanding and reading
  This can be adjusted if you need more responsive sketch

  Remember that 4 downlink messages are possible per day.
  The acknowledge answer could take more than 30 Seconds before return to the system.

  You need to configure the SigFox backend. Refer to http://miscellany.bouillot.org/post/153988507825/sigfox-two-way-communications-setting-nodes

  created 03 Dec 2016
  by Alexandre (alexandre@fltm-labs.com)

  This example is CC SA
  https://github.com/abouillot
*/

#include <SmeSFX.h>
#include <Arduino.h>

#include <TimeLib.h>  // this can be <Time.h> on case sensitive file system, but wouldn't work on Windows

time_t nextTimeUpdate = 0;  // time when the next time update request will be performe
const long timeUpdateInterval = 24 * 60 * 60; // value in second update is requested every 24 hours
//const long timeUpdateInterval = 60; // during development, increase the refresh frequency - BUT NOT FOR PRODUCTION !!!!

// max length of message consider the register name + register Value + the symbol for write/read command
char sfcCommandMsg[20];
#define SFX_FRAME "54494D45"  // dummy message

// the setup function runs once when you press reset or power the board
void setup() {
  SerialUSB.begin(115200);

  sfxAntenna.begin();

  while (!SerialUSB) {
    ;
  }
}

void requestTime() {
  sfxAntenna.sfxSendDataAck(SFX_FRAME, strlen(SFX_FRAME), true); // send the data;

  while (1) {
    // if message has been received correctly print it out
    if (sfxAntenna.hasSfxAnswer()) {
      if (sfxAntenna.getSfxError() == SME_SFX_OK) {
        // ACK received

        // copy localy the data received by the module
        char msg[128];
        memcpy(msg, (const char*)sfxAntenna.getLastReceivedMessage(), 128);

        unsigned long long val = 0; // store the 8 bytes data
        
        char buf[9];
        if (memcmp(msg, "+RX=", 4) == 0) {
          memcpy(&buf[0], &msg[4], 8);
          buf[8] = 0;
          val = strtoul(&buf[0], NULL, 16);

          setTime(val);

          // here we could return, if we are only interested by the time part.

          // The following part shows how to receive the full buffer and store it in 8 bytes memory

          // make space for the lower part
          val <<= 32;
          memcpy(&buf[0], &msg[12], 8);
          buf[8] = 0; // not realy needed...
          val += strtoul(&buf[0], NULL, 16);

          // val now contain the 8 bytes transmited from the network
          return;
        }

        sfxAntenna.setSfxDataMode(); // move in Data Mode if need
      }
      else {
        // ACK not received
        sfxAntenna.setSfxDataMode(); // move in Data Mode if need
        return;
      }
    }
  }

}

void digitalClockDisplay() {
  // digital clock display of the time
  SerialUSB.print("Unix time = ");
  SerialUSB.println(now());

  // Print date...
  SerialUSB.print(day(now()));
  SerialUSB.print("/");
  SerialUSB.print(month(now()));
  SerialUSB.print("/");
  SerialUSB.print(year(now()));
  SerialUSB.print("\t");

  // ...and time
  print2digits(hour(now()));
  SerialUSB.print(":");
  print2digits(minute(now()));
  SerialUSB.print(":");
  print2digits(second(now()));

  SerialUSB.println();
}

// function to print number, ensuring it uses 2 digits
void print2digits(int number) {
  if (number < 10) {
    SerialUSB.print("0");
  }
  SerialUSB.print(number);
}


// the loop function runs over and over again forever
void loop() {
  if (now() >= nextTimeUpdate) {
    requestTime();
    SerialUSB.print("updateTime ");
    nextTimeUpdate = now() + timeUpdateInterval;
    SerialUSB.println(nextTimeUpdate);
  }

  // Here you can add your SigFox data collection activities

  digitalClockDisplay();
  delay(1000);

}
