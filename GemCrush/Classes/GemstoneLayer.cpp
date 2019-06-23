#include "GemstoneLayer.h"
#include "Gemstone.h"

#define GEMSTONE_GAP 1
#define GEMSTONE_NUM 6

GemstoneLayer::GemstoneLayer()
	:m_matrixLeftTopX(0.f)
	,m_matrixLeftTopY(0.f)
	,m_width(0)
	,m_height(0)
	,m_matrix(nullptr)
{
}

GemstoneLayer::~GemstoneLayer()
{
	SDL_SAFE_DELETE(m_matrix);
}

Scene* GemstoneLayer::createScene()
{
	auto scene = Scene::create();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto layer = GemstoneLayer::create(Rect(Point::ZERO, visibleSize));

	scene->addChild(layer);
	return scene;
}

GemstoneLayer* GemstoneLayer::create(const Rect& rect)
{
	auto layer = new GemstoneLayer();

	if (layer && layer->init(rect))
		layer->autorelease();
	else
		SDL_SAFE_DELETE(layer);

	return layer;
}

bool GemstoneLayer::init(const Rect& rect)
{
	m_visibleRect = rect;
	m_width = 6;
	m_height = 8;
	//创建背景图片
	auto background = Sprite::create("background.png");
	background->setAnchorPoint(Point(0.f, 0.f));
	this->addChild(background);
	//加载资源
	Director::getInstance()->getSpriteFrameCache()->addSpriteFramesWithFile("gemres.xml");
	//初始化矩阵
	this->initMatrix();

	return true;
}
void GemstoneLayer::initMatrix()
{
	//初始化
	m_matrixLeftTopX = (m_visibleRect.size.width - Gemstone::getWidth() * m_width - (m_width - 1) * GEMSTONE_GAP) / 2;
	m_matrixLeftTopY = (m_visibleRect.size.height - Gemstone::getWidth() * m_height - (m_height - 1) * GEMSTONE_GAP) / 2;

	m_matrix = new Gemstone*[m_width * m_height];
	for (unsigned row = 0; row < m_height; row++)
	{
		for (unsigned col = 0; col < m_width; col++)
		{
			auto stone = this->generateGemstone(row, col);
			// add child
			this->addChild(stone);
			m_matrix[row * m_width + col] = stone;
			printf("%d,%d ", row, col);
		}
		printf("\n");
	}
}

Gemstone* GemstoneLayer::generateGemstone(unsigned row, unsigned col)
{
	Gemstone* stone = Gemstone::create(row, col);
	//随机绑定精灵 [1,6]
	int type = rand() % GEMSTONE_NUM + 1;
	string filename = StringUtils::format("sushi_%dn.png", type);
	auto sprite = Sprite::createWithSpriteFrameName(filename);
	stone->bindSprite(sprite);

	// create action
	Point endPosition = this->getPositionOfGemstone(row, col);
	Point startPosition = Point(endPosition.x, endPosition.y - m_visibleRect.size.height / 2);
	stone->setPosition(startPosition);
	//float speed = fabs(startPosition.y) / (2 * m_visibleRect.size.height);
	float speed = (endPosition.y - startPosition.y) / 700.f;
	stone->runAction(MoveTo::create(speed, endPosition));

	return stone;
}

Point GemstoneLayer::getPositionOfGemstone(unsigned row, unsigned col)
{
	float x = m_matrixLeftTopX + (Gemstone::getWidth() + GEMSTONE_GAP) * col + Gemstone::getWidth() / 2;
	float y = m_matrixLeftTopY + (Gemstone::getWidth() + GEMSTONE_GAP) * row + Gemstone::getWidth() / 2;
	return Point(x + m_visibleRect.origin.x, y + m_visibleRect.origin.y);
}
