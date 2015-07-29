/* small adjustments by Rolf Hut to adapt for use with Spark Core. 
*/

// This #include statement was automatically added by the Spark IDE.
//#include "SparkIntervalTimer.h"

// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_VS1053.h"

// This #include statement was automatically added by the Spark IDE.
#include "sd-card-library/sd-card-library.h"

// This #include statement was automatically added by the Spark IDE.
//#include "Sd2Card.h"


/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
//#include <SPI.h>
//#include <Adafruit_VS1053.h>
//#include "SD.h"

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example
#define BREAKOUT_RESET  D6//9      // VS1053 reset pin (output)
#define BREAKOUT_CS     A2//10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    D5//8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_CS     D4//7      // VS1053 chip select pin (output)
#define SHIELD_DCS    A7//6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS D3//4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ D0//3       // VS1053 Data request, ideally an Interrupt pin


//for triggering to play the file:
#define TRIGGERPIN D1

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  //Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);
  
bool playing=false;



void setup() {
    pinMode(TRIGGERPIN,INPUT);
  Serial.begin(9600);
  delay(10000); //to allow user to start up serial com.
  VS1053Test();

}

void loop() {
  // File is playing in the background
  if ((musicPlayer.stopped()) && playing) {
    Serial.println("Done playing music");
    playing=false;
  }
  
  delay(1000);
  
  //if triggerPin is made high, test again.
  if (digitalRead(TRIGGERPIN)) VS1053Test();
}

void VS1053Test(){
    Serial.println("Adafruit VS1053 Simple Test");

    if (! musicPlayer.begin()) { // initialise the music player
      Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    } else {
      Serial.println(F("VS1053 found"));
      SD.begin(CARDCS);    // initialise the SD card
  
      // Set volume for left, right channels. lower numbers == louder volume!
      musicPlayer.setVolume(20,20);

      // Timer interrupts are not suggested, better to use DREQ interrupt!
      //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

      // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
      // audio playing
      musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  
      // Play one file, don't return until complete
      Serial.println(F("Playing track 001"));
      playing=true;
      musicPlayer.playFullFile("tardis.mp3");
      // Play another file in the background, REQUIRES interrupts!
      //Serial.println(F("Playing track 002"));
      //musicPlayer.startPlayingFile("track002.mp3");
    }
}