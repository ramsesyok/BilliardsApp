cmake_minimum_required(VERSION 3.10.0)
project(BilliardsApp VERSION 0.1.0 LANGUAGES C CXX)
# ──── C++17 を有効化 ────────────────────────────────
set(CMAKE_CXX_STANDARD 17)                     # C++ の規格を 17 に設定
set(CMAKE_CXX_STANDARD_REQUIRED ON)            # 規格が必須であることを指定
set(CMAKE_CXX_EXTENSIONS OFF)                  # GNU 拡張（-std=gnu++17）ではなく純粋な -std=c++17 を使用
# ────────────────────────────────────────────────────

add_executable(BilliardsApp main.cpp PhysicsEngine.cpp Ball.cpp)

target_include_directories(BilliardsApp PRIVATE ${CMAKE_SOURCE_DIR}/olcPixelGameEngine)

target_compile_options(BilliardsApp PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
    -finput-charset=UTF-8
    -fexec-charset=UTF-8
    -Wno-inconsistent-missing-override
  >
  $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)


# ----------------------------------------
# 単体テスト (Catch2 v3 amalgamated) の設定
# ----------------------------------------

# テスト実行有効化
enable_testing()

# テスト用実行ファイル「test_ball」をビルド
add_executable(test_ball
  tests/catch_amalgamated.cpp
  tests/test_Ball.cpp
  Ball.cpp
)
target_compile_options(test_ball PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
    -finput-charset=UTF-8
    -fexec-charset=UTF-8
    -Wno-inconsistent-missing-override
  >
  $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

# Catch2 のヘッダが tests/catch_amalgamated.hpp として存在する前提で、
# tests/ をインクルードディレクトリに追加
target_include_directories(test_ball PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/olcPixelGameEngine
  ${CMAKE_SOURCE_DIR}/tests
)
# CTest にテストを登録
add_test(NAME BallUnitTests COMMAND test_ball)

