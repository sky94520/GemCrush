#ifndef __GemstoneLayer_H__
#define __GemstoneLayer_H__

#include "SDL_Engine/SDL_Engine.h"
USING_NS_SDL;

class Gemstone;

class GemstoneLayer : public Layer
{
public:
	GemstoneLayer();
	virtual ~GemstoneLayer();

	static Scene* createScene();
	static GemstoneLayer* create(const Rect& rect);
	bool init(const Rect& rect);

	void initMatrix();
private:
	Gemstone* generateGemstone(unsigned row, unsigned col);
	Point getPositionOfGemstone(unsigned row, unsigned col);
private:
	//矩阵起始点左上角
	float m_matrixLeftTopX;
	float m_matrixLeftTopY;
	//矩阵的宽高个数
	unsigned m_width;
	unsigned m_height;
	//矩阵 一维数组表示二维数组
	Gemstone** m_matrix;
	//可视包围盒
	Rect m_visibleRect;
};
#endif