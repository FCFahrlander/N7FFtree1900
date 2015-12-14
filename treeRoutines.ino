
/*

    File:  treeRoutines.ino (Version: 19.00)
    Title: Supplementary functions for N7FF Christmas Tree Lights
    Created by Frank Fahrlander on December 14, 2015.
    Copyright © 2015 Frank Fahrlander. All rights reserved.

======================================== Function list ================================================

    beep                      Make a tone without interrupts

    blinkRandomly             Randomly blink a channel
    blinkShow                 Blink using 7 different patterns

    channelLight              Light up a specific channel

    clearBv                   Set all LED brightnesses data to 0

    colorBlinkRotate          Rotate blinking through colors
    colorLight                Light up a specific color
    colorSlowChange           Changes colors/intensities slowly
    colorWheel                Vary RGB brightnesses in a sequence

    cycleBinary               Binary Cycle brightness
    cycleLinear               Linear Cycle brightness

    fluctuate                 Fluctuate color intensities slowly
    irGetKey                  Get and interpret IR key

    keyboardCheck             Check if keyboard input present
    keyboardNumber            Get a number from the user
    keyboardWait              Wait for user keyboard input

    ledEnable                 Enable LEDs based on encoded value

    levelLight                Light at a specific level
    limbLight                 Light up a specific limb
    nodeLight                 Light up a specific node

    pickIt                    Select or rotate through object

    rampBrightness            Ramp up brightness by color
    rampBrightnessUpDown      Ramp brightness up and down
    randomBrightness          Set LEDs to random brightness
    rgbCycle1                 Cycle brightness on RGB channels
    rgbCycle2                 Rotate thru R,G,B brightness
    rgbLight                  Light a specific color combo

    serial_putchar            Function needed for printf to work
    setColor                  Adjust brightness of one specific color
    setMax                    Set LED max limit and Increment
    setRGB                    Set R,G and B LED intensities

    sliceLight                Light up a specific slice
    sliceRotate               Slice rotate
    sliceRotateProgram        Slice rotate program

    snake                     Cyclically snake lights from top to bottom

    starRandom                Twinkle the topper
    starSet                   Set topper brightnesses

    userStop                  Check for KB key or IR OK key
    wait                      Delay with timeout/interrupt control
    wait2                     Pause & check between show acts

*/


//==============================================================
void beep(int freq, unsigned int len) {                         // Make a tone w/o interrupts
  unsigned long sTime;                                          // start time of the tone
  unsigned int period;                                          // 1/2 wavelength of tone
  if (!useSound) return;                                        // return, if silenced
  sTime = millis();                                             // get start time
  period = 500000 / freq;                                       // frequency to half period
  while ((millis() - sTime) < len) {                            // until tone duration complete
    digitalWrite(tonePin, LOW);                                 // tone pin HIGH
    delayMicroseconds(period);                                  // wait half wavelength
    digitalWrite(tonePin, HIGH);                                // tone pin LOW
    delayMicroseconds(period);                                  // wait half wavelength
  }
}



//==============================================================---------------------------------------
void blinkRandomly(int d) {                                     // Randomly blink a channel
  int c, n;
  setRGB(Max, Max, Max);                                        // Turn all LEDs on
  while (true) {
    for (n = 0; n < 45; n++) {
      c = random(Nch);
      if (bV[c] > 0) bV[c] = 0;
      else           bV[c] = Max;
    }
    tlcWrite();
    if (wait(d)) return;
  }
}



