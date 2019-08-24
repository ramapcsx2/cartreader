//******************************************
// SUPER NINTENDO MODULE
//******************************************

/******************************************
  Defines
 *****************************************/
// SNES Hi and LoRom, SA is HI with different Sram dumping
#define EX 4
#define SA 3
#define HI 1
#define LO 0

/******************************************
   Variables
 *****************************************/
// Define SNES Cart Reader Variables
int romSpeed = 0;      // 0 = SlowROM, 3 = FastROM
int romChips = 0;      // 0 = ROM only, 1 = ROM & RAM, 2 = ROM & Save RAM,  3 = ROM & DSP1, 4 = ROM & RAM & DSP1, 5 = ROM & Save RAM & DSP1, 19 = ROM & SFX
// 227 = ROM & RAM & GameBoy data, 243 = CX4, 246 = ROM & DSP2
byte romSizeExp = 0;   // ROM-Size Exponent
int cartCountry = 255;
boolean NP = false;
byte cx4Type = 0;
byte cx4Map = 0;

/******************************************
  Menu
*****************************************/
// SNES/Nintendo Power SF Memory start menu
static const char snsMenuItem1[] PROGMEM = "Super Nintendo";
static const char snsMenuItem2[] PROGMEM = "NPower SF Memory";
static const char snsMenuItem3[] PROGMEM = "Satellaview";
static const char snsMenuItem4[] PROGMEM = "HiROM repro";
static const char snsMenuItem5[] PROGMEM = "LoROM repro";
static const char* const menuOptionsSNS[] PROGMEM = {snsMenuItem1, snsMenuItem2, snsMenuItem3, snsMenuItem4, snsMenuItem5};

// SNES menu items
static const char SnesMenuItem1[] PROGMEM = "Read Rom";
static const char SnesMenuItem2[] PROGMEM = "Read Save";
static const char SnesMenuItem3[] PROGMEM = "Write Save";
static const char SnesMenuItem4[] PROGMEM = "Test SRAM";
static const char SnesMenuItem5[] PROGMEM = "Cycle cart";
static const char SnesMenuItem6[] PROGMEM = "Reset";
static const char* const menuOptionsSNES[] PROGMEM = {SnesMenuItem1, SnesMenuItem2, SnesMenuItem3, SnesMenuItem4, SnesMenuItem5, SnesMenuItem6};

// Manual config menu items
static const char confMenuItem1[] PROGMEM = "Use header info";
static const char confMenuItem2[] PROGMEM = "4MB LoRom 256K Sram";
static const char confMenuItem3[] PROGMEM = "4MB HiRom 64K Sram";
static const char confMenuItem4[] PROGMEM = "6MB ExRom 256K Sram";
static const char confMenuItem5[] PROGMEM = "Reset";
static const char* const menuOptionsConf[] PROGMEM = {confMenuItem1, confMenuItem2, confMenuItem3, confMenuItem4, confMenuItem5};

// SNES start menu
void snsMenu() {
  // create menu with title and 4 options to choose from
  unsigned char snsCart;
  // Copy menuOptions out of progmem
  convertPgm(menuOptionsSNS, 5);
  snsCart = question_box("Select Cart Type", menuOptions, 5, 0);

  // wait for user choice to come back from the question box menu
  switch (snsCart)
  {
    case 0:
      display_Clear();
      display_Update();
      setup_Snes();
      mode = mode_SNES;
      break;

    case 1:
      display_Clear();
      display_Update();
      setup_SFM();
      mode = mode_SFM;
      break;

    case 2:
      display_Clear();
      display_Update();
      setup_SV();
      mode = mode_SV;
      break;

    case 3:
      display_Clear();
      display_Update();
      hiROM = 1;
      setup_Flash8();
      id_Flash8();
      wait();
      mode = mode_FLASH8;
      break;

    case 4:
      display_Clear();
      display_Update();
      hiROM = 0;
      setup_Flash8();
      id_Flash8();
      wait();
      mode = mode_FLASH8;
      break;
  }
}

// SNES Menu
void snesMenu() {
  // create menu with title and 7 options to choose from
  unsigned char mainMenu;
  // Copy menuOptions out of progmem
  convertPgm(menuOptionsSNES, 6);
  mainMenu = question_box("SNES Cart Reader", menuOptions, 6, 0);

  // wait for user choice to come back from the question box menu
  switch (mainMenu)
  {
    case 0:
      display_Clear();
      // Change working dir to root
      sd.chdir("/");
      readROM_SNES();
      compare_checksum();
      break;

    case 1:
      if (sramSize > 0) {
        display_Clear();
        // Change working dir to root
        sd.chdir("/");
        readSRAM();
      }
      else {
        display_Clear();
        print_Error(F("Does not have SRAM"), false);
      }
      break;

    case 2:
      if (sramSize > 0) {
        display_Clear();
        // Change working dir to root
        sd.chdir("/");
        writeSRAM(1);
        unsigned long wrErrors;
        wrErrors = verifySRAM();
        if (wrErrors == 0) {
          println_Msg(F("Verified OK"));
          display_Update();
        }
        else {
          print_Msg(F("Error: "));
          print_Msg(wrErrors);
          println_Msg(F(" bytes "));
          print_Error(F("did not verify."), false);
        }
      }
      else {
        display_Clear();
        print_Error(F("Does not have SRAM"), false);
      }
      break;

    case 3:
      if (sramSize > 0) {
        display_Clear();
        println_Msg(F("Warning:"));
        println_Msg(F("This can erase"));
        println_Msg(F("your save games"));
        println_Msg(F(""));
        println_Msg(F(""));
        println_Msg(F("Press any button to"));
        println_Msg(F("start sram testing"));
        display_Update();
        wait();
        display_Clear();
        // Change working dir to root
        sd.chdir("/");
        readSRAM();
        eraseSRAM(0x00);
        eraseSRAM(0xFF);
        writeSRAM(0);
        unsigned long wrErrors = verifySRAM();
        if (wrErrors == 0) {
          println_Msg(F("Restored OK"));
          display_Update();
        }
        else {
          print_Msg(F("Error: "));
          print_Msg(wrErrors);
          println_Msg(F(" bytes "));
          print_Error(F("did not verify."), false);
        }
      }
      else {
        display_Clear();
        print_Error(F("Does not have SRAM"), false);
      }
      break;

    case 4:
      // For arcademaster1 (Markfrizb) multi-game carts
      // Set reset pin to output (PH0)
      DDRH |= (1 << 0);
      // Switch RST(PH0) to LOW
      PORTH &= ~(1 << 0);
      display_Clear();
      print_Msg("Resetting...");
      display_Update();
      delay(3000);  // wait 3 secs to switch to next game
      asm volatile ("  jmp 0");
      break;

    case 5:
      asm volatile ("  jmp 0");
      break;
  }
  println_Msg(F(""));
  println_Msg(F("Press Button..."));
  display_Update();
  wait();
}

// Menu for manual configuration
void confMenu() {
  // create menu with title and 5 options to choose from
  unsigned char subMenu;
  // Copy menuOptions out of progmem
  convertPgm(menuOptionsConf, 5);
  subMenu = question_box("Choose mapping", menuOptions, 5, 0);

  // wait for user choice to come back from the question box menu
  switch (subMenu)
  {
    case 0:
      break;

    case 1:
      romType = LO;
      numBanks = 128;
      sramSize = 256;
      strcpy(romName, "LOROM");
      break;

    case 2:
      romType = HI;
      numBanks = 64;
      sramSize = 64;
      strcpy(romName, "HIROM");
      break;

    case 3:
      romType = EX;
      numBanks = 96;
      sramSize = 256;
      strcpy(romName, "EXROM");
      break;

    case 4:
      // Reset
      asm volatile ("  jmp 0");
      break;
  }
}

/******************************************
   Setup
 *****************************************/
