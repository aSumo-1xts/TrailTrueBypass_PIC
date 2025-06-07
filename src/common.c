/**
 * @file        common.c
 * @author      aSumo
 * @version     1.0
 * @date        2025-06-07
 * @copyright   (c) 2025 aSumo
 * @brief       PICマイコンによらず共通な関数を定義するファイル
 */

#include "header.h"

bool isTrail = false;  //!< バイパスモード: トレイル / ノーマル
bool state = false;    //!< エフェクト: ON / OFF

//! エフェクトON/OFFの切り替えマクロ（割り込み中の関数の実行が非推奨であるため）
#define TURN()            \
  do {                    \
    PhotoCoupler = true;  \
    waitPhotoCoupler();   \
    Relay02 = !Relay02;   \
    waitRelay();          \
    if (!isTrail) {       \
      Relay01 = !Relay01; \
      waitRelay();        \
    }                     \
    PhotoCoupler = false; \
    LED = !LED;           \
    state = !state;       \
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
 * @fn      void scanModeSw(void)
 * @brief   モードスイッチをスキャンして、適宜モードを切り替える関数
 *
 */
void scanModeSw(void) {
  if (!ModeSwitch) {             // モードスイッチが倒されている
    waitChattering();            // 二度見
    if (!ModeSwitch) {           // 倒されている！
      if (!isTrail) {            // 今ノーマルモードならば
        if (!state) {            // エフェクトOFFならば（ONのときは状態が共通）
          PhotoCoupler = true;   // PhotoCouplerをON
          waitPhotoCoupler();    // PhotoCouplerの立ち上がりを待つ
          Relay01 = true;        // Relay01をON
          waitRelay();           // Relay01の動作を待つ
          PhotoCoupler = false;  // PhotoCouplerをOFF
        }
        isTrail = true;  // トレイルモードへ移行
      }
    }
  } else {                       // モードスイッチが倒されていない
    waitChattering();            // 二度見
    if (ModeSwitch) {            // 倒されていない！
      if (isTrail) {             // 今トレイルモードならば
        if (!state) {            // エフェクトOFFならば（ONのときは状態が共通）
          PhotoCoupler = true;   // PhotoCouplerをON
          waitPhotoCoupler();    // PhotoCouplerの立ち上がりを待つ
          Relay01 = false;       // Relay01をOFF
          waitRelay();           // Relay01の動作を待つ
          PhotoCoupler = false;  // PhotoCouplerをOFF
        }
        isTrail = false;  // ノーマルモードへ移行
      }
    }
  }
}

/**
 * @fn      void bypass(void)
 * @brief   バイパスの挙動を定義する関数
 * @details
 * トレイルモードの是非をスキャンしながらエフェクトON/OFFの割り込みを待つ
 */
void bypass() {
  // ポップノイズを逃がす処理
  PhotoCoupler = true;           // フォトカプラでミュート
  waitPhotoCoupler();            // PhotoCouplerの立ち上がりを待つ
  for (int i = 0; i < 4; i++) {  // 必ず偶数回
    Relay02 = !Relay02;          // Relay02をトグル
    Relay01 = !Relay01;          // Relay01をトグル
  }
  PhotoCoupler = false;  // ミュート解除

  // この時点では、必ず isTrail==false かつ state==false であることに留意
  scanModeSw();  // まず最初にモードスイッチに合わせる

  if (initState) {  // 初期状態がONの指定なら
    TURN();
  }

  ei();  // ここで割り込みを許可

  while (true) {
    scanModeSw();  // モードスイッチを見張りながら割り込み待ち
  }
}

/**
 * @fn      void insideISR(void)
 * @brief   割り込み中に実行される関数
 * @details 割り込み禁止状態で実行されることに注意
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