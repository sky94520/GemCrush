#include "Gemstone.h"

const string Gemstone::END_ACTION = "gem end action";

Gemstone::Gemstone()
	:m_row(0)
	,m_col(0)
{
	_cascadeScale = true;
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

Sprite* Gemstone::bindSpriteWithSpriteFrameName(const string& spriteName)
{
	m_filename = spriteName;
	return Entity::bindSpriteWithSpriteFrameName(spriteName);
}

bool Gemstone::equals(Gemstone* pStone) const
{
	return m_filename == pStone->m_filename;
}

void Gemstone::explode()
{
	float duration = 0.3f;

	// 1. 宝石缩小,之后移除自己
	ScaleTo* scaleTo = ScaleTo::create(duration, 0.f);
	auto seq = Sequence::createWithTwoActions(scaleTo, CallFunc::create([this]() 
	{
		_eventDispatcher->dispatchCustomEvent(END_ACTION, this);
		this->removeFromParent();
	}));
	seq->setTag(ACTION_TAG);
	this->runAction(seq);

	// 2. 创建circle的缩小动画，之后移除自己
	auto circleSprite = Sprite::createWithSpriteFrameName("circle.png");
	this->getParent()->addChild(circleSprite, 10);

	circleSprite->setPosition(this->getPosition());
	circleSprite->setScale(0.f, 0.f);
	circleSprite->runAction(Sequence::createWithTwoActions(ScaleTo::create(duration, 1.f),
		RemoveSelf::create()));
}

void Gemstone::moveTo(float duration, const Point& pos)
{
	MoveTo* moveTo = MoveTo::create(duration, pos);
	moveTo->setTag(Entity::ACTION_TAG);
	auto seq = Sequence::createWithTwoActions(moveTo, CallFunc::create([this]() 
	{
		_eventDispatcher->dispatchCustomEvent(END_ACTION, this);
	}));
	seq->setTag(ACTION_TAG);
	this->runAction(seq);
}

void Gemstone::moveBack(float duration, const Point& pos)
{
	//记录当前的位置
	auto curPos = this->getPosition();
	auto seq = Sequence::create(MoveTo::create(duration, pos),
		MoveTo::create(duration, curPos),
		CallFunc::create([this]() 
	{
		_eventDispatcher->dispatchCustomEvent(END_ACTION, this);
	}), nullptr);
	seq->setTag(Entity::ACTION_TAG);
	this->runAction(seq);
}