void setup_Snes() {
  // Set cicrstPin(PG1) to Output
  DDRG |= (1 << 1);
  // Output a high signal until we're ready to start
  PORTG |= (1 << 1);
  // Set cichstPin(PG0) to Input
  DDRG &= ~(1 << 0);
  
  // Set Address Pins to Output
  //A0-A7
  DDRF = 0xFF;
  //A8-A15
  DDRK = 0xFF;
  //BA0-BA7
  DDRL = 0xFF;
  //PA0-PA7
  DDRA = 0xFF;

  // Set Control Pins to Output RST(PH0) CS(PH3) WR(PH5) RD(PH6)
  DDRH |= (1 << 0) | (1 << 3) | (1 << 5) | (1 << 6);
  // Switch RST(PH0) and WR(PH5) to HIGH
  PORTH |= (1 << 0) | (1 << 5);
  // Switch CS(PH3) and RD(PH6) to LOW
  PORTH &= ~((1 << 3) | (1 << 6));

  // Set Refresh(PE5) to Output
  DDRE |= (1 << 5);
  // Switch Refresh(PE5) to LOW (needed for SA-1)
  PORTE &= ~(1 << 5);

  // Set CPU Clock(PH1) to Output
  DDRH |= (1 << 1);
  //PORTH &= ~(1 << 1);

  // Set IRQ(PH4) to Input
  DDRH &= ~(1 << 4);
  // Activate Internal Pullup Resistors
  //PORTH |= (1 << 4);

  // Set expand(PG5) to Imput
  DDRG &= ~(1 << 5);
  // Activate Internal Pullup Resistors
  //PORTG |= (1 << 5);

  // Set Data Pins (D0-D7) to Input
  DDRC = 0x00;
  // Enable Internal Pullups
  //PORTC = 0xFF;

  // Unused pins
  // Set wram(PE4) to Output
  DDRE |= (1 << 4);
  //PORTE &= ~(1 << 4);
  // Set pawr(PJ1) to Output
  DDRJ |= (1 << 1);
  //PORTJ &= ~(1 << 1);
  // Set pard(PJ0) to Output
  DDRJ |= (1 << 0);
  //PORTJ &= ~(1 << 0);

  // everything is set up. wait while the capacitors charge
  delay(1000);

  // Adafruit Clock Generator
  clockgen.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  clockgen.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  clockgen.set_pll(SI5351_PLL_FIXED, SI5351_PLLB);
  clockgen.set_freq(2147727200ULL, SI5351_CLK0);
  clockgen.set_freq(307200000ULL, SI5351_CLK2);
  clockgen.output_enable(SI5351_CLK0, 0);
  clockgen.output_enable(SI5351_CLK1, 0);
  clockgen.output_enable(SI5351_CLK2, 1); // output only CIC clock at first

  // Start CIC by outputting a low signal to cicrstPin(PG1)
  PORTG  &= ~(1 << 1);

  // Wait for CIC reset
  delay(84);
  // now freeze CIC comms, SA-1 (if present) should be unlocked
  clockgen.output_enable(SI5351_CLK2, 0);
  delay(84);
  // and run the power hungy addon chips (SA-1, etc)
  clockgen.output_enable(SI5351_CLK0, 1);

  // Print all the info
  getCartInfo_SNES();
}

/******************************************
   I/O Functions
 *****************************************/
// Switch control pins to write
void controlOut_SNES() {
  // Switch RD(PH6) and WR(PH5) to HIGH
  PORTH |= (1 << 6) | (1 << 5);
  // Switch CS(PH3) to LOW
  PORTH &= ~(1 << 3);
}

// Switch control pins to read
void controlIn_SNES() {
  // Switch WR(PH5) to HIGH
  PORTH |= (1 << 5);
  // Switch CS(PH3) and RD(PH6) to LOW
  PORTH &= ~((1 << 3) | (1 << 6));
}

/******************************************
   Low level functions
 *****************************************/
// Write one byte of data to a location specified by bank and address, 00:0000
void writeBank_SNES(byte myBank, word myAddress, byte myData) {
  PORTL = myBank;
  PORTF = myAddress & 0xFF;
  PORTK = (myAddress >> 8) & 0xFF;
  PORTC = myData;

  // Arduino running at 16Mhz -> one nop = 62.5ns
  // Wait till output is stable
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Switch WR(PH5) to LOW
  PORTH &= ~(1 << 5);

  // Leave WR low for at least 60ns
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Switch WR(PH5) to HIGH
  PORTH |= (1 << 5);

  // Leave WR high for at least 50ns
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
}

// Read one byte of data from a location specified by bank and address, 00:0000
byte readBank_SNES(byte myBank, word myAddress) {
  PORTL = myBank;
  PORTF = myAddress & 0xFF;
  PORTK = (myAddress >> 8) & 0xFF;

  // Arduino running at 16Mhz -> one nop = 62.5ns -> 1000ns total
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Read
  byte tempByte = PINC;
  return tempByte;
}

void readLoRomBanks( unsigned int start, unsigned int total, SdFile *file)
{
  byte buffer[512];

  for (int currBank = start; currBank < total; currBank++) {
    // Dump the bytes to SD 512B at a time
    for (long currByte = 32768; currByte < 65536; currByte += 512) {
      for (int c = 0; c < 512; c++) {
        buffer[c] = readBank_SNES(currBank, currByte + c);
      }
      file->write(buffer, 512);
    }
  }
}

void readHiRomBanks( unsigned int start, unsigned int total, SdFile *file)
{
  byte buffer[512];

  for (int currBank = start; currBank < total; currBank++) {
    for (long currByte = 0; currByte < 65536; currByte += 512) {
      for (int c = 0; c < 512; c++) {
        buffer[c] = readBank_SNES(currBank, currByte + c);
      }
      file->write(buffer, 512);
    }
  }
}

/******************************************
  SNES ROM Functions
******************************************/
void getCartInfo_SNES() {
  boolean manualConfig = 0;

  // Print start page
  if (checkcart_SNES() == 0) {
    // Checksum either corrupt or 0000
    manualConfig = 1;
    errorLvl = 1;
    rgb.setColor(255, 0, 0);

    display_Clear();
    println_Msg(F("ERROR"));
    println_Msg(F("Rom header corrupt"));
    println_Msg(F("or missing"));
    println_Msg(F(""));
    println_Msg(F(""));
    println_Msg(F("Press button for"));
    println_Msg(F("manual configuration"));
    println_Msg(F("or powercycle if SA1"));
    display_Update();
    wait();
    // Wait() clears errors but in this case we still have an error
    errorLvl = 1;
  }

  display_Clear();
  print_Msg(F("Name: "));
  println_Msg(romName);

  print_Msg(F("Type: "));
  if (romType == HI)
    print_Msg(F("HiROM"));
  else if (romType == LO)
    print_Msg(F("LoROM"));
  else if (romType == EX)
    print_Msg(F("ExHiRom"));
  else
    print_Msg(romType);
  print_Msg(F(" "));
  if (romSpeed == 0)
    println_Msg(F("SlowROM"));
  else if (romSpeed == 2)
    println_Msg(F("SlowROM"));
  else if (romSpeed == 3)
    println_Msg(F("FastROM"));
  else
    println_Msg(romSpeed);

  print_Msg(F("ICs: ROM "));
  if (romChips == 0)
    println_Msg(F("ONLY"));
  else if (romChips == 1)
    println_Msg(F("RAM"));
  else if (romChips == 2)
    println_Msg(F("SAVE"));
  else if (romChips == 3)
    println_Msg(F("DSP1"));
  else if (romChips == 4)
    println_Msg(F("DSP1 RAM"));
  else if (romChips == 5)
    println_Msg(F("DSP1 SAVE"));
  else if ((romChips == 19) || (romChips == 20) || (romChips == 21) || (romChips == 26))
    println_Msg(F("SuperFX"));
  else if (romChips == 52) {
    println_Msg(F("SA1 RAM"));
    romType = SA;
  }
  else if (romChips == 53) {
    println_Msg(F("SA1 RAM BATT"));
    romType = SA;
  }
  else if (romChips == 69) {
    println_Msg(F("SDD1 BATT"));
  }
  else if (romChips == 227)
    println_Msg(F("RAM GBoy"));
  else if (romChips == 243)
    println_Msg(F("CX4"));
  else if (romChips == 246)
    println_Msg(F("DSP2"));
  else if (romChips == 245)
    println_Msg(F("SPC RAM BATT"));
  else if (romChips == 249)
    println_Msg(F("SPC RAM RTC"));
  else
    println_Msg(F(""));

  print_Msg(F("Rom Size: "));
  print_Msg(romSize);
  println_Msg(F("Mbit"));

  print_Msg(F("Banks: "));
  print_Msg(numBanks);
  print_Msg(F(" Chips: "));
  println_Msg(romChips);

  print_Msg(F("Sram Size: "));
  print_Msg(sramSize);
  println_Msg(F("Kbit"));

  print_Msg(F("ROM Version: 1."));
  println_Msg(romVersion);

  print_Msg(F("Checksum: "));
  println_Msg(checksumStr);
  display_Update();

  // Wait for user input
  if (enable_OLED) {
    println_Msg(F(" "));
    println_Msg(F(" "));
    println_Msg(F("Press Button..."));
    display_Update();
    wait();
  }

  else if (enable_Serial) {
    println_Msg(F(" "));
  }

  // Start manual config
  if (manualConfig == 1) {
    confMenu();
  }
}

