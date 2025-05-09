/**
 * @file        init.c
 * @author      aSumo
 * @version     1.0
 * @date        2025-05-05
 * @copyright   (c) 2025 aSumo
 * @brief       Functions for EEPROM and initialization.
 */

#include "TrailBypass_PIC12F6XX.h"

void disAnalog(void) {
#ifdef ANSEL
  ANSEL = 0x00;
#endif
#ifdef ADCON0
  ADCON0 = 0x00;
#endif
};  //!< アナログ機能を無効化する関数

/**
 * @fn      void flashLED(uint8_t interval, uint8_t times)
 * @param   types   点滅の間隔
 * @param   times   点滅の回数
 * @brief   LEDを点滅させる関数
 */
void flashLED(uint8_t interval, uint8_t times) {
  uint8_t i = 0;
  for (i = 0; i < times; i++) {
    if (interval == 10) {
      LED = true;
      __delay_ms(interval_10);
      LED = false;
      __delay_ms(interval_10);
    } else if (interval == 50) {
      LED = true;
      __delay_ms(interval_50);
      LED = false;
      __delay_ms(interval_50);
    }
  }
}

/**
 * @fn      uint8_t eepRead(uint8_t adress)
 * @brief   EEPROMからデータを読み込む関数
 * @param   adress  データを読み込む番地
 * @return  読み込んだデータ
 */
uint8_t eepRead(uint8_t adress) {
  EEADR = adress;  // アドレスを指定
  RD = 1;          // 読み込みを開始
  while (RD) {
    ;
  };  // RD==0まで待つ
  return EEDATA;  // 読み込んだ値を返却
}

/**
 * @fn      uint8_t eepWrite(uint8_t adress, uint8_t myData)
 * @brief   EEPROMにデータを書き込む関数
 * @param   adress  データの書き込み番地
 * @param   myData  書き込むデータ（少なくとも数値型であること！）
 */
void eepWrite(uint8_t adress, uint8_t myData) {
  GIE = 0;   // 割り込みを禁止
  WREN = 1;  // 書き込みを許可

  EEADR = adress;   // アドレスを指定
  EEDATA = myData;  // 書き込みたいデータ
  EECON2 = 0x55;    // 暗号その1
  EECON2 = 0xAA;    // 暗号その2
  WR = 1;           // 書き込みをセット
  while (WR) {
    ;  // WR==0まで待つ
  };

  WREN = 0;  // 書き込みを禁止
  GIE = 1;   // 割り込みを許可
}

/**
 * @fn      void init(void)
 * @brief   諸々の初期化を行う関数
 */
void init() {
  GIE = 0;  // 念のため割り込みを禁止しておく

  int flag_i = 0;  // forループ用のカウンタはuint8_tだとまずいらしい

  disAnalog();  // アナログ機能を備えている場合は無効化

  CMCON = 0x07;         // コンパレータ     不使用
  TRISIO = 0b00011000;  // GP3とGP4は入力、他は出力
  GPIO = 0;             // 全てのピンをLOWに設定（Mode: Normal / Effect: OFF）

  //=== interruptのための2行ここから ===//
  INTCON |= 0b10001000;  // GIE=1, GPIE=1 (GPIO割り込み有効)
  IOC3 = 1;              // GP3のピン変化割り込み有効
  //=== interruptのための2行ここまで ===//

  // EEPROMからuint8_t型をbool型に変換して読み込む（0:false / 1:true）
  initState = (bool)(eepRead(eepAdd_initState));
  momentary = (bool)(eepRead(eepAdd_momentary));
  timing = (bool)(eepRead(eepAdd_timing));

  // 踏みながら電源を入れてモード切り替え
  if (!FootSwitch) {
    waitChattering();

    if (!FootSwitch) {  // surely pressed
      flashLED(10, 3);

      // Alternate or Momentary
      for (flag_i = 0; flag_i < 50; flag_i++) {  // flag_i*10秒間待ってやる
        if (!FootSwitch) {
          __delay_ms(10);          // 踏まれている間は待ち続ける
        } else {                   // 離されたら
          momentary = !momentary;  // 論理反転
          eepWrite(eepAdd_momentary, (uint8_t)momentary);  // EEPROMに書き込む
          break;  // forループごと抜ける
        }
      }

      // ON or OFF when Power-ON
      if (flag_i > 45) {
        // initStateの設定に入る
        initState = !initState;  // 論理反転
        eepWrite(eepAdd_initState,
                 (uint8_t)initState);  // EEPROMに書き込む
        flashLED(50, 3);               // initState設定完了！
        while (!FootSwitch) {
          ;  // 離されるまでキープ
        };
      }
    }

    waitChattering();
  }
}