// Animation.cpp
#include "Animation.h"
#include <algorithm>
#include <comdef.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// 在文件顶部添加
using namespace Gdiplus;

// 构造函数
Animation::Animation()
    : width_(0), height_(0), type_(BLOCKING),
    duration_(1000), frameDuration_(100) {
}

// 初始化动画
void Animation::init(const std::vector<std::wstring>& framePaths,
    int width, int height,
    AnimationType type,
    DWORD duration,
    DWORD frameDuration)
{
    framePaths_ = framePaths;
    width_ = width;
    height_ = height;
    type_ = type;
    duration_ = duration;
    frameDuration_ = frameDuration;
    frames_.resize(framePaths_.size());
}

// 加载动画帧
bool Animation::loadFrames() {
    for (size_t i = 0; i < framePaths_.size(); i++) {
        // 使用临时变量检查加载结果
        IMAGE temp;
        loadimage(&temp, framePaths_[i].c_str(), width_, height_,true);

        // 检查是否加载成功
        if (temp.getwidth() == 0 || temp.getheight() == 0) {
            return false;
        }

        frames_[i] = temp;
    }
    return true;
}

// 播放动画
void Animation::play(int startX, int startY, int endX, int endY,
    PathFunction pathFunc,
    DWORD rop)
{
    isPlaying_ = true;
    DWORD startTime = GetTickCount();

    while (isPlaying_) {
        DWORD elapsed = GetTickCount() - startTime;
        float progress = min(1.0f, static_cast<float>(elapsed) / duration_);

        // 计算当前位置
        int currentX = startX;
        int currentY = startY;

        if (pathFunc) {
            pathFunc(progress, currentX, currentY, startX, startY, endX, endY);
        }
        else {
            PathFunctions::linear(progress, currentX, currentY, startX, startY, endX, endY);
        }

        // 计算当前帧
        int frameIndex = min(static_cast<int>(frames_.size()) - 1,
            static_cast<int>(elapsed / frameDuration_));

        // 绘制动画帧
        if (alpha_ == 255) {
            // 完全不透明
            putimage(currentX, currentY, width_, height_,
                &frames_[frameIndex], 0, 0, rop);
        }
        else {
            // 使用 GDI+ 实现透明度
            HDC hdc = GetImageHDC();
            Graphics graphics(hdc);

            // 创建 GDI+ 位图
            Bitmap bmp(width_, height_, PixelFormat32bppARGB);
            BitmapData bmpData;
            Rect rect(0, 0, width_, height_);
            bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

            // 从 EasyX IMAGE 复制数据
            BYTE* src = (BYTE*)GetImageBuffer(&frames_[frameIndex]);
            int srcPitch = width_ * 4; // 32位图像每行字节数
            BYTE* dst = (BYTE*)bmpData.Scan0;
            for (int y = 0; y < height_; y++) {
                for (int x = 0; x < width_; x++) {
                    int idx = y * srcPitch + x * 4;

                    // 直接获取BGRA分量（EasyX使用BGRA格式）
                    BYTE b = src[idx];
                    BYTE g = src[idx + 1];
                    BYTE r = src[idx + 2];
                    BYTE originalAlpha = src[idx + 3];

                    // 计算实际透明度：结合设置的alpha和原始透明度
                    BYTE a = (originalAlpha * alpha_) / 255;

                    // 保持BGRA顺序写入目标位图
                    dst[0] = b;
                    dst[1] = g;
                    dst[2] = r;
                    dst[3] = a;

                    dst += 4;
                }
                // 处理目标位图的行填充
                dst += bmpData.Stride - width_ * 4;
            }
            bmp.UnlockBits(&bmpData);

            // 绘制到屏幕
            graphics.DrawImage(&bmp, currentX, currentY);
        }

        FlushBatchDraw();

        // 检查动画结束
        if (elapsed >= duration_) {
            isPlaying_ = false;
        }

        // 允许退出
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            closegraph();
            exit(0);
        }

        Sleep(10); // 减少CPU占用
    }
}

// 检查是否正在播放
bool Animation::isPlaying() const {
    return isPlaying_;
}

// 获取动画类型
Animation::AnimationType Animation::getType() const {
    return type_;
}

// ================= 路径函数实现 =================

void PathFunctions::linear(float progress, int& x, int& y,
    int startX, int startY, int endX, int endY) {
    x = startX + static_cast<int>((endX - startX) * progress);
    y = startY + static_cast<int>((endY - startY) * progress);
}

void PathFunctions::parabolic(float progress, int& x, int& y,
    int startX, int startY, int endX, int endY) {
    x = startX + static_cast<int>((endX - startX) * progress);
    // 抛物线方程 y = a*x^2，这里简化计算
    float height = 100.0f; // 抛物线高度
    float normalizedProgress = progress * 2 - 1; // -1到1
    y = startY + static_cast<int>((endY - startY) * progress - height * normalizedProgress * normalizedProgress);
}

void PathFunctions::circular(float progress, int& x, int& y,
    int centerX, int centerY, int radius) {
    float angle = progress * 2 * 3.14159f;
    x = centerX + static_cast<int>(cos(angle) * radius);
    y = centerY + static_cast<int>(sin(angle) * radius);
}

void PathFunctions::heartbeat(float progress, int& x, int& y,
    int centerX, int centerY) {
    x = centerX;
    // 心跳波形：两个脉冲
    float pulse = 0;
    if (progress < 0.3f) {
        pulse = sin(progress * 10 * 3.14159f) * 20;
    }
    else if (progress < 0.7f) {
        pulse = sin((progress - 0.3f) * 7 * 3.14159f) * 15;
    }
    y = centerY + static_cast<int>(pulse);
}

void PathFunctions::shake(float progress, int& x, int& y,
    int centerX, int centerY, int intensity) {
    x = centerX + (rand() % (2 * intensity) - intensity);
    y = centerY + (rand() % (2 * intensity) - intensity);
}