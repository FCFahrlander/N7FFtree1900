
/*

    File:  treeTLC.ino (Version: 19.00)
    Title: TLC5947 related functions for N7FF Christmas Tree Lights
    Created by Frank Fahrlander on December 14, 2015.
    Copyright © 2015 Frank Fahrlander. All rights reserved.

    Control                   Set tree controls
    dumpBv                    Dump bV values in hex
    Test                      Test code
    tlcLight                  Light up a specific TLC
    tlcWrite                  Write data to all TLC5974's


*/



//==============================================================---------------------------------------
void Control(void) {                                            // Set tree controls
  int key;
  Serial.print(F("\n 0=clear    1=Trace    2=Bandaid   3=Write    4=Sound"));
  Serial.print(F("\n 5=StarON   6=Twinkle  7=DumpBv    8=MaxChg   9=Test"));
  Serial.print(F("\nUP=tlcChg  DN=BLUtgl  LF=GRNtgl   RT=REDtgl  OK=Quit\n"));

  while (true) {
    if (keyboardCheck()) return;
    key = irGetKey();
    switch (key) {

      case OK:
        return;

      case 0:
        clearBv();
        break;

      case 1:
        tlcTrace = !tlcTrace;
        if (tlcTrace) Serial.print(F("\ntlcTrace ON"));
        else          Serial.print(F("\ntlcTrace OFF"));
        break;

      case 2:
        useBandaid = !useBandaid;
        if (useBandaid) Serial.print(F("\nBandaid ON"));
        else            Serial.print(F("\nBandaid OFF"));
        break;

      case 3:
        tlcWrite();
        break;

      case 4:
        useSound = !useSound;
        if (useSound) Serial.print(F("\nSound ON"));
        else          Serial.print(F("\nSound OFF"));
        break;

      case 5:
        starOn = !starOn;
        if (starOn) Serial.print(F("\nStar ON"));
        else        Serial.print(F("\nStar OFF"));
        break;

      case 6:
        if (starTwinkle) {
          if      (starPeriod > 800) starPeriod = 800;
          else if (starPeriod > 400) starPeriod = 400;
          else if (starPeriod > 200) starPeriod = 200;
          else if (starPeriod > 100) starPeriod = 100;
          else {
            starTwinkle = false;
            Serial.print(F("\nTwinkle OFF"));
          }
        } else {
          starTwinkle = true;
          starPeriod  = 1600;
          Serial.print(F("\nTwinkle ON"));
        }
        break;

      case 7:
        dumpBv();
        break;

      case 8:
        if (Max < 4095) Max = 4095;
        else            Max = 255;
        break;

      case 9:
        Test();
        break;

      case UP:
        if (tlcDelay < 100) tlcDelay = 100;
        else                tlcDelay = 10;
        break;

      case DOWN:  // Blue
        if (ledOff[0]) ledOff[0] = false;
        else           ledOff[0] = true;
        break;
      case RIGHT:  // Green
        if (ledOff[1]) ledOff[1] = false;
        else           ledOff[1] = true;
        break;
      case LEFT:   // Red
        if (ledOff[2]) ledOff[2] = false;
        else           ledOff[2] = true;
        break;

      default:
        break;
    }
  }
}



//==============================================================---------------------------------------
void dumpBv(void) {                                             // Dump bV values in hex
  int c;
  Serial.print(F("\n BLU GRN RED blu grn red BLU GRN RED blu grn red"));
  Serial.print(F(" BLU GRN RED blu grn red BLU GRN RED blu grn red"));
  for (c = 0; c < Nch; c++) {                                   // for each channel
    if (!(c % 24)) Serial.print(F("\n"));                       // in groups of 24
    printf(" %3x", bV[c]);                                      // print value
  }
  Serial.print(F("\n"));
}



//==============================================================---------------------------------------
void Test(void) {                                               // Test code
  int key;
  while (true) {
    if (keyboardCheck()) return;
    key = irGetKey();
    switch (key) {

      case 0:                                                   // bandaid and No trace
        useBandaid = true;
        tlcTrace = false;
        tlcLight(2, 2048, true);
        tlcLight(4, 2048, false);
        break;

      case 1:                                                   // No bandaid and No trace
        useBandaid = false;
        tlcTrace = false;
        tlcLight(2, 2048, true);
        tlcLight(4, 2048, false);
        break;

      case 2:                                                   // No bandaid and trace
        useBandaid = false;
        tlcTrace = true;
        tlcLight(2, 2048, true);
        tlcLight(4, 2048, false);
        break;

      case OK:                                                  //  End of Test session
        return;
        break;
    }
  }
}



//==============================================================---------------------------------------
int tlcLight(int tlc, int b, bool clearIt) {                    // Light a specific TLCs
  int t;
  int c;
  t = tlc;
  if (t < 0)     t = Ntlc - 1;
  if (t >= Ntlc) t = 0;
  if (clearIt) clearBv();
  for (c = 0; c < 23; c++) bV[t * 24 + c] = b;
  tlcWrite();
  return (t);
}



//==============================================================---------------------------------------
void tlcWrite(void) {                                           // Write data to all TLC5974's
  int   c;                                                      // Channel number (24 ch/TLC)
  int   b;                                                      // bit number     (12 bits/ch)
  int   v;                                                      // brightness of ch
  for (c = Nch - 1; c >= 0 ; c--) {                             // Scan highest to lowest ch
    if (starOn && (c >= (Nch - 3)) ) {                          // use starV if starOn & 1st 3 ch
      v = starV[Nch - 1 - c];                                   // Star brightness values
    }
    else {
      if (ledOff[c % 3]) v = 0;                                 // check if color is disabled
      else v = bV[c];                                           // Brightness for ch
    }
    if (useBandaid && (v == 2048)) v = 2047;                    // 2048 causes mysterious problem
    for (b = 11; b >= 0; b--) {                                 // 12bits/ch. MSB first
      PORTD &= 0xdf;                                            // Clock LOW
      if (v & (1 << b)) PORTD |= 0x10;                          // Data HIGH
      else              PORTD &= 0xef;                          // Data LOW
      PORTD |= 0x20;                                            // Clock HIGH
    }
    if (tlcTrace) {                                             // TLC trace
      // display after each TLC channel shifted out
      PORTD &= 0xdf;                                            // Clock LOW
      
      PORTD |= 0x80;                                            // Latch HIGH
      PORTD &= 0x7f;                                            // Latch LOW
      delay(tlcDelay);
    }
  }
  PORTD &= 0xdf;                                                // Clock LOW
  PORTD |= 0x40;                                                // Blank HIGH
  PORTD |= 0x80;                                                // Latch HIGH
  PORTD &= 0x7f;                                                // Latch LOW
  PORTD &= 0xbf;                                                // Blank LOW
}


