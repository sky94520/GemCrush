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
	,m_nRunningAction(0)
	,m_bTouchEnabled(false)
	,m_pSrcGem(nullptr)
	,m_pDestGem(nullptr)
{
}

GemstoneLayer::~GemstoneLayer()
{
	if (m_matrix != nullptr)
	{
		delete[] m_matrix;
	}
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
	m_height = 9;
	//创建背景图片
	auto background = Sprite::create("background.png");
	background->setAnchorPoint(Point(0.f, 0.f));
	this->addChild(background);
	//加载资源
	Director::getInstance()->getSpriteFrameCache()->addSpriteFramesWithFile("gemres.xml");
	//初始化矩阵
	this->initMatrix();
	//添加触碰事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = SDL_CALLBACK_2(GemstoneLayer::onTouchBegan, this);
	listener->onTouchMoved = SDL_CALLBACK_2(GemstoneLayer::onTouchMoved, this);
	_eventDispatcher->addEventListener(listener, this);
	//监听宝石动作结束回调函数
	_eventDispatcher->addEventCustomListener(Gemstone::END_ACTION, 
		SDL_CALLBACK_1(GemstoneLayer::endActionCallback, this), this);

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
	//当没有实体执行Action时，检测和删除满足消除条件的实体
	if (m_nRunningAction == 0) 
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

	float duration = (endPosition.y - startPosition.y) / 700.f;
	//运行平移动作
	stone->moveTo(duration, endPosition);
	m_nRunningAction += 1;

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
	for (unsigned i = 0; i < m_height * m_width; i++)
	{
		Gemstone *stone = m_matrix[i];
		if (stone == nullptr)
		{
			continue;
		}
		// start count chain
		std::list<Gemstone*> colChainList;
		this->getColChain(stone, colChainList);

		std::list<Gemstone*> rowChainList;
		this->getRowChain(stone, rowChainList);

		std::list<Gemstone*>& longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
		if (longerList.size() >= 3)
		{
			this->removeGems(longerList);
			//TODO:特殊
			if (longerList.size() > 3)
			{
			}
			return;
		}
	}
}

void GemstoneLayer::getColChain(Gemstone *stone, std::list<Gemstone*> &chainList)
{
	// 插入第一个实体
	chainList.push_back(stone);

	//向前检测相同类型的实体
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
	//向后检测相同类型的寿司
	neighborCol = stone->getCol() + 1;
	while (neighborCol < (int)m_width) 
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
	while (neighborRow < (int)m_height) 
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
	std::list<Gemstone*>::iterator itList;
	int number = 0;
	//依次取出sushiList的值
	for (itList = list.begin(); itList != list.end(); itList++)
	{
		Gemstone* stone = *itList;
		// 从m_matrix中移除sushi
		m_matrix[stone->getRow() * m_width + stone->getCol()] = nullptr;
		//爆炸后移除自己
		stone->explode();
		number++;
	}
	//增加当前运行动作的宝石数目
	m_nRunningAction += number;
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
					float duration = (endPosition.y - startPosition.y) / 600.f;
					stone->moveTo(duration, endPosition);
					m_nRunningAction++;
					// 设置寿司的新行
					stone->setRow(newRow);
				}
			}
		}
		// 记录col列上空缺数
		colEmptyInfo[col] = removedStoneOfCol;
	}

	// 2. 创建新的实体
	for (unsigned col = 0; col < m_width; col++)
	{
		for (int row = 0; row < colEmptyInfo[col]; row++) 
		{
			this->addGemstone(row, col);
		}
	}
	free(colEmptyInfo);
}

Gemstone* GemstoneLayer::getGemOfPoint(const Point& pos)
{
	Gemstone* stone = nullptr;
	Rect rect;
	for (unsigned i = 0; i < m_height * m_width; i++)
	{
		stone = m_matrix[i];
		if (stone != nullptr) 
		{
			rect.origin.x = stone->getPositionX() - stone->getContentSize().width / 2;
			rect.origin.y = stone->getPositionY() - stone->getContentSize().height / 2;
			rect.size = stone->getContentSize();
			if (rect.containsPoint(pos))
			{
				return stone;
			}
		}
	}
	return nullptr;
}

bool GemstoneLayer::onTouchBegan(Touch* touch, SDL_Event* event)
{
	m_pSrcGem = nullptr;
	m_pDestGem = nullptr;

	if (m_bTouchEnabled)
	{
		auto location = touch->getLocation();
		m_pSrcGem = this->getGemOfPoint(location);
	}
	return m_bTouchEnabled;
}

