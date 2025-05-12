/**
 * @file        common.c
 * @author      aSumo
 * @version     1.0
 * @date        2025-05-12
 * @copyright   (c) 2025 aSumo
 * @brief       PICマイコンによらず共通な関数を定義するファイル
 */

#include "header.h"

bool isTrail = false;        //!< トレイルモードのフラグ
bool flag_alternate = true;  //!< 反転フラグ（オルタネイト用）

//! 関数として書くと割り込み処理の中から呼ぶとき良くないらしいので、マクロで回避
#define TURN()       \
  do {               \
    di();            \
    if (isTrail) {   \
      TURN_Trail();  \
    } else {         \
      TURN_Normal(); \
    }                \
    ei();            \
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
 * @fn      void flashLED(uint8_t interval, uint8_t times)
 * @param   types   点滅の間隔
 * @param   times   点滅の回数
 * @brief   LEDを点滅させる関数
 */
void flashLED(uint8_t interval, uint8_t times) {
  for (uint8_t i = 0; i < times; i++) {
    switch (interval) {
      case 1:
        LED = true;
        __delay_ms(interval_short);
        LED = false;
        __delay_ms(interval_short);
        break;

      case 2:
        LED = true;
        __delay_ms(interval_long);
        LED = false;
        __delay_ms(interval_long);
        break;

      default:
        break;
    }
  }
}

/**
 * @fn      void disAnalog(void)
 * @brief   アナログ機能を無効化する関数
 * @details 予約語の宣言の有無からチップのアナログ機能の有無を判別
 */
void disAnalog(void) {
#ifdef ANSEL
  ANSEL = 0x00;
#endif
#ifdef ADCON0
  ADCON0 = 0x00;
#endif
};  //!< アナログ機能を無効化する関数

/**
 * @fn     void setInitState(void)
 * @brief  EEPROMの値を読み書きして初期状態を設定する関数
 */
void setInitState(void) {
  int flag_i = 0;  // 踏まれている時間をカウントする変数

  // EEPROMからuint8_t型をbool型に変換して読み込む（0:false / 1:true）
  initState = (bool)(eepRead(eepAdd_initState));
  momentary = (bool)(eepRead(eepAdd_momentary));
  timing = (bool)(eepRead(eepAdd_timing));

  // 踏みながら電源を入れてモード切り替え
  if (!FootSwitch) {
    waitChattering();

    if (!FootSwitch) {  // surely pressed
      flashLED(1, 3);

      // Alternate or Momentary
      for (flag_i = 0; flag_i < 300; flag_i++) {  // flag_i*10秒間待ってやる
        if (!FootSwitch) {
          __delay_ms(10);          // 踏まれている間は待ち続ける
        } else {                   // 離されたら
          momentary = !momentary;  // 論理反転
          eepWrite(eepAdd_momentary, (uint8_t)momentary);  // EEPROMに書き込む
          break;  // forループごと抜ける
        }
      }

      // Effect ON/OFF when Power-ON
      if (flag_i > 275) {
        // initStateの設定に入る
        initState = !initState;  // 論理反転
        eepWrite(eepAdd_initState,
                 (uint8_t)initState);  // EEPROMに書き込む
        flashLED(2, 3);                // initState設定完了！
        while (!FootSwitch) {
          ;  // 離されるまでキープ
        };
      }
    }

    waitChattering();
  }
}

/**
 * @fn     void transfer(void)
 * @brief   通常モードからトレイルモードに切り替える関数
 * @details 通常モードからトレイルモードに切り替える関数
 */
void transfer() {
  di();                // 割り込み禁止
  Relay01 = !Relay01;  // Relay01の状態を反転する
  waitRelay();         // Relay01の動作を待つ
  Relay02 = !Relay02;  // Relay02の状態を反転する
  waitRelay();         // Relay02の動作を待つ
  ei();                // 割り込み許可
}

/**
 * @fn      void bypass(void)
 * @brief   バイパスの挙動を定義する関数
 * @details
 * トレイルモードの是非をスキャンしながらエフェクトON/OFFの割り込みを待つ
 */
void bypass() {
  bool is1stTrail = true;  //!< フラグが切り替わって最初のループかどうか

  if (initState) {  // 初期状態がONの指定なら
    TURN();         // 手動でひっくり返しておく
  }

  ei();  // ここで割り込みを許可

  while (true) {               // ループ開始時点では通常モードであることに注意！
    if (!ModeSwitch) {         // ModeSwitchが倒されている
      waitChattering();        // 本当に倒されているか？
      if (!ModeSwitch) {       // 倒されている！
        isTrail = true;        // とにかくトレイルモードにする
        if (is1stTrail) {      // 初回のみ
          transfer();          // 通常モードからトレイルモードに切り替える
          is1stTrail = false;  // フラグをクリア
        }
      }
    } else {                  // ModeSwitchが倒されていない
      waitChattering();       // 本当に倒されていないか？
      if (ModeSwitch) {       // 倒されていない
        isTrail = false;      // とにかく通常モードにする
        if (!is1stTrail) {    // 初回のみ
          transfer();         // トレイルモードから通常モードに切り替える
          is1stTrail = true;  // フラグを立てる
        }
      }
    }  // あとはひたすら割り込み待ち
  }
}

/**
 * @fn      void insideISR(void)
 * @brief   割り込み中に実行される関数
 */
void insideISR(void) {
  waitChattering();

  if (!FootSwitch) {  // 踏まれているか？
    if (momentary) {  // モーメンタリ動作ならば
      TURN();         // ひっくり返す
      while (!FootSwitch) {
        ;  // 離されるまでキープ
      }
      TURN();        // 離されたらもう一度ひっくり返す
    } else {         // オルタネイト動作ならば
      if (timing) {  // タイミングの指定によっては離されるまで待ってから
        while (!FootSwitch) {
          ;  // 離されるまでキープ
        }
      }
      TURN();  // ひっくり返す
    }
  }
}