void checkAltConf() {
  char tempStr1[2];
  char tempStr2[5];
  char sizeStr[3];
  char bankStr[3];

  if (myFile.open("snes.txt", O_READ)) {
    while (myFile.available()) {
      // Read 4 bytes into String, do it one at a time so byte order doesn't get mixed up
      sprintf(tempStr1, "%c", myFile.read());
      strcpy(tempStr2, tempStr1);
      sprintf(tempStr1, "%c", myFile.read());
      strcat(tempStr2, tempStr1);
      sprintf(tempStr1, "%c", myFile.read());
      strcat(tempStr2, tempStr1);
      sprintf(tempStr1, "%c", myFile.read());
      strcat(tempStr2, tempStr1);

      // Check if string is a match
      if (strcmp(tempStr2, checksumStr) == 0) {

        // Skip the , in the file
        myFile.seekSet(myFile.curPosition() + 1);

        // Read next two bytes into a string
        romSize = myFile.read() - 48;
        romSize = romSize * 10 +  myFile.read() - 48;

        // Skip the , in the file
        myFile.seekSet(myFile.curPosition() + 1);

        // Add next two bytes to the string
        numBanks  = myFile.read() - 48;
        numBanks  =  numBanks  * 10 +  myFile.read() - 48;
      }
      // If no match empty string advance by 8 and try again
      else {
        myFile.seekSet(myFile.curPosition() + 8);
      }
    }
  }
  // Close the file:
  myFile.close();
}

// Read header information
boolean checkcart_SNES() {
  // set control to read
  dataIn();

  // Get Checksum as string
  sprintf(checksumStr, "%02X%02X", readBank_SNES(0, 65503), readBank_SNES(0, 65502));

  romType = readBank_SNES(0, 0xFFD5);
  if ((romType >> 5) != 1) {  // Detect invalid romType byte due to too long ROM name (22 chars)
    romType = LO; // LoROM   // Krusty's Super Fun House (U) 1.0 & Contra 3 (U)
  }
  else if (romType == 0x35) {
    romType = EX; // Check if ExHiROM
  }
  else if (romType == 0x3A) {
    romType = HI; // Check if SPC7110
  }
  else {
    romType &= 1; // Must be LoROM or HiROM
  }

  // Check RomSpeed
  romSpeed = (readBank_SNES(0, 65493) >> 4);

  // Check RomChips
  romChips = readBank_SNES(0, 65494);

  if (romChips == 69) {
    romSize = 48;
    numBanks = 96;
    romType = HI;
  }
  else if (romChips == 243) {
    cx4Type = readBank_SNES(0, 65481) & 0xF;
    if (cx4Type == 2) { // X2
      romSize = 12;
      numBanks = 48;
    }
    else if (cx4Type == 3) { // X3
      romSize = 16;
      numBanks = 64;
    }
  }
  else if ((romChips == 245) && (romType == HI)) {
    romSize = 24;
    numBanks = 48;
  }
  else if ((romChips == 249) && (romType == HI)) {
    romSize = 40;
    numBanks = 80;
  }
  else {
    // Check RomSize
    byte romSizeExp = readBank_SNES(0, 65495) - 7;
    romSize = 1;
    while (romSizeExp--)
      romSize *= 2;

    if ((romType == EX) || (romType == SA)) {
      numBanks = long(romSize) * 2;
    }
    else {
      numBanks = (long(romSize) * 1024 * 1024 / 8) / (32768 + (long(romType) * 32768));
    }
  }

  //Check SD card for alt config
  checkAltConf();

  // Get name
  byte myByte = 0;
  byte myLength = 0;
  for (unsigned int i = 65472; i < 65492; i++) {
    myByte = readBank_SNES(0, i);
    if (((char(myByte) >= 48 && char(myByte) <= 57) || (char(myByte) >= 65 && char(myByte) <= 122)) && myLength < 15) {
      romName[myLength] = char(myByte);
      myLength++;
    }
  }
  // If name consists out of all japanese characters use game code
  if (myLength == 0) {
    // Get rom code
    romName[0] = 'S';
    romName[1] = 'H';
    romName[2] = 'V';
    romName[3] = 'C';
    romName[4] = '-';
    for (unsigned int i = 0; i < 4; i++) {
      myByte = readBank_SNES(0, 0xFFB2 + i);
      if (((char(myByte) >= 48 && char(myByte) <= 57) || (char(myByte) >= 65 && char(myByte) <= 122)) && myLength < 4) {
        romName[myLength + 5] = char(myByte);
        myLength++;
      }
    }
    if (myLength == 0) {
      // Rom code unknown
      romName[0] = 'U';
      romName[1] = 'N';
      romName[2] = 'K';
      romName[3] = 'N';
      romName[4] = 'O';
      romName[5] = 'W';
      romName[6] = 'N';
    }
  }

  // Read sramSizeExp
  byte sramSizeExp;
  if ((romChips == 19) || (romChips == 20) || (romChips == 21) || (romChips == 26)) {
    // SuperFX
    if (readBank_SNES(0, 0x7FDA) == 0x33) {
      sramSizeExp = readBank_SNES(0, 0x7FBD);
    }
    else {
      if (strncmp(romName, "STARFOX2", 8) == 0) {
        sramSizeExp = 6;
      }
      else {
        sramSizeExp = 5;
      }
    }
  }
  else {
    // No SuperFX
    sramSizeExp = readBank_SNES(0, 0xFFD8);
  }

  // Calculate sramSize
  if (sramSizeExp != 0) {
    sramSizeExp = sramSizeExp + 3;
    sramSize = 1;
    while (sramSizeExp--)
      sramSize *= 2;
  }
  else {
    sramSize = 0;
  }

  // Check Cart Country
  cartCountry = readBank_SNES(0, 65497);

  // ROM Version
  romVersion = readBank_SNES(0, 65499);

  // Test if checksum is equal to reverse checksum
  if (((word(readBank_SNES(0, 65500)) + (word(readBank_SNES(0, 65501)) * 256)) + (word(readBank_SNES(0, 65502)) + (word(readBank_SNES(0, 65503)) * 256))) == 65535 ) {
    if (strcmp("0000", checksumStr) == 0) {
      return 0;
    }
    else {
      return 1;
    }
  }
  // Either rom checksum is wrong or no cart is inserted
  else {
    return 0;
  }
}

