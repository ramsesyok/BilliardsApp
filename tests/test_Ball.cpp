// test_Ball.cpp

#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"
#include "Ball.hpp"

// Vec2 は Ball.hpp で定義されています

TEST_CASE("Collision: Two balls exchange velocities (same mass, elastic)", "[collision]")
{
    // 2つのボールを水平に衝突させる
    double r = 0.5;
    Ball a(0.0, 0.0, 1.0, 0.0, r);
    Ball b(1.0, 0.0, -1.0, 0.0, r);

    REQUIRE(a.isColliding(b)); // 初期状態で衝突している
    a.resolveCollision(b);

    // 完全弾性衝突・同質量の場合は速度が入れ替わる
    REQUIRE(a.velocity().x == Catch::Approx(-1.0));
    REQUIRE(a.velocity().y == Catch::Approx(0.0));
    REQUIRE(b.velocity().x == Catch::Approx(1.0));
    REQUIRE(b.velocity().y == Catch::Approx(0.0));
}

TEST_CASE("Reflect off each wall correctly", "[reflect]")
{
    double r = 0.5;
    // 左壁で反射
    Ball left(0.4, 5.0, -2.0, 0.0, r);
    left.reflect(0.0, 10.0, 0.0, 10.0);
    REQUIRE(left.position().x == Catch::Approx(0.5)); // 壁からはみ出した分だけ補正
    REQUIRE(left.velocity().x == Catch::Approx(2.0)); // x方向速度が反転

    // 右壁で反射
    Ball right(9.6, 5.0, 2.0, 0.0, r);
    right.reflect(0.0, 10.0, 0.0, 10.0);
    REQUIRE(right.position().x == Catch::Approx(9.5));  // 補正後の位置
    REQUIRE(right.velocity().x == Catch::Approx(-2.0)); // x方向速度が反転

    // 下壁で反射
    Ball bottom(5.0, 0.4, 0.0, -3.0, r);
    bottom.reflect(0.0, 10.0, 0.0, 10.0);
    REQUIRE(bottom.position().y == Catch::Approx(0.5)); // 補正後の位置
    REQUIRE(bottom.velocity().y == Catch::Approx(3.0)); // y方向速度が反転

    // 上壁で反射
    Ball top(5.0, 9.6, 0.0, 3.0, r);
    top.reflect(0.0, 10.0, 0.0, 10.0);
    REQUIRE(top.position().y == Catch::Approx(9.5));  // 補正後の位置
    REQUIRE(top.velocity().y == Catch::Approx(-3.0)); // y方向速度が反転
}

TEST_CASE("applyFriction: Ball slows to rest", "[friction]")
{
    double r = 0.5;
    Ball b(5.0, 5.0, 2.0, 0.0, r);
    // 摩擦係数を大きめ、短いステップで速度を減衰させる
    b.applyFriction(10.0, 0.1); // μ=10, dt=0.1s
    // a = -μg = -10 * 9.81 ≈ -98.1 m/s^2 なので dt=0.1 で Δv ≈ -9.81 m/s → v が負になるためゼロになる
    REQUIRE(b.velocity().x == Catch::Approx(0.0));
    REQUIRE(b.velocity().y == Catch::Approx(0.0));
}

TEST_CASE("applyMagnus: Spin introduces perpendicular acceleration", "[magnus]")
{
    double r = 0.5;
    // 右向きに1m/sで動いてスピンを持つボール
    Ball b(5.0, 5.0, 1.0, 0.0, r, /*spin=*/10.0);
    Vec2 beforeVel = b.velocity();
    b.applyMagnus(0.01, 0.1);
    Vec2 afterVel = b.velocity();
    // x速度はほぼ同じか少し減少／増加するが、y方向速度はプラスかマイナスがつく
    REQUIRE(afterVel.x == Catch::Approx(beforeVel.x + 0.0).margin(1e-6));
    REQUIRE(std::abs(afterVel.y) > 0.0);
}

TEST_CASE("applySpinDecay: Spin decays exponentially", "[spin]")
{
    double r = 0.5;
    Ball b(0.0, 0.0, 0.0, 0.0, r, 5.0);
    b.applySpinDecay(1.0, 1.0);
    REQUIRE(b.spin() == Catch::Approx(5.0 * std::exp(-1.0)));
}

TEST_CASE("update: Combined physics step", "[update]")
{
    double r = 0.5;
    // 反射・摩擦・マグナス・スピンを含めた update() の挙動検証
    Ball b(1.0, 1.0, 2.0, 0.0, r, 10.0);

    // テーブル境界は x∈[0,2], y∈[0,2] と仮定
    double xMin = 0.0, xMax = 2.0;
    double yMin = 0.0, yMax = 2.0;
    double μ = 0.1;
    double k = 0.01;
    double sd = 0.5;
    // dt 小：位置はほぼ直線移動しつつ摩擦＋マグナスでわずかに変化
    b.update(0.01, μ, k, sd, xMin, xMax, yMin, yMax);
    Vec2 v1 = b.velocity();
    Vec2 p1 = b.position();
    // x方向に少し減速しているはず
    REQUIRE(v1.x < 2.0);
    // y方向にもマグナスでわずかな速度がついている
    REQUIRE(std::abs(v1.y) > 0.0);
    // 位置は速度に基づいて微増
    REQUIRE(p1.x > 1.0);
    REQUIRE(p1.y == Catch::Approx(1.0 + v1.y * 0.01).margin(1e-6));

    // 次に反射テスト：テーブル右端に向かって移動させる
    Ball c(1.9, 1.0, 5.0, 0.0, r, 0.0);
    c.update(0.1, 0.0, 0.0, 0.0, xMin, xMax, yMin, yMax);
    // 1.9 + 5.0*0.1 = 2.4 で壁を越える → 反射後は右向き速度が負に
    REQUIRE(c.position().x == Catch::Approx(2.0 - r));
    REQUIRE(c.velocity().x < 0.0);
}
