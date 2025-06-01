---
marp: true
---

<!--
theme: default
paginate: true
-->

# テスト駆動開発 (TDD) 入門

---

## 目次

1. TDD の概要と目的
2. 初歩のステップ：`isColliding` 関数

   * 何をテストするか？
   * RED-GREEN-REFACTOR の流れ
   * テストケースの例
3. まとめと次のステップ

---

## 1. TDD の概要と目的

* TDD（Test-Driven Development）は「先にテストを書く」だけでなく
  **常に動くコード（テストが通る状態）を維持しながら少しずつ機能を追加**する手法
* 主なサイクル：

  1. **RED**（テストが失敗する）
  2. **GREEN**（最低限の実装でテストを通す）
  3. **REFACTOR**（リファクタリングして品質向上）
* 学習者には「テスト → 実装 → リファクタリング → テスト → …」を実感してもらうことが最重要

---

## 2. 初歩のステップ：`isColliding` 関数

### 目的・背景

* Ball クラスにおける **衝突判定** を担当するメソッド
* 2 つの円 (Ball) が重なっているかどうかを判定するだけなので、ロジックがシンプル
* 「距離² ≤ (半径₁ + 半径₂)²」という数学的条件さえ覚えれば OK
* 初心者が TDD の流れを体験するには最適

---

## 2.1. `isColliding` のシグネチャ

```cpp
// Ball.hpp 抜粋
class Ball {
public:
    //... 省略 ...

    // 衝突判定
    bool isColliding(const Ball& other) const;

    //... 省略 ...
};
```

* メソッド名：`isColliding`
* 引数：もう一方の `Ball` オブジェクトへの参照
* 返り値：`bool` (`true` → 衝突している、`false` → 衝突していない)

---

## 2.2. 衝突判定ロジック

1. **距離ベクトル** を計算

   $$
   \Delta = \mathbf{pos}_1 - \mathbf{pos}_2
   $$
2. **距離の二乗** を求める

   $$
   \mathrm{dist2} = \Delta.x^2 + \Delta.y^2
   $$
3. **半径和の二乗** を求める

   $$
   r_\text{sum} = r_1 + r_2,\quad r_\text{sum}^2 = (r_1 + r_2)^2
   $$
4. 比較

   * $\mathrm{dist2} \le r_\text{sum}^2$ ⇒ 衝突 (`true`)
   * それ以外 ⇒ 非衝突 (`false`)

---

## 2.3. RED-GREEN-REFACTOR の手順

1. **RED**

   * まだ `isColliding()` を実装していない状態で、テストコードを書いてテストを実行 → 失敗
2. **GREEN**

   * 最低限の実装をしてテストが通るようにする
   * 例：テストケースごとに `return false;` → `return true;` を切り替えながら、最後に汎用的な判定ロジックを入れる
3. **REFACTOR**

   * コードを読みやすく整理し、重複を排除して品質向上
   * テストはそのまま通り続ける

---

## 2.4. テストケース例（赤フェーズ）

```cpp
// tests/test_Ball.cpp (一部抜粋)

TEST_CASE("isColliding: 同位置での衝突", "[collision]") {
    Ball a(0.0, 0.0,  0.0,  0.0, 1.0);
    Ball b(0.0, 0.0,  0.0,  0.0, 1.0);
    REQUIRE(a.isColliding(b) == true);
}

TEST_CASE("isColliding: 半径和ちょうどの距離で衝突", "[collision]") {
    Ball a(0.0, 0.0,  0.0,  0.0, 1.0);
    Ball b(2.0, 0.0,  0.0,  0.0, 1.0);
    // 中心間距離 = 2.0、半径和 = 2.0 → 衝突
    REQUIRE(a.isColliding(b) == true);
}

TEST_CASE("isColliding: 半径和より少し離れている場合", "[collision]") {
    Ball a(0.0, 0.0,  0.0,  0.0, 1.0);
    Ball b(2.001, 0.0, 0.0,  0.0, 1.0);
    // 中心間距離 = 2.001、半径和 = 2.0 → 衝突しない
    REQUIRE(a.isColliding(b) == false);
}
```