unsigned int calc_checksum (char* fileName, char* folder) {
  unsigned int calcChecksum = 0;
  unsigned int calcChecksumChunk = 0;
  int calcFilesize = 0;
  unsigned int c = 0;
  unsigned long i = 0;
  unsigned long j = 0;

  if (strcmp(folder, "root") != 0)
    sd.chdir(folder);

  // If file exists
  if (myFile.open(fileName, O_READ)) {
    calcFilesize = myFile.fileSize() * 8 / 1024 / 1024;

    // Nintendo Power (SF Memory Cassette)
    // Read up to 0x60000 then add FFs to 0x80000
    if (NP == true) {
      for (i = 0; i < (0x60000 / 512); i++) {
        myFile.read(sdBuffer, 512);
        for (c = 0; c < 512; c++) {
          calcChecksumChunk += sdBuffer[c];
        }
        calcChecksum = calcChecksumChunk;
      }
      calcChecksum += 0xF47C; // FFs from 0x60000-0x80000
    }
    else if ((calcFilesize == 10) || (calcFilesize == 12) || (calcFilesize == 20) || (calcFilesize == 24)) {
      unsigned long calcBase = 0;
      unsigned long calcMirror = 0;
      byte calcMirrorCount = 0;
      if (calcFilesize > 16)
        calcBase = 2097152;
      else
        calcBase = 1048576;
      calcMirror = myFile.fileSize() - calcBase;
      calcMirrorCount = calcBase / calcMirror;

      // Momotarou Dentetsu Happy Fix 3MB (24Mbit)
      if ((calcFilesize == 24) && (romChips == 245)) {
        for (i = 0; i < (myFile.fileSize() / 512); i++) {
          myFile.read(sdBuffer, 512);
          for (c = 0; c < 512; c++) {
            calcChecksumChunk += sdBuffer[c];
          }
        }
        calcChecksum = 2 * calcChecksumChunk;
      }
      else {
        // Base 8/16 Mbit chunk
        for (j = 0; j < (calcBase / 512); j++) {
          myFile.read(sdBuffer, 512);
          for (c = 0; c < 512; c++) {
            calcChecksumChunk += sdBuffer[c];
          }
        }
        calcChecksum = calcChecksumChunk;
        calcChecksumChunk = 0;
        // Add the mirrored chunk
        for (j = 0; j < (calcMirror / 512); j++) {
          myFile.read(sdBuffer, 512);
          for (c = 0; c < 512; c++) {
            calcChecksumChunk += sdBuffer[c];
          }
        }
        calcChecksum +=  calcMirrorCount * calcChecksumChunk;
      }
    }
    else if ((calcFilesize == 40) && (romChips == 85)) {
      // Daikaijuu Monogatari 2 Fix 5MB (40Mbit)
      // Add the 4MB (32Mbit) start
      for (j = 0; j < (4194304 / 512); j++) {
        myFile.read(sdBuffer, 512);
        for (c = 0; c < 512; c++) {
          calcChecksumChunk += sdBuffer[c];
        }
        calcChecksum = calcChecksumChunk;
      }
      calcChecksumChunk = 0;
      // Add the 1MB (8Mbit) end
      for (j = 0; j < (1048576 / 512); j++) {
        myFile.read(sdBuffer, 512);
        for (c = 0; c < 512; c++) {
          calcChecksumChunk += sdBuffer[c];
        }
      }
      calcChecksum +=  4 * calcChecksumChunk;
    }
    else if (calcFilesize == 48) {
      // Star Ocean/Tales of Phantasia Fix 6MB (48Mbit)
      // Add the 4MB (32Mbit) start
      for (j = 0; j < (4194304 / 512); j++) {
        myFile.read(sdBuffer, 512);
        for (c = 0; c < 512; c++) {
          calcChecksumChunk += sdBuffer[c];
        }
        calcChecksum = calcChecksumChunk;
      }
      calcChecksumChunk = 0;
      // Add the 2MB (16Mbit) end
      for (j = 0; j < (2097152 / 512); j++) {
        myFile.read(sdBuffer, 512);
        for (c = 0; c < 512; c++) {
          calcChecksumChunk += sdBuffer[c];
        }
      }
      calcChecksum +=  2 * calcChecksumChunk;
    }
    else {
      //calcFilesize == 2 || 4 || 8 || 16 || 32 || 40 || etc
      for (i = 0; i < (myFile.fileSize() / 512); i++) {
        myFile.read(sdBuffer, 512);
        for (c = 0; c < 512; c++) {
          calcChecksumChunk += sdBuffer[c];
        }
        calcChecksum = calcChecksumChunk;
      }
    }
    myFile.close();
    sd.chdir();
    return (calcChecksum);
  }
  else {
    // Else show error
    print_Error(F("DUMP ROM 1ST"), false);
    return 0;
  }
}

boolean compare_checksum() {

  println_Msg(F("Calculating Checksum"));
  display_Update();

  strcpy(fileName, romName);
  strcat(fileName, ".sfc");

  // last used rom folder
  EEPROM_readAnything(10, foldern);
  sprintf(folder, "SNES/ROM/%s/%d", romName, foldern - 1);

  char calcsumStr[5];
  sprintf(calcsumStr, "%04X", calc_checksum(fileName, folder));

  if (strcmp(calcsumStr, checksumStr) == 0) {
    print_Msg(F("Result: "));
    println_Msg(calcsumStr);
    println_Msg(F("Checksum matches"));
    display_Update();
    return 1;
  }
  else {
    print_Msg(F("Result: "));
    println_Msg(calcsumStr);
    print_Error(F("Checksum Error"), false);
    display_Update();
    return 0;
  }
}

