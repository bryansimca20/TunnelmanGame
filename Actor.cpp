#include "Actor.h"
#include "StudentWorld.h"
#include <vector>

using namespace std;

bool Actor::inBound(int x, int y)
{
	if (x != -1 && y != -1 && x != 61 && y != 61)
		return true;
	else
		return false;
}

bool Actor::TunnelManNearby(int x, int y)
{
	double distance = sqrt(pow(x - m_StudentWorld->getTunnelMan()->getX(), 2) + pow(y - m_StudentWorld->getTunnelMan()->getY(), 2));
	if (distance <= 4)
		return true;
	else
		return false;
}

bool Actor::isPickupAble(int x, int y, int x2, int y2)
{
	double distance = sqrt(pow(x - x2, 2) + pow(y - y2, 2));
	if (distance <= 3)
		return true;
	else
		return false;
}

void TunnelMan::doSomething()
{
	//check whether if tunnelman is currently alive. 
	//If not, doSomething() must return immediately
	if (isAlive == false)
		return;

	//do we have to check whether the tunnelman 
	//goes out of bounds (e.g. going out from the x = 63, y = 63 range)
	int keyPressed;
	if (m_StudentWorld->getKey(keyPressed))
	{
		switch (keyPressed)
		{
		//For every DIRECTIONAL KEY, check whether the direction that is intended is in bound;
		//Also check whether they are facing the direction of the keys.
		//If they are not facing the direction of the key, then the tunnelman should turn, not move.
		case KEY_PRESS_LEFT:
			if (!m_StudentWorld->BoulderPresent(getX()-1,getY()) && inBound(getX() -1, getY()) && getDirection() == left)
				moveTo(getX() - 1, getY());
			else if (getDirection() != left)
				setDirection(left);
				break;
		case KEY_PRESS_DOWN:
			if (!m_StudentWorld->BoulderPresent(getX(), getY()-1) && inBound(getX(), getY() - 1) && getDirection() == down)
				moveTo(getX(), getY() - 1);
			else if (getDirection() != down)
				setDirection(down);
				break;
		case KEY_PRESS_RIGHT:
			if (!m_StudentWorld->BoulderPresent(getX()+1, getY()) && inBound(getX() + 1, getY()) && getDirection() == right)
				moveTo(getX() + 1, getY());
			else if (getDirection() != right)
				setDirection(right);
				break;
		case KEY_PRESS_UP:
			if (!m_StudentWorld->BoulderPresent(getX(), getY()+1) && inBound(getX(), getY() + 1) && getDirection() == up)
				moveTo(getX(), getY() + 1);
			else if (getDirection() != up)
				setDirection(up);
				break;
		case 'z':
		case 'Z':
			if (m_Sonar > 0)
			{
				m_StudentWorld->activateSonar();
				decreaseSonar();
				break;
			}
			else
				break;
		case KEY_PRESS_TAB:
			if (m_GoldNuggets > 0)
			{
				m_StudentWorld->TunnelMandropGold();
				break;
			}
			else
				break;
		case KEY_PRESS_SPACE:
			if (m_Water > 0)
			{
				m_StudentWorld->playSound(SOUND_PLAYER_SQUIRT);
				addSquirt(getDirection());
				break;
			}
			else
				break;
		}
	}

	//Implement a function such that if the tunnelman overlaps any earth
	//The earth would be removed or destroyed.
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m_StudentWorld->digEarth(getX() + i, getY() + j);
}

void TunnelMan::addSquirt(Direction dir)
{
	if (dir == up)
	{
		if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX(), getY() + 7))
			m_StudentWorld->getSquirts().push_back(new Squirt(m_StudentWorld, this, dir, getX(), getY() + 4));
	}
	else if (dir == left)
	{
		if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX() - 4, getY()))
			m_StudentWorld->getSquirts().push_back(new Squirt(m_StudentWorld, this, dir, getX() - 4, getY()));
	}
	else if (dir == right)
	{
		if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX() + 7, getY()))
			m_StudentWorld->getSquirts().push_back(new Squirt(m_StudentWorld, this, dir, getX() + 4, getY()));
	}
	else if (dir == down)
	{
		if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX() , getY() - 4))
			m_StudentWorld->getSquirts().push_back(new Squirt(m_StudentWorld, this, dir, getX(), getY() - 4));
	}
}