* 最初に「いずれも失敗するテスト」を書いて `RED`
* 次に簡易実装を加えて `GREEN` へ持っていく

---

## 2.5. 最低限の実装から RED→GREEN

```cpp
// Ball.cpp の一部（最小限の実装）

bool Ball::isColliding(const Ball& other) const {
    // 仮実装: まずは常に false でテストを通らない状態 (RED)
    // return false;

    // 次に常に true にしてみて、一部のテストだけ通る (暫定 GREEN)
    // return true;

    // 最終的に本実装：
    Vec2 delta = this->m_pos - other.m_pos;
    double dist2 = delta.x * delta.x + delta.y * delta.y;
    double rsum = this->m_radius + other.m_radius;
    return dist2 <= (rsum * rsum);
}
```

* **ステップ 1 (RED)**: `return false;` → すべてのテストが失敗
* **ステップ 2 (GREEN)**: 「同位置」「半径和ちょうど」ぐらいは `true` 返すように暫定で `return true;`
* **ステップ 3 (REFACTOR)**: 実際の判定式を実装し、全テストが安定して通る状態に

---

## 2.6. RED→GREEN フィードバック例

1. **RED**

   ```
   > cmake --build build && ctest  
   …  
   [FAIL] isColliding: 同位置での衝突  
   [FAIL] isColliding: 半径和ちょうどの距離で衝突  
   [FAIL] isColliding: 半径和より少し離れている場合
   ```
2. **GREEN（一時）**

   ```cpp
   bool Ball::isColliding(const Ball& other) const {
       return true;
   }
   ```

   ```
   > ctest  
   [PASS] 同位置での衝突  
   [PASS] 半径和ちょうどの距離  
   [FAIL] 半径和より少し離れている場合
   ```
3. **実装（本番）**
   判定式を入れる

   ```
   > ctest  
   [PASS] 同位置での衝突  
   [PASS] 半径和ちょうどの距離  
   [PASS] 半径和より少し離れている場合
   ```

---

## 2.7. 追加テストケースの例

* **斜めに離れている場合**

  ```cpp
  TEST_CASE("isColliding: 斜め方向", "[collision]") {
      Ball a(0.0, 0.0, 0.0, 0.0, 1.0);
      Ball b(1.0, 1.0, 0.0, 0.0, 1.0);
      // 中心間距離 = √2 ≈ 1.414, 半径和 = 2.0 → 衝突
      REQUIRE(a.isColliding(b) == true);
  }
  ```
* **ゼロ半径での挙動**

  ```cpp
  TEST_CASE("isColliding: どちらかが半径0", "[collision]") {
      Ball a(0.0, 0.0, 0.0, 0.0, 0.0);
      Ball b(0.0, 0.0, 0.0, 0.0, 1.0);
      REQUIRE(a.isColliding(b) == true);
      // aの半径が0だけど中心同じ → 衝突とみなす
  }
  ```

---

## 3. まとめと次のステップ

* **まとめ**

  1. TDD は「常にテストが通る状態」を保ちながらコーディングを進める手法
  2. 最初は `isColliding` のような「単純なロジック」を選ぶことが重要
  3. RED → GREEN → REFACTOR のサイクルを繰り返し、テストケースを徐々に増やしていく
* **次のステップ**

  1. `reflect` 関数を同様に TDD で実装
  2. `applySpinDecay` → `applyFriction` → `applyMagnus` と難易度を上げる
  3. 最後に `update(...)` を統合し、複合的な挙動をテスト

---

## おわりに

* この教材を通じて、**常に動くコードをキープする感覚** を身につける
* 小さい単位でテストを書き、**すぐ実装→動かす→安全にリファクタリング** を体験する
* 次回は「反射（reflect）」を TDD で取り組んでみましょう！

ありがとうございました。