//==============================================================---------------------------------------
void blinkShow(void) {                                          // Blink using 7 different patterns
  int p;
  int j;
  int c;
  //  program:   0   1   2   3   4   5   6   7,  8,  9
  int p1[] = {  10,  5,  5,  5,  5,  5,  5,  5,  5,  5 };       // Number of repeat performances
  int p2[] = {   0,  1,  1,  0,  0,  0,  0,  1,  1,  0 };       // Red in Backround?
  int p3[] = {   0,  0,  0,  1,  1,  0,  0,  0,  1,  1 };       // Green in Background?
  int p4[] = {   0,  0,  0,  0,  0,  1,  1,  1,  0,  1 };       // Blue in Background?
  int p5[] = { 150, 50, 50, 50, 50, 50, 50, 50, 50, 50 };       // Number of random selections
  int p6[] = {   1,  3,  3,  3,  3,  3,  3,  3,  3,  3 };       // Every 1 or 1/3 of them
  int p7[] = {   0,  1,  2,  0,  2,  0,  1,  1,  2,  0 };       // random color 0=r, 1=g and 2=b
  int p8[] = {  10, 60, 60, 60, 60, 60, 60, 60, 60, 60 };       // delay in msec.
  while (true) {
    for (p = 0; p < 10; p++) {
      for (j = 0; j < p1[p]; j++) {
        setRGB(p2[p]*Max, p3[p]*Max, p4[p]*Max);                // Set background
        for (j = 0; j < p5[p]; j++) {
          c = (p6[p] * random(Nch / p6[p])) + p7[p];            // Select a random channel
          if (!bV[c]) bV[c] = Max;
          else        bV[c] = 0;
          tlcWrite();
          if (wait(p8[p])) return;
        }
      }
    }
  }
}



//==============================================================---------------------------------------
int channelLight(int channel, int b, bool clearIt) {            // Light a specific channel
  int n;
  n = channel;
  if (n < 0)    n = Nch - 1;
  if (n >= Nch) n = 0;
  if (clearIt) clearBv();
  bV[n] = b;
  tlcWrite();
  return (n);
}



//==============================================================---------------------------------------
void clearBv() {                                                // Clear brightness buffer
  for (int i = 0;  i < Nch; i++) bV[i] = 0;
  return;
}



//==============================================================---------------------------------------
void colorBlinkRotate(int count) {                              // Rotate blinking through colors
  int c, ch, n;
  clearBv();
  while (true) {
    for (c = 0; c < 3; c++) {
      setColor(c, Max);
      for (n = 0; n < count; n++) {
        for (ch = ledType[c]; ch < Nch; ch += 3) {
          bV[ch] = random(0, Max + 1);
        }
        tlcWrite();
        if (wait(500)) return;
      }
      setColor(c, 0);
    }
  }
}



//==============================================================---------------------------------------
int colorLight(int color, int b, bool clearIt) {                // Light a specific color
  int  c;
  c = color;
  if (c <  0) c = 2;
  if (c >= 3) c = 0;
  if (clearIt) clearBv();
  setColor(c, b);
  return (c);
}



//==============================================================---------------------------------------
void colorSlowChange() {                                        // Changes colors/intensities slowly
  int c, b;
  while (true) {
    for (c = 1; c < 8; c++) {
      for (b = Inc; b <= Max; b += Inc) {
        rgbLight(c, b, true);
        if (wait(300)) return;
      }
    }
  }
}



//==============================================================---------------------------------------
void colorWheel(void) {                                         // Vary RGB brightnesses in sequence
  int r, g, b;
  while (true) {
    for (r = Max, g = 0, b = 0;    g <= Max; g += Inc) {
      setRGB(r, g, b);
      if (wait(100)) return;
    }
    for (r = Max, g = Max, b = 0;    r >= 0; r -= Inc) {
      setRGB(r, g, b);
      if (wait(100)) return;
    }
    for (r = 0, g = Max, b = 0;    b <= Max; b += Inc) {
      setRGB(r, g, b);
      if (wait(100)) return;
    }
    for (  r = 0, g = Max, b = Max;  g >= 0; g -= Inc) {
      setRGB(r, g, b);
      if (wait(100)) return;
    }
    for (r = 0, g = Max, b = Max;  r <= Max; r += Inc) {
      setRGB(r, g, b);
      if (wait(100)) return;
    }
    for (r = Max, g = 0, b = Max;    b >= 0; b -= Inc) {
      setRGB(r, g, b);
      if (wait(100)) return;
    }
  }
}