void Squirt::doSomething()
{
	if (isDead())
		return;

	if (distanceTravelled == 0)
		setDead();
	else if (hitProtester())
	{
		setDead();
		return;
	}
	else
	{
		Direction dir = getDirection();
		if (dir == up)
		{
			if (getWorld()->isValidSpotToMove(getX(), getY(), getX(), getY() + 1))
			{
				moveTo(getX(), getY() + 1);
				decrementDistanceTravelled();
				return;
			}
			else
			{
				setDead();
				return;
			}
		}
		else if (dir == down)
		{
			if (getWorld()->isValidSpotToMove(getX(), getY(), getX(), getY() - 1))
			{
				moveTo(getX(), getY() - 1);
				decrementDistanceTravelled();
				return;
			}
			else
			{
				setDead();
				return;
			}
		}
		else if (dir == left)
		{
			if (getWorld()->isValidSpotToMove(getX(), getY(), getX() - 1, getY()))
			{
				moveTo(getX() - 1, getY());
				decrementDistanceTravelled();
				return;
			}
			else
			{
				setDead();
				return;
			}
		}
		else if (dir == right)
		{
			if (getWorld()->isValidSpotToMove(getX(), getY(), getX() + 1, getY()))
			{
				moveTo(getX() + 1, getY());
				decrementDistanceTravelled();
				return;
			}
			else
			{
				setDead();
				return;
			}
		}
	}
}

bool Squirt::hitProtester()
{
	vector<Protester*> protesters = getWorld()->getProtester();
	for (int i = 0; i < protesters.size(); i++)
	{
		double distance = sqrt(pow(getX() - protesters[i]->getX(), 2) + pow(getY() - protesters[i]->getY(), 2));
		if (distance <= 3)
			return true;
	}
	return false;
}

void Boulders::doSomething()
{
	//check to see if it's still alive or not.
	if (isAlive == false)
		return;

	//If there's no Earth below, then we enter a waiting state
	if (NoEarthBelow() == true)
		ticks_waited++;

	if (ticks_waited == 30)
		FallingState = true;


	if (FallingState)
	{
		//If the boulder is in a falling state then..
		if (isSoundActivated == false)
		{
			m_StudentWorld->playSound(SOUND_FALLING_ROCK);
			isSoundActivated = true;
		}

		//If the boulder comes within a radius of 3 of the tunnelMan, 
		//then the tunnelman will instantly lose a life.
		if (killTunnelMan())
			m_StudentWorld->getTunnelMan()->setDead();

		killProtesters();

		//TODO : implement a function that when it hits a protester it kills it
		//if (killProtester())

		//Keep falling if there's no earth or boulders below
		if (NoEarthBelow() && NoBouldersBelow() && inBound(getX(), getY() - 1))
			moveTo(getX(), getY() - 1);
		else //Once the boulder has hit an Earth or Boulders, then it's set to dead.
			isAlive = false;
	}
	return;
}

void Boulders::killProtesters()
{
	vector<Protester*> protesters = m_StudentWorld->getProtester();
	for (int i = 0; i < protesters.size(); i++)
	{
		double distance = sqrt(pow(getX() - protesters[i]->getX(), 2) + pow(getY() - protesters[i]->getY(), 2));
		if (distance <= 3)
		{
			protesters[i]->setLeaveOilState();
			m_StudentWorld->playSound(SOUND_PROTESTER_GIVE_UP);
			m_StudentWorld->getTunnelMan()->addScore(500);
			return;
		}
	}
}

bool Boulders::killTunnelMan()
{
	double distance = sqrt(pow(m_StudentWorld->getTunnelMan()->getX() - getX(), 2) + pow(m_StudentWorld->getTunnelMan()->getY() - getY(), 2));
	if (distance <= 3)
		return true;
	else
		return false;
}

bool Boulders::NoEarthBelow()
{
	for (int i = 0; i < 4; i++)
	{
		//if there is ANY Earth is present below it, immediately return false;
		if (m_StudentWorld->EarthPresent(getX()+i,getY()-1))
			return false;
	}
	//if there is no Earth for the whole 4 squares, then it returns true;
	return true;
}

