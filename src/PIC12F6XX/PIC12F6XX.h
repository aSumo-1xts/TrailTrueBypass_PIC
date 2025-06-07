#ifndef PIC12F6XX_H
#define PIC12F6XX_H

#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

#define _XTAL_FREQ 4000000  //!< クロック周波数         4MHz

#pragma config FOSC = INTRCIO  // 内部クロック          使用
#pragma config WDTE = OFF      // Watch-Dogタイマー     不使用
#pragma config PWRTE = ON      // パワーアップタイマー  使用
#pragma config MCLRE = OFF     // リセットピン          不使用
#pragma config BOREN = ON      // Brown-Out検出         使用
#pragma config CP = ON         // プログラム保護        使用
#pragma config CPD = ON        // データ保護            使用

// ピンアウト
#define Relay01 GP0       //!< Relayピンその1
#define Relay02 GP1       //!< Relayピンその2
#define PhotoCoupler GP2  //!< Relayピンその3
#define FootSwitch GP3    //!< FootSwitchピン（Momentary）
#define ModeSwitch GP4    //!< ModeSwitchピン（Alternate）
#define LED GP5           //!< LEDピン

// EEPROMから読み込むグローバル変数
extern bool initState;  // ペダルの初期状態
extern bool momentary;  // モーメンタリ動作の是非
extern bool timing;     // オルタネイト動作時、いつ反転するか

// 関数のプロトタイプ宣言
uint8_t eepRead(uint8_t adress);
void eepWrite(uint8_t adress, uint8_t myData);
void init(void);

#endif /* PIC12F6XX_H */
