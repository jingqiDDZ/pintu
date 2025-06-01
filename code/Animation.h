#pragma once
#include <graphics.h>
#include <vector>
#include <functional>
#include <string>

// ����·���������Ͷ���
using PathFunction = std::function<void(float progress, int& x, int& y, int startX, int startY, int endX, int endY)>;

// ������
class Animation {
public:
    // ��������
    enum AnimationType {
        NON_BLOCKING,   // �����������������û�������
        BLOCKING        // ������������ֹ�û�������
    };

    // ���캯��
    Animation();

    // ��ʼ������
    void init(const std::vector<std::wstring>& framePaths,
        int width, int height,
        AnimationType type = BLOCKING,
        DWORD duration = 1000,
        DWORD frameDuration = 100);

    // ���ض���֡
    bool loadFrames();

    // ���Ŷ���
    void play(int startX, int startY, int endX, int endY,
        PathFunction pathFunc = nullptr,
        DWORD rop = SRCCOPY);

    // ����Ƿ����ڲ���
    bool isPlaying() const;

    // ��ȡ��������
    AnimationType getType() const;

    //͸����(255��͸��)
    void setAlpha(BYTE alpha) { alpha_ = alpha; }
    BYTE getAlpha() const { return alpha_; }

private:
    std::vector<std::wstring> framePaths_;  // ֡·��
    std::vector<IMAGE> frames_;       // ֡ͼ��
    int width_;                       // �������
    int height_;                      // �����߶�
    AnimationType type_;              // ��������
    DWORD duration_;                  // ��ʱ��(����)
    DWORD frameDuration_;             // ÿ֡ʱ��(����)
    bool isPlaying_ = false;          // ����״̬
    BYTE alpha_ = 255;                //Ĭ�ϲ�͸��
};

// ·��������
namespace PathFunctions {
    // ����·����Ĭ�ϣ�
    void linear(float progress, int& x, int& y, int startX, int startY, int endX, int endY);

    // ������·��
    void parabolic(float progress, int& x, int& y, int startX, int startY, int endX, int endY);

    // Բ��·��
    void circular(float progress, int& x, int& y, int centerX, int centerY, int radius);

    // ����Ч��
    void heartbeat(float progress, int& x, int& y, int centerX, int centerY);

    // �������
    void shake(float progress, int& x, int& y, int centerX, int centerY, int intensity);
}