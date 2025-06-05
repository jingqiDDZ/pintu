//唔很好这个文件存在的意义就是制作黑色透明覆盖层和显示带有按钮的图片（弹窗）

#pragma once
#include "head.h"
#include "button.h"
#include "putimage_pro.h"

struct ImgSet {		//包含图片以及其位置大小
	IMAGE img;
	int x, y, w, h;
};

//用于确认请求的弹窗（是否购买，是否进入关卡）
class ConfirmWindow {
public:
	const TCHAR* text;			//介绍文本（eg.“是否开始关卡？”）
	ImgSet imgset;				//窗口图图片素材
	Button backbtn;				//返回按钮
	Button confirmbtn;			//确认按钮
	//bool visible;				//确认是否可见

	ConfirmWindow(const TCHAR* ttext) : text(ttext) {
		//加载默认弹窗图片
		imgset.w = 400, imgset.h = 300;
		imgset.x = (WD_width - imgset.w) / 2;
		imgset.y = (WD_height - imgset.h) / 2;
		loadimage(&imgset.img, _T("./assets/image/ui/window.png"), imgset.w, imgset.h, true);

		//初始化按钮
		backbtn = Button(0.43, 0.58, 110, 50, _T("返回"), RGB(200, 200, 200), RGB(180, 180, 180));
		confirmbtn = Button(0.57, 0.58, 110, 50, _T("确认"), RGB(200, 200, 200), RGB(180, 180, 180));
	}

	void draw() {
		//绘制黑色覆盖层
		HDC hdc = GetImageHDC(); //获取 EasyX 的 HDC
		//创建半透明画刷
		Graphics graphics(hdc);
		Gdiplus::Color color(128, 0, 0, 0); //ARGB(透明度, R, G, B)
		SolidBrush brush(color);
		//覆盖整个窗口
		graphics.FillRectangle(&brush, 0, 0, getwidth(), getheight());

		//绘制图片以及按钮
		putimage_alpha(imgset.x, imgset.y, &imgset.img);
		backbtn.draw();
		confirmbtn.draw();

		//绘制文本
		setbkmode(TRANSPARENT);
		settextcolor(BLACK);
		settextstyle(30, 0, _T("宋体"));
		int textW = textwidth(text);
		outtextxy(imgset.x + (imgset.w - textW) / 2, imgset.y + 80, text);
	}

};

//用于展示的弹窗（成就cg，物品介绍）
class ImageWindow {
public:
	ImgSet pic;						//窗口图图片素材(图像)
	ImgSet text;					//窗口图图片素材(文字介绍)
	Button backbtn;							//返回按钮

	ImageWindow() {

	}

	ImageWindow(double tw, double th, string path) {
		//加载图像图片
		pic.w = WD_width * tw, pic.h = WD_height * th;
		pic.x = WD_width * 0.06;
		pic.y = WD_height * 0.2;
		string tmppath = path + "pic.png";
		wstring wpath(tmppath.begin(), tmppath.end());
		loadimage(&pic.img, wpath.c_str(), pic.w, pic.h, true);

		//加载文字图片
		text.w = WD_width * 0.25, text.h = text.w * 1.5;
		text.x = WD_width * 0.7;
		text.y = WD_height * 0.2;
		tmppath = path + "text.png";
		wpath = wstring(tmppath.begin(), tmppath.end());
		loadimage(&text.img, wpath.c_str(),text.w, text.h, true);

		//初始化按钮
		backbtn = Button(0.8, 0.8, 110, 50, _T("返回"), RGB(200, 200, 200), RGB(180, 180, 180));
	}

	void draw() {
		//绘制黑色覆盖层
		HDC hdc = GetImageHDC(); //获取 EasyX 的 HDC
		//创建半透明画刷
		Graphics graphics(hdc);
		Gdiplus::Color color(128, 0, 0, 0); //ARGB(透明度, R, G, B)
		SolidBrush brush(color);
		//覆盖整个窗口
		graphics.FillRectangle(&brush, 0, 0, getwidth(), getheight());

		//绘制图片以及按钮
		putimage_alpha(pic.x, pic.y, &pic.img);
		putimage_alpha(text.x, text.y, &text.img);
		backbtn.draw();
	}
};