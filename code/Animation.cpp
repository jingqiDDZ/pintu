// Animation.cpp
#include "Animation.h"
#include <algorithm>
#include <comdef.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// ���ļ��������
using namespace Gdiplus;



// ���캯��
Animation::Animation()
    : width_(0), height_(0), type_(BLOCKING),
    duration_(1000), frameDuration_(100) {
}

// ��ʼ������
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

// ���ض���֡
bool Animation::loadFrames() {
    for (size_t i = 0; i < framePaths_.size(); i++) {
        // ʹ����ʱ���������ؽ��
        IMAGE temp;
        loadimage(&temp, framePaths_[i].c_str(), width_, height_, true);

        // ����Ƿ���سɹ�
        if (temp.getwidth() == 0 || temp.getheight() == 0) {
            return false;
        }

        frames_[i] = temp;
    }
    return true;
}

// ���Ŷ�������������ר�ã�
void Animation::play(int startX, int startY, int endX, int endY,
    PathFunction pathFunc,
    DWORD rop)
{
    isPlaying_ = true;
    isStaying_ = false; // ����ͣ��״̬
    DWORD startTime = GetTickCount();

    // === ���汳������ ===
    IMAGE background;
    int saveWidth = width_ + 20;  // �Դ�һЩ��ֹ��Ե����
    int saveHeight = height_ + 20;
    Resize(&background, saveWidth, saveHeight);

    // ������ʼλ�õı���
    getimage(&background,
        startX - 10, startY - 10,
        saveWidth, saveHeight);

    int lastX = startX;
    int lastY = startY;

    while (isPlaying_) {
        DWORD elapsed = GetTickCount() - startTime;
        float progress = min(1.0f, static_cast<float>(elapsed) / duration_);

        // === ����Ƿ����ͣ���׶� ===
        if (progress >= 1.0f && stayDuration_ > 0 && !isStaying_) {
            isStaying_ = true;
            stayStartTime_ = GetTickCount(); // ��¼ͣ����ʼʱ��
        }

        // ���㵱ǰλ��
        int currentX = startX;
        int currentY = startY;

        // �������ͣ��״̬������λ��
        if (!isStaying_) {
            if (pathFunc) {
                pathFunc(progress, currentX, currentY, startX, startY, endX, endY);
            }
            else {
                PathFunctions::linear(progress, currentX, currentY, startX, startY, endX, endY);
            }
        }
        else {
            // ͣ��״̬���������յ�λ��
            currentX = endX;
            currentY = endY;
        }

        // === �ָ���һ֡���� ===
        putimage(lastX - 10, lastY - 10, &background);

        // ���±���Ϊ��ǰλ��
        getimage(&background,
            currentX - 10, currentY - 10,
            saveWidth, saveHeight);
        // =========================

        lastX = currentX;
        lastY = currentY;

        // ���㵱ǰ֡
        int frameIndex;
        if (!isStaying_) {
            frameIndex = min(static_cast<int>(frames_.size()) - 1,
                static_cast<int>(elapsed / frameDuration_));
        }
        else {
            // ͣ��״̬��ʹ�����һ֡
            frameIndex = frames_.size() - 1;
        }

        // ���ƶ���֡
        if (alpha_ == 255) {
            // ��ȫ��͸��
            putimage(currentX, currentY, width_, height_,
                &frames_[frameIndex], 0, 0, rop);
        }
        else {
            // ʹ�� GDI+ ʵ��͸����
            HDC hdc = GetImageHDC();
            Graphics graphics(hdc);

            // ���� GDI+ λͼ
            Bitmap bmp(width_, height_, PixelFormat32bppARGB);
            BitmapData bmpData;
            Rect rect(0, 0, width_, height_);
            bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

            // �� EasyX IMAGE ��������
            BYTE* src = (BYTE*)GetImageBuffer(&frames_[frameIndex]);
            int srcPitch = width_ * 4; // 32λͼ��ÿ���ֽ���
            BYTE* dst = (BYTE*)bmpData.Scan0;
            for (int y = 0; y < height_; y++) {
                for (int x = 0; x < width_; x++) {
                    int idx = y * srcPitch + x * 4;

                    BYTE originalAlpha = src[idx + 3];
                    if (originalAlpha == 0) {  // ��ȫ͸������������
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
                // ����Ŀ��λͼ�������
                dst += bmpData.Stride - width_ * 4;
            }
            bmp.UnlockBits(&bmpData);

            // ���Ƶ���Ļ
            graphics.DrawImage(&bmp, currentX, currentY);
        }

        FlushBatchDraw();

        // == = ��鶯������ == =
            if (isStaying_) {
                // ͣ���׶Σ����ͣ��ʱ���Ƿ����
                if (GetTickCount() - stayStartTime_ >= stayDuration_) {
                    isPlaying_ = false;
                }
            }
        // ��鶯����������ͣ��״̬��
            else if (elapsed >= duration_) {
                // �����ͣ��ʱ�䣬����ͣ��״̬��������������
                if (stayDuration_ > 0) {
                    isStaying_ = true;
                    stayStartTime_ = GetTickCount();
                }
                else {
                    isPlaying_ = false;
                }
            }

        // �����˳�
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            closegraph();
            exit(0);
        }

        Sleep(0);
        // Sleep(max(1, static_cast<int>(frameDuration_ - (GetTickCount() - startTime - elapsed))));
    }
}

// ��ʼ����������
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

    // ���汳��
    saveWidth_ = width_ + 20;
    saveHeight_ = height_ + 20;
    Resize(&background_, saveWidth_, saveHeight_);
    getimage(&background_, startX - 10, startY - 10, saveWidth_, saveHeight_);
}