//==============================================================---------------------------------------
void cycleBinary(void) {                                        // Binary Cycle brightness
  int c;
  int b;
  int n = 1;
  while (true) {
    for (b = 0; b <= Max; b += n * Inc) {                       // steps of n*Inc
      for (c = 0; c < Nch; c++) bV[c] = b;                      // set every channel brightness
      tlcWrite();                                               // Update TLCs
      if (wait(200)) return;
      n *= 2;                                                   // double step factor
    }
    for (b = Max; b >= 0; b -= n * Inc) {                       // steps of n*Inc
      for (c = 0; c < Nch; c++) bV[c] = b;                      // set every channel brightness
      tlcWrite();                                               // Update TLCs
      if (wait(200)) return;
      if (n == 1) break;
      n /= 2;                                                   // halve step factor
    }
  }
}



//==============================================================---------------------------------------
void cycleLinear(void) {                                        // Linear Cycle brightness
  int c;
  int b;
  while (true) {
    for (b = 0; b <= Max; b += Inc) {                           // go thru 60 Inc steps
      for (c = 0; c < Nch; c++) bV[c] = b;                      // set every channel brightness
      tlcWrite();                                               // Update TLCs
      if (wait(100)) return;
    }
    for (b = Max; b >= 0; b -= Inc) {                           // go thru 60 Inc steps
      for (c = 0; c < Nch; c++) bV[c] = b;                      // set every channel brightness
      tlcWrite();                                               // Update TLCs
      if (wait(100)) return;
    }
  }
}



//==============================================================---------------------------------------
void fluctuate() {                                              //  Fluctuate color intensities
  boolean up[3]  = { true,  false,  true };
  int     stp[3] = { Inc / 2, Inc / 3, Inc / 4 };
  int     mx[3]  = { Max,     Max,   Max };
  int     b[3];
  int     i;
  for (i = 0; i < 3; i++) b[i] = random(mx[i] + 1);
  while (true) {
    setRGB(b[0], b[1], b[2]);
    if (wait(50)) return;
    for (i = 0; i < 3; i++) {
      if (up[i]) {
        b[i] += stp[i];
        if (b[i] > mx[i]) {
          up[i] = false;
          b[i] -= stp[i];
        }
      } else {
        b[i] -= stp[i];
        if (b[i] < 0) {
          up[i] = true;
          b[i] += stp[i];
        }
      }
    }
  }
}



//==============================================================---------------------------------------
int irGetKey(void) {                                            // Get and interpret IR key
  /*
       Returned values:  0-9 = Number keys (0-9)     10 = UP    (U)
                          11 = DOWN  (D)             12 = RIGHT (R)
                          13 = LEFT  (L)             14 = OK    (K)
                          -1 = No key
                          -2 = Key too soon (gauged by IRWAIT)
                          -3 = Key is not defined
  */
  int           i;
  unsigned long t;
  static int    lastKey = 0;
  int           result;
  if (!irrecv.decode(&results)) return (-1);                    // -1 for no key pressed
  t = millis();                                                 // Note time key arrived
  for (i = 0; i < Ncodes; i++) {                                // check if defined code
    if (results.value == ir[i].code) break;                     // stop looking when matched
  }
  if (i == Ncodes) result = -3;                                 // -3 for Code undefined
  else              result = ir[i].key;                         // Code is defined
  if ( (lastIRtime > t) ||                                      // Unless forced ignore
       ( (t - lastIRtime) < IRWAIT)                             // or too soon since last key
     )              result = -2;                                // ignore this key
  if ( (results.value == 0xffffffff) &&                         // if repeating key
       ( (lastKey == UP) || (lastKey == DOWN) ) ) {             // and UP or DOWN key
    result = lastKey;                                           // reply with last key sent
  }
  irrecv.resume();                                              // Restart looking for IR inputs
  lastIRtime = t;                                               // remember when hit
  if (result >= 0 ) {                                           // if a defined key pressed
    lastKey = result;                                           // save last key reported
    beep(600, 50);                                              // make tone
  }
  return (result);                                              // return result
}



