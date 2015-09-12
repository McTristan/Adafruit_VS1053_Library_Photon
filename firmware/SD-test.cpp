#include "SD.h"

int8_t SPI_init(void);
int8_t sd_test(void);
void pin_init(void);

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;

#define FILE_SIZE_MB 1
#define FILE_SIZE (1000000UL*FILE_SIZE_MB)
#define BUF_SIZE 100

// Run code even without connection to Spark Cloud
SYSTEM_MODE(MANUAL);

#define SCK_pin A3
#define SS_pin A2
#define CD_pin D4


void setup(void){

  Serial.begin(9600);

//  pinMode(CD_pin,INPUT_PULLUP);  // Card Detect

  pin_init();

  while(Serial.available()) Serial.read();      // MAKESHIFT FLUSH
  while(!Serial.available()) Spark.process();
}

void loop(void){

  int8_t result = 0;
  SPI.end();
  pin_init();

  Serial.print("\nInsert uSD and hit Enter...");
  while(Serial.available()) Serial.read();      // MAKESHIFT FLUSH
  while(!Serial.available()) Spark.process();


  Serial.print("\nInitializing SD card...");

    SPI_init();
    result = sd_test();
    pin_init();

  if(result == 0)
    Serial.println("Card not inserted!");
  else if(result == 1){
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
  }
  else if(result == 2)
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
  else if(result == 3)
    Serial.println("Could not OPEN test file.");
  else if(result == 4)
    Serial.println("Could not WRITE to file.");
  else if(result == 5)
    Serial.println("Could not READ file.");
  else if(result == 8){
    pin_init();
    Serial.println("\nMicro SD Test successful! :D\n");
  }


  Serial.println("\nEnd of Test. HOORAY!");
  while(Serial.available()) Serial.read();      // MAKESHIFT FLUSH
  while(!Serial.available()) Spark.process();

}


int8_t sd_test(void){

  // if(digitalRead(CD_pin) == LOW){
  //   return 0;
  // }

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  // Initialize HARDWARE SPI with user defined chipSelect
  if (!card.init(SPI_FULL_SPEED, SS_pin)) {
    return 1;
  } else {
   Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch(card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    return 2;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                         // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);

  // open or create file - truncate existing file.
  if (!file.open(&root, String(random(1029102))+".DAT", O_CREAT | O_TRUNC | O_RDWR)) {
    return 3;
  }
  // fill buf with known data
  uint8_t buf[BUF_SIZE];

  for (uint16_t i = 0; i < (BUF_SIZE-2); i++) {
    buf[i] = 'A' + (i % 26);
  }
  buf[BUF_SIZE-2] = '\r';
  buf[BUF_SIZE-1] = '\n';
  Serial.print("\nFile size ");
  Serial.print(FILE_SIZE_MB);
  Serial.println(" MB");
  uint32_t n = FILE_SIZE/sizeof(buf);
  Serial.println("Starting write test.  Please wait up to a minute");

  // do write test
  uint32_t t = millis();
  for (uint32_t i = 0; i < n; i++) {
    if (file.write(buf, sizeof(buf)) != sizeof(buf)) {
      return 4;
    }
  }
  t = millis() - t;
  file.sync();
  double r = (double)file.fileSize()/t;
  Serial.print("Write ");
  Serial.print(r);
  Serial.println(" kB/sec");
  Serial.println();
  Serial.println("Starting read test.  Please wait up to a minute");
  // do read test
  file.rewind();
  t = millis();
  for (uint32_t i = 0; i < n; i++) {
    if (file.read(buf, sizeof(buf)) != sizeof(buf)) {
      return 5;
    }
  }
  t = millis() - t;
  r = (double)file.fileSize()/t;
  Serial.print("Read ");
  Serial.print(r);
  Serial.println(" kB/sec");
  file.close();

  return 8;
}


void pin_init(void){

  // Turn off SPI to take over A2-A5 pins
  SPI.end();

  pinMode(A2, OUTPUT);  // SS Pin
  digitalWrite(A2,LOW);

  pinMode(A3, OUTPUT);  // SCK Pin
  digitalWrite(A3,LOW);

  pinMode(A4, OUTPUT);  // MISO Pin
  digitalWrite(A4,LOW);

  pinMode(A5, OUTPUT);  // MOSI Pin
  digitalWrite(A5,LOW);
}

int8_t SPI_init(void){
  digitalWrite(D7,HIGH);  // Enable Vout

  // Disable all SPI devices
  digitalWrite(SS_pin,HIGH);
  digitalWrite(D2,HIGH);
  digitalWrite(D3,HIGH);

  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();
  return 1;
}