// ���·���������
bool Animation::updateNonBlocking() {
    if (!isPlaying_) return false;

    DWORD elapsed = GetTickCount() - startTime_;
    float progress = min(1.0f, static_cast<float>(elapsed) / duration_);

    // ����Ƿ����ͣ���׶�
    if (progress >= 1.0f && stayDuration_ > 0 && !isStaying_) {
        isStaying_ = true;
        stayStartTime_ = GetTickCount();
    }

    // ���㵱ǰλ��
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

    // === ɾ�������ָ��ͱ������ ===

    // ���㵱ǰ֡
    int frameIndex;
    if (!isStaying_) {
        frameIndex = min(static_cast<int>(frames_.size()) - 1,
            static_cast<int>(elapsed / frameDuration_));
    }
    else {
        frameIndex = frames_.size() - 1;
    }

    // ���ƶ���֡����͸���ȣ�
    if (alpha_ == 255) {
        putimage(currentX, currentY, width_, height_,
            &frames_[frameIndex], 0, 0, rop_);
    }
    else {
        // ... [����ԭ�е�͸���Ȼ��ƴ���] ...
    }

    // ʹ�� GDI+ ʵ��͸����
    HDC hdc = GetImageHDC();
    Graphics graphics(hdc);

    // ���� GDI+ λͼ
    Bitmap bmp(width_, height_, PixelFormat32bppARGB);
    BitmapData bmpData;
    Rect rect(0, 0, width_, height_);
    bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

    // �� EasyX IMAGE ��������
    BYTE* src = (BYTE*)GetImageBuffer(&frames_[frameIndex]);
    int srcPitch = width_ * 4; // 32λͼ��ÿ���ֽ���
    BYTE* dst = (BYTE*)bmpData.Scan0;
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            int idx = y * srcPitch + x * 4;

            BYTE originalAlpha = src[idx + 3];
            if (originalAlpha == 0) {  // ��ȫ͸������������
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
        // ����Ŀ��λͼ�������
        dst += bmpData.Stride - width_ * 4;
    }
    bmp.UnlockBits(&bmpData);

    // ���Ƶ���Ļ
    graphics.DrawImage(&bmp, currentX, currentY);

    // ��鶯������
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


// ����Ƿ����ڲ���
bool Animation::isPlaying() const {
    return isPlaying_;
}

// ��ȡ��������
Animation::AnimationType Animation::getType() const {
    return type_;
}

// ================= ·������ʵ�� =================

void PathFunctions::linear(float progress, int& x, int& y,
    int startX, int startY, int endX, int endY) {
    x = startX + static_cast<int>((endX - startX) * progress);
    y = startY + static_cast<int>((endY - startY) * progress);
}

void PathFunctions::parabolic(float progress, int& x, int& y,
    int startX, int startY, int endX, int endY) {
    x = startX + static_cast<int>((endX - startX) * progress);
    // �����߷��� y = a*x^2������򻯼���
    float height = 100.0f; // �����߸߶�
    float normalizedProgress = progress * 2 - 1; // -1��1
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
    // �������Σ���������
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