//==============================================================---------------------------------------
int keyboardCheck(void) {                                       // Check if keyboard input present
  int len;
  memFree = freeMemory();
  if (memFree < minFree) minFree = memFree;
  if (memFree < 100) Serial.print(F("\nStorage low!"));
  if (Serial.available()) {                                     // Check serial interface
    len = Serial.readBytes(inBuffer, Nbuff);                    // Get input into inBuffer
    inBuffer[len] = 0;                                          // terminate the buffer at \n
    return (len);                                               // return characters in inBuffer
  }
  return (0);                                                   // return 0 if no input
}



//==============================================================---------------------------------------
int keyboardNumber(void) {                                      // Get a number from the user
  int len;
  int result;
  len = keyboardWait();                                         // Wait for keyboard input
  inBuffer[len] = 0;                                            // Terminate string at \n
  sscanf(inBuffer, "%d", &result);                              // decode characters as number
  return (result);
}



//==============================================================---------------------------------------
int keyboardWait(void) {                                        // Wait for user keyboard input
  int len;
  while (true) {
    len = keyboardCheck();                                      // Check for keys
    if (len) break;                                             // if some, break out
  }
  return (len);                                                 // return characters in inBuffer
}



//==============================================================---------------------------------------
void ledEnable(int leds) {                                      // Enable LEDs based on encoded value
  if (leds & 0x1) ledOff[2] = false;                            // RED
  else            ledOff[2] = true;
  if (leds & 0x2) ledOff[1] = false;                            // GREEN
  else            ledOff[1] = true;
  if (leds & 0x4) ledOff[0] = false;                            // BLUE
  else            ledOff[0] = true;
}



//==============================================================---------------------------------------
int levelLight(int level) {                                     // Light at specific level
  int n;
  n = level;
  if (n < 0)       n = Nlevel - 1;
  if (n >= Nlevel) n = 0;
  setMax(bVlevel[n]);
  setRGB(Max, Max, Max);
  return (n);
}



//==============================================================---------------------------------------
int limbLight(int limb, int b, bool clearIt) {                  // Light a specific limb
  int i;
  int n;
  n = limb;
  if (n < 0)       n = Nlimbs - 1;
  if (n >= Nlimbs) n = 0;
  if (clearIt) clearBv();
  for (i = 0; i < 3; i++) bV[(n * 3) + i] = b;
  tlcWrite();
  return (n);
}



//==============================================================---------------------------------------
int nodeLight(int node, int b, bool clearIt) {                  // Light a specific node
  int i;
  int n;
  n = node;
  if (n < 0)      n = Nnode - 1;
  if (n >= Nnode) n = 0;
  if (clearIt) clearBv();
  for (i = nodeCh[n]; i < nodeCh[n + 1]; i++) bV[i] = b;
  tlcWrite();
  return (n);
}



//==============================================================---------------------------------------
bool nodeRotate(int cnt, int dir, int tm) {                     // Node rotate
  int i, n, node;
  for (i = 0; i < cnt; i++) {
    if (dir < 0) node = Nnode-1;
    else         node = 0; 
    for (n = 0; n<Nnode; n++) {
      nodeLight(node, Max, true);
      node += dir;
      if (wait(tm)) return (true);
    }
  }
  return (false);
}



//==============================================================---------------------------------------
void nodeRotateProgram() {                                     // Node rotate program
  int i;
  int dir = 1;
  for (i = 1; i < 8; i++) {
    ledEnable(i);
    if (nodeRotate(2, dir, 100-(i*10))) {
      ledEnable(_RED_GRN_BLU);
      setRGB(0,0,0);
      return;
    }
    if (dir == 1) dir = -1;
    else          dir =  1;
  }
  setRGB(0,0,0);
}