bool Boulders::NoBouldersBelow()
{
	vector<Boulders*> boulders = m_StudentWorld->getBoulders();
	for (int i = 0; i < boulders.size(); i++)
	{
		if (boulders[i] == this)
			continue;
		else
		{
			double distance = sqrt(pow(getX() - boulders[i]->getX(), 2) + pow(getY() - boulders[i]->getY(), 2));
			if (distance < 4)
				return true;
		}
	}
	return true;
}

void GoldNuggets::doSomething()
{
	if (isAlive == false)
		return;

	if (!permanent_state && ticks_waited != 100)
	{
		ticks_waited++;
	}
	else if (!permanent_state && ticks_waited == 100)
	{
		isAlive = false;
		return;
	}

	if (TunnelManCanPickup && isPickupAble(getX(), getY(), m_StudentWorld->getTunnelMan()->getX(), m_StudentWorld->getTunnelMan()->getY()))
	{
		isAlive = false;
		m_StudentWorld->playSound(SOUND_GOT_GOODIE);
		m_StudentWorld->getTunnelMan()->addScore(10);
		m_StudentWorld->getTunnelMan()->addGoldNuggets();
	}
	else if (!TunnelManCanPickup)
	{
		std::vector<Protester*> Protesters = m_StudentWorld->getProtester();
		for (int i = 0; i < Protesters.size(); i++)
		{
			if (isPickupAble(getX(), getY(), Protesters[i]->getX(), Protesters[i]->getY()))
			{
				isAlive = false;
				m_StudentWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
				m_StudentWorld->getTunnelMan()->addScore(25);
				Protesters[i]->setLeaveOilState();
			}
		}
	}
	else if (TunnelManNearby(getX(), getY()))
	{
		setVisible(true);
		return;
	}
}

void OilBarrels::doSomething()
{
	if (isDead())
		return;

	if (isPickupAble(getX(), getY(), getTunnelMan()->getX(), getTunnelMan()->getY()))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getTunnelMan()->addScore(1000);
		getTunnelMan()->PickedUpOil();
		
	}
	else if (TunnelManNearby(getX(), getY()))
	{
		setVisible(true);
		return;
	}
	
}

void Sonars::doSomething()
{
	if (isDead())
		return;

	int T = fmax(100, 300 - 10 * getWorld()->getLevel());

	if (ticks_waited == T) //If the temporary state has lasted
	{
		setDead();
		return;
	}

	if (isPickupAble(getX(), getY(), getTunnelMan()->getX(), getTunnelMan()->getY())) //If TunnelMan picks up the item
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getTunnelMan()->addScore(75);
		getTunnelMan()->addSonars();
	}
	//In a temporary state
	ticks_waited++;
}

void WaterPool::doSomething()
{
	if (isDead())
		return;

	int T = fmax(100, 300 - 10 * getWorld()->getLevel());

	if (ticks_waited == T) //If the temporary state has lasted
	{
		setDead();
		return;
	}

	if (isPickupAble(getX(), getY(), getTunnelMan()->getX(), getTunnelMan()->getY())) //If TunnelMan picks up the item
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getTunnelMan()->addScore(100);
		getTunnelMan()->addWater();
	}
	//In a temporary state
	ticks_waited++;
}

