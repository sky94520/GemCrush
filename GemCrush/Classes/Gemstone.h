#ifndef __Gemstone_H__
#define __Gemstone_H__

#include <string>

#include "Entity.h"

class Gemstone : public Entity
{
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
	//是否在运行动作或者动画
	bool isRunningAction();

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