// Read rom to SD card
void readROM_SNES() {
  // Set control
  dataIn();
  controlIn_SNES();

  // Get name, add extension and convert to char array for sd lib
  strcpy(fileName, romName);
  strcat(fileName, ".sfc");

  // create a new folder for the save file
  EEPROM_readAnything(10, foldern);
  sprintf(folder, "SNES/ROM/%s/%d", romName, foldern);
  sd.mkdir(folder, true);
  sd.chdir(folder);

  //clear the screen
  display_Clear();
  print_Msg(F("Saving to "));
  print_Msg(folder);
  println_Msg(F("/..."));
  display_Update();

  // write new folder number back to eeprom
  foldern = foldern + 1;
  EEPROM_writeAnything(10, foldern);

  //open file on sd card
  if (!myFile.open(fileName, O_RDWR | O_CREAT)) {
    print_Error(F("Can't create file on SD"), true);
  }

  //Dump Derby Stallion '96 (Japan) Actual Size is 24Mb
  if ((romType == LO) && (numBanks == 128) && (strcmp("CC86", checksumStr) == 0)) {
    // Read Banks 0x00-0x3F for the 1st/2nd MB
    for (int currBank = 0; currBank < 64; currBank++) {
      // Dump the bytes to SD 512B at a time
      for (long currByte = 32768; currByte < 65536; currByte += 512) {
        for (int c = 0; c < 512; c++) {
          sdBuffer[c] = readBank_SNES(currBank, currByte + c);
        }
        myFile.write(sdBuffer, 512);
      }
    }
    //Read Bank 0x80-9F for the 3rd MB
    for (int currBank = 128; currBank < 160; currBank++) {
      // Dump the bytes to SD 512B at a time
      for (long currByte = 32768; currByte < 65536; currByte += 512) {
        for (int c = 0; c < 512; c++) {
          sdBuffer[c] = readBank_SNES(currBank, currByte + c);
        }
        myFile.write(sdBuffer, 512);
      }
    }
  }

  //Dump Low-type ROM
  else if (romType == LO) {
    if (romChips == 243) { //0xF3
      cx4Map = readBank_SNES(0, 32594); //0x7F52
      if ((cx4Type == 2) && (cx4Map != 0)) { //X2
        dataOut();
        controlOut_SNES();
        writeBank_SNES(0, 32594, 0); // Set 0x7F52 to 0
        dataIn();
        controlIn_SNES();
      }
      else if ((cx4Type == 3) && (cx4Map == 0)) { //X3
        dataOut();
        controlOut_SNES();
        writeBank_SNES(0, 32594, 1); // Set 0x7F52 to 1
        dataIn();
        controlIn_SNES();
      }
    }
    if (romSize > 24) {
      // ROM > 96 banks (up to 128 banks)
      readLoRomBanks( 0x80, numBanks + 0x80, &myFile );
    } else {
      // Read up to 96 banks starting at bank 0×00.
      readLoRomBanks( 0, numBanks, &myFile );
    }
    if (romChips == 243) { //0xF3
      // Restore CX4 Mapping Register
      dataOut();
      controlOut_SNES();
      writeBank_SNES(0, 32594, cx4Map); // 0x7F52
      dataIn();
      controlIn_SNES();
    }
  }

  // Dump SDD1 High-type ROM
  else if ((romType == HI) && (romChips == 69)) {
    println_Msg(F("Dumping SDD1 HiRom"));
    display_Update();
    controlIn_SNES();
    byte initialSOMap = readBank_SNES(0, 18439);

    for (int currMemmap = 0; currMemmap < (numBanks / 16); currMemmap++) {

      dataOut();
      controlOut_SNES();

      writeBank_SNES(0, 18439, currMemmap);

      dataIn();
      controlIn_SNES();

      readHiRomBanks( 240, 256, &myFile );
    }

    dataOut();
    controlOut_SNES();

    writeBank_SNES(0, 18439, initialSOMap);

    dataIn();
    controlIn_SNES();
  }

  // Dump SPC7110 High-type ROM
  else if ((romType == HI) && ((romChips == 245) || (romChips == 249))) {
    println_Msg(F("Dumping SPC7110 HiRom"));
    display_Update();

    // 0xC00000-0xDFFFFF
    print_Msg(F("Part 1"));
    display_Update();
    readHiRomBanks( 192, 224, &myFile );

    if (numBanks > 32) {
      dataOut();
      controlOut_SNES();
      // Set 0x4834 to 0xFF
      writeBank_SNES( 0, 0x4834, 0xFF );

      dataIn();
      controlIn_SNES();

      // 0xE00000-0xEFFFFF
      print_Msg(F(" 2"));
      display_Update();
      readHiRomBanks( 224, 240, &myFile );

      if (numBanks > 48) {
        // 0xF00000-0xFFFFFF
        print_Msg(F(" 3"));
        display_Update();
        readHiRomBanks( 240, 256, &myFile );

        dataOut();
        controlOut_SNES();

        // Set 0x4833 to 3
        writeBank_SNES( 0, 0x4833, 3 );

        dataIn();
        controlIn_SNES();

        // 0xF00000-0xFFFFFF
        print_Msg(F(" 4"));
        display_Update();
        readHiRomBanks( 240, 256, &myFile );
      }
      println_Msg(F(""));

      // Return mapping registers to initial settings...
      dataOut();
      controlOut_SNES();

      writeBank_SNES( 0, 0x4833, 2 );
      writeBank_SNES( 0, 0x4834, 0 );

      dataIn();
      controlIn_SNES();
    }
  }

  // Dump standard High-type ROM
  else if ((romType == HI) || (romType == SA) || (romType == EX)) {
    println_Msg(F("Dumping HiRom..."));
    display_Update();

    readHiRomBanks( 192, numBanks + 192, &myFile );
  }

  // Close the file:
  myFile.close();
}

/******************************************
  SNES SRAM Functions
*****************************************/
// Write file to SRAM
void writeSRAM (boolean browseFile) {
  if (browseFile) {
    filePath[0] = '\0';
    sd.chdir("/");
    fileBrowser("Select srm file");
    // Create filepath
    sprintf(filePath, "%s/%s", filePath, fileName);
    display_Clear();
  }
  else
    sprintf(filePath, "%s", fileName);

  //open file on sd card
  if (myFile.open(filePath, O_READ)) {

    // Set pins to output
    dataOut();

    // Set RST RD WR to High and CS to Low
    controlOut_SNES();

    // LoRom
    if (romType == LO) {
      // Sram size
      long lastByte = (long(sramSize) * 128);

      if ((romChips == 19) || (romChips == 20) || (romChips == 21) || (romChips == 26)) { // SuperFX
        if (lastByte > 0x10000) { // Large SuperFX SRAM (no known carts)
          sramBanks = lastByte / 0x10000;
          for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
            for (long currByte = 0x0000; currByte < 0x10000; currByte++) {
              writeBank_SNES(currBank, currByte, myFile.read());
            }
          }
        }
        else { // SuperFX SRAM
          for (long currByte = 0; currByte < lastByte; currByte++) {
            writeBank_SNES(0x70, currByte, myFile.read());
          }
        }
      }
      else if (lastByte > 0x8000) { // Large SRAM Fix
        sramBanks = lastByte / 0x8000;
        for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
          for (long currByte = 0x0000; currByte < 0x8000; currByte++) {
            writeBank_SNES(currBank, currByte, myFile.read());
          }
        }
      }
      else {
        for (long currByte = 0; currByte <  lastByte; currByte++) {
          writeBank_SNES(0x70, currByte, myFile.read());
        }
      }
    }
    // HiRom
    else if (romType == HI) {
      if ((romChips == 245) || (romChips == 249)) { // SPC7110 SRAM
        // Configure SPC7110 SRAM Register
        // Set 0x4830 to 0x80
        writeBank_SNES(0, 0x4830, 0x80);
        // Sram size
        long lastByte = (long(sramSize) * 128) + 0x6000;
        // Write to sram bank
        for (long currByte = 0x6000; currByte < lastByte; currByte++) {
          writeBank_SNES(0x30, currByte, myFile.read());
        }
        // Reset SPC7110 SRAM Register
        dataOut();
        // Reset 0x4830 to 0x0
        writeBank_SNES(0, 0x4830, 0);
        dataIn();
      }
      else {
        // Writing SRAM on HiRom needs CS(PH3) to be high
        PORTH |=  (1 << 3);
        // Sram size
        long lastByte = (long(sramSize) * 128);
        if (lastByte > 0x2000) { // Large SRAM Fix
          sramBanks = lastByte / 0x2000;
          for (int currBank = 0x30; currBank < sramBanks + 0x30; currBank++) {
            for (long currByte = 0x6000; currByte < 0x8000; currByte++) {
              writeBank_SNES(currBank, currByte, myFile.read());
            }
          }
        }
        else {
          lastByte += 0x6000;
          // Write to sram bank
          for (long currByte = 0x6000; currByte < lastByte; currByte++) {
            writeBank_SNES(0x30, currByte, myFile.read());
          }
        }
      }
    }
    // ExHiRom
    else if (romType == EX) {
      // Writing SRAM on HiRom needs CS(PH3) to be high
      PORTH |=  (1 << 3);
      // Sram size
      long lastByte = (long(sramSize) * 128) + 0x6000;
      for (long currByte = 0x6000; currByte < lastByte; currByte++) {
        writeBank_SNES(0xB0, currByte, myFile.read());
      }
    }
    // SA1
    else if (romType == SA) {
      long lastByte = (long(sramSize) * 128);
      // Enable CPU Clock
      clockgen.set_freq(357954500ULL, SI5351_CLK1);
      clockgen.output_enable(SI5351_CLK1, 1);

      // Direct writes to BW-RAM (SRAM) in banks 0x40-0x43 don't work
      // Break BW-RAM (SRAM) into 0x2000 blocks
      byte lastBlock = 0;
      lastBlock = lastByte / 0x2000;

      // Writing SRAM on SA1 needs CS(PH3) to be high
      // PORTH |=  (1 << 3);

      // Setup BW-RAM
      // Set 0x2224 (SNES BMAPS) to map SRAM Block 0 to 0x6000-0x7FFF
      writeBank_SNES(0, 0x2224, 0);
      // Set 0x2226 (SNES SBWE) to 0x80 Write Enable
      writeBank_SNES(0, 0x2226, 0x80);
      // Set 0x2228 (SNES BWPA) to 0x00 BW-RAM Write-Protected Area
      writeBank_SNES(0, 0x2228, 0);
      delay(1000);

      // Use $2224 (SNES) to map BW-RAM block to 0x6000-0x7FFF
      // Use $2226 (SNES) to write enable the BW-RAM
      byte firstByte = 0;
      for (byte currBlock = 0; currBlock < lastBlock; currBlock++) {
        // Set 0x2224 (SNES BMAPS) to map SRAM Block to 0x6000-0x7FFF
        writeBank_SNES(0, 0x2224, currBlock);
        // Set 0x2226 (SNES SBWE) to 0x80 Write Enable
        writeBank_SNES(0, 0x2226, 0x80);
        for (long currByte = 0x6000; currByte < 0x8000; currByte += 512) {
          myFile.read(sdBuffer, 512);
          if ((currBlock == 0) && (currByte == 0x6000)) {
            firstByte = sdBuffer[0];
          }
          for (int c = 0; c < 512; c++) {
            writeBank_SNES(0, currByte + c, sdBuffer[c]);
          }
        }
      }
      // Rewrite First Byte
      writeBank_SNES(0, 0x2224, 0);
      writeBank_SNES(0, 0x2226, 0x80);
      writeBank_SNES(0, 0x6000, firstByte);
      // Disable CPU clock
      clockgen.output_enable(SI5351_CLK1, 0);
    }

    // Set pins to input
    dataIn();

    // Close the file:
    myFile.close();
    println_Msg(F("SRAM writing finished"));
    display_Update();

  }
  else {
    print_Error(F("File doesnt exist"), false);
  }
}

