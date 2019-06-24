#ifndef __Gemstone_H__
#define __Gemstone_H__

#include <string>

#include "Entity.h"

class Gemstone : public Entity
{
public:
	static const string END_ACTION;
public:
	Gemstone();
	virtual ~Gemstone();
	static Gemstone* create(unsigned row, unsigned col);
	bool init(unsigned row, unsigned col);
	//override
	Sprite* bindSpriteWithSpriteFrameName(const string& spriteName);

	bool equals(Gemstone* pStone) const;
	//爆炸特效
	void explode();
	//移动到指定位置
	void moveTo(float duration, const Point& pos);
	//回转移动
	void moveBack(float duration, const Point& pos);

	unsigned int getRow() const { return m_row; }
	void setRow(unsigned row) { m_row = row; }
	unsigned int getCol() const { return m_col; }
	void setCol(unsigned col) { m_col = col; }

	static int getWidth() { return 90; }
private:
	unsigned m_row;
	unsigned m_col;
	//精灵名称 主要用于判断
	string m_filename;
};
#endif