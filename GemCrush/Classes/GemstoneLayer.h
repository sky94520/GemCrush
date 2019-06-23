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
	void update(float dt);
private:
	//生成
	void addGemstone(unsigned row, unsigned col);
	//获取行列对应的位置
	Point getPositionOfGemstone(unsigned row, unsigned col);
	//更新宝石,尝试消除宝石
	void updateGems();
	//获取整列最长的宝石列
	void getColChain(Gemstone *stone, std::list<Gemstone*> &chainList);
	//获取整行最长的宝石行
	void getRowChain(Gemstone *stone, std::list<Gemstone*> &chainList);
	//移除宝石
	void removeGems(std::list<Gemstone*> &list);
	//空缺处的宝石下落，然后生成新的宝石
	void fillVacancies();
	//根据位置获取点击的宝石
	Gemstone* getGemOfPoint(const Point& pos);
	//触碰事件
	bool onTouchBegan(Touch* touch, SDL_Event* event);
	void onTouchMoved(Touch* touch, SDL_Event* event);
	//交换宝石,尝试消除，消除失败则撤销
	void swapGems();
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
	//标识当前是否有动作
	bool m_bRunningAction;
	//是否可触碰
	bool m_bTouchEnabled;
	//点击的宝石
	Gemstone* m_pSrcGem;
	//与之交换的宝石
	Gemstone* m_pDestGem;
};
#endif