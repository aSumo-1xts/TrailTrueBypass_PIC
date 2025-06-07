#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

#include "./PIC12F6XX/PIC12F6XX.h"

// その他ハードウェアに関する設定
static inline void waitChattering() {
  __delay_ms(5);
}  // 長くとも数十 [ms] らしい
static inline void waitPhotoCoupler() {
  __delay_us(5);
}  // PC817なら上昇時間4 [us] / 下降時間3 [us]
static inline void waitRelay() {
  __delay_ms(2);
}  // AZ850なら動作時間2 [ms] / 復帰時間1 [ms]

// その他EEPROM関連
#define eepAdd_initState 0x00  //!< アドレスその1
#define eepAdd_momentary 0x02  //!< アドレスその2
#define eepAdd_timing 0x04     //!< アドレスその3

#define interval_short 50  //!< LED用のインターバル [ms]
#define interval_long 150  //!< LED用のインターバル [ms]

// プロトタイプ宣言
void disAnalog(void);
void flashLED(uint8_t interval, uint8_t times);
void setInitState(void);
void transfer(void);
void bypass(void);
void insideISR(void);
