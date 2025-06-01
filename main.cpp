// main.cpp

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "PhysicsEngine.hpp"
#include <vector>
#include <cmath>

class BilliardsApp : public olc::PixelGameEngine
{
public:
    BilliardsApp()
    {
        sAppName = "9-Ball Simulation (PhysicsEngine + olcPGE)";
    }

private:
    // PhysicsEngine インスタンス
    PhysicsEngine engine{
        2.7432, 1.3716, // テーブル幅 (m), テーブル高さ (m)
        0.02,           // 摩擦係数 μ
        0.0005,         // マグナス係数
        1.0             // スピン減衰率
    };

    // 描画用カラー（インデックスに対応して 0: 手球, 1～9: 的球）
    std::vector<olc::Pixel> renderColors;

public:
    bool OnUserCreate() override
    {
        // ─── Ball の配置設定 ─────────────────────
        // 手球の初期位置・速度・スピン
        Vec2 cuePos{engine.GetTableWidth() * 0.2,
                    engine.GetTableHeight() * 0.5};
        Vec2 cueVel{3.0, 1.0}; // 3.0 m/s, 1.0 m/s
        double cueSpin = 30.0; // 初期スピン

        // ラックの中心
        Vec2 rackCenter{engine.GetTableWidth() * 0.8,
                        engine.GetTableHeight() * 0.5};
        double r = 0.028575; // ボール半径 (m)

        // ダイヤ型オフセット (m単位) 1～9番
        double rowH = r * std::sqrt(3.0);
        std::vector<Vec2> rackOffsets = {
            {0.0, 0.0},         // 1番
            {-r, rowH},         // 2番
            {r, rowH},          // 3番
            {-2 * r, 2 * rowH}, // 4番
            {0.0, 2 * rowH},    // 5番
            {2 * r, 2 * rowH},  // 6番
            {-r, 3 * rowH},     // 7番
            {r, 3 * rowH},      // 8番
            {0.0, 4 * rowH}     // 9番
        };

        // 初期化関数を呼び出して Ball を生成
        engine.InitializeBalls(cuePos, cueVel, cueSpin,
                               rackCenter, rackOffsets, r);

        // ─── レンダリング用カラー設定 ─────────────
        // Ball インスタンスは 1 (手球) + 9 (的球) = 10 個
        renderColors.resize(engine.GetBalls().size());
        for (size_t i = 0; i < renderColors.size(); ++i)
        {
            if (i == 0)
            {
                renderColors[i] = olc::WHITE; // 手球
            }
            else
            {
                // 1～9番的球の公式カラー
                switch (i)
                {
                case 1:
                    renderColors[i] = olc::YELLOW;
                    break; // 1番：黄
                case 2:
                    renderColors[i] = olc::BLUE;
                    break; // 2番：青
                case 3:
                    renderColors[i] = olc::RED;
                    break; // 3番：赤
                case 4:
                    renderColors[i] = olc::MAGENTA;
                    break; // 4番：紫
                case 5:
                    renderColors[i] = olc::Pixel(255, 165, 0);
                    break; // 5番：オレンジ
                case 6:
                    renderColors[i] = olc::GREEN;
                    break; // 6番：緑
                case 7:
                    renderColors[i] = olc::Pixel(150, 75, 0);
                    break; // 7番：ブラウン
                case 8:
                    renderColors[i] = olc::BLACK;
                    break; // 8番：黒
                case 9:
                    renderColors[i] = olc::GREY;
                    break; // 9番：灰色
                default:
                    renderColors[i] = olc::WHITE;
                    break;
                }
            }
        }

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // ─── 物理演算アップデート ───────────────────
        engine.Update(fElapsedTime);

        // ─── 描画 ─────────────────────────────────
        Clear(olc::DARK_GREEN);

        int sw = ScreenWidth();
        int sh = ScreenHeight();

        // テーブル境界を画面全体に描画
        DrawRect(0, 0, sw, sh, olc::WHITE);

        // 物理座標 [m] → 画面座標 [px] の変換係数
        float sx = float(sw) / float(engine.GetTableWidth());
        float sy = float(sh) / float(engine.GetTableHeight());

        // 各ボールを描画
        const auto &balls = engine.GetBalls();
        for (size_t i = 0; i < balls.size(); ++i)
        {
            Vec2 pos = balls[i].position();                                        // (m)
            float px = static_cast<float>(pos.x) * sx;                             // (px)
            float py = static_cast<float>(pos.y) * sy;                             // (px)
            float pr = static_cast<float>(balls[i].radius()) * ((sx + sy) * 0.5f); // (px)

            // カラーは renderColors[i]
            FillCircle(int(px), int(py), int(pr), renderColors[i]);
        }

        return true;
    }
};

int main()
{
    BilliardsApp demo;
    // 800×400 ピクセル、各ピクセル 2×2 倍で拡大表示
    if (demo.Construct(800, 400, 2, 2))
        demo.Start();
    return 0;
}
