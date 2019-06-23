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

	bool equals(Gemstone* pStone);

	unsigned int getRow() const { return m_row; }
	unsigned int getCol() const { return m_col; }

	static int getWidth() { return 90; }
private:
	unsigned m_row;
	unsigned m_col;
};
#endif