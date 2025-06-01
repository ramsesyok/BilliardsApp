#pragma once

#include <cmath>

// 2Dベクトル
struct Vec2
{
    double x{0}, y{0};
    Vec2() = default;
    Vec2(double x_, double y_) : x(x_), y(y_) {}
    Vec2 operator+(const Vec2 &o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2 &o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(double s) const { return {x * s, y * s}; }
    Vec2 &operator+=(const Vec2 &o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }
    Vec2 &operator-=(const Vec2 &o)
    {
        x -= o.x;
        y -= o.y;
        return *this;
    }
    double dot(const Vec2 &o) const { return x * o.x + y * o.y; }
    double length() const { return std::sqrt(x * x + y * y); }
    Vec2 normalized() const
    {
        double len = length();
        return len > 1e-9 ? Vec2{x / len, y / len} : Vec2{0, 0};
    }
};

class Ball
{
public:
    Ball(double x, double y,
         double vx, double vy,
         double r,
         double spin = 0.0);

    // 衝突判定
    bool isColliding(const Ball &other) const;

    // 同質量・完全弾性衝突応答
    void resolveCollision(Ball &other);

    // テーブル反射 (長方形)
    void reflect(double xMin, double xMax,
                 double yMin, double yMax);

    // 滑動摩擦による速度減衰 (μ, 重力加速度 g=9.81m/s²)
    void applyFriction(double frictionCoeff, double dt);

    // マグナス効果 (spin × velocity)
    void applyMagnus(double magnusCoeff, double dt);

    // スピン減衰
    void applySpinDecay(double decayRate, double dt);

    // 1ステップまとめて更新
    void update(double dt,
                double frictionCoeff,
                double magnusCoeff,
                double spinDecay,
                double xMin, double xMax,
                double yMin, double yMax);

    // ゲッター
    Vec2 position() const { return m_pos; }
    Vec2 velocity() const { return m_vel; }
    double spin() const { return m_spin; }
    double radius() const { return m_radius; }

private:
    Vec2 m_pos;
    Vec2 m_vel;
    double m_radius;
    double m_spin;
};
