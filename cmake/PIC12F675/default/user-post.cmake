# src/*.c をすべて取得
file(GLOB_RECURSE LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")

# 対象の OBJECT ライブラリに追加
target_sources(PIC12F675_default_default_XC8_compile PRIVATE ${LIB_SOURCES})

# ヘッダ参照が必要な場合
target_include_directories(PIC12F675_default_default_XC8_compile PRIVATE "${CMAKE_SOURCE_DIR}/src")
