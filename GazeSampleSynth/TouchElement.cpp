#include "Windows.h"
#include "TouchElement.h"

bool TouchElement::paint(HDC hdc)
{
	//HBRUSH brush = CreateSolidBrush(TE_COLOR_OFF);

	SelectObject(hdc, GetStockObject(DC_BRUSH));
	if (isActivated)
	{
		SetDCBrushColor(hdc, TE_COLOR_ON);
	}
	else
	{
		SetDCBrushColor(hdc, TE_COLOR_OFF);
	}
	SelectObject(hdc, GetStockObject(DC_PEN));
	if (isActivated)
	{
		SetDCPenColor(hdc, TE_COLOR_ON);
	}
	else
	{
		SetDCPenColor(hdc, TE_COLOR_OFF);
	}


	Ellipse(hdc, posX, posY, posX + 2 * cornerRadius, posY + 2 * cornerRadius);
	Ellipse(hdc, posX + width - 2*cornerRadius, posY, posX + width, posY + 2 * cornerRadius);
	Ellipse(hdc, posX, posY + height - 2*cornerRadius, posX + 2 * cornerRadius, posY + height);
	Ellipse(hdc, posX + width - 2 * cornerRadius, posY + height - 2 * cornerRadius, posX + width, posY + height);
	Rectangle(hdc, posX + cornerRadius, posY, posX + width - cornerRadius, posY + 2 * cornerRadius);
	Rectangle(hdc, posX + width - 2*cornerRadius, posY + cornerRadius, posX + width, posY + height - cornerRadius);
	Rectangle(hdc, posX + cornerRadius, posY + height - 2 * cornerRadius, posX + width - cornerRadius, posY + height);
	Rectangle(hdc, posX, posY + cornerRadius, posX + 2 * cornerRadius, posY + height - cornerRadius);
	Rectangle(hdc, posX + cornerRadius, posY + cornerRadius, posX + width - cornerRadius, posY + height - cornerRadius);
	return true;
}

bool TouchElement::isOnTouchElement(int px, int py) const
{
	return px <= posX + width && px >= posX && py >= posY && py <= posY + height;
}

void TouchElement::setActivated(bool act)
{
	isActivated = act;
}


void TouchElement::setWidth(int w) 
{
	width = w;
}
void TouchElement::setHeight(int h) 
{
	height = h;
}
void TouchElement::setCornerRadius(int cr) 
{
	cornerRadius = cr;
}
void TouchElement::setPosX(int px) 
{
	posX = px;
}
void TouchElement::setPosY(int py) 
{
	posY = py;
}
int TouchElement::getWidth() {
	return width;
}
int TouchElement::getHeight() {
	return height;
}
int TouchElement::getCornerRadius() {
	return cornerRadius;
}
int TouchElement::getPosX() {
	return posX;
}
int TouchElement::getPosY() {
	return posY;
}
