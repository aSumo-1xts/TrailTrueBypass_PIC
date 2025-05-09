/**
 * @file        main.c
 * @author      aSumo
 * @version     2.0
 * @date        2025-05-05
 * @copyright   (c) 2024 aSumo
 * @brief       Program for Normal Trail Bypass with PIC microcontroller.
 * @details     デバイスに合わせてヘッダーファイルだけを変更すれば済みます。
 */

#include "TrailBypass_PIC12F6XX.h"

//! EEPROM（0x00~0x07）の初期値を定義するマクロ
__EEPROM_DATA(0, 0, 0, 0, 0, 0, 0, 0);

/**
 * @fn      void main(void)
 * @brief   サボり上司
 */
int main(void) {
  init();
  bypass();
  return 0;
}