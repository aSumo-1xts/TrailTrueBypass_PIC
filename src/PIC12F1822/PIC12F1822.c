/**
 * @file        PIC12F1822.c
 * @author      aSumo
 * @version     1.0
 * @date        2025-05-12
 * @copyright   (c) 2025 aSumo
 */

#include "PIC12F1822.h"

#include "../header.h"

//! EEPROM（0x00~0x07）の初期値を定義するマクロ
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);

// EEPROMから読み込むグローバル変数
// ここは本当に仮の値で、初期値はEEPROMに設定することで変更できる（main.c）
bool initState = false;  //!< ペダルの初期状態
bool momentary = false;  //!< モーメンタリ動作の是非
bool timing = false;     //!< オルタネイト動作時、いつ反転するか

/**
 * @fn      uint8_t eepRead(uint8_t adress)
 * @brief   EEPROMからデータを読み込む関数
 * @param   adress  データを読み込む番地
 * @return  読み込んだデータ
 */
uint8_t eepRead(uint8_t address) {
  EEADR = address;    // Set EEPROM address
  EECON1bits.RD = 1;  // Begin EEPROM read
  while (EECON1bits.RD) {
    ;
  }  // Wait for read to complete
  return EEDATA;  // Return the data byte read
}

/**
 * @fn      uint8_t eepWrite(uint8_t adress, uint8_t myData)
 * @brief   EEPROMにデータを書き込む関数
 * @param   adress  データの書き込み番地
 * @param   myData  書き込むデータ（少なくとも数値型であること！）
 */
void eepWrite(uint8_t address, uint8_t myData) {
  EECON1bits.WREN = 1;  // Enable EEPROM writes
  EEADR = address;      // Set EEPROM address
  EEDATA = myData;      // Load data to write
  // Required unlock sequence
  EECON2 = 0x55;
  EECON2 = 0xAA;
  EECON1bits.WR = 1;  // Trigger write
  while (EECON1bits.WR) {
    ;
  }  // Wait for write to finish
  EECON1bits.WREN = 0;  // Disable writes
}

/**
 * @fn      void init(void)
 * @brief   諸々の初期化を行う関数
 */
void init(void) {
  disAnalog();
  CM1CON0 = 0x00;
#ifdef CM2CON0
  CM2CON0 = 0x00;
#endif
  OSCCON = 0b01111000;  // 内部クロックを16 MHz に設定
  TRISA = 0x08;         // RA3 input, others output
  LATA = 0x00;          // All outputs LOW

  // Enable interrupt-on-change on RA3 (foot switch pin)
  INTCONbits.IOCIE = 1;  // Enable IOC (pin change) interrupts
  IOCAP = 0x08;          // Interrupt on rising edge of RA3
  IOCAN = 0x08;          // Interrupt on falling edge of RA3
  di();                  // Disable global interrupts until ready (XC8 macro)

  setInitState();
}

/**
 * @fn      void __interrupt isr(void)
 * @brief   bypass_interrupt()の無限whileループ中に呼び出される
 * @details 割り込み処理の中で関数を呼ぶとあんまり良くないらしい
 * @n       参考:
 * https://developerhelp.microchip.com/xwiki/bin/view/software-tools/xc8/non-reentrant/
 */
void __interrupt() isr(void) {
  if (INTCONbits.IOCIF) {  // If an interrupt-on-change occurred
    insideISR();
    IOCAFbits.IOCAF3 = 0;
    INTCONbits.IOCIF = 0;
  }
}