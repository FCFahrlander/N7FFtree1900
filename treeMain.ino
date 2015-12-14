
/*

    File:  treeMain.ino (Version: 19.00)
    Title: Main functions for N7FF Christmas Tree Lights
    Created by Frank Fahrlander on December 14, 2015.
    Copyright © 2015 Frank Fahrlander. All rights reserved.

======================================== Function list ================================================

    doCommand                 Process user command
    doProgram                 Do pattern programs
    loop                      Main program loop
    setup                     Initial setup


*/



//==============================================================---------------------------------------
void doCommand(int nr) {                                        // Process user command
  int c;
  int cnt;
  int saveMax;
  if ( (nr > 0) && (nr <= Ncmds) ) {
    strcpy_P(inBuffer, (char*)pgm_read_word(&(Cmd[nr - 1])));
    printf("\nCommand: %2d %s", nr, &inBuffer[3]);
  }
  switch (nr) {
    case -1:                                                    // Do nothing command
      break;
    case 0:                                                     // List the commands
      setRGB(0, 0, 0);
      Serial.print(F("\n\n"));
      cnt = 2;
      for (c = 0; c < Ncmds; c++)
      {
        if (c == Nir) {
          Serial.print(F("\nKeyboard only commands:"));
          cnt = 2;
        }
        strcpy_P(inBuffer, (char*)pgm_read_word(&(Cmd[c])));
        if (cnt < 2) {
          cnt++;
          printf("   %2d %s", c + 1, &inBuffer[3]);
        }
        else {
          cnt = 0;
          printf("\n%2d %s", c + 1, &inBuffer[3]);
        }
      }
      break;
    case 1:                                                     // Adjust brightness w/ arrows
      pickIt(5, 0, 0, Max, true);
      break;
    case 2:                                                     // Toggle RGB combos
      pickIt(8, 0, 0, Max, true);
      break;
    case 3:                                                     // Pick channels using arrows
      starOn = false;
      pickIt(1, 0, 0, Max, true);
      starOn = true;
      break;
    case 4:                                                     // Pick slice using arrows
      pickIt(3, 0, 0, Max, true);
      break;
    case 5:                                                     // Pick limb with arrows
      pickIt(4, 0, 0, Max, true);
      break;
    case 6:                                                     // Pick TLC using arrows
      starOn = false;
      pickIt(6, 0, 0, Max, true);
      starOn = true;
      break;
    case 7:                                                     // Do program selection
      doProgram(0, false);
      break;
    case 8:                                                     // Do full program brightly
      saveMax = Max;
      setMax(4095);
      doProgram(0, true);
      setMax(saveMax);
      break;
    case 9:                                                     // Control and test
      Control();
      break;

    // Keyboard only commands follow

    case 34:                                                    // Dump brightness values
      dumpBv();
      break;
    default:                                                    // commands
      doProgram(nr, false);
      break;
  }
  printf("\n\n(Max:0x%x (%d)   minFree:%d)   Enter command.", Max, Max, minFree);
  beep(1000, 100);
}



