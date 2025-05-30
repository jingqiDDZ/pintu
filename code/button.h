/***************************************************************
 * 文件功能: 游戏按钮系统实现
 *
 * 包含类:
 *   - BaseButton: 按钮基类(抽象)
 *   - Button: 文字按钮(支持悬停变色)
 *   - ImageButton: 图片按钮(支持半透明效果)
 *
 * 特性:
 *   - 相对坐标定位(百分比)
 *   - 自动文字大小适配
 *   - 多场景按钮初始化接口
 *
 * 修改记录:
 *   2025-05-30 v1.0 同日添加了伪按钮
 ***************************************************************/

#pragma once 
#include "head.h"
#include "record.h"

 //按钮基类
class BaseButton {
public:
	int x, y, width, height;        // 按钮位置和大小
	bool is_pressed;                // 是否悬停/按下

	//记得以上变量的初始化要用基类的构造函数
	BaseButton() = default;
	BaseButton(int x, int y, int w, int h, bool pressed = false)
		: x(x), y(y), width(w), height(h), is_pressed(pressed) {
	}
	virtual ~BaseButton() = default;

	//纯虚函数
	virtual bool checkAbove(int mouseX, int mouseY) = 0;
	virtual void draw() = 0;

};

//按钮类
class Button : public BaseButton {
public:
	const TCHAR* text;				//按钮上的文字
	COLORREF color, press_color;			//按钮颜色，鼠标指针悬停在上方后颜色

	//构造函数
	Button() {

	}

	Button(double x_pos, double y_pos, int w, int h, const TCHAR* t, COLORREF c, COLORREF pc) :
		BaseButton(
			(int)(WD_width* x_pos - w / 2),
			(int)(WD_height* y_pos - h / 2),
			w, h, false),
		text(t), color(c), press_color(pc) {

	}

	//绘制按钮的函数
	void draw() override {

		//绘制长方形的按钮
		setfillcolor(is_pressed ? press_color : color);
		fillrectangle(x, y, x + width, y + height);

		//绘制边框
		setfillcolor(BLACK);
		rectangle(x, y, x + width, y + height);

		//绘制文字
		setbkmode(TRANSPARENT);		//文字背景透明
		settextcolor(BLACK);
		int font_size = height * 0.6;			//根据按钮高度设置文字高度
		settextstyle(font_size, 0, _T("宋体"));
		int textW = textwidth(text);
		int times = 0;			//最多循环次数
		//循环缩小字体直到文字宽度适配按钮()
		while (textW > width && times < 5) {
			times++;
			font_size *= 0.8;
			settextstyle(font_size, 0, _T("宋体"));
			textW = textwidth(text);
		}
		int textH = textheight(text);
		//居中绘制
		outtextxy(x + (width - textW) / 2, y + (height - textH) / 2, text);
	}

	//检查鼠标是否位于按钮上方（悬停或者点击都用这个）
	bool checkAbove(int mouseX, int mouseY) override {
		if (mouseX >= x && mouseY >= y && mouseX <= x + width && mouseY <= y + height)
			is_pressed = true;
		else
			is_pressed = false;
		return is_pressed;
	}


};

//显示图片的按钮
class ImageButton : public BaseButton {
public:
	IMAGE image;		//按钮图片

	// 构造函数
	ImageButton() {}

	ImageButton(double x_pos, double y_pos, int w, int h, const string& imagePath)		//同样传入相对位置x_pos，y_pos（图像中心点相对窗口位置），还有image对应的文件位置
		:BaseButton(
			(int)(WD_width* x_pos - w / 2),
			(int)(WD_height* y_pos - h / 2),
			w, h, false) {

		wstring wpath(imagePath.begin(), imagePath.end());
		loadimage(&image, wpath.c_str(), width, height);

	}

	// 绘制按钮
	void draw() override {
		putimage(x, y, &image);
		if (is_pressed) {
			DrawAlphaRect(x, y, width, height, 90);
		}
	}

	// 检查鼠标是否在按钮上方
	bool checkAbove(int mouseX, int mouseY) override {
		is_pressed = (mouseX >= x && mouseY >= y &&
			mouseX <= x + width && mouseY <= y + height);
		return is_pressed;
	}

private:
	//绘制白色半透明矩形的函数
	void DrawAlphaRect(int x, int y, int width, int height, int alpha) {		//alpha: 0 全透明； 255 不透明
		HDC hdc = GetImageHDC(); // 获取 EasyX 的 HDC

		Graphics graphics(hdc);
		Gdiplus::Color color(alpha, 255, 255, 255); // ARGB: Alpha, R, G, B
		SolidBrush brush(color);

		graphics.FillRectangle(&brush, x, y, width, height);
	}
};


//显示图片的伪按钮(不可交互，可以添加黑色覆盖层)
class FakeButton : public BaseButton {
public:
	IMAGE image;		//按钮图片
	bool is_dark = false;
	// 构造函数
	FakeButton() {}

	FakeButton(double x_pos, double y_pos, int w, int h, const string& imagePath, bool dark)		//同样传入相对位置x_pos，y_pos（图像中心点相对窗口位置），还有image对应的文件位置
		:BaseButton(
			(int)(WD_width* x_pos - w / 2),
			(int)(WD_height* y_pos - h / 2),
			w, h, false) {

		wstring wpath(imagePath.begin(), imagePath.end());
		loadimage(&image, wpath.c_str(), width, height);
		is_dark = dark;
	}

	// 绘制按钮
	void draw() override {
		putimage(x, y, &image);
		if (is_dark) {
			DrawAlphaRect(x, y, width, height, 170);
		}
	}

	// 检查鼠标是否在按钮上方（敷衍一下
	bool checkAbove(int mouseX, int mouseY) override {
		return false;
	}

private:
	//绘制黑色半透明矩形
	void DrawAlphaRect(int x, int y, int width, int height, int alpha) {		//alpha: 0 全透明； 255 不透明
		HDC hdc = GetImageHDC(); // 获取 EasyX 的 HDC

		Graphics graphics(hdc);
		Gdiplus::Color color(alpha, 0, 0, 0); // ARGB: Alpha, R, G, B
		SolidBrush brush(color);

		graphics.FillRectangle(&brush, x, y, width, height);
	}
};

vector<unique_ptr<BaseButton>> initMainMenuBtn();
vector<unique_ptr<BaseButton>> initLevelBtn(PlayerData player);
vector<unique_ptr<BaseButton>> initShopBtn(PlayerData player, vector<ShopItemConfig> shopConfigs, string path);
vector<unique_ptr<BaseButton>> initBagBtn(PlayerData player, vector<ShopItemConfig> shopConfigs, string path);
vector<unique_ptr<BaseButton>> initAchieveBtn(PlayerData player, vector<AchievementConfig> achConfigs, string path);
//vector<ImageButton> initShopImgBtn(PlayerData player, vector<ShopItemConfig> shopConfigs, string path);		//t添加了通用路径path
