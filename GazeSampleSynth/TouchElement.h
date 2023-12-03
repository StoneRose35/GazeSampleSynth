#pragma once
#include <Windows.h>

#define TE_COLOR_OFF RGB(192, 163,34)
#define TE_COLOR_ON RGB(23,53,4)
class TouchElement
{
public:
	bool paint(HDC hdc);
	void setWidth(int w);
	void setHeight(int h);
	void setCornerRadius(int cr);
	void setPosX(int px);
	void setPosY(int py);
	int getWidth();
	int getHeight();
	int getCornerRadius();
	int getPosX();
	int getPosY();
	bool isOnTouchElement(int posX,int posY) const;
	void setActivated(bool act);
private:
	int width, height, cornerRadius, posX, posY;
	bool isActivated;
};

