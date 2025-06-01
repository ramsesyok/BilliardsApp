#include "PhysicsEngine.hpp"
#include <cmath>

// コンストラクタ：テーブル幅・高さと物理パラメータをセット
PhysicsEngine::PhysicsEngine(double tableWidth, double tableHeight,
                             double frictionCoeff, double magnusCoeff, double spinDecay)
    : m_xMin(0.0), m_xMax(tableWidth), m_yMin(0.0), m_yMax(tableHeight), m_friction(frictionCoeff), m_magnus(magnusCoeff), m_spinDecay(spinDecay)
{
}

// ボールの初期化
void PhysicsEngine::InitializeBalls(const Vec2 &cueBallPos,
                                    const Vec2 &cueBallVel,
                                    double cueBallSpin,
                                    const Vec2 &rackCenter,
                                    const std::vector<Vec2> &rackOffsets,
                                    double ballRadius)
{
    m_balls.clear();

    // 1) 手球を作成
    m_balls.emplace_back(
        cueBallPos.x, cueBallPos.y,
        cueBallVel.x, cueBallVel.y,
        ballRadius,
        cueBallSpin);

    // 2) ラックのオフセットに従って的球を作成
    for (const auto &off : rackOffsets)
    {
        m_balls.emplace_back(
            rackCenter.x + off.x,
            rackCenter.y + off.y,
            0.0, 0.0, // 的球は初期速度ゼロ
            ballRadius,
            0.0 // 的球は初期スピンなし
        );
    }
}

// 全ペアの衝突を検出して応答を行う
void PhysicsEngine::ResolveAllCollisions()
{
    size_t N = m_balls.size();
    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = i + 1; j < N; ++j)
        {
            if (m_balls[i].isColliding(m_balls[j]))
            {
                m_balls[i].resolveCollision(m_balls[j]);
            }
        }
    }
}

// 各ボールに摩擦・マグナス・スピン減衰・位置更新・テーブル反射を適用
void PhysicsEngine::UpdateBallPhysics(Ball &ball, double dt)
{
    // Ball::update を内部で呼んでも良いが、
    // 明示的にそれぞれのパラメータを渡しておく形にするとわかりやすい。
    ball.update(
        dt,
        m_friction,
        m_magnus,
        m_spinDecay,
        m_xMin, m_xMax,
        m_yMin, m_yMax);
}

// 物理演算を 1 フレーム分進める
void PhysicsEngine::Update(double dt)
{
    // 1) 全ボール間の衝突判定・応答
    ResolveAllCollisions();

    // 2) 各ボールの摩擦・マグナス・スピン・位置・テーブル反射を適用
    for (auto &b : m_balls)
    {
        UpdateBallPhysics(b, dt);
    }
}

// 現在のボール群を返す
const std::vector<Ball> &PhysicsEngine::GetBalls() const
{
    return m_balls;
}

// テーブル幅を返す
double PhysicsEngine::GetTableWidth() const
{
    return m_xMax - m_xMin;
}

// テーブル高さを返す
double PhysicsEngine::GetTableHeight() const
{
    return m_yMax - m_yMin;
}