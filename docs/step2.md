---
marp: true
---

<!--
theme: default
paginate: true
-->

## ステップ2：`reflect` 関数の TDD 解説

---

## 1. `reflect` の概要

* **目的**:
  ボールがテーブル四辺の境界に当たったときに

  1. 位置をはみ出し分だけ補正し
  2. 速度の該当成分を反転する
* **対象**:
  `Ball` クラスのメソッド

  ```cpp
  void reflect(double xMin, double xMax, double yMin, double yMax);
  ```
* **なぜ TDD に適しているか**:

  * ロジックは if 文＋単純な加減算・符号反転だけで構成される
  * 境界条件（左／右／上／下）のテストパターンが明確かつ少数
  * 初心者でもテストケースをイメージしやすい

---

## 2. シグネチャと前提

```cpp
// Ball.hpp の一部
class Ball {
public:
    // ...省略...
    void reflect(double xMin, double xMax, double yMin, double yMax);
    // ...省略...
};
```

* 引数: テーブルの境界

  * `xMin` / `xMax` : 水平方向の最小値・最大値
  * `yMin` / `yMax` : 垂直方向の最小値・最大値
* 前提:

  * `Ball` の内部で `m_pos`（中心座標）と `m_vel`（速度）がメンバにある
  * `m_radius`（半径）がすでにセットされている

---

## 3. RED フェーズ：最初のテストコードを書く

### 3.1 テストケース案

* 【ケース①】左壁に当たる

  * 初期: `x = 0.4, vx = -2.0, r = 0.5`
  * 計算: 中心は左壁から `0.4 - 0.5 = -0.1` はみ出し → 補正後 `x = 0.5`
  * 速度: `vx = -2.0` → `+2.0` に反転

* 【ケース②】右壁に当たる

  * 初期: `x = 9.6, vx = +2.0, r = 0.5, xMax = 10.0`
  * はみ出し: `9.6 + 0.5 = 10.1` → 補正後 `x = 9.5`
  * 速度: `vx = +2.0` → `-2.0` に反転

* 【ケース③】上壁・下壁も同様に作成

  * 下: `y = 0.4, vy = -3.0, r = 0.5` → `y = 0.5, vy = +3.0`
  * 上: `y = 9.6, vy = +3.0, r = 0.5` → `y = 9.5, vy = -3.0`

```cpp
TEST_CASE("reflect: 左壁に当たる", "[reflect]") {
    Ball b(0.4, 5.0, -2.0, 0.0, 0.5);
    b.reflect(0.0, 10.0, 0.0, 10.0);
    REQUIRE(b.position().x == Catch::Approx(0.5));
    REQUIRE(b.velocity().x == Catch::Approx( 2.0));
}

TEST_CASE("reflect: 右壁に当たる", "[reflect]") {
    Ball b(9.6, 5.0, 2.0, 0.0, 0.5);
    b.reflect(0.0, 10.0, 0.0, 10.0);
    REQUIRE(b.position().x == Catch::Approx(9.5));
    REQUIRE(b.velocity().x == Catch::Approx(-2.0));
}

TEST_CASE("reflect: 下壁に当たる", "[reflect]") {
    Ball b(5.0, 0.4, 0.0, -3.0, 0.5);
    b.reflect(0.0, 10.0, 0.0, 10.0);
    REQUIRE(b.position().y == Catch::Approx(0.5));
    REQUIRE(b.velocity().y == Catch::Approx( 3.0));
}

TEST_CASE("reflect: 上壁に当たる", "[reflect]") {
    Ball b(5.0, 9.6, 0.0, 3.0, 0.5);
    b.reflect(0.0, 10.0, 0.0, 10.0);
    REQUIRE(b.position().y == Catch::Approx(9.5));
    REQUIRE(b.velocity().y == Catch::Approx(-3.0));
}
```

* 最初にこれをテストファイルに書いて\*\*RED（失敗）\*\*を確認
* 実装はまだ何もないまたは `return;` のみ → 全部失敗

---

## 4. GREEN フェーズ：最低限の実装

```cpp
// Ball.cpp の一部
void Ball::reflect(double xMin, double xMax, double yMin, double yMax) {
    // 仮実装: とりあえず左壁だけ動くようにする（例示）
    if (m_pos.x - m_radius < xMin) {
        m_pos.x = xMin + m_radius;
        m_vel.x = -m_vel.x;
    }
    // ここでまず「左壁に当たるケース」のテストが通るようにする
    // 他のケースは暫定的に何もしない
}
```