void readSRAM () {
  // set control
  controlIn_SNES();

  // Get name, add extension and convert to char array for sd lib
  strcpy(fileName, romName);
  strcat(fileName, ".srm");

  // create a new folder for the save file
  EEPROM_readAnything(10, foldern);
  sprintf(folder, "SNES/SAVE/%s/%d", romName, foldern);
  sd.mkdir(folder, true);
  sd.chdir(folder);

  // write new folder number back to eeprom
  foldern = foldern + 1;
  EEPROM_writeAnything(10, foldern);

  //open file on sd card
  if (!myFile.open(fileName, O_RDWR | O_CREAT)) {
    print_Error(F("SD Error"), true);
  }
  int sramBanks = 0;
  if (romType == LO) {
    // Sram size
    long lastByte = (long(sramSize) * 128);
    if ((romChips == 19) || (romChips == 20) || (romChips == 21) || (romChips == 26)) { // SuperFX
      if (lastByte > 0x10000) { // Large SuperFX SRAM (no known carts)
        sramBanks = lastByte / 0x10000;
        for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
          for (long currByte = 0x0000; currByte < 0x10000; currByte++) {
            myFile.write(readBank_SNES(currBank, currByte));
          }
        }
      }
      else { // SuperFX SRAM
        for (long currByte = 0; currByte < lastByte; currByte++) {
          myFile.write(readBank_SNES(0x70, currByte));
        }
      }
    }
    else if (lastByte > 0x8000) { // Large SRAM Fix
      sramBanks = lastByte / 0x8000;
      for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
        for (long currByte = 0x0000; currByte < 0x8000; currByte++) {
          myFile.write(readBank_SNES(currBank, currByte));
        }
      }
    }
    else {
      for (long currByte = 0; currByte < lastByte; currByte++) {
        myFile.write(readBank_SNES(0x70, currByte));
      }
    }
  }
  else if (romType == HI) {
    if ((romChips == 245) || (romChips == 249)) { // SPC7110 SRAM
      // Configure SPC7110 SRAM Register
      dataOut();
      // Set 0x4830 to 0x80
      writeBank_SNES(0, 0x4830, 0x80);
      dataIn();
      // Sram size
      long lastByte = (long(sramSize) * 128) + 0x6000;
      for (long currByte = 0x6000; currByte < lastByte; currByte++) {
        myFile.write(readBank_SNES(0x30, currByte));
      }
      dataOut();
      // Reset 0x4830 to 0x0
      writeBank_SNES(0, 0x4830, 0);
      dataIn();
    }
    else {
      // Dumping SRAM on HiRom needs CS(PH3) to be high
      PORTH |=  (1 << 3);
      // Sram size
      long lastByte = (long(sramSize) * 128);
      if (lastByte > 0x2000) { // Large SRAM Fix
        sramBanks = lastByte / 0x2000;
        for (int currBank = 0x30; currBank < sramBanks + 0x30; currBank++) {
          for (long currByte = 0x6000; currByte < 0x8000; currByte++) {
            myFile.write(readBank_SNES(currBank, currByte));
          }
        }
      }
      else {
        lastByte += 0x6000;
        for (long currByte = 0x6000; currByte < lastByte; currByte++) {
          myFile.write(readBank_SNES(0x30, currByte));
        }
      }
    }
  }
  else if (romType == EX) {
    // Dumping SRAM on HiRom needs CS(PH3) to be high
    PORTH |=  (1 << 3);
    // Sram size
    long lastByte = (long(sramSize) * 128) + 0x6000;
    for (long currByte = 0x6000; currByte < lastByte; currByte++) {
      myFile.write(readBank_SNES(0xB0, currByte));
    }
  }
  else if (romType == SA) {
    // Dumping SRAM on HiRom needs CS(PH3) to be high
    PORTH |=  (1 << 3);
    // Sram size
    long lastByte = (long(sramSize) * 128);
    if (lastByte > 0x10000) {
      sramBanks = lastByte / 0x10000;
      for (int currBank = 0x40; currBank < sramBanks + 0x40; currBank++) {
        for (long currByte = 0; currByte < 0x10000; currByte++) {
          myFile.write(readBank_SNES(currBank, currByte));
        }
      }
    }
    else {
      for (long currByte = 0x0; currByte < lastByte; currByte++) {
        myFile.write(readBank_SNES(0x40, currByte));
      }
    }
  }

  // Close the file:
  myFile.close();

  // Signal end of process
  display_Clear();
  print_Msg(F("Saved to "));
  print_Msg(folder);
  println_Msg(F("/..."));
  display_Update();
}

