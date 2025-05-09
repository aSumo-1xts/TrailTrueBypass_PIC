#ifndef TrueBypass_PIC12F6XX_H
#define TrueBypass_PIC12F6XX_H

#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

#pragma config FOSC = INTRCIO  // 内部クロック          使用
#pragma config WDTE = OFF      // Watch-Dogタイマー     不使用
#pragma config PWRTE = ON      // パワーアップタイマー  使用
#pragma config MCLRE = OFF     // リセットピン          不使用
#pragma config BOREN = ON      // Brown-Out検出         使用
#pragma config CP = ON         // プログラム保護        使用
#pragma config CPD = ON        // データ保護            使用
#define _XTAL_FREQ 20000000    // クロック周波数        20 MHz

// ピンアウト
#define Relay01 GP0     // Relayピンその1
#define Relay02 GP1     // Relayピンその2
#define Relay03 GP2     // Relayピンその3
#define FootSwitch GP3  // FootSwitchピン（Momentary）
#define ModeSwitch GP4  // ModeSwitchピン（Alternate）
#define LED GP5         // LEDピン

// その他ハードウェアに関する設定
#define waitChattering() __delay_ms(5)  // 長くとも数十 [ms] らしい
#define waitPhotoCoupler() \
  __delay_us(10)                   // PC817なら上昇時間4 [us] / 下降時間3 [us]
#define waitRelay() __delay_ms(3)  // AZ850なら動作時間2 [ms] / 復帰時間1 [ms]

// EEPROMから読み込むグローバル変数
// ここは本当に仮の値で、初期値はEEPROMに設定することで変更できる（main.c）
bool initState = false;  // ペダルの初期状態
bool momentary = false;  // モーメンタリ動作の是非
bool timing = false;     // オルタネイト動作時、いつ反転するか

// その他EEPROM関連
#define eepAdd_initState 0x00  // アドレスその1
#define eepAdd_momentary 0x02  // アドレスその2
#define eepAdd_timing 0x04     // アドレスその3

// LEDの点滅時間
#define interval_10 10
#define interval_50 50

// EEPROMから読み込むグローバル変数
extern bool initState;  // ペダルの初期状態
extern bool momentary;  // モーメンタリ動作の是非
extern bool timing;     // オルタネイト動作時、いつ反転するか

// 関数のプロトタイプ宣言
void init(void);
void bypass(void);

#endif /* PIC12F6XX_H */