* **手順**

  1. 左壁ケースのテストだけ通るように `if` ブロックを追加
  2. `ctest` 実行 → 左壁のテストは通るが、他３つは失敗のまま
* **次に**

  ```cpp
  if (m_pos.x - m_radius < xMin) {
    ...
  } else if (m_pos.x + m_radius > xMax) {
    m_pos.x = xMax - m_radius;
    m_vel.x = -m_vel.x;
  }
  ```

  を追加して右壁ケースも通す
* **最終的に**

  ```cpp
  if (m_pos.x - m_radius < xMin) { ... }
  else if (m_pos.x + m_radius > xMax) { ... }

  if (m_pos.y - m_radius < yMin) { ... }
  else if (m_pos.y + m_radius > yMax) { ... }
  ```

  とし、すべてのテストを通す
* **ポイント**

  * **テストを一つずつ緑にする** → 赤いテストが残っていてもよい
  * 最低限の条件分岐を段階的に足していき、「少しずつテスト数を減らしていく」

---

## 5. REFACTOR フェーズ：実装の整理

* GREEN フェーズで「まずは実装した」が、可読性や重複が気になる場合、ここでリファクタ
* たとえば、以下のように整理できる

  ```cpp
  void Ball::reflect(double xMin, double xMax, double yMin, double yMax) {
      // X方向
      if (m_pos.x - m_radius < xMin) {
          m_pos.x = xMin + m_radius;
          m_vel.x = -m_vel.x;
      }
      else if (m_pos.x + m_radius > xMax) {
          m_pos.x = xMax - m_radius;
          m_vel.x = -m_vel.x;
      }
      // Y方向
      if (m_pos.y - m_radius < yMin) {
          m_pos.y = yMin + m_radius;
          m_vel.y = -m_vel.y;
      }
      else if (m_pos.y + m_radius > yMax) {
          m_pos.y = yMax - m_radius;
          m_vel.y = -m_vel.y;
      }
  }
  ```
* **意識すべき点**

  * テーブル境界チェックごとに `if-else` が重複しているように見えるが、
    「X方向」「Y方向」で分けることで読みやすさを確保
  * テストはすべて通ったまま（GREEN の状態を維持）
  * さらにメンバ変数名や処理順を変える場合も、テスト結果に影響がないことを確認する

---

## 6. 追加テスト・境界ケース例

* **端ぴったりケース**

  ```cpp
  TEST_CASE("reflect: 端ぴったりタッチは反射しない", "[reflect]") {
      Ball b(1.0, 5.0, -2.0, 0.0, 1.0);
      // 位置: x = r = 1.0、壁ちょうど
      b.reflect(0.0, 10.0, 0.0, 10.0);
      // はみ出しなし → 位置・速度ともに変化なし
      REQUIRE(b.position().x == Catch::Approx(1.0));
      REQUIRE(b.velocity().x == Catch::Approx(-2.0));
  }
  ```
* **極端にはみ出すケース**

  ```cpp
  TEST_CASE("reflect: 極端にはみ出した場合", "[reflect]") {
      Ball b(-5.0, 5.0, -1.0, 0.0, 1.0);
      b.reflect(0.0, 10.0, 0.0, 10.0);
      // 中心 x は -5.0 - 1.0 = -6.0 はみ出し → 補正後 x=1.0
      REQUIRE(b.position().x == Catch::Approx(1.0));
      // 速度反転
      REQUIRE(b.velocity().x == Catch::Approx( 1.0));
  }
  ```
* **上下同時にはみ出すケース (抑制的実装では発生しない想定)**

  * 通常の `if-else` ロジックで上下両方にはみ出すときは、
    「Y方向の補正」が最後に行われるため、X は最初に修正され、次に Y も修正される
  * 重要なのは、**テストで想定した挙動**に合致するか確認すること

---

## 7. まとめ

1. **RED**: まずテストケースを書き、全て失敗を確認
2. **GREEN**: テストが通るための最小限の実装を少しずつ追加
3. **REFRACTOR**: コードの可読性や重複を排除し、品質を向上
4. **追加テスト**: 「端ぴったり」「極端にはみ出す」など境界ケースを増やす → RED→GREEN→REFRACTOR

* このサイクルを繰り返すことで、**常に動く状態**を保ちながら安全に `reflect` の機能を拡充できる
* 次回は「`applySpinDecay`」「`applyFriction`」など、
  さらに “計算ロジックあり” の関数にTDDを適用してみましょう

---

## ご清聴ありがとうございました

* 質問やフィードバックがあればぜひ共有ください！