// Check if the SRAM was written without any error
unsigned long verifySRAM() {
  //open file on sd card
  if (myFile.open(filePath, O_READ)) {

    // Variable for errors
    writeErrors = 0;

    // Set control
    controlIn_SNES();

    int sramBanks = 0;
    if (romType == LO) {
      // Sram size
      long lastByte = (long(sramSize) * 128);
      if ((romChips == 19) || (romChips == 20) || (romChips == 21) || (romChips == 26)) { // SuperFX
        if (lastByte > 0x10000) { // Large SuperFX SRAM (no known carts)
          sramBanks = lastByte / 0x10000;
          for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
            for (long currByte = 0; currByte < 0x10000; currByte += 512) {
              //fill sdBuffer
              myFile.read(sdBuffer, 512);
              for (int c = 0; c < 512; c++) {
                if ((readBank_SNES(currBank, currByte + c)) != sdBuffer[c]) {
                  writeErrors++;
                }
              }
            }
          }
        }
        else { // SuperFX SRAM
          for (long currByte = 0; currByte < lastByte; currByte += 512) {
            //fill sdBuffer
            myFile.read(sdBuffer, 512);
            for (int c = 0; c < 512; c++) {
              if ((readBank_SNES(0x70, currByte + c)) != sdBuffer[c]) {
                writeErrors++;
              }
            }
          }
        }
      }
      else if (lastByte > 0x8000) { // Large SRAM Fix
        sramBanks = lastByte / 0x8000;
        for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
          for (long currByte = 0; currByte < 0x8000; currByte += 512) {
            //fill sdBuffer
            myFile.read(sdBuffer, 512);
            for (int c = 0; c < 512; c++) {
              if ((readBank_SNES(currBank, currByte + c)) != sdBuffer[c]) {
                writeErrors++;
              }
            }
          }
        }
      }
      else {
        for (long currByte = 0; currByte < lastByte; currByte += 512) {
          //fill sdBuffer
          myFile.read(sdBuffer, 512);
          for (int c = 0; c < 512; c++) {
            if ((readBank_SNES(0x70, currByte + c)) != sdBuffer[c]) {
              writeErrors++;
            }
          }
        }
      }
    }
    else if (romType == HI) {
      if ((romChips == 245) || (romChips == 249)) { // SPC7110 SRAM
        // Configure SPC7110 SRAM Register
        dataOut();
        // Set 0x4830 to 0x80
        writeBank_SNES(0, 0x4830, 0x80);
        dataIn();
        // Sram size
        long lastByte = (long(sramSize) * 128) + 0x6000;
        for (long currByte = 0x6000; currByte < lastByte; currByte += 512) {
          //fill sdBuffer
          myFile.read(sdBuffer, 512);
          for (int c = 0; c < 512; c++) {
            if ((readBank_SNES(0x30, currByte + c)) != sdBuffer[c]) {
              writeErrors++;
            }
          }
        }
        dataOut();
        // Reset 0x4830 to 0x0
        writeBank_SNES(0, 0x4830, 0);
        dataIn();
      }
      else {
        // Dumping SRAM on HiRom needs CS(PH3) to be high
        PORTH |=  (1 << 3);
        // Sram size
        long lastByte = (long(sramSize) * 128);
        if (lastByte > 0x2000) { // Large SRAM Fix
          sramBanks = lastByte / 0x2000;
          for (int currBank = 0x30; currBank < sramBanks + 0x30; currBank++) {
            for (long currByte = 0x6000; currByte < 0x8000; currByte += 512) {
              //fill sdBuffer
              myFile.read(sdBuffer, 512);
              for (int c = 0; c < 512; c++) {
                if ((readBank_SNES(currBank, currByte + c)) != sdBuffer[c]) {
                  writeErrors++;
                }
              }
            }
          }
        }
        else {
          lastByte += 0x6000;
          for (long currByte = 0x6000; currByte < lastByte; currByte += 512) {
            //fill sdBuffer
            myFile.read(sdBuffer, 512);
            for (int c = 0; c < 512; c++) {
              if ((readBank_SNES(0x30, currByte + c)) != sdBuffer[c]) {
                writeErrors++;
              }
            }
          }
        }
      }
    }
    else if (romType == EX) {
      // Dumping SRAM on HiRom needs CS(PH3) to be high
      PORTH |=  (1 << 3);
      // Sram size
      long lastByte = (long(sramSize) * 128) + 0x6000;
      for (long currByte = 0x6000; currByte < lastByte; currByte += 512) {
        //fill sdBuffer
        myFile.read(sdBuffer, 512);
        for (int c = 0; c < 512; c++) {
          if ((readBank_SNES(0xB0, currByte + c)) != sdBuffer[c]) {
            writeErrors++;
          }
        }
      }
    }
    else if (romType == SA) {
      // Dumping SRAM on HiRom needs CS(PH3) to be high
      PORTH |=  (1 << 3);
      // Sram size
      long lastByte = (long(sramSize) * 128);

      if (lastByte > 0x10000) {
        sramBanks = lastByte / 0x10000;
        for (int currBank = 0x40; currBank < sramBanks + 0x40; currBank++) {
          for (long currByte = 0x0; currByte < 0x10000; currByte += 512) {
            //fill sdBuffer
            myFile.read(sdBuffer, 512);
            for (int c = 0; c < 512; c++) {
              if ((readBank_SNES(currBank, currByte + c)) != sdBuffer[c]) {
                writeErrors++;
              }
            }
          }
        }
      }
      else {
        for (long currByte = 0x0; currByte < lastByte; currByte += 512) {
          //fill sdBuffer
          myFile.read(sdBuffer, 512);
          for (int c = 0; c < 512; c++) {
            if ((readBank_SNES(0x40, currByte + c)) != sdBuffer[c]) {
              writeErrors++;
            }
          }
        }
      }
      // Reset SA1
      // Set pins to input
      dataIn();
      // Close the file:
      myFile.close();
      if (writeErrors == 0) {
        println_Msg(F("Verified OK"));
      }
      else {
        print_Msg(F("Error: "));
        print_Msg(writeErrors);
        println_Msg(F(" bytes "));
        print_Error(F("did not verify."), false);
      }
      display_Update();
      wait();
      // Set reset pin to output (PH0)
      DDRH |= (1 << 0);
      // Switch RST(PH0) to LOW
      PORTH &= ~(1 << 0);
      display_Clear();
      print_Msg("Resetting...");
      display_Update();
      delay(3000);  // wait 3 secs
      asm volatile ("  jmp 0");
    }
    // Close the file:
    myFile.close();
    return writeErrors;
  }
  else {
    print_Error(F("Can't open file"), false);
  }
}

