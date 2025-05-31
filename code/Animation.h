#pragma once
#include <graphics.h>
#include <vector>
#include <functional>
#include <string>

// 动画路径函数类型定义
using PathFunction = std::function<void(float progress, int& x, int& y, int startX, int startY, int endX, int endY)>;

// 动画类
class Animation {
public:
    // 动画类型
    enum AnimationType {
        NON_BLOCKING,   // 非阻塞动画（允许用户操作）
        BLOCKING        // 阻塞动画（禁止用户操作）
    };

    // 构造函数
    Animation();

    // 初始化动画
    void init(const std::vector<std::wstring>& framePaths,
        int width, int height,
        AnimationType type = BLOCKING,
        DWORD duration = 1000,
        DWORD frameDuration = 100);

    // 加载动画帧
    bool loadFrames();

    // 播放动画
    void play(int startX, int startY, int endX, int endY,
        PathFunction pathFunc = nullptr,
        DWORD rop = SRCCOPY);

    // 检查是否正在播放
    bool isPlaying() const;

    // 获取动画类型
    AnimationType getType() const;

    //透明度(255不透明)
    void setAlpha(BYTE alpha) { alpha_ = alpha; }
    BYTE getAlpha() const { return alpha_; }

private:
    std::vector<std::wstring> framePaths_;  // 帧路径
    std::vector<IMAGE> frames_;       // 帧图像
    int width_;                       // 动画宽度
    int height_;                      // 动画高度
    AnimationType type_;              // 动画类型
    DWORD duration_;                  // 总时长(毫秒)
    DWORD frameDuration_;             // 每帧时长(毫秒)
    bool isPlaying_ = false;          // 播放状态
    BYTE alpha_ = 255;                //默认不透明
};

// 路径函数库
namespace PathFunctions {
    // 线性路径（默认）
    void linear(float progress, int& x, int& y, int startX, int startY, int endX, int endY);

    // 抛物线路径
    void parabolic(float progress, int& x, int& y, int startX, int startY, int endX, int endY);

    // 圆形路径
    void circular(float progress, int& x, int& y, int centerX, int centerY, int radius);

    // 心跳效果
    void heartbeat(float progress, int& x, int& y, int centerX, int centerY);

    // 随机抖动
    void shake(float progress, int& x, int& y, int centerX, int centerY, int intensity);
}