void Protester::doSomething()
{
	if (isDead())
		return;

	if (m_HitPoints <= 0)
	{
		setDead();
		return;
	}

	if (ticksWaitedAfterShot != 0)
	{
		decrementTicksWaitedAfterShot();
		return;
	}

	if (ShoutingInterval != 0)
		ShoutingInterval--;

	if (isShotBySquirt())
	{
		m_StudentWorld->playSound(SOUND_PROTESTER_ANNOYED);
		decrementHitPoints(2);

		if (m_HitPoints <= 0)
		{
			m_StudentWorld->playSound(SOUND_PROTESTER_GIVE_UP);
			setLeaveOilState();
			m_TunnelMan->addScore(100);
			return;
		}

		int N = fmax(50, 100 - m_StudentWorld->getLevel() * 10);
		setTicksWaitedAfterShot(N);
		return;
	}

	if (canPickUpGoldNuggets() && !leaveOilState)
	{
		m_StudentWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
		m_TunnelMan->addScore(25);
		setLeaveOilState();
		return;
	}
	
	if (ticksPerpen != 0)
		decrementTicksPerpen();
	else if (ticksPerpen == 0 && checkPerpen(getDirection()))
	{
		movePerpen(getDirection());
	}
	int ticksToWaitBetweenMoves = fmax(0, 3 - m_StudentWorld->getLevel() / 4);

	if (ticksWaited != ticksToWaitBetweenMoves)
		ticksWaited++;
	else
	{
		//Can annoy tunnelMan
		if (TunnelManNearby(getX(), getY()) && isFacingTunnelMan(getTunnelMan()->getX(), getTunnelMan()->getY()) && ShoutingInterval == 0)
		{
			m_StudentWorld->playSound(SOUND_PROTESTER_YELL);
			m_TunnelMan->decreaseHitPoints(2);
			setShoutingInterval(15);
			return;
		}
		//In line with tunnelman and can chase him (nothing blocking his path)
		else if (inLineWithTunnelMan(getTunnelMan()->getX(), getTunnelMan()->getY()) && !TunnelManNearby(getX(), getY())
			&& m_StudentWorld->canMoveTowardsTunnelMan(getX(), getY(), getTunnelMan()->getX(), getTunnelMan()->getY()))
		{
			chaseTunnelMan(getTunnelMan()->getX(), getTunnelMan()->getY());
			setTickWaited(0);
			return;
		}

		if (numSquaresToMoveInCurrentDirection <= 0)
			pickDirection();
		else
			GoToCurrDirection();

		setTickWaited(0);
	}
}

void Protester::movePerpen(Direction dir)
{
	int N = rand() % 23 + 8;
	if (dir == left || dir == right)
	{
		if (getWorld()->isValidSpotToMove(getX(), getY(), getX(), getY() + 1) && getWorld()->isValidSpotToMove(getX(), getY(), getX(), getY() - 1))
		{
			int random = rand() % 2;
			if (random == 1)
			{
				setDirection(up);
				setnumSquaresToMoveInCurrentDirection(N);
				setTicksPerpen(200);
				return;
			}
			else
			{
				setDirection(down);
				setnumSquaresToMoveInCurrentDirection(N);
				setTicksPerpen(200);
				return;
			}
		}
		else if (getWorld()->isValidSpotToMove(getX(), getY(), getX(), getY() + 1))
		{
			setDirection(up);
			setnumSquaresToMoveInCurrentDirection(N);
			setTicksPerpen(200);
			return;
		}
		else
		{
			setDirection(down);
			setnumSquaresToMoveInCurrentDirection(N);
			setTicksPerpen(200);
			return;
		}
	}
	else if (dir == up || dir == down)
	{
		if (getWorld()->isValidSpotToMove(getX(), getY(), getX() + 1, getY()) && getWorld()->isValidSpotToMove(getX(), getY(), getX() - 1, getY()))
		{
			int random = rand() % 2;
			if (random == 1)
			{
				setDirection(right);
				setnumSquaresToMoveInCurrentDirection(N);
				setTicksPerpen(200);
				return;
			}
			else
			{
				setDirection(left);
				setnumSquaresToMoveInCurrentDirection(N);
				setTicksPerpen(200);
				return;
			}
		}
		else if (getWorld()->isValidSpotToMove(getX(), getY(), getX() + 1, getY()))
		{
			setDirection(right);
			setnumSquaresToMoveInCurrentDirection(N);
			setTicksPerpen(200);
			return;
		}
		else
		{
			setDirection(left);
			setnumSquaresToMoveInCurrentDirection(N);
			setTicksPerpen(200);
			return;
		}
	}
}

bool Protester::checkPerpen(Direction dir)
{
	if (dir == left || dir == right)
	{
		if (getWorld()->isValidSpotToMove(getX(), getY(), getX(), getY() + 1) || getWorld()->isValidSpotToMove(getX(), getY(), getX(), getY() - 1))
		{
			return true;
		}
	}
	else if (dir == up || dir == down)
	{
		if (getWorld()->isValidSpotToMove(getX(), getY(), getX() + 1, getY()) || getWorld()->isValidSpotToMove(getX(), getY(), getX() - 1, getY()))
		{
			return true;
		}
	}
	return false;
}

