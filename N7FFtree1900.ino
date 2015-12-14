
/*

    File:  N7FFtree1900.ino (Version: 19.00)
    Title: N7FF Christmas Tree Lights
    Created by Frank Fahrlander on December 14, 2015.
    Copyright © 2015 Frank Fahrlander. All rights reserved.

*/


#include <IRremote.h>
#include <IRremoteInt.h>
#include <avr/pgmspace.h>
#include <MemoryFree.h>

//======================== Interactive commands ================----------------------------------------
const char c01[] PROGMEM  = "   Pick brightness        ";
const char c02[] PROGMEM  = "   Pick color combination ";
const char c03[] PROGMEM  = "   Pick channel           ";
const char c04[] PROGMEM  = "   Pick slice             ";
const char c05[] PROGMEM  = "   Pick limb              ";
const char c06[] PROGMEM  = "   Pick TLC               ";
const char c07[] PROGMEM  = "   Program                ";
const char c08[] PROGMEM  = "   Program - bright       ";
const char c09[] PROGMEM  = "   Control                ";
//==================== Pattern display commands ================----------------------------------------
//                           .........2 digit # of next command to run after this one
//        cmd #              ||.......time to run this command (' '=90 seconds '1-9'=10-90 seconds)
//          ||               |||
//          vv               vvv
const char c10[] PROGMEM  = "11 Random intensities     ";  // 10
const char c11[] PROGMEM  = "20 Color wheel            ";  // 11
const char c12[] PROGMEM  = "17 Cycle Linear           ";  // 05
const char c13[] PROGMEM  = "18 Cycle Binary           ";  // 03
const char c14[] PROGMEM  = "19 Cycle thru RGB levels  ";  // 08
const char c15[] PROGMEM  = "21 Ramp by color          ";  // 01 start
const char c16[] PROGMEM  = "14 Slowly 0->Max->0...    ";  // 07
const char c17[] PROGMEM  = "16 Blink show             ";  // 06
const char c18[] PROGMEM  = "12 Random blink - group   ";  // 04
const char c19[] PROGMEM  = "10 Rotate Blink by color  ";  // 09
const char c20[] PROGMEM  = "15 Fluctuate colors       ";  // 12 end
const char c21[] PROGMEM  = "13 Cycle RGB brightness   ";  // 02
const char c22[] PROGMEM  = "   Snake LEDs             ";
const char c23[] PROGMEM  = "   Color slow change      ";
const char c24[] PROGMEM  = "   Rotate slice           ";
const char c25[] PROGMEM  = "   Rotate nodes           ";
const char c26[] PROGMEM  = "   Rotate channels b-t    ";
const char c27[] PROGMEM  = "   Rotate channels t-b    ";
const char c28[] PROGMEM  = "   Rotate limbs l-r       ";
const char c29[] PROGMEM  = "   Rotate limbs r-l       ";
const char c30[] PROGMEM  = "   Rotate levels          ";
const char c31[] PROGMEM  = "   Rotate TLCs b-t        ";
const char c32[] PROGMEM  = "   Rotate TLCs t-b        ";
const char c33[] PROGMEM  = "   Rotate R,G & B         ";
//=============== Keyboard development commands ================----------------------------------------
const char c34[] PROGMEM  = "   Dump bV table          ";

#define Fcmd    15                                              // first command to do in cycle pgm  
#define Nir     33
#define Ncmds   34

const char* const Cmd[Ncmds] PROGMEM = {
  c01, c02, c03, c04, c05, c06, c07, c08, c09, c10,
  c11, c12, c13, c14, c15, c16, c17, c18, c19, c20,
  c21, c22, c23, c24, c25, c26, c27, c28, c29, c30,
  c31, c32, c33, c34
};


//================= General global data ========================----------------------------------------
FILE           serial_stdout;                                   // Required for fprint to work

#define Nbuff 50                                                // buffer size (keyboard & cmd titles)
char           inBuffer[Nbuff];                                 // keyboard/string input buffer

bool           masterStop = false;                              // Global control pattern execution
unsigned long  stopTime   = 0L;                                 // Used in conjunction w/ masterStop
bool           timeOut    = false;                              // true means masterStop applied