//==============================================================---------------------------------------
void doProgram(int specific, bool allCmds) {                    // Do programmed patterns
  int p;
  unsigned long t;
  int cycle;
  int nextCmd = 11;

  if (allCmds) cycle = 10;                                      // if all, specific must be zero
  else         cycle = Fcmd;

  while (true) {                                                // while loop permits running show
    if (specific == 0) p = cycle;
    else               p = specific;
    strcpy_P(inBuffer, (char*)pgm_read_word(&(Cmd[p - 1])));
    
    if (specific == 0) {
      if (allCmds) {
        nextCmd = p+1;
        if (nextCmd > Nir) nextCmd = 10;
        t = 20;
      } else {
        nextCmd = 10 * (inBuffer[0] - '0') + (inBuffer[1] - '0'); // 1st & 2nd chars = next in cycle
        if ( (nextCmd > Nir) || (nextCmd < 0)) nextCmd = Fcmd;
        if (inBuffer[2] == ' ') t = 90;                           // 3rd char = duration (' '=60)
        else t = 10 * (inBuffer[2] - '0');
        if ( (t < 1) || (t > 120) ) t = 90;
      }
      stopTime = millis() + t * 1000;
      masterStop = true;                                          // masterStop terminate after time
      Serial.print(F("\n "));
      Serial.print(&inBuffer[3]);
    }

    switch (p) {
      case 10:                                                  // Pick random intensities
        randomBrightness(100);
        break;
      case 11:                                                  // Color Wheel
        colorWheel();
        break;
      case 12:                                                  // Cycle Linear
        cycleLinear();
        break;
      case 13:                                                  // Cycle Binary
        cycleBinary();
        break;
      case 14:                                                  // Cycle RGB brightness - 10 steps
        rgbCycle1();
        break;
      case 15:                                                  // Ramp up brightness by color
        rampBrightness();
        break;
      case 16:                                                  // Cycle color slowly up & down
        rampBrightnessUpDown();
        break;
      case 17:                                                  // Blink show
        blinkShow();
        break;
      case 18:                                                  // Blink channels at random
        blinkRandomly(200);
        break;
      case 19:                                                  // Rotate blink through colors
        colorBlinkRotate(12);
        break;
      case 20:                                                  // Fluctuate colors
        fluctuate();
        break;
      case 21:                                                  // Cycle RGB w/ varying brightness
        rgbCycle2();
        break;
      case 22:                                                  // Snake(s) on the tree
        snake();
        break;
      case 23:                                                  // Slow color change
        colorSlowChange();
        break;
      case 24:                                                  // Rotate slice program
        sliceRotateProgram();
        break;
      case 25:                                                  // Rotate nodes program
        nodeRotateProgram();
        break;
      case 26:                                                  // Rotate channels 0-Nch
        pickIt(1,  1,  50, Max, true);
        break;
      case 27:                                                  // Rotate channels Nch-0
        pickIt(1, -1,    1, Max, true);
        break;
      case 28:                                                  // Rotate limbs bottom to top
        pickIt(4,  1,  100, Max, true);
        break;
      case 29:                                                  // Rotate limbs top to bottom
        pickIt(4, -1,   30, Max, true);
        break;
      case 30:                                                  // Rotate thru levels
        pickIt(5,  1, 2000, Max, true);
        break;
      case 31:                                                  // Rotate TLCs 0-Ntlc
        pickIt(6,  1,  100, Max, true);
        break;
      case 32:                                                  // Rotate TLCs Ntlc-0
        pickIt(6, -1,   30, Max, true);
        break;
      case 33:                                                  // Rotate colors 0-7
        pickIt(8,  1, 2000, Max, true);
        break;

      default:                                                  // all others
        break;
    }
    if (specific != 0) return;
    if (wait2()) return;                                        // lights off for 2 seconds
    cycle = nextCmd;
  }
}



//==============================================================---------------------------------------
void loop() {                                                   // Main program loop
  int c;
  int len;
  len = keyboardCheck();
  if (len) {                                                    // If there was keyboard input
    irFirstRcvd = false;                                        // ignore any IR digit received
    if (len == 1) {                                             // If Enter key
      doCommand(-1);                                            // do an illegal command
    } else {                                                    // more than just Enter key
      sscanf(inBuffer, "%d", &lastCmd);                         // decode input as a number
      if (lastCmd <= Ncmds) doCommand(lastCmd);                 // do the command
      else lastCmd = -1;
    }
  }
  c = irGetKey();                                               // get any IT key pressed
  if (c >= 0) {                                                 // defined key was pressed
    if (c < 10) {                                               // If it was a numeric key
      if (irFirstRcvd) {                                        // If first digit received
        irFirstRcvd = false;                                    // start over next time
        lastCmd = (lastCmd * 10) + c;                           // Calculate command number
        if (lastCmd > Nir) lastCmd = -1;                        // Keyboard commands ignored
        doCommand(lastCmd);                                     // Do the command
      } else {                                                  // this is the first digit
        lastCmd = c;                                            // start lastCmd
        irFirstRcvd = true;                                     // Note looking for second digit
      }
    }
    else {                                                      // Key is a special key
      irFirstRcvd = false;                                      // cancel command number creation
    }                                                           // end of special IR key processing
  }                                                             // end of IR key input processing
}



//==============================================================---------------------------------------
void setup() {                                                  // Initial setup
  // Set up stdout for printf operation
  fdev_setup_stream(&serial_stdout,
                    serial_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &serial_stdout;
  Serial.begin(115200);                                         // Serial bit rate
  Serial.setTimeout(20);                                        // makes keyboard input work
  pinMode(tonePin, OUTPUT);                                     // Set up for speaker beeps
  pinMode(pClock,  OUTPUT);                                     // clock port to TLCs
  pinMode(pData,   OUTPUT);                                     // data port to TLCs
  pinMode(pBlank,  OUTPUT);                                     // blank port to TLCs
  pinMode(pLatch,  OUTPUT);                                     // latch port to TLCs

  digitalWrite(pBlank, HIGH);                                   // Disable display of all LEDs
  digitalWrite(pLatch, LOW);                                    // Start with latch LOW
  irrecv.enableIRIn();                                          // Start the IR receiver
  setRGB(0, 0, 0);                                              // Turn all LEDs off
  digitalWrite(pBlank, LOW);                                    // Enable display of all LEDs
  delay(2000);                                                  // Give time to get monitor up
  doProgram(33, false);                                         // Start off with program
}



