/**
 * @file        bypass.c
 * @author      aSumo
 * @version     1.0
 * @date        2025-05-05
 * @copyright   (c) 2025 aSumo
 * @brief       Functions for turn relays.
 */

#include "TrailBypass_PIC12F6XX.h"

//! 関数として書くと割り込み処理の中から呼ぶとき良くないらしいので、マクロで回避
#define TURN()       \
  do {               \
    GIE = 0;         \
    if (isTrail) {   \
      TURN_Trail();  \
    } else {         \
      TURN_Normal(); \
    }                \
    GIE = 1;         \
  } while (false)

//! トレイルモード時のTURN()
#define TURN_Trail()    \
  do {                  \
    Relay01 = true;     \
    waitRelay();        \
    Relay02 = true;     \
    waitRelay();        \
    Relay03 = !Relay03; \
    waitRelay();        \
    LED = !LED;         \
  } while (false)

//! 通常モード時のTURN()
#define TURN_Normal()   \
  do {                  \
    Relay01 = !Relay01; \
    waitRelay();        \
    Relay02 = !Relay02; \
    waitRelay();        \
    Relay03 = !Relay03; \
    waitRelay();        \
    LED = !LED;         \
  } while (false)

/**
 * @fn     void transfer(void)
 * @brief   通常モードからトレイルモードに切り替える関数
 * @details 通常モードからトレイルモードに切り替える関数
 */
void transfer() {
  GIE = 0;             // 割り込み禁止
  Relay01 = !Relay01;  // Relay01の状態を反転する
  waitRelay();         // Relay01の動作を待つ
  Relay02 = !Relay02;  // Relay02の状態を反転する
  waitRelay();         // Relay02の動作を待つ
  GIE = 1;             // 割り込み許可
}

bool state = false;      //!< ペダルの状態（現状では不要）
bool flag1 = true;       //!< 反転フラグ（モーメンタリ用）
bool flag2 = true;       //!< 反転フラグ（オルタネイト用）
bool isTrail = false;    //!< トレイルモードのフラグ
bool is1stTrail = true;  //!< フラグが切り替わって最初のループかどうか

/**
 * @fn      void bypass(void)
 * @brief   バイパスの挙動を定義する関数
 * @details トレイルモードの是非をスキャンしながら割り込みを待つ
 */
void bypass() {
  if (initState) {  // 初期状態がONの指定なら
    TURN();         // 手動でひっくり返しておく
  }

  GIE = 1;  // ここで割り込みを許可

  while (true) {          // ループ開始時点では通常モードであることに注意！
    if (!ModeSwitch) {    // ModeSwitchが倒されている
      waitChattering();   // 本当に倒されているか？
      if (!ModeSwitch) {  // 倒されている
        isTrail = true;   // とにかくトレイルモードにする

        if (is1stTrail) {      // 初回のみ
          transfer();          // 通常モードからトレイルモードに切り替える
          is1stTrail = false;  // フラグをクリア
        }
      }
    } else {              // ModeSwitchが倒されていない
      waitChattering();   // 本当に倒されていないか？
      if (ModeSwitch) {   // 倒されていない
        isTrail = false;  // とにかく通常モードにする

        if (!is1stTrail) {    // 初回のみ
          transfer();         // トレイルモードから通常モードに切り替える
          is1stTrail = true;  // フラグを立てる
        }
      }
    }  // あとはひたすら割り込み待ち
  }
}

/**
 * @fn      void __interrupt isr(void)
 * @brief   フットスイッチが踏まれると割り込んでエフェクトをON/OFFする関数
 * @details 割り込み処理の中で関数を呼ぶとあんまり良くないらしい
 * @n       参考:
 * https://developerhelp.microchip.com/xwiki/bin/view/software-tools/xc8/non-reentrant/
 */
void __interrupt() isr(void) {
  if (GPIF) {  // GPIO変化割り込みフラグが立っているか？
    waitChattering();
    if (GPIF) {             // 本当に立っているか？
      GPIF = 0;             // 割り込みフラグをクリア
      if (momentary) {      // モーメンタリ動作ならば
        TURN();             // ひっくり返す
      } else {              // オルタネイト動作ならば
        if (!FootSwitch) {  // フットスイッチが踏まれているか？
          if (flag2) {      // フラグが立っているならば
            if (timing) {   // タイミングの指定によっては離されるまで待つ
              while (!FootSwitch) {
                ;  // 離されるまでキープ
              }
            }
            TURN();         // ひっくり返す
            flag2 = false;  // 反転フラグをクリア
          }
        } else {         // フットスイッチが離されているか？
          flag2 = true;  // 反転フラグを立てる
        }
      }
    }
  }
}