#include "head.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

// 初始化 GDI+（只需在程序入口处调用一次）
void InitGdiplus() {
	static bool inited = false;
	if (!inited) {
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		inited = true;
	}
}

// 透明绘制函数
void putimage_alpha(int x, int y, IMAGE* img) {
	InitGdiplus();
	int w = img->getwidth();
	int h = img->getheight();

	// 获取 EasyX 的 HDC
	HDC hdc = GetImageHDC();

	// 创建 GDI+ Bitmap 并复制像素
	Bitmap bmp(w, h, PixelFormat32bppARGB);
	BitmapData bmpData;
	Gdiplus::Rect rect(0, 0, w, h);
	bmp.LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);

	DWORD* src = GetImageBuffer(img);
	BYTE* dst = (BYTE*)bmpData.Scan0;
	for (int i = 0; i < w * h; ++i) {
		DWORD c = src[i];
		dst[i * 4 + 0] = (c & 0x000000FF);        // B
		dst[i * 4 + 1] = (c & 0x0000FF00) >> 8;   // G
		dst[i * 4 + 2] = (c & 0x00FF0000) >> 16;  // R
		dst[i * 4 + 3] = (c & 0xFF000000) >> 24;  // A
	}
	bmp.UnlockBits(&bmpData);

	// 用 GDI+ 绘制到窗口
	Graphics graphics(hdc);
	graphics.DrawImage(&bmp, x, y, w, h);
}
