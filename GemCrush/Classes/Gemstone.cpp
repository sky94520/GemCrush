#include "Gemstone.h"

Gemstone::Gemstone()
	:m_row(0)
	,m_col(0)
{
}

Gemstone::~Gemstone()
{
}

Gemstone* Gemstone::create(unsigned row, unsigned col)
{
	auto entity = new Gemstone();

	if (entity && entity->init(row, col))
		entity->autorelease();
	else
		SDL_SAFE_DELETE(entity);

	return entity;
}

bool Gemstone::init(unsigned row, unsigned col)
{
	m_row = row;
	m_col = col;

	return true;
}

bool Gemstone::equals(Gemstone* pStone)
{
	return true;
}
