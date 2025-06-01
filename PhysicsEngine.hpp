#pragma once

#include <vector>
#include "Ball.hpp"

class PhysicsEngine
{
public:
    /// コンストラクタ
    /// テーブルサイズ（m単位）と物理パラメータを引数で受け取る
    PhysicsEngine(double tableWidth, double tableHeight,
                  double frictionCoeff, double magnusCoeff, double spinDecay);

    /// シミュレーション中の全ボールを初期化する
    /// - cueBallPos, cueBallVel, cueBallSpin は手球の初期位置・速度・スピン
    /// - rackCenter: ラックを作る中心位置 (m単位)
    /// - rackOffsets: ラックを構成するボールの相対オフセット (m単位) のリスト
    /// - ballRadius: すべてのボールの半径 (m単位)
    void InitializeBalls(const Vec2 &cueBallPos,
                         const Vec2 &cueBallVel,
                         double cueBallSpin,
                         const Vec2 &rackCenter,
                         const std::vector<Vec2> &rackOffsets,
                         double ballRadius);

    /// 1ステップ分だけ物理演算を進める
    /// - dt: タイムステップ（秒）
    void Update(double dt);

    /// 現在のすべての Ball オブジェクトの参照を返す（描画用に使う）
    const std::vector<Ball> &GetBalls() const;

    /// テーブル幅（m単位） を返す
    double GetTableWidth() const;

    /// テーブル高さ（m単位） を返す
    double GetTableHeight() const;

private:
    // テーブル境界（物理単位：メートル）
    double m_xMin = 0.0;
    double m_xMax = 0.0;
    double m_yMin = 0.0;
    double m_yMax = 0.0;

    // 物理パラメータ
    double m_friction = 0.0;  // 摩擦係数 μ
    double m_magnus = 0.0;    // マグナス係数
    double m_spinDecay = 0.0; // スピン減衰率

    // シミュレーション対象のボール群
    std::vector<Ball> m_balls;

    /// 全ボールに対して衝突判定＋応答を行う
    void ResolveAllCollisions();

    /// 単一ボールに対して摩擦・マグナス・スピン減衰・反射を適用し、位置を更新する
    /// - dt: タイムステップ（秒）
    void UpdateBallPhysics(Ball &ball, double dt);
};