//==============================================================---------------------------------------
void pickIt(int kind, int inc, int t, int b, bool clearIt) {    // select or rotate through object
  /*
      Pick an "object" and display it (manually or automatically)
      if t (milliseconds) is > 0, selection rotates through each instance
      inc can be <0 or >0 and dictates the direction and amount of rotation


                       kind  Object      Function used
                       ====  ==========  =============
                        1    channel     channelLight
                        2    node        nodeLight
                        3    slice       sliceLight
                        4    limb        limbLight
                        5    brightness  levelLight
                        6    TLC         tlcLight
                        7    Color       colorLight
                        8    RGB combo   rgbLight

  */

  int  n = -1;
  int  key;
  bool picked = false;

  while (true) {
    if (t == 0) {                                               // IR key selection only
      picked = false;
      // Look for keyboard input
      key = irGetKey();
      if (key == OK) {
        timeOut = false;
        masterStop = false;
        return;
      }
      if ((key ==   UP) || (key == RIGHT)) {
        n++;
        picked = true;
      }
      if ((key == DOWN) || (key ==  LEFT)) {
        n--;
        picked = true;
      }
    } else {      // automatic increment +/-
      n += inc;
      picked = true;
    }

    if (picked) {
      switch (kind) {
        case 1:
          n = channelLight(n, b, clearIt);
          break;
        case 2:
          n = nodeLight(n, b, clearIt);
          break;
        case 3:
          n = sliceLight(n, b, clearIt);
          break;
        case 4:
          n = limbLight(n, b, clearIt);
          break;
        case 5:
          n = levelLight(n);
          break;
        case 6:
          n = tlcLight(n, b, clearIt);
          break;
        case 7:
          n = colorLight(n, b, clearIt);
          break;
        case 8:
          n = rgbLight(n, b, clearIt);
          break;
        default:
          break;
      }
    }
    if (wait(t)) return;                                        // wait works with dPeriod 0 or >0
  }
}



//==============================================================---------------------------------------
void rampBrightness() {                                         // Ramp brightness levels by color
  int c;
  int b;
  int cnt;
  cnt = Inc / 3;
  if (cnt < 1) cnt = 1;
  setRGB(0, 0, 0);                                              // Turn them all off
  while (true) {
    for (c = 0; c < 3; c++) {                                   // for each color
      for (b = 0; b < Max; b += cnt) {                          // increment brightness
        setColor(c, b);
        if (wait(50)) return;
      }
      setColor(c, 0);                                           // turn it off entirely
    }
  }
}



//==============================================================---------------------------------------
void rampBrightnessUpDown() {                                   // bring brightness up and down
  int b, c;
  while (true) {
    for (c = 0; c < 3; c++) {
      clearBv();
      for (b = 0; b <= Max; b += Inc) {
        setColor(c, b);
        if (wait(50)) return;
      }
      for (b = Max; b >= 0; b -= Inc) {
        setColor(c, b);
        if (wait(50)) return;
      }
    }
  }
}



//==============================================================---------------------------------------
void randomBrightness(int period) {                             // Set LEDs to random brightness
  int c;
  while (true) {                                                // until user stops it
    for (c = 0; c < Nch; c++) bV[c] = random(Max);
    tlcWrite();
    if (wait(period)) return;
  }
}



//==============================================================---------------------------------------
void rgbCycle1(void) {                                          // Cycle brightness on RGB channels
  int r, g, b;
  while (true) {
    for (r = 0; r < 100; r+=10) {                               // For each RED LED brightness
      for (g = 0; g < 100; g+=10) {                             // For each GRN LED brightness
        for (b = 0; b < 100; b+=10) {                           // For each BLU LED brightness
          setRGB((Max*r)/100, (Max*g)/100, (Max*b)/100);        // set brightness for r,g and b
          if (wait(50)) return;
        }
      }
    }
  }
}