// Overwrite the entire SRAM
boolean eraseSRAM (byte b) {
  print_Msg(F("0x"));
  print_Msg(b, HEX);
  print_Msg(F(": "));
  display_Update();

  // Set pins to output
  dataOut();

  // Set control pins
  controlOut_SNES();

  int sramBanks = 0;
  if (romType == LO) {
    // Sram size
    long lastByte = (long(sramSize) * 128);

    if ((romChips == 19) || (romChips == 20) || (romChips == 21) || (romChips == 26)) { // SuperFX
      if (lastByte > 0x10000) { // Large SuperFX SRAM (no known carts)
        sramBanks = lastByte / 0x10000;
        for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
          for (long currByte = 0x0000; currByte < 0x10000; currByte++) {
            writeBank_SNES(currBank, currByte, b);
          }
        }
      }
      else { // SuperFX SRAM
        for (long currByte = 0; currByte < lastByte; currByte++) {
          writeBank_SNES(0x70, currByte, b);
        }
      }
    }
    else if (lastByte > 0x8000) { // Large SRAM Fix
      sramBanks = lastByte / 0x8000;
      for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
        for (long currByte = 0x0000; currByte < 0x8000; currByte++) {
          writeBank_SNES(currBank, currByte, b);
        }
      }
    }
    else {
      for (long currByte = 0; currByte <  lastByte; currByte++) {
        writeBank_SNES(0x70, currByte, b);
      }
    }
  }
  else if (romType == HI) {
    if ((romChips == 245) || (romChips == 249)) { // SPC7110 SRAM
      // Configure SPC7110 SRAM Register
      // Set 0x4830 to 0x80
      writeBank_SNES(0, 0x4830, 0x80);
      // Sram size
      long lastByte = (long(sramSize) * 128) + 0x6000;
      // Write to sram bank
      for (long currByte = 0x6000; currByte < lastByte; currByte++) {
        writeBank_SNES(0x30, currByte, b);
      }
      // Reset SPC7110 SRAM Register
      dataOut();
      // Reset 0x4830 to 0x0
      writeBank_SNES(0, 0x4830, 0);
      dataIn();
    }
    else {
      // Writing SRAM on HiRom needs CS(PH3) to be high
      PORTH |=  (1 << 3);
      // Sram size
      long lastByte = (long(sramSize) * 128);
      if (lastByte > 0x2000) { // Large SRAM Fix
        sramBanks = lastByte / 0x2000;
        for (int currBank = 0x30; currBank < sramBanks + 0x30; currBank++) {
          for (long currByte = 0x6000; currByte < 0x8000; currByte++) {
            writeBank_SNES(currBank, currByte, b);
          }
        }
      }
      else {
        lastByte += 0x6000;
        // Write to sram bank
        for (long currByte = 0x6000; currByte < lastByte; currByte++) {
          writeBank_SNES(0x30, currByte, b);
        }
      }
    }
  }
  // ExHiRom
  else if (romType == EX) {
    // Writing SRAM on HiRom needs CS(PH3) to be high
    PORTH |=  (1 << 3);
    // Sram size
    long lastByte = (long(sramSize) * 128) + 0x6000;
    for (long currByte = 0x6000; currByte < lastByte; currByte++) {
      writeBank_SNES(0xB0, currByte, b);
    }
  }
  // SA1
  else if (romType == SA) {
    long lastByte = (long(sramSize) * 128);
    // Enable CPU Clock
    clockgen.set_freq(357954500ULL, SI5351_CLK1);
    clockgen.output_enable(SI5351_CLK1, 1);

    // Direct writes to BW-RAM (SRAM) in banks 0x40-0x43 don't work
    // Break BW-RAM (SRAM) into 0x2000 blocks
    // Use $2224 to map BW-RAM block to 0x6000-0x7FFF
    byte lastBlock = 0;
    lastBlock = lastByte / 0x2000;

    // Writing SRAM on SA1 needs CS(PH3) to be high
    // PORTH |=  (1 << 3);

    // Setup BW-RAM
    // Set 0x2224 (SNES BMAPS) to map SRAM Block 0 to 0x6000-0x7FFF
    writeBank_SNES(0, 0x2224, 0);
    // Set 0x2226 (SNES SBWE) to 0x80 Write Enable
    writeBank_SNES(0, 0x2226, 0x80);
    // Set 0x2228 (SNES BWPA) to 0x00 BW-RAM Write-Protected Area
    writeBank_SNES(0, 0x2228, 0);
    delay(1000);

    // Use $2224 (SNES) to map BW-RAM block to 0x6000-0x7FFF
    // Use $2226 (SNES) to write enable the BW-RAM
    for (byte currBlock = 0; currBlock < lastBlock; currBlock++) {
      // Set 0x2224 (SNES BMAPS) to map SRAM Block to 0x6000-0x7FFF
      writeBank_SNES(0, 0x2224, currBlock);
      // Set 0x2226 (SNES SBWE) to 0x80 Write Enable
      writeBank_SNES(0, 0x2226, 0x80);
      for (long currByte = 0x6000; currByte < 0x8000; currByte += 512) {
        for (int c = 0; c < 512; c++) {
          writeBank_SNES(0, currByte + c, b);
        }
      }
    }
    // Rewrite First Byte
    writeBank_SNES(0, 0x2224, 0);
    writeBank_SNES(0, 0x2226, 0x80);
    writeBank_SNES(0, 0x6000, b);
    // Disable CPU clock
    clockgen.output_enable(SI5351_CLK1, 0);
  }

  dataIn();

  // Variable for errors
  writeErrors = 0;

  // Set control
  controlIn_SNES();

  sramBanks = 0;
  if (romType == LO) {
    // Sram size
    long lastByte = (long(sramSize) * 128);
    if ((romChips == 19) || (romChips == 20) || (romChips == 21) || (romChips == 26)) { // SuperFX
      if (lastByte > 0x10000) { // Large SuperFX SRAM (no known carts)
        sramBanks = lastByte / 0x10000;
        for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
          for (long currByte = 0; currByte < 0x10000; currByte += 512) {
            for (int c = 0; c < 512; c++) {
              if ((readBank_SNES(currBank, currByte + c)) != b) {
                writeErrors++;
              }
            }
          }
        }
      }
      else { // SuperFX SRAM
        for (long currByte = 0; currByte < lastByte; currByte += 512) {
          for (int c = 0; c < 512; c++) {
            if ((readBank_SNES(0x70, currByte + c)) != b) {
              writeErrors++;
            }
          }
        }
      }
    }
    else if (lastByte > 0x8000) { // Large SRAM Fix
      sramBanks = lastByte / 0x8000;
      for (int currBank = 0x70; currBank < sramBanks + 0x70; currBank++) {
        for (long currByte = 0; currByte < 0x8000; currByte += 512) {
          for (int c = 0; c < 512; c++) {
            if ((readBank_SNES(currBank, currByte + c)) != b) {
              writeErrors++;
            }
          }
        }
      }
    }
    else {
      for (long currByte = 0; currByte < lastByte; currByte += 512) {
        for (int c = 0; c < 512; c++) {
          if ((readBank_SNES(0x70, currByte + c)) != b) {
            writeErrors++;
          }
        }
      }
    }
  }
  else if (romType == HI) {
    if ((romChips == 245) || (romChips == 249)) { // SPC7110 SRAM
      // Configure SPC7110 SRAM Register
      dataOut();
      // Set 0x4830 to 0x80
      writeBank_SNES(0, 0x4830, 0x80);
      dataIn();
      // Sram size
      long lastByte = (long(sramSize) * 128) + 0x6000;
      for (long currByte = 0x6000; currByte < lastByte; currByte += 512) {
        for (int c = 0; c < 512; c++) {
          if ((readBank_SNES(0x30, currByte + c)) != b) {
            writeErrors++;
          }
        }
      }
      dataOut();
      // Reset 0x4830 to 0x0
      writeBank_SNES(0, 0x4830, 0);
      dataIn();
    }
    else {
      // Dumping SRAM on HiRom needs CS(PH3) to be high
      PORTH |=  (1 << 3);
      // Sram size
      long lastByte = (long(sramSize) * 128);
      if (lastByte > 0x2000) { // Large SRAM Fix
        sramBanks = lastByte / 0x2000;
        for (int currBank = 0x30; currBank < sramBanks + 0x30; currBank++) {
          for (long currByte = 0x6000; currByte < 0x8000; currByte += 512) {
            for (int c = 0; c < 512; c++) {
              if ((readBank_SNES(currBank, currByte + c)) != b) {
                writeErrors++;
              }
            }
          }
        }
      }
      else {
        lastByte += 0x6000;
        for (long currByte = 0x6000; currByte < lastByte; currByte += 512) {
          for (int c = 0; c < 512; c++) {
            if ((readBank_SNES(0x30, currByte + c)) != b) {
              writeErrors++;
            }
          }
        }
      }
    }
  }
  else if (romType == EX) {
    // Dumping SRAM on HiRom needs CS(PH3) to be high
    PORTH |=  (1 << 3);
    // Sram size
    long lastByte = (long(sramSize) * 128) + 0x6000;
    for (long currByte = 0x6000; currByte < lastByte; currByte += 512) {
      for (int c = 0; c < 512; c++) {
        if ((readBank_SNES(0xB0, currByte + c)) != b) {
          writeErrors++;
        }
      }
    }
  }
  else if (romType == SA) {
    // Dumping SRAM on HiRom needs CS(PH3) to be high
    PORTH |=  (1 << 3);
    // Sram size
    long lastByte = (long(sramSize) * 128);
    if (lastByte > 0x10000) {
      sramBanks = lastByte / 0x10000;
      for (int currBank = 0x40; currBank < sramBanks + 0x40; currBank++) {
        for (long currByte = 0x0; currByte < 0x10000; currByte += 512) {
          for (int c = 0; c < 512; c++) {
            if ((readBank_SNES(currBank, currByte + c)) != b) {
              writeErrors++;
            }
          }
        }
      }
    }
    else {
      for (long currByte = 0x0; currByte < lastByte; currByte += 512) {
        for (int c = 0; c < 512; c++) {
          if ((readBank_SNES(0x40, currByte + c)) != b) {
            writeErrors++;
          }
        }
      }
    }
  }
  if (writeErrors == 0) {
    println_Msg(F("OK"));
    return 1;
  }
  else {
    println_Msg(F("ERROR"));
    return 0;
  }
  display_Update();
}

//******************************************
// End of File
//******************************************
