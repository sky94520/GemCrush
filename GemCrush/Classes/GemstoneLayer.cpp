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
	,m_bRunningAction(true)
{
}

GemstoneLayer::~GemstoneLayer()
{
	/*
	if (m_matrix != nullptr)
	{
		delete[] m_matrix;
	}
	*/
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

	this->scheduleUpdate();

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
			this->addGemstone(row, col);
		}
	}
}

void GemstoneLayer::update(float)
{
	//TODO:待更新
	// 检测游戏场景中是否有Actions
	if (m_bRunningAction) 
	{
		// 每帧检测一次
		m_bRunningAction = false;
		//依次检测每个寿司，看它们是否正在执行某个Action
		for (int i = 0; i < m_height * m_width; i++) {
			Gemstone *stone = m_matrix[i];
			//如果stone正在执行Action，那m_bRunningAction就为真
			if (stone && stone->isRunningAction()) 
			{
				m_bRunningAction = true;
				break;
			}
		}
	}
	//当没有实体执行Action时，检测和删除满足消除条件的实体
	if (!m_bRunningAction) 
	{
		this->updateGems();
	}
}

void GemstoneLayer::addGemstone(unsigned row, unsigned col)
{
	Gemstone* stone = Gemstone::create(row, col);
	//随机绑定精灵 [1,6]
	int type = rand() % GEMSTONE_NUM + 1;
	string filename = StringUtils::format("sushi_%dn.png", type);
	stone->bindSpriteWithSpriteFrameName(filename);

	// create action
	Point endPosition = this->getPositionOfGemstone(row, col);
	Point startPosition = Point(endPosition.x, endPosition.y - m_visibleRect.size.height / 2);
	stone->setPosition(startPosition);
	//float speed = fabs(startPosition.y) / (2 * m_visibleRect.size.height);
	float speed = (endPosition.y - startPosition.y) / 700.f;
	MoveTo* move = MoveTo::create(speed, endPosition);
	move->setTag(Entity::ACTION_TAG);
	stone->runAction(move);

	//添加
	this->addChild(stone);
	m_matrix[row * m_width + col] = stone;
}

Point GemstoneLayer::getPositionOfGemstone(unsigned row, unsigned col)
{
	float x = m_matrixLeftTopX + (Gemstone::getWidth() + GEMSTONE_GAP) * col + Gemstone::getWidth() / 2;
	float y = m_matrixLeftTopY + (Gemstone::getWidth() + GEMSTONE_GAP) * row + Gemstone::getWidth() / 2;
	return Point(x + m_visibleRect.origin.x, y + m_visibleRect.origin.y);
}

void GemstoneLayer::updateGems()
{
	for (int i = 0; i < m_height * m_width; i++)
	{
		Gemstone *stone = m_matrix[i];
		if (stone == nullptr)
		{
			continue;
		}
		// start count chain
		std::list<Gemstone*> colChainList;
		getColChain(stone, colChainList);

		std::list<Gemstone*> rowChainList;
		getRowChain(stone, rowChainList);

		std::list<Gemstone *> &longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
		if (longerList.size() >= 3)
		{
			this->removeGems(longerList);
			//特殊
			if (longerList.size() > 3)
			{
			}
			return;
		}
	}
}

void GemstoneLayer::getColChain(Gemstone *stone, std::list<Gemstone*> &chainList)
{
	chainList.push_back(stone);// 插入第一个实体

	//向前检测相同图标值（ImgIndex）的实体
	//stone前一列实体所在列数值
	int neighborCol = stone->getCol() - 1;
	while (neighborCol >= 0) 
	{
		Gemstone* neighborStone = m_matrix[stone->getRow() * m_width + neighborCol];
		if (neighborStone && neighborStone->equals(stone))
		{
			chainList.push_back(neighborStone);//插入该“邻居”实体
			neighborCol--;//继续向前
		}
		else 
		{
			break;
		}
	}
	//向后检测相同图标值（ImgIndex）的寿司
	neighborCol = stone->getCol() + 1;
	while (neighborCol < m_width) 
	{
		Gemstone *neighborStone = m_matrix[stone->getRow() * m_width + neighborCol];
		if (neighborStone && neighborStone->equals(stone)) 
		{
			chainList.push_back(neighborStone);
			neighborCol++;
		}
		else 
		{
			break;
		}
	}
}

void GemstoneLayer::getRowChain(Gemstone *stone, std::list<Gemstone*> &chainList)
{
	// add first stone
	chainList.push_back(stone);

	int neighborRow = stone->getRow() - 1;
	while (neighborRow >= 0)
	{
		Gemstone *neighborStone = m_matrix[neighborRow * m_width + stone->getCol()];
		if (neighborStone && neighborStone->equals(stone)) 
		{
			chainList.push_back(neighborStone);
			neighborRow--;
		}
		else
		{
			break;
		}
	}

	neighborRow = stone->getRow() + 1;
	while (neighborRow < m_height) 
	{
		Gemstone *neighborStone = m_matrix[neighborRow * m_width + stone->getCol()];
		if (neighborStone && neighborStone->equals(stone)) 
		{
			chainList.push_back(neighborStone);
			neighborRow++;
		}
		else 
		{
			break;
		}
	}
}

void GemstoneLayer::removeGems(std::list<Gemstone*> &list)
{
	m_bRunningAction = true;

	std::list<Gemstone*>::iterator itList;
	//依次取出sushiList的值
	for (itList = list.begin(); itList != list.end(); itList++)
	{
		Gemstone *stone = *itList;
		// 从m_matrix中移除sushi
		m_matrix[stone->getRow() * m_width + stone->getCol()] = nullptr;
		//爆炸后移除自己
		stone->explode();
	}
	// 填补空位，它根据NULL的m_matrix值计算填补位置
	this->fillVacancies();
}

void GemstoneLayer::fillVacancies()
{
	int* colEmptyInfo = (int *)malloc(sizeof(int) * m_width);
	memset((void *)colEmptyInfo, 0, sizeof(int) * m_width);

	// 1. 让空缺处上面的寿司精灵向下落
	Gemstone* stone = nullptr;
	for (unsigned col = 0; col < m_width; col++) 
	{
		unsigned removedStoneOfCol = 0;//记录一列中空缺的精灵数
		// 从下向上
		for (int row = m_height - 1; row >= 0; row--)
		{
			stone = m_matrix[row * m_width + col];
			if (nullptr == stone)
			{
				removedStoneOfCol++;
			}
			else
			{
				if (removedStoneOfCol > 0) 
				{
					//计算寿司精灵的新行数
					int newRow = row + removedStoneOfCol;
					// 转换
					m_matrix[newRow * m_width + col] = stone;
					m_matrix[row * m_width + col] = nullptr;
					// 移动到新位置
					Point startPosition = stone->getPosition();
					Point endPosition = this->getPositionOfGemstone(newRow, col);
					float speed = (endPosition.y - startPosition.y) / 500.f;
					//停止之前的动作
					stone->stopAllActions();
					MoveTo* moveTo = MoveTo::create(speed, endPosition);
					moveTo->setTag(Entity::ACTION_TAG);
					stone->runAction(moveTo);
					// 设置寿司的新行
					stone->setRow(newRow);
				}
			}
		}
		// 记录col列上空缺数
		colEmptyInfo[col] = removedStoneOfCol;
	}

	// 2. 创建新的寿司精灵并让它落到上方空缺的位置
	for (int col = 0; col < m_width; col++)
	{
		for (int row = 0; row < colEmptyInfo[col]; row++) 
		{
			this->addGemstone(row, col);
		}
	}
	free(colEmptyInfo);
}