//==============================================================---------------------------------------
void rgbCycle2() {                                              // Rotate thru R,G,B brightness
  int b = 50;                                                   // Initial brightness
  int c;                                                        // color
  int period = 150;
  int upper;
  int cnt = 0;
  setRGB(0, 0, 0);
  upper = Max - 11;
  if (upper < 1) upper = Max;
  while (true) {
    for (c = 0; c < 3; c++) {
      setColor(c, b);                                           // turn on the color
      if (wait(period)) return;
      clearBv();
    }
    cnt++;
    if (cnt > 3) {
      cnt = 0;
      b = 10 + random(upper);
      period = 50 + random(100);
    }
  }
}



//==============================================================---------------------------------------
int rgbLight(int code, int b, bool clearIt) {                   // Light a specific color combo
  int  c;
  c = code;
  if (c <  1) c = 7;
  if (c >= 8) c = 1;
  if (clearIt) clearBv();
  setColor(RED,   (c & 0x01) ? b : 0);
  setColor(GREEN, (c & 0x02) ? b : 0);
  setColor(BLUE,  (c & 0x04) ? b : 0);
  tlcWrite();
  return (c);
}



//==============================================================---------------------------------------
int serial_putchar(char c, FILE * f) {                          // needed for printf
  if (c == '\n') serial_putchar('\r', f);
  return Serial.write(c) == 1 ? 0 : 1;
}



//==============================================================---------------------------------------
void setColor(int c, int b) {                                   // Adjust brightness of one color
  int i;
  for (i = 0; i < Nch; i++) if (ledType[i % 3] == c) bV[i] = b;
  tlcWrite();
}



//==============================================================---------------------------------------
void setMax(int b) {                                            // Set LED max limit and Increment
  Max = b;
  if (Max > 4095) Max = 4095;
  if (Max < 2)    Max = 2;
  Inc = Max / 40;
  if (Inc < 1) Inc = 1;
  return;
}


//==============================================================---------------------------------------
void setRGB(int r, int g, int b) {                              // Adjust brightness by color
  int c;                                                        // Channel number
  for (c = 0; c < Nch; c++) {                                   // For each channel,
    switch (ledType[c % 3]) {                                   // based on color assigned to port
      case RED:
        if (r >= 0) bV[c] = r;
        break;
      case GREEN:
        if (g >= 0) bV[c] = g;
        break;
      case BLUE:
        if (b >= 0) bV[c] = b;
        break;
    }
  }
  tlcWrite();
}



//==============================================================---------------------------------------
int sliceLight(int slice, int b, bool clearIt) {                // Light a specific slice
  int n;
  int base;
  int node;
  int to;
  int from;
  int first6[Nslice] = { 0, 3,  6,  9, 12, 15 }; // for nodes 0,1,4,5,6,7,8 and 9
  int first7[Nslice] = { 0, 3,  9, 12, 15, 18 }; // for 10
  int first8[Nslice] = { 0, 3,  9, 12, 15, 21 }; // for nodes 2,3

  n = slice;
  if (n <  0)      n = Nslice - 1;
  if (n >= Nslice) n = 0;
  if (clearIt) clearBv();
  for (node = 0; node < Nnode; node++) {
    base = nodeCh[node];
    switch (node) {
      case 2:
      case 3:
        // 8 limb node
        from = base + first8[n];
        if ( (n == 1) || (n == 4) )  to = from + 6;
        else                         to = from + 3;
        break;
      case 10:
        // 7 limb node
        from = base + first7[n];
        if (n == 1)  to = from + 6;
        else         to = from + 3;
        break;
      default:
        // 6 limb node
        from = base + first6[n];
        to   = from + 3;
        break;
    }
    while (from < to) bV[from++] = b;
  }
  tlcWrite();
  return (n);
}



//==============================================================---------------------------------------
bool sliceRotate(int cnt, int dir, int tm) {                    // Slice rotate
  int i, s, slice;
  for (i = 0; i < cnt; i++) {
    if (dir < 0) slice = Nslice-1;
    else         slice = 0; 
    for (s = 0; s <Nslice; s++) {
      sliceLight(slice, Max, true);
      slice += dir;
      if (wait(tm)) return (true);
    }
  }
  return (false);
}



