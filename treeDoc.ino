
/*
    File:  treeDoc.ino (Version: 19.00)
    Title: Documentation file for N7FF Christmas Tree Lights
    Created by Frank Fahrlander on December 14, 2015.
    Copyright © 2015 Frank Fahrlander. All rights reserved.
 
============================================== History ================================================

  I purchased a GE pre-lit Christmas tree in 2007. The lighting was composed of custom strings of
  clear Constant-on incandescent bulbs. Each string included 1, 2 or 3 groups of 49 constant-on bulbs
  and one fuse bulb in series. The strings making up the tree were configured as shown below:

    1  string  of 100 bulbs wired as 10 substrings of 10 bulbs each (used on the top of tree)
    1  string  of  50 bulbs wired as  1 substring  of 50 bulbs
    3  strings of 150 bulbs wired as  8 substrings composed of: 2 of 25, 4 of 17 and 2 of 16
    5  strings of 100 bulbs wired as  6 substrings composed of: 4 of 17 and 2 of 16
    1  string  of 100 bulbs wired as  4 substrings of 25 bulbs each

  There was a total of 1200 bulbs on the tree. When a Constant-on bulb burned out, it shorted itself
  out so that the other bulbs in the substring would stay lit. The remaining bulbs just burn a little
  brighter. This is certainly preferred to having a substring of 50 bulbs go black. However, as
  more bulbs burn out, the stress on the other bulbs increases which reduces their life. In a tree
  with 1200 bulbs you tend to miss burnt out bulbs. These factors plus the bulbs just getting older
  contributed to 'avalanche" type failures in recent years. I've actually watched a substring all of
  a sudden get brighter and brighter and in a few seconds all of the bulbs in the substring burn have
  burnt out including the fuse bulb. It happens too quick to stop. Yo just have to keep up with the
  maintenance on these type trees.

  Over the last 7 years, I have replaced about 500 bulbs. In the last 2 years it become common now to
  replace 50 to 150 bulbs each season. If the tree weren't so beautiful I would have given up on it.
  Now, however, the thought of an even more beautiful tree with less maintenance and an opportunity
  to employ an Arduino Micro to control the LEDs was all the motivation I needed for this project.

============================================= New Design ===============================================

  The 1416 LEDs in the new tree design are driven using an Arduino Pro Mini and 9 Adafruit TLC5947
  LED driver modules connected in series. The TLCs are driven using the SPI interface. Each of the TLCs
  has 24 channels of programmable 12 bit PWM output to drive LEDs. The output current on each channel
  is limited to a maximum of 15 milliamperes. The number of LEDs the TLC channel can drive is
  a function of the supply voltage (which can be as large as 30 volts for the TLC) and the forward
  voltages of the LEDs used. The LEDs I used have the following forward voltages:

                               LED      Forward   Max size  Min voltage
                               Color    Voltage   String     Required
                              =======   ========  =======   ===========
                               Red        1.6       14         29
                               Green      2.1       13         27 
                               Blue       2.4        9         28  
      
  The tree has 11 nodes along the trunk of the tree. Node 0 is at the bottom and node 10 is the
  top section of the tree. Each node is where there are 6 to 8 limbs attached to the trunk.

  Each limb is lit using 3 TLC channels. One channel drives a string of red LEDs, one drives a
  string of green LEDs and the third drives a string of blue LEDs.
  
  The topper can be driven by 3 TLC channels. Currently, only one channel is used to drive 9
  white LEDs in series. Three are in the center and 6 surround the center..

  The organization of the sections and limbs and LEDs is shown in the following table: 

     Node         limbs   Cum     #    #      #      Cum             Cum    Cum      TLC
    Height  Node   per     #     ch   LEDs  LEDs      #     # TLC   # TLC   # of    channel
   (inches)   #    node  limbs  /limb /ch   /node    LEDs    ch's    ch's   TLCs   addresses
   ======== ====  =====  =====  ===== ====  =====   =====   =====   =====   =====  =========
    71.00   star    1      72    1     9       9     1416      3     216     9.00   213-215
    62.50    10     7      71    3     5     105     1407     21     213     8.87   192-212
    62.25     9     6      64    3     4      72     1302     18     192     8.00   174-191
    56.75     8     6      58    3     5      90     1230     18     174     7.25   156-173
    51.00     7     6      52    3     6     108     1140     18     156     6.50   138-155
    45.00     6     6      46    3     6     108     1032     18     138     7.75   120-137
    39.50     5     6      40    3     7     126      924     18     120     5.00   102-119
    34.00     4     6      34    3     7     126      798     18     102     4.25    84-101
    28.50     3     8      28    3     8     192      672     24      84     3.50    60-83
    23.00     2     8      20    3     8     192      480     24      60     2.50    36-59
    18.50     1     6      12    3     8     144      288     18      36     1.50    18-35
    16.00     0     6       6    3     8     144      144     18      18     0.75     0-17
    Bottom                  0                           0              0     

  The cabling to the TLCs provides the serial data, the clock and 2 other control signals as
  well as the 30 volts used to drive the modules and the LEDs. The data is clocked into all
  TLCs by bit shifting 24 sets of 12 bit PWM words data per TLC. The data for all the TLCs is
  held and minipulated in the bV table and then written to the TLCs using the
  tlcWrite function.
  
=========================== TLC/Channel assignments and construction history ===========================

    TLC#   Chan Adr   Limb Plugs    
    ----   --------   ----------
   TLC 8   204  201  | 10E  10D |    December  3, 2015: Completed topper (Project end for 2015)
   64-71   207  198  | 10F  10C |
   100%    210  195  | 10G  10B |    November 22, 2015: 71 limbs completed (node 10 completed)
   (top)   213  192  | 11A  10A |
                      ----------
                        ||||||
                      ----------
   TLC 7   180  177  | 09C  09B |    November 15, 2015: 64 limbs completed (node 9 completed)
   56-63   183  174  | 09D  09A |
    89%    186  171  | 09E  08F |    November 14, 2015: 58 limbs completed (node 8 completed)
   59.0"   189  168  | 09F  08E |
                      ----------
                        ||||||
                      ----------
   TLC 6   156  153  | 08A  07F |    November 13, 2015: 52 limbs completed (node 7 completed)
   48-55   159  150  | 08B  07E |
    78%    162  147  | 08C  07D |    November 11, 2015: 50 limbs completed (node 6 completed)
   53.5"   165  144  | 08D  07C |
                      ----------
                        ||||||
                      ----------
   TLC 5   132  129  | 06E  06D |
   40-47   135  126  | 06F  06C |
    67%    138  123  | 07A  06B |
   47.5"   141  120  | 07B  06A |    November  9, 2015: 42 limbs completed
                      ----------
                        ||||||
                     ----------
   TLC 4   108  105  | 05C  05B |    November  8, 2015: 40 limbs completed (node 5 completed)
   32-39   111  102  | 05D  05A |
    56%    114  099  | 05E  04F |    November  6, 2015: 34 limbs completed (node 4 completed)
   36.3"   117  096  | 05F  04E |     
                      ----------
                        ||||||
                      ----------
   TLC 3   084  081  | 04A  03H |    November  5, 2015: 31 Limbs completed
   24-31   087  078  | 04B  03G |
    44%    090  075  | 04C  03F |
   30.5"   093  072  | 04D  03E |    November  3, 2015: 28 limbs completed (node 3 completed)
                      ----------
                        ||||||
                      ----------
   TLC 2   060  057  | 03A  02H |
   16-23   063  054  | 03B  02G |    November  1, 2015: 24 limbs completed
    33%    066  051  | 03C  02F |     October 31, 2015: 21 limbs completed (node 2 completed)
   25.5"   069  048  | 03D  02E |     October 30, 2015: 18 limbs completed
                      ----------
                        ||||||
                      ----------
   TLC 1   036  033  | 02A  01F |     
   08-15   039  030  | 02B  01E |     October 28, 2015: 14 limbs completed (node 1 completed)
    22%    042  027  | 02C  01D |     October 25, 2015: 10 limbs completed (node 0 completed)
   20.5"   045  024  | 02D  01C |     October 21, 2015:  4 limbs completed
                      ----------
                        ||||||
                      ----------    
   TLC 0   012  009  | 00E  00D |     October 18, 2015:  1 limb completed
   00-07   015  006  | 00F  00C |     October 17, 2015: Built board and mounted in box
    11%    018  003  | 01A  00B |     October 16, 2015: TLCs mounted to tree trunk
   13.5"   021  000  | 01B  00A |     October 14, 2015: TLCs wired together for first time
                      ----------
                                       August 17, 2015: Inital tree analysis completed
                                         July 23, 2015: Started project
    
=============================================  Costs  ==================================================

       $131  LEDs
       $177  Electronics
       $200  Cable, solder, heat shrink tubing, crimping tool, connectors, pins
       -----   
       $508  Total out of pocket       
  
        200  Constructing light strings and attaching them to the tree, limb by limb
         70  Tree and wire prep
         90  Programming the ProMini and evolving the program (1900 lines of code)
         40  building electronics
       ----
        400  Total person hours
        
            
====================== Program versions and their size (Arduino Pro Mini) ==============================

       Ver    Date    Code space used     Data space used    Remaining
      =====  ======  ==================  =================  =========== 
       1.00  Jul 27   3,912 bytes (12%)    228 bytes (11%)  1,820 bytes (Initial TLC program)
       1.01  Jul 29  13,194 bytes (42%)  1,018 bytes (49%)  1,030 bytes (First working version)
       1.02  Jul 30  13,102 bytes (42%)  1,102 bytes (53%)    946 bytes (First use of IR rcvr)
       1.03  Aug  2  14,016 bytes (45%)  1,208 bytes (58%)    840 bytes
       1.04  Aug  3  16,240 bytes (52%)  1,389 bytes (67%)    659 bytes
       1.05  Aug  5  16,438 bytes (53%)    991 bytes (48%)  1,057 bytes
       1.06  Aug 10  18,100 bytes (58%)  1,121 bytes (54%)    927 bytes
       1.07  Aug 12  20,492 bytes (66%)  1,076 bytes (52%)    972 bytes
       1.08  Aug 12  21,722 bytes (70%)  1,168 bytes (57%)    880 bytes
       1.09  Aug 14  22,576 bytes (73%)  1,319 bytes (64%)    729 bytes
       2.00  Aug 16  22,576 bytes (73%)  1,319 bytes (64%)    729 bytes
       2.01  Aug 17  22,576 bytes (73%)  1,319 bytes (64%)    729 bytes
       3.00  Aug 21  24,556 bytes (79%)  1,896 bytes (92%)    152 bytes (Low Memory Warning)
       3.01  Aug 23  23,804 bytes (77%)  1,676 bytes (81%)    372 bytes (Low Memory Warning)
       4.00  Aug 23  23,878 bytes (77%)  1,252 bytes (61%)    796 bytes
       4.01  Aug 26  25,236 bytes (82%)  1,481 bytes (72%)    567 bytes
       5.00  Sep 06  22,704 bytes (73%)  1,881 bytes (91%)    167 bytes (Low Memory Warning) 
       6.00  Sep 10  21,120 bytes (68%)  1,878 bytes (91%)    170 bytes (Low Memory Warning) 
       7.00  Sep 14  21,070 bytes (68%)  1,082 bytes (52%)    966 bytes
       8.00  Oct 12  21,876 bytes (71%)  1,133 bytes (55%)    915 bytes
       9.00  Oct 18  21,960 bytes (71%)  1,148 bytes (56%)    900 bytes
       9.01  Oct 29  21,740 bytes (70%)  1,120 bytes (54%)    928 bytes
       9.02  Oct 30  22,970 bytes (74%)  1,208 bytes (58%)    840 bytes
      10.00  Nov  1  23,340 bytes (75%)  1,206 bytes (58%)    842 bytes
      10.01  Nov  3  23,610 bytes (76%)  1,232 bytes (60%)    816 bytes
      11.00  Nov  6  24,492 bytes (79%)  1,252 bytes (61%)    796 bytes
      12.00  Nov 14  24,708 bytes (80%)  1,247 bytes (60%)    801 bytes
      13.00  Nov 16  26,168 bytes (85%)  1,521 bytes (74%)    527 bytes (Bad tlcWrite)
      14.00  Nov 18  25,134 bytes (81%)  1,495 bytes (72%)    553 bytes (Bad tlcWrite)
      14.01  Nov 18  25,382 bytes (82%)  1,489 bytes (72%)    559 bytes (Bad tlcWrite) 
      14.02  Nov 19  25,412 bytes (82%)  1,495 bytes (72%)    553 bytes
      15.00  Nov 20  25,466 bytes (82%)  1,495 bytes (72%)    553 bytes
      15.01  Nov 21  25,724 bytes (83%)  1,495 bytes (72%)    553 bytes
      16.00  Nov 21  26,066 bytes (84%)  1,509 bytes (73%)    539 bytes (new/larger menu/pgm)
      16.01  Nov 23  25,862 bytes (84%)  1,512 bytes (73%)    536 bytes
      16.02  Nov 25  25,712 bytes (83%)  1,556 bytes (75%)    492 bytes
      17.00  Nov 26  25,766 bytes (83%)  1,468 bytes (71%)    580 bytes (new doProgram logic)
      17.01  Nov 27  26,674 bytes (86%)  1,480 bytes (72%)    568 bytes (added slow colorWheel)
      17.02  Nov 27  25,494 bytes (82%)  1,437 bytes (70%)    611 bytes (drop showDriver)
      17.03  Nov 28  26,226 bytes (85%)  1,496 bytes (73%)    552 bytes (added setParam, new test)
      17.04  Nov 29  25,842 bytes (84%)  1,500 bytes (73%)    548 bytes (1 colorwheel)
      17.05  Nov 30  25,722 bytes (83%)  1,486 bytes (72%)    562 bytes (new slice/node rotates)
      18.00  Dec  3  24,974 bytes (81%)  1,399 bytes (68%)    649 bytes (Control approach, star)
      18.01  Dec  7  24,974 bytes (81%)  1,399 bytes (68%)    649 bytes (documentation only)
      19.00  Dec 14  25,020 bytes (81%)  1,399 bytes (68%)    649 bytes (Test function, GitHub)
      
======================================= Example execution ==============================================

 Ramp by color          
 Cycle RGB brightness   
 Cycle Binary           
 Random blink - group   
 Cycle Linear           
 Blink show             
 Slowly 0->Max->0...    
 Cycle thru RGB levels  
 Rotate Blink by color  
 Random intensities     
 Color wheel            
 Fluctuate colors       
 Ramp by color          

  <hit OK on IR remote>
  
 1 Pick brightness            2 Pick color combination     3 Pick channel           
 4 Pick slice                 5 Pick limb                  6 Pick TLC               
 7 Program                    8 Program - bright           9 Control                
10 Random intensities        11 Color wheel               12 Cycle Linear           
13 Cycle Binary              14 Cycle thru RGB levels     15 Ramp by color          
16 Slowly 0->Max->0...       17 Blink show                18 Random blink - group   
19 Rotate Blink by color     20 Fluctuate colors          21 Cycle RGB brightness   
22 Snake LEDs                23 Color slow change         24 Rotate slice           
25 Rotate nodes              26 Rotate channels b-t       27 Rotate channels t-b    
28 Rotate limbs l-r          29 Rotate limbs r-l          30 Rotate levels          
31 Rotate TLCs b-t           32 Rotate TLCs t-b           33 Rotate R,G & B         
Keyboard only commands:
34 Dump bV table          

(Max:0xff (255)   minFree:413)   Enter command.
Command:  8 Program - bright       
 Random intensities     
 Color wheel            
 Cycle Linear           
 Cycle Binary           
 Cycle thru RGB levels  
 Ramp by color          
 Slowly 0->Max->0...    
 Blink show             
 Random blink - group   
 Rotate Blink by color  
 Fluctuate colors       
 Cycle RGB brightness   
 Snake LEDs             
 Color slow change      
 Rotate slice           
 Rotate nodes           
 Rotate channels b-t    
 Rotate channels t-b    
 Rotate limbs l-r       
 Rotate limbs r-l       
 Rotate levels          
 Rotate TLCs b-t        
 Rotate TLCs t-b        
 Rotate R,G & B         
 Random intensities     
 Color wheel            

   <Hit OK on IR remote>
   
(Max:0xff (255)   minFree:399)   Enter command.


 1 Pick brightness            2 Pick color combination     3 Pick channel           
 4 Pick slice                 5 Pick limb                  6 Pick TLC               
 7 Program                    8 Program - bright           9 Control                
10 Random intensities        11 Color wheel               12 Cycle Linear           
13 Cycle Binary              14 Cycle thru RGB levels     15 Ramp by color          
16 Slowly 0->Max->0...       17 Blink show                18 Random blink - group   
19 Rotate Blink by color     20 Fluctuate colors          21 Cycle RGB brightness   
22 Snake LEDs                23 Color slow change         24 Rotate slice           
25 Rotate nodes              26 Rotate channels b-t       27 Rotate channels t-b    
28 Rotate limbs l-r          29 Rotate limbs r-l          30 Rotate levels          
31 Rotate TLCs b-t           32 Rotate TLCs t-b           33 Rotate R,G & B         
Keyboard only commands:
34 Dump bV table          

(Max:0xff (255)   minFree:399)   Enter command.
Command:  9 Control                
 0=clear    1=Trace    2=Bandaid   3=Write    4=Sound
 5=StarON   6=Twinkle  7=DumpBv    8=MaxChg   9=Test24
UP=tlcChg  DN=BLUtgl  LF=GRNtgl   RT=REDtgl  OK=Quit

     <entered 4 on IR remote>
     
Sound ON


========================================== Future ideas ================================================

      1. Waterfall of limbs (slices lighting from top to bottom at different rates)
      2. 2 snakes that meet and cross or explode
      3. Control the tree from a web browser on WiFi connection
      4. Control the tree from an iPhone app
      5. Control colors from sound input


===================================== Problems Observed ================================================

   1. 2048 Problem
      ------------

   When the value 2048 (exactly 50% duty cycle) is used to light LEDs, sometimes LEDs do not light
   or light very dimly (perhaps brightness of 10-20). The situation is most easily demonstrated by
   lighting 2 neighboring TLCs with this brilliance. The higher numbered TLC (one further down the
   daisy chain) exhibits the dim/out conditions about 85% of the time. The following table shows
   the combinations of TLCs involved the failure has been observed:
   
   0-1-2-3-4-5-6-7-8
   0---------------8 (see note)
     1-2
     1---3
       2-3
       2---4
         3-4
         3---5
           4-5
           4---6
             5-6
             5---7
               6-7
               6---8
                 7-8
             
   The failures often involve corrupted data passed to the higher numbered TLC. Using a trace feature,
   I have observed the dim values being shifted to subsequent TLC when new data is shifted in. 

   An odd circumstance is that failure combinations 0-1 and 0-2 have never been observed. Also, TLC 0
   has been observed to perform most matching the intent of the programmed patterns.
   
   I have tried a few things to get at the root cause of this problem. I tried changing the timing
   of the clock, latch and data signals. None of these changes made any difference. I also turned
   on the OE (blank) line during the tlcWrite operation. It made absolutely no difference. The
   corruption of the data may be occuring as it exits TLC2. Sometimes it is observed that all LEDs
   driven by a TLC go bright for a very short time and then go dim or off.
   
   The 0-8 combination observed is unique. I have not observed any other combinations involving TLC0.
   The data corruption seems to occur after TLC0 but it is odd that this corruption hasn't been
   observed ending up in a TLC closer to TLC0.

   The program avoids this problem by applying a "bandaid" in the tlcWrite function. This code
   shifts a value of 2047 to the TLCs every time it sees 2048 in the brightness table.
   
   2. The "flash" problem
      -------------------
   
   I have observed LEDs quickly lighting to full brilliance for brief
   instant and then go out. (a quick flash). I also have seen LEDs go to a bright level
   and stay on even when the brightness level was set lower. While tracing the 2048 problem, I have
   seem especially bright flashing.
   
   3. "RF" Problem
      ------------

   When placing the LEDs on the limbs while lit I observed that just handling the led strings
   would sometimes alter their brightness. It seemed as if there was an RF signal on the lines
   and my handling them was affecting the PWM duty cycles. No precise experiment was performed.

   The TLC5947 has a 4 MHz. internal oscillator which it uses for internal clocking. The data
   shifting into the TLC5947 shift registers can take place at speeds up to 30 MHz but the
   shifting from the Arduino is estimated to be at 0.75 KHz rate. It takes about 1.3 Msec to
   complete a tlcWrite (9 TLCs x 24 channels/TLC x 12 bits/channel). The Pro Mini 5v model has
   a 16 MHz internal clock. That is 62.5 ns per clock. Most machine language instructions use
   just one clock. The number of instructions
   
   Since the PWM signal is generated using a 12 bit counter, the PWM frequency is about 1 KHz.
   (4 MHz. / 4096).

     Kevin arrah's video on the TLC5940 uses the blank pin as a critical part of the interface.
     He showed it pulsing every 512 microseconds. It is a high rising pulse 200 nsec wide.  It
     is used to restart the PWM counter. He also uses this sequence: Blank high, then Latch
     high,then Latch low and then Blank low.
   

   4. Fluttering
      ----------

   Sometimes the LEDs from a TLC (usually the top one or one near the top) will start fluttering
   after they have been displaying the same pattern for a while. Just refreshing the display
   (tlcWrite) stops the fluttering. The flutter rate of is about 5-10 Hz.
          

*/
