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
        loadimage(&temp, framePaths_[i].c_str(), width_, height_, true);

        // 检查是否加载成功
        if (temp.getwidth() == 0 || temp.getheight() == 0) {
            return false;
        }

        frames_[i] = temp;
    }
    return true;
}

// 播放动画（阻塞动画专用）
void Animation::play(int startX, int startY, int endX, int endY,
    PathFunction pathFunc,
    DWORD rop)
{
    isPlaying_ = true;
    isStaying_ = false; // 重置停留状态
    DWORD startTime = GetTickCount();

    // === 保存背景区域 ===
    IMAGE background;
    int saveWidth = width_ + 20;  // 稍大一些防止边缘残留
    int saveHeight = height_ + 20;
    Resize(&background, saveWidth, saveHeight);

    // 保存起始位置的背景
    getimage(&background,
        startX - 10, startY - 10,
        saveWidth, saveHeight);

    int lastX = startX;
    int lastY = startY;

    while (isPlaying_) {
        DWORD elapsed = GetTickCount() - startTime;
        float progress = min(1.0f, static_cast<float>(elapsed) / duration_);

        // === 检查是否进入停留阶段 ===
        if (progress >= 1.0f && stayDuration_ > 0 && !isStaying_) {
            isStaying_ = true;
            stayStartTime_ = GetTickCount(); // 记录停留开始时间
        }

        // 计算当前位置
        int currentX = startX;
        int currentY = startY;

        // 如果不是停留状态，更新位置
        if (!isStaying_) {
            if (pathFunc) {
                pathFunc(progress, currentX, currentY, startX, startY, endX, endY);
            }
            else {
                PathFunctions::linear(progress, currentX, currentY, startX, startY, endX, endY);
            }
        }
        else {
            // 停留状态，保持在终点位置
            currentX = endX;
            currentY = endY;
        }

        // === 恢复上一帧背景 ===
        putimage(lastX - 10, lastY - 10, &background);

        // 更新背景为当前位置
        getimage(&background,
            currentX - 10, currentY - 10,
            saveWidth, saveHeight);
        // =========================

        lastX = currentX;
        lastY = currentY;

        // 计算当前帧
        int frameIndex;
        if (!isStaying_) {
            frameIndex = min(static_cast<int>(frames_.size()) - 1,
                static_cast<int>(elapsed / frameDuration_));
        }
        else {
            // 停留状态，使用最后一帧
            frameIndex = frames_.size() - 1;
        }

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

                    BYTE originalAlpha = src[idx + 3];
                    if (originalAlpha == 0) {  // 完全透明的像素跳过
                        dst += 4;
                        continue;
                    }

                    BYTE b = src[idx];
                    BYTE g = src[idx + 1];
                    BYTE r = src[idx + 2];
                    BYTE a = (originalAlpha * alpha_) / 255;

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

        // == = 检查动画结束 == =
            if (isStaying_) {
                // 停留阶段，检查停留时间是否结束
                if (GetTickCount() - stayStartTime_ >= stayDuration_) {
                    isPlaying_ = false;
                }
            }
        // 检查动画结束（非停留状态）
            else if (elapsed >= duration_) {
                // 如果有停留时间，进入停留状态而不是立即结束
                if (stayDuration_ > 0) {
                    isStaying_ = true;
                    stayStartTime_ = GetTickCount();
                }
                else {
                    isPlaying_ = false;
                }
            }

        // 允许退出
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            closegraph();
            exit(0);
        }

        Sleep(0);
        // Sleep(max(1, static_cast<int>(frameDuration_ - (GetTickCount() - startTime - elapsed))));
    }
}

// 开始非阻塞动画
void Animation::startNonBlocking(int startX, int startY, int endX, int endY,
    PathFunction pathFunc,
    DWORD rop)
{
    isPlaying_ = true;
    isStaying_ = false;
    startTime_ = GetTickCount();
    startX_ = startX;
    startY_ = startY;
    endX_ = endX;
    endY_ = endY;
    pathFunc_ = pathFunc;
    rop_ = rop;
    lastX_ = startX;
    lastY_ = startY;

    // 保存背景
    saveWidth_ = width_ + 20;
    saveHeight_ = height_ + 20;
    Resize(&background_, saveWidth_, saveHeight_);
    getimage(&background_, startX - 10, startY - 10, saveWidth_, saveHeight_);
}

// 更新非阻塞动画
bool Animation::updateNonBlocking() {
    if (!isPlaying_) return false;

    DWORD elapsed = GetTickCount() - startTime_;
    float progress = min(1.0f, static_cast<float>(elapsed) / duration_);

    // 检查是否进入停留阶段
    if (progress >= 1.0f && stayDuration_ > 0 && !isStaying_) {
        isStaying_ = true;
        stayStartTime_ = GetTickCount();
    }

    // 计算当前位置
    int currentX = startX_;
    int currentY = startY_;

    if (!isStaying_) {
        if (pathFunc_) {
            pathFunc_(progress, currentX, currentY, startX_, startY_, endX_, endY_);
        }
        else {
            PathFunctions::linear(progress, currentX, currentY, startX_, startY_, endX_, endY_);
        }
    }
    else {
        currentX = endX_;
        currentY = endY_;
    }

    // === 删除背景恢复和保存操作 ===

    // 计算当前帧
    int frameIndex;
    if (!isStaying_) {
        frameIndex = min(static_cast<int>(frames_.size()) - 1,
            static_cast<int>(elapsed / frameDuration_));
    }
    else {
        frameIndex = frames_.size() - 1;
    }

    // 绘制动画帧（带透明度）
    if (alpha_ == 255) {
        putimage(currentX, currentY, width_, height_,
            &frames_[frameIndex], 0, 0, rop_);
    }
    else {
        // ... [保持原有的透明度绘制代码] ...
    }

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

            BYTE originalAlpha = src[idx + 3];
            if (originalAlpha == 0) {  // 完全透明的像素跳过
                dst += 4;
                continue;
            }

            BYTE b = src[idx];
            BYTE g = src[idx + 1];
            BYTE r = src[idx + 2];
            BYTE a = (originalAlpha * alpha_) / 255;

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

    // 检查动画结束
    if (isStaying_) {
        if (GetTickCount() - stayStartTime_ >= stayDuration_) {
            isPlaying_ = false;
            return false;
        }
    }
    else if (elapsed >= duration_) {
        if (stayDuration_ > 0) {
            isStaying_ = true;
            stayStartTime_ = GetTickCount();
        }
        else {
            isPlaying_ = false;
            return false;
        }
    }

    return true;
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