int            memFree;                                         // current free memory
int            minFree    = 2048;                               // Minimum free memory ever seen

bool           useSound   = true;                               // sound is on by default



//==================== Ports and LED/TLC related ===============----------------------------------------
#define IRpin   11                                              // pin for the IR sensor
#define tonePin 12                                              // speaker output
#define pData   4                                               // PORTD on=(| 0x10) off=(& 0xef)
#define pClock  5                                               // PORTD on=(| 0x20) off=(& 0xdf)
#define pBlank  6                                               // PORTD on=(| 0x40) off=(& 0xbf) 
#define pLatch  7                                               // PORTD on=(| 0x80) off=(& 0x7f)

#define Ntlc    9                                               // Number of chained TLC5947's
#define Nch     216                                             // 9 x 24 channels
int     bV[Nch];                                                // brightness values per channel

int     Max = 24;                                               // Maximum LED brightness
int     Inc = 2;                                                // 1/40th Max

bool          starOn      = true;                               // true = starV used (not bV) for Star
bool          starTwinkle = true;                               // true = star brightness varied
unsigned long starChanged;                                      // millis time of last twinkle
unsigned long starPeriod  = 1600;                               // msec time between twinkles
int           starV[3]    = { 255, 255, 255 };                  // star brightness values

bool    tlcTrace    = false;                                    // true = display after each channel
int     tlcDelay    = 100;                                      // msec delay if tlcTrace true

bool    useBandaid  = true;                                     // true avoids 2048 brightness

#define Nlevel 10                                               // Number of light levels
int     bVlevel[Nlevel] = {   1,   15,   31,  63,   127,        // Brightness selection levels
                            255,  511, 1023, 2047, 4095
                          };

#define RED    0
#define GREEN  1
#define BLUE   2
int     ledType[3] = { BLUE, GREEN, RED };                      // LED color within 3 channel group

#define _ALL_OFF     0                                          // RGB color codes
#define _RED         1
#define _GRN         2
#define _RED_GRN     3
#define _BLU         4
#define _RED_BLU     5
#define _GRN_BLU     6
#define _RED_GRN_BLU 7

bool    ledOff[3]  = {false, false, false};                     // BLUE, GREEN, RED leds enabled

#define Nnode  11
int     nodeCh[Nnode + 1]  = {   0,  18,  36,  60,  84, 102,    // Channel nr of nodes plus star
                               120, 138, 156, 174, 192, 213
                             };

#define Nlimbs 72                                               // Includes star on top
#define Nslice 6                                                // vertical strip of limbs


//===================== IR Remote related ======================----------------------------------------
#define IRWAIT  100                                             // time betwen valid IR key hits
#define UP      10                                              // IR UP key
#define DOWN    11                                              // IR DOWN arrow key
#define RIGHT   12                                              // IR RIGHT arrow key
#define LEFT    13                                              // IR LEFT arrow key 
#define OK      14                                              // IR OKAY key

IRrecv          irrecv(IRpin);                                  // IR Receive object
decode_results  results;                                        // IR decode structure

unsigned long   lastIRtime  = 0;                                // Time that last IR key was hit
int             lastCmd     = 0;                                // Current IR command number
boolean         irFirstRcvd = false;                            // True = 1st of 2 digits received

#define Ncodes  15                                              // # of raw IR codes defined
struct          codeKeyPair {                                   // Map between Key code and key
  unsigned long code;                                           // raw code
  int           key;                                            // key it represents
}               ir[Ncodes] = {
  // Key codes for Linda's chucked Magnavox DVD/VHS player IR remote control
  { 0xe144728d,  0}, { 0xe144a25d,  1}, { 0xe144629d,  2}, { 0xe144e21d,  3}, { 0xe14412ed,  4},
  { 0xe144926d,  5}, { 0xe14452ad,  6}, { 0xe144d22d,  7}, { 0xe14432cd,  8}, { 0xe144b24d,  9},
  { 0xe144ee11, 10}, { 0xe1441ee1, 11}, { 0xe1446e91, 12}, { 0xe144ae51, 13}, { 0xe1444eb1, 14}
};