bool Protester::isShotBySquirt()
{
	vector<Squirt*> squirt = getWorld()->getSquirts();
	for (int i = 0; i < squirt.size(); i++)
	{
		int radius = sqrt(pow(getX() - squirt[i]->getX(), 2) + pow(getY() - squirt[i]->getY(), 2));
		if (radius <= 3)
		{
			return true;
		}
	}
	return false;
}

bool Protester::canPickUpGoldNuggets()
{
	vector<GoldNuggets*> goldnuggets = getWorld()->getGoldNuggets();
	for (int i = 0; i < goldnuggets.size(); i++)
	{
		int radius = sqrt(pow(getX() - goldnuggets[i]->getX(), 2) + pow(getY() - goldnuggets[i]->getY(), 2));
		if (radius <= 3)
		{
			return true;
		}
	}
	return false;
}

void Protester::GoToCurrDirection()
{
	Direction currDir = getDirection();

	switch (currDir)
	{
	case up:
		if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX(), getY() + 1))
			moveTo(getX(), getY() + 1);
		else
			setnumSquaresToMoveInCurrentDirection(0);
		break;
	case down:
		if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX(), getY() - 1))
			moveTo(getX(), getY() - 1);
		else
			setnumSquaresToMoveInCurrentDirection(0);
		break;
	case right:
		if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX() + 1, getY()))
			moveTo(getX() + 1, getY());
		else
			setnumSquaresToMoveInCurrentDirection(0);
		break;
	case left:
		if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX() - 1, getY()))
			moveTo(getX() - 1, getY());
		else
			setnumSquaresToMoveInCurrentDirection(0);
		break;
	}
	decrementNumSquaresToMoveInCurrentDirection();
}

void Protester::pickDirection()
{
	bool move = false;
	while (move == false)
	{
		int num = rand() % 53 + 8;
		setnumSquaresToMoveInCurrentDirection(num);
		int N = rand() % 4;
		switch (N)
		{
		case 0: //MOVE UP
			if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX(), getY() + 1))
			{
				setDirection(up);
				move = true;
				break;
			}
			else
				break;
		case 1: //MOVE DOWN
			if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX(), getY() - 1))
			{
				setDirection(down);
				move = true;
				break;
			}
			else
				break;
		case 2: //MOVE RIGHT
			if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX() + 1, getY()))
			{
				setDirection(right);
				move = true;
				break;
			}
			else
				break;
		case 3: //MOVE LEFT
			if (m_StudentWorld->isValidSpotToMove(getX(), getY(), getX() - 1, getY()))
			{
				setDirection(left);
				move = true;
				break;
			}
			else
				break;
		}
	}
}

void Protester::chaseTunnelMan(int TunnelMan_x, int TunnelMan_y)
{
	setnumSquaresToMoveInCurrentDirection(0);
	if (TunnelMan_y > getY()) //TunnelMan is straight above
	{
		setDirection(up);
		moveTo(getX(), getY() + 1);
	}
	else if (TunnelMan_y < getY()) //TunnelMan is straight below
	{
		setDirection(down);
		moveTo(getX(), getY() - 1);
	}
	else if (TunnelMan_x > getX()) //TunnelMan is straight on the right
	{
		setDirection(right);
		moveTo(getX() + 1, getY());
	}
	else if (TunnelMan_x < getX()) //TunnelMan is straight on the left
	{
		setDirection(left);
		moveTo(getX() - 1, getY());
	}
}

bool Protester::inLineWithTunnelMan(int TunnelMan_x, int TunnelMan_y)
{
	if (getX() == TunnelMan_x)
		return true;
	else if (getY() == TunnelMan_y)
		return true;
	else
		return false;
}
bool Protester::isFacingTunnelMan(int TunnelMan_x, int TunnelMan_y)
{
	if (getX() - TunnelMan_x <= 0 && getDirection() == right) //TunnelMan is on the right
		return true;
	else if (getX() - TunnelMan_x >= 0 && getDirection() == left) //TunnelMan is on the left
		return true;
	else if (getY() - TunnelMan_y <= 0 && getDirection() == up) //TunnelMan is above
		return true;
	else if (getY() - TunnelMan_y >= 0 && getDirection() == down) //TunnelMan is below
		return true;
	else
		return false;
}

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
