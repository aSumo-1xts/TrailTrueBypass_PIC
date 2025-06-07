# TrailTrueBypass

[![KiBot](https://github.com/aSumo-1xts/TrailTrueBypass_PIC/actions/workflows/kibot.yaml/badge.svg)](https://github.com/aSumo-1xts/TrailTrueBypass_PIC/actions/workflows/kibot.yaml)
[![Doxygen](https://github.com/aSumo-1xts/TrailTrueBypass_PIC/actions/workflows/doxygen.yaml/badge.svg)](https://github.com/aSumo-1xts/TrailTrueBypass_PIC/actions/workflows/doxygen.yaml)
[![pages-build-deployment](https://github.com/aSumo-1xts/TrailTrueBypass_PIC/actions/workflows/pages/pages-build-deployment/badge.svg)](https://github.com/aSumo-1xts/TrailTrueBypass_PIC/actions/workflows/pages/pages-build-deployment)

[![KiCad: 9.0.2](https://img.shields.io/badge/-9.0.2-black?style=flat&logo=kicad&logoSize=auto&link=https%3A%2F%2Fdownloads.kicad.org%2Fkicad%2Fwindows%2Fexplore%2Fstable)](https://downloads.kicad.org/kicad/windows/explore/stable)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

8ピンのPICマイコンでトレイルバイパスを実装するための基板とソースコードです。

## Features

- Normal true bypass when Trail mode off
- Selectable operating mode: Alternate or Momentary
- Set the power-on state: ON or OFF
- (Option) Selectable timing to switch: when-pressed or when-released
- Automatically bypasses if power is accidentally turned off

## Verified on

- PIC12F629
- PIC12F675

---

Coming soon?

- PIC12F1822
- PIC16F15313

## dir: KiCad

KiCad関連のファイル一式です。

ガーバーファイルはReleaseからダウンロードできます。

## dir: src

PICマイコン用のソースコード群です。

プロジェクト管理の様式はVScode用のMPLAB Extension Packに準拠しています。

コンパイル後のhexファイルはReleaseからダウンロードできます。

PICマイコン間で共通のソースコードはsrc直下に、そうでないものはそのハードウェアの名前の付いたディレクトリに入っています。

src/header.hの冒頭で所望のPICマイコンのヘッダーファイルをincludeするだけで、そのPICマイコンが書き込み対象となります。