//==============================================================---------------------------------------
void sliceRotateProgram() {                                     // Slice rotate program
  int i;
  int dir = 1;
  for (i = 1; i < 8; i++) {
    ledEnable(i);
    if (sliceRotate(3, dir, 200-(i*10))) {
      ledEnable(_RED_GRN_BLU);
      setRGB(0,0,0);
      return;
    }
    if (dir == 1) dir = -1;
    else          dir =  1;
  }
  setRGB(0,0,0);
}



//==============================================================---------------------------------------
void snake() {                                                  // snake all lights bottom to top
  int nr = 21;                                                  // number of channels in snake
  int t = 10;                                                   // delay between moves
  int i;                                                        // iteration counter
  int head;                                                     // head of snake
  int tail;                                                     // tail of snake
  while (true) {
    // snake from bottom to top
    setRGB(0, 0, 0);
    tail = 0;
    for (head = tail; head < nr; head++) {
      bV[head] = Max;
      tlcWrite();
      if (wait(t)) return;
    }
    for (i = 0; i < 3 * Nch; i++) {
      head++;
      if (head >= Nch) head = 0;
      bV[head] = Max;
      bV[tail] = 0;
      tail++;
      if (tail >= Nch) tail = 0;
      tlcWrite();
      if (wait(t)) return;
    }
    // snake from top to bottom
    tail = Nch - 1;
    setRGB(0, 0, 0);
    for (head = tail; tail - head < nr; head--) {
      bV[head] = Max;
      tlcWrite();
      if (wait(t)) return;
    }
    for (i = 0; i < 3 * Nch; i++) {
      head--;
      if (head < 0) head = Nch - 1;
      bV[head] = Max;
      bV[tail] = 0;
      tail--;
      if (tail < 0) tail = Nch - 1;
      tlcWrite();
      if (wait(t)) return;
    }
  }
}



//==============================================================---------------------------------------
void starRandom() {                                             // Twinkle the topper
  unsigned long t;
  t = millis();
  if ((t - starChanged) < starPeriod) return;
  starV[0] = (Max + random(Max))/2;
  starV[1] = (Max + random(Max))/2;
  starV[2] = (Max + random(Max))/2;
  starChanged = t;
  return;
}



//==============================================================---------------------------------------
void starSet(int b) {                                           // Set topper brightnesses
  starOn = true;
  starV[0] = starV[1] = starV[2] = b;
  starChanged = millis();
  return;
}



//==============================================================---------------------------------------
boolean userStop(void) {                                        // Check for KB key or IR OK key
  unsigned long t;
  if ( (keyboardCheck()) || (irGetKey() == OK)  ) {             // Keyboard input or IR OK key
    timeOut = false;                                            // was direct user action
    masterStop = false;                                         // turn off timeout stop control
    return (true);
  }
  if (masterStop) {                                             // Permit limited executions
    t = millis();                                               // Get current time
    if (t >= stopTime) {                                        // if past stop time
      timeOut = true;                                           // note that it was due to timeout
      return (true);
    }
  }
  return (false);                                               // otherwise false
}



//==============================================================---------------------------------------
bool wait(int period) {                                         // Delay with timeout/interrupt control
  int cnt, i, t;
  if (period > 500) {
    t = period / 10;
    cnt = 10;
  } else {
    t = period;
    cnt = 1;
  }
  for (i = 0; i < cnt; i++) {
    if (t > 0) {
      if (starTwinkle) starRandom();
      delay(t);
    }
    if (userStop()) return (true);
  }
  return (false);
}



//==============================================================---------------------------------------
bool wait2(void) {                                              // Pause & check between acts
  if (!timeOut) return (true);
  setRGB(0, 0, 0);
  delay(2000);
  return (false);
}



