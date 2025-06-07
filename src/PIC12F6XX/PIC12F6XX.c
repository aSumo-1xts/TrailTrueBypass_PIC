/**
 * @file        PIC12F6XX.c
 * @author      aSumo
 * @version     2.0
 * @date        2025-05-12
 * @copyright   (c) 2024 aSumo
 * @brief       PIC12F629, PIC12F675用の関数を定義するファイル
 */

#include "PIC12F6XX.h"

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
  WREN = 1;  // 書き込みを許可

  EEADR = adress;   // アドレスを指定
  EEDATA = myData;  // 書き込みたいデータ
  EECON2 = 0x55;    // 暗号その1
  EECON2 = 0xAA;    // 暗号その2
  WR = 1;           // 書き込みをセット
  while (WR) {
    ;
  };  // WR==0まで待つ

  WREN = 0;  // 書き込みを禁止
}

/**
 * @fn      void init_scan(void)
 * @brief   諸々の初期化を行う関数（割り込み式）
 * @details init_scanとほぼ同じだが、割り込みを有効にするための設定を行う
 */
void init() {
  disAnalog();  // アナログ機能を備えている場合は無効化

  CMCON = 0x07;   // コンパレータ     不使用
  TRISIO = 0x18;  // GP3, GP4を入力、他は出力
  GPIO = 0;       // 全てのピンをLOWに設定

  //=== interruptのための行ここから ===//
  INTCON |= 0b10001000;  // GIE=1, GPIE=1 (GPIO割り込み有効)
  IOC3 = 1;              // GP3のピン変化割り込みを有効化
  di();                  // とは言ったものの、bypass関数までは割り込み禁止
  //=== interruptのための行ここまで ===//

  setInitState();
}

/**
 * @fn      void __interrupt isr(void)
 * @brief   bypass_interrupt()の無限whileループ中に呼び出される
 */
void __interrupt() isr(void) {
  if (GPIF) {     // GPIO変化割り込みフラグが立っているか？
    insideISR();  // 割り込み処理を実行
    GPIF = 0;     // 割り込みフラグをクリア
  }
}