void GemstoneLayer::onTouchMoved(Touch* touch, SDL_Event* event)
{
	//src == nullptr || 不可点击
	if (m_pSrcGem == nullptr || !m_bTouchEnabled || m_nRunningAction != 0)
	{
		return;
	}
	unsigned row = m_pSrcGem->getRow();
	unsigned col = m_pSrcGem->getCol();

	auto location = touch->getLocation();
	auto delta = location - touch->getStartLocation();
	//根据哪个滑动的大来确定位置
	if (fabs(delta.x) <= Gemstone::getWidth() / 2 && fabs(delta.y) <= Gemstone::getWidth() / 2)
		return;

	//x轴
	if(fabs(delta.x) > fabs(delta.y))
	{
		col = delta.x > 0.f ? col + 1 : col - 1;
		//在范围之内
		if (col >= 0 && col < m_width)
		{
			m_pDestGem = m_matrix[row * m_width + col];
			this->swapGems();
		}
	}
	//y轴
	else
	{
		row = delta.y > 0.f ? row + 1 : row - 1;
		if (row >= 0 && row < m_height)
		{
			m_pDestGem = m_matrix[row * m_width + col];
			this->swapGems();
		}
	}
}

void GemstoneLayer::swapGems()
{
	m_bTouchEnabled = false;
	if (m_pSrcGem == nullptr || m_pDestGem == nullptr) 
	{
		//TODO:m_movingVertical = true;
		return;
	}

	Point posOfSrc = m_pSrcGem->getPosition();
	Point posOfDest = m_pDestGem->getPosition();
	float time = 0.2f;

	// 1.转换宝石
	m_matrix[m_pSrcGem->getRow() * m_width + m_pSrcGem->getCol()] = m_pDestGem;
	m_matrix[m_pDestGem->getRow() * m_width + m_pDestGem->getCol()] = m_pSrcGem;
	int tmpRow = m_pSrcGem->getRow();
	int tmpCol = m_pSrcGem->getCol();
	m_pSrcGem->setRow(m_pDestGem->getRow());
	m_pSrcGem->setCol(m_pDestGem->getCol());
	m_pDestGem->setRow(tmpRow);
	m_pDestGem->setCol(tmpCol);

	// 2.check for removeable
	std::list<Gemstone *> colChainListOfFirst;
	getColChain(m_pSrcGem, colChainListOfFirst);

	std::list<Gemstone *> rowChainListOfFirst;
	getRowChain(m_pSrcGem, rowChainListOfFirst);

	std::list<Gemstone *> colChainListOfSecond;
	getColChain(m_pDestGem, colChainListOfSecond);

	std::list<Gemstone *> rowChainListOfSecond;
	getRowChain(m_pDestGem, rowChainListOfSecond);

	if (colChainListOfFirst.size() >= 3
		|| rowChainListOfFirst.size() >= 3
		|| colChainListOfSecond.size() >= 3
		|| rowChainListOfSecond.size() >= 3) 
	{
		// just swap
		m_pSrcGem->moveTo(time, posOfDest);
		m_pDestGem->moveTo(time, posOfSrc);
		m_nRunningAction += 2;
		return;
	}

	// 3.no chain, swap back
	m_matrix[m_pSrcGem->getRow() * m_width + m_pSrcGem->getCol()] = m_pDestGem;
	m_matrix[m_pDestGem->getRow() * m_width + m_pDestGem->getCol()] = m_pSrcGem;
	tmpRow = m_pSrcGem->getRow();
	tmpCol = m_pSrcGem->getCol();
	m_pSrcGem->setRow(m_pDestGem->getRow());
	m_pSrcGem->setCol(m_pDestGem->getCol());
	m_pDestGem->setRow(tmpRow);
	m_pDestGem->setCol(tmpCol);

	m_pSrcGem->moveBack(time, posOfDest);
	m_pDestGem->moveBack(time, posOfSrc);
	m_nRunningAction += 2;
}

void GemstoneLayer::endActionCallback(EventCustom* ec)
{
	m_nRunningAction--;

	if (m_nRunningAction == 0)
	{
		m_bTouchEnabled = true;
	}
	else if (m_nRunningAction < 0)
	{
		printf("error:m_nRunningAction must >= 0\n");
	}
}
