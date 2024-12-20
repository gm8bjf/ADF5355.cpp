/**********************************************************************
**  Device: ADF5355                                                  **
**  File  : ADF5355.cpp                                              **
**                                                                   **
**  Created by Bo, OZ2M, http://www.rudius.net/oz2m                  **
**                                                                   **
**  Description:                                                     **
**  This file is a sample code for your reference. About ADF5355     **
**  module use on Arduino with SPI.                                  **
**                                                                   **
**  This code is free software; you can redistribute it and/or       **
**  modify it under the terms of the GNU Lesser General Public       **
**  License as published by the Free Software Foundation; either     **
**  version 2.1 of the License, or (at your option)                  **
**  any later version.                                               **
**                                                                   **
**  Copyright (C) Bo, OZ2M                                           **
**                                                                   **
***********************************************************************
** Register sequences modified to be as per the data sheet GM8BJF    **
** and GM4WZG  2/2/2018                                              **
***********************************************************************
** Modified to reduce key-clicks  GM8BJF 20/12/2024                  **
**********************************************************************/
#include <Arduino.h>

#include "ADF5355.h"
/*
                  Reg 0       Reg 1         Reg 2       Reg3        Reg 4       Reg5        Reg 6       Reg 7       Reg 8       Reg 9       Reg10       Reg 11      Reg12      
*/                  
const uint32_t ftw[8][13] =
{ 
      {0x200320, 0x1506901, 0x6907FFF2, 0x40000003, 0x3600BD84, 0x800025, 0x35CCC076, 0x120000E7, 0x102D0428, 0x1B1AFCC9, 0xC0283A, 0x61300B, 0x1041C},   // PI4 tone0        * 0 *  The relevant Frequency Tuning Words
      {0x200320, 0x1515EC1, 0x91FFFFF2, 0x40000003, 0x3600BD84, 0x800025, 0x35CCC076, 0x120000E7, 0x102D0428, 0x1B1AFCC9, 0xC0283A, 0x61300B, 0x1041C},   // PI4 tone1        * 1 *  Six FTW are needed for a PI4 + CW + carrier sequence
      {0x200320, 0x1525481, 0x3E515552, 0x40000003, 0x3600BD84, 0x800025, 0x35CCC076, 0x120000E7, 0x102D0428, 0x1B1AFCC9, 0xC0283A, 0x61300B, 0x1041C},   // PI4 tone2        * 2 *  Set each FTW set to the number of bytes need
      {0x200320, 0x1534A41, 0xE3E7FFF2, 0x40000003, 0x3600BD84, 0x800025, 0x35CCC076, 0x120000E7, 0x102D0428, 0x1B1AFCC9, 0xC0283A, 0x61300B, 0x1041C},   // PI4 tone3        * 3 *  PI4: index 0-3. CW FSK: index 4. CW carrier: index 5
      {0x200320, 0x14E7D81, 0x7B55552,  0x40000003, 0x3600BD84, 0x800025, 0x35CCC076, 0x120000E7, 0x102D0428, 0x1B1AFCC9, 0xC0283A, 0x61300B, 0x1041C},   // CW -FSK tone     * 4 *
      {0x200320, 0x1506901, 0x6907FFF2, 0x40000003, 0x3600BD84, 0x800025, 0x35CCC076, 0x120000E7, 0x102D0428, 0x1B1AFCC9, 0xC0283A, 0x61300B, 0x1041C},   // CW carrier tone  * 5 *  Example: 50.002MHz using a 63.8976 MHz reference.

      {0x200320, 0x8A5BE51, 0xBE5BFFF2, 0x40000003, 0x3600BD84, 0x800025, 0x35CCC076, 0x120000E7, 0x102D0428, 0x1B1AFCC9, 0xC0283A, 0x61300B, 0x1041C},   // CW -FSK tone     * 6 *
      {0x200320, 0x8A76271, 0x6277FFF2, 0x40000003, 0x3600BD84, 0x800025, 0x35CCC076, 0x120000E7, 0x102D0428, 0x1B1AFCC9, 0xC0283A, 0x61300B, 0x1041C}    // CW carrier tone  * 7 *  Example: 50.460 MHz for an ADF5355 with a 63.8796 ref.
};
                 
/*
    Bo, OZ2M: It is possible to modify/speed up changing frequency
              if some registers are identical. Reloading of identical
              contents, typically found in registers 3-12, is superfluous.
              If so, you will have to modify the SetFrequency function
              slightly yourself. But don't forget to load all registers
              at least ones. This can be done in the Initialize function.
              Also loading all registers can cause clicks because of PLL
              recalibration. Thus you may have to experiment a bit.
*/

// Public

ADF5355::ADF5355(byte LE)
{
    pinLE = LE;
}

void ADF5355::Initialize()
{
    SPI.begin();                            // Setup SPI bus
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));   // set spi speed to 100kHz
  
    pinMode(pinLE, OUTPUT);
    digitalWrite(pinLE, HIGH);
        

     WriteRegister(ftw[1][12]);     // Register initialisation sequence as per data sheet
     WriteRegister(ftw[1][11]);     // Register values from AD demo software
     WriteRegister(ftw[1][10]);
     WriteRegister(ftw[1][9]);
     WriteRegister(ftw[1][8]);     
     WriteRegister(ftw[1][7]);
     WriteRegister(ftw[1][6]);
     WriteRegister(ftw[1][5]);
     WriteRegister(ftw[1][4]);     
     WriteRegister(ftw[1][3]);
     WriteRegister(ftw[1][2]);
     WriteRegister(ftw[1][1]);
     delayMicroseconds(161);
     WriteRegister(ftw[1][0]);
     delayMicroseconds(1);      
}

void ADF5355::SetFrequency(byte ftwNo)  //Changed for key-click elimination.

/* frequency update sequence as per data sheet */
/*
{
  // Auto-cal on //
    WriteRegister(ftw[ftwNo][10]);
    WriteRegister(ftw[ftwNo][4] | (1UL << 4) ); // db4 = 1
    WriteRegister(ftw[ftwNo][2]);
    WriteRegister(ftw[ftwNo][1]);
    WriteRegister(ftw[ftwNo][0]   &  ~(1UL << 21));  // db21 = 0   Bernie's Magic Sauce!!
    WriteRegister(ftw[ftwNo][4]);  
    delayMicroseconds(161);
    WriteRegister(ftw[ftwNo][0]); 
    delayMicroseconds(1);

}  
*/
{
  // Modified to reduce key clicks //
  // only vary frequency registers and load data with bit 21 Reg 21 set to 0 //
 
    WriteRegister(ftw[ftwNo][2]);
    WriteRegister(ftw[ftwNo][1]);
    WriteRegister(ftw[ftwNo][0]   &  ~(1UL << 21));  // db21 = 0  Bit twiddle - Bernie's Magic Sauce!!
    // switching bit 21 to 0 to turn off Auto frequency calibration - Stops key clicks!  //
    // Reg 0 required to load updated data.
    delayMicroseconds(161);

} 
 

// Private

void ADF5355::WriteRegister(const uint32_t value)
{
    digitalWrite(pinLE, LOW);

    for (int8_t i = 3; i >= 0; i--)
        SPI.transfer((value >> 8 * i) & 0xFF);

    digitalWrite(pinLE, HIGH);
    delayMicroseconds(3);
    digitalWrite(pinLE, LOW);
}
