#include "Adafruit_VS1053/Adafruit_VS1053.h"
#include "Adafruit_VS1053/SD.h"

#define BREAKOUT_MOSI A5      // Output data, to VS1053/SD card
#define BREAKOUT_MISO A4      // Input data, from VS1053/SD card
#define BREAKOUT_CLK A3       // SPI Clock, shared with SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example

#define BREAKOUT_CS     A2      // VS1053 chip select pin (output)
#define BREAKOUT_RESET  D6      // VS1053 reset pin (output)
#define BREAKOUT_DCS    D2      // VS1053 Data/command select pin (output) - it's labeled XDCS - do not mix it up with SDCS

// These are common pins between breakout and shield
#define BREAKOUT_CARDCS D3     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ D0                 // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(BREAKOUT_MOSI, BREAKOUT_MISO, BREAKOUT_CLK, BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, BREAKOUT_CARDCS);

bool isInitialized = false;

void setup() {
   // pinMode(TRIGGERPIN,INPUT);
    Spark.function("playsong", playSong);
        
    Serial.begin(9600);
    delay(1000); //to allow user to start up serial com.
    
	if (SD.begin(BREAKOUT_MOSI, BREAKOUT_MISO, BREAKOUT_CLK, BREAKOUT_CARDCS))
	{  
		if (! musicPlayer.begin()) { // initialise the music player
		  Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
		} else {
		  isInitialized = true;
		  Serial.println(F("VS1053 found"));
		}
		
		musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
		musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
	}
}

void loop() {   
   //just do nothing - all the action will be triggered over the cloud
  delay(250);  
}

// Trigger it using the Spark CLI tools or whatever you use to communicate with your cores - the functionname is "playsong" - use a filename as a parameter to
// play the song. Subdirectories work with a slash (i.e. test/test.mp3 to play test.mp3 from the test subdirectory)
int playSong(String command)
{
    char filename[255]; // Or something long enough to hold the longest file name you will ever use. - caution: the SD-library supports only 8.3 filenames!
    
	command.toCharArray(filename, sizeof(filename));
   
    musicPlayer.setVolume(20,20);
    
    musicPlayer.stopPlaying();
    musicPlayer.startPlayingFile(filename);
    
    return 1;
}