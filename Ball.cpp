#include "Ball.hpp"

Ball::Ball(double x, double y,
           double vx, double vy,
           double r,
           double spin)
    : m_pos{x, y}, m_vel{vx, vy}, m_radius(r), m_spin(spin)
{
}

// 衝突判定
bool Ball::isColliding(const Ball &other) const
{
    double rsum = m_radius + other.m_radius;
    return (m_pos - other.m_pos).dot(m_pos - other.m_pos) <= rsum * rsum;
}

// 弾性衝突応答（同質量）
void Ball::resolveCollision(Ball &other)
{
    Vec2 delta = m_pos - other.m_pos;
    double dist2 = delta.dot(delta);
    if (dist2 < 1e-12)
        return; // 完全重なりは無視

    Vec2 dv = m_vel - other.m_vel;
    double fac = dv.dot(delta) / dist2;
    Vec2 impulse = delta * fac;

    m_vel -= impulse;
    other.m_vel += impulse;
}

// テーブル反射
void Ball::reflect(double xMin, double xMax,
                   double yMin, double yMax)
{
    if (m_pos.x - m_radius < xMin)
    {
        m_pos.x = xMin + m_radius;
        m_vel.x = -m_vel.x;
    }
    else if (m_pos.x + m_radius > xMax)
    {
        m_pos.x = xMax - m_radius;
        m_vel.x = -m_vel.x;
    }
    if (m_pos.y - m_radius < yMin)
    {
        m_pos.y = yMin + m_radius;
        m_vel.y = -m_vel.y;
    }
    else if (m_pos.y + m_radius > yMax)
    {
        m_pos.y = yMax - m_radius;
        m_vel.y = -m_vel.y;
    }
}

// 摩擦
void Ball::applyFriction(double μ, double dt)
{
    const double g = 9.81;
    Vec2 dir = m_vel.normalized();
    Vec2 a = dir * (-μ * g);
    m_vel += a * dt;
    // 逆向きなら止める
    if (m_vel.dot(dir) < 0)
        m_vel = {0, 0};
}

// マグナス
void Ball::applyMagnus(double k, double dt)
{
    if (std::abs(m_spin) < 1e-6 || m_vel.length() < 1e-6)
        return;
    Vec2 perp{-m_vel.y, m_vel.x};
    Vec2 a = perp.normalized() * (k * m_spin * m_vel.length());
    m_vel += a * dt;
}

// スピン減衰
void Ball::applySpinDecay(double decay, double dt)
{
    m_spin *= std::exp(-decay * dt);
}

// 1ステップ更新
void Ball::update(double dt,
                  double μ,
                  double k,
                  double spinDecay,
                  double xMin, double xMax,
                  double yMin, double yMax)
{
    applyFriction(μ, dt);
    applyMagnus(k, dt);
    applySpinDecay(spinDecay, dt);
    m_pos += m_vel * dt;
    reflect(xMin, xMax, yMin, yMax);
}
