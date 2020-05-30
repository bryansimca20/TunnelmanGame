#include "StudentWorld.h"
#include "Actor.h"
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <random>
#include <cmath>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init()
{	
	generateTunnelMan();
	generateEarth();
	generateBoulder();
	generateGoldNuggets();
	generateOilBarrels();
	
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	// update and display the status and situation of the game
	ostringstream oss;
	oss << "Lvl: " << getLevel() << " Lives: " << getLives() << " Hlth: " << (m_TunnelMan->getHitPoints()) * 10 << "% Wtr: "
		<< m_TunnelMan->getWater() << " Gld: " << m_TunnelMan->getGoldNuggets() << " Oil Left : " << m_TunnelMan->getOilLeft() <<
		" Sonar: " << m_TunnelMan->getSonar() << " Scr: " << m_TunnelMan->getScore();
	string s = oss.str();
	setGameStatText(s);

	generateProtesters();

	//Randomly get a chance to generate waterpool or sonar kits every tick.
	generateWaterPool();
	generateSonar();

	//ask the waterppol to do something
	for (int i = 0; i < m_WaterPool.size(); i++)
	{
		m_WaterPool[i]->doSomething();
	}

	//Ask the sonars to do something
	for (int i = 0; i < m_Sonars.size(); i++)
	{
		m_Sonars[i]->doSomething();
	}

	//Asks the oil Barrel to do something
	for (int i = 0; i < m_OilBarrels.size(); i++)
	{
		m_OilBarrels[i]->doSomething();
	}
	// asks the TunnelMan to do something
	m_TunnelMan->doSomething();

	//ask the boulders to do something
	for (int i = 0; i < m_Boulders.size(); i++)
	{
		m_Boulders[i]->doSomething();
	}

	//Ask the gold nuggets to do something
	for (int i = 0; i < m_GoldNuggets.size(); i++)
	{
		m_GoldNuggets[i]->doSomething();
	}

	//Ask the regular protesters to do something
	for (int i = 0; i < m_Protester.size(); i++)
	{
		m_Protester[i]->doSomething();
	}

	//Ask the squirts to do somethng
	for (int i = 0; i < m_Squirt.size(); i++)
	{
		m_Squirt[i]->doSomething();
	}

	cleanBoulder();
	cleanGoldNuggets();
	cleanOils();
	cleanSonars();
	cleanWaterPool();
	cleanRegularProtester();
	cleanSquirts();

	if (m_TunnelMan->getHitPoints() == 0)
		m_TunnelMan->setDead();

	if (m_TunnelMan->isDead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	if (m_TunnelMan->getOilLeft() == 0)
		return GWSTATUS_FINISHED_LEVEL;

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	//delete all the dynamically allocated Earths
	for (vector<vector<Earth*>>::iterator it = m_Earth.begin(); it != m_Earth.end();)
	{
		for (vector<Earth*>::iterator it2 = m_Earth[0].begin(); it2 != m_Earth[0].end();)
		{
			if (*it2 != nullptr)
				delete* it2;
			it2 = m_Earth[0].erase(it2);
		}
		it = m_Earth.erase(it);
	}

	//delete all dynamically allocated Boulders
	for (vector<Boulders*>::iterator it = m_Boulders.begin(); it != m_Boulders.end();)
	{
		if (*it != nullptr)
			delete * it;
		it = m_Boulders.erase(it);
	}

	//delete all dynamically allocated WaterPool
	for (vector<WaterPool*>::iterator it = m_WaterPool.begin(); it != m_WaterPool.end();)
	{
		if (*it != nullptr)
			delete * it;
		it = m_WaterPool.erase(it);
	}

	//delete all dynamically allocated GoldNuggets
	for (vector<GoldNuggets*>::iterator it = m_GoldNuggets.begin(); it != m_GoldNuggets.end();)
	{
		if (*it != nullptr)
			delete * it;
		it = m_GoldNuggets.erase(it);
	}

	//delete all dynamically allocated Sonars
	for (vector<Sonars*>::iterator it = m_Sonars.begin(); it != m_Sonars.end();)
	{
		if (*it != nullptr)
			delete * it;
		it = m_Sonars.erase(it);
	}

	//delete all dynamically allocated RegularProtester
	for (vector<Protester*>::iterator it = m_Protester.begin(); it != m_Protester.end();)
	{
		if (*it != nullptr)
			delete * it;
		it = m_Protester.erase(it);
	}

	//delete all dynamcally allocated Squirts
	for (vector<Squirt*>::iterator it = m_Squirt.begin(); it != m_Squirt.end();)
	{
		if (*it != nullptr)
			delete * it;
		it = m_Squirt.erase(it);
	}
	//Delete the tunnelman
	if (m_TunnelMan != nullptr)
		delete m_TunnelMan;
}

void StudentWorld::cleanSquirts()
{
	for (vector<Squirt*>::iterator it = m_Squirt.begin(); it != m_Squirt.end();)
	{
		if ((*it)->isDead())
		{
			delete* it;
			it = m_Squirt.erase(it);
		}
		else
			it++;
	}
}

void StudentWorld::cleanBoulder()
{
	for (vector<Boulders*>::iterator it = m_Boulders.begin(); it != m_Boulders.end();)
	{
		if ((*it)->isDead())
		{
			delete* it;
			it = m_Boulders.erase(it);
		}
		else
			it++;
	}
}

void StudentWorld::activateSonar()
{
	for (int i = 0; i < m_GoldNuggets.size(); i++)
	{
		int radius = sqrt(pow(m_TunnelMan->getX() - m_GoldNuggets[i]->getX(), 2) + pow(m_TunnelMan->getY() - m_GoldNuggets[i]->getY(), 2));
		if (radius <= 12)
			m_GoldNuggets[i]->setVisible(true);
	}

	for (int i = 0; i < m_OilBarrels.size(); i++)
	{
		int radius = sqrt(pow(m_TunnelMan->getX() - m_OilBarrels[i]->getX(), 2) + pow(m_TunnelMan->getY() - m_OilBarrels[i]->getY(), 2));
		if (radius <= 12)
			m_OilBarrels[i]->setVisible(true);
	}
}

void StudentWorld::TunnelMandropGold()
{
	m_GoldNuggets.push_back(new GoldNuggets(m_TunnelMan->getX(), m_TunnelMan->getY(), this, true, false, false));
}

void StudentWorld::cleanGoldNuggets()
{
	for (vector<GoldNuggets*>::iterator it = m_GoldNuggets.begin(); it != m_GoldNuggets.end();)
	{
		if ((*it)->isDead())
		{
			delete* it;
			it = m_GoldNuggets.erase(it);
		}
		else
			it++;
	}
}

void StudentWorld::cleanOils()
{
	for (vector<OilBarrels*>::iterator it = m_OilBarrels.begin(); it != m_OilBarrels.end();)
	{
		if ((*it)->isDead())
		{
			delete* it;
			it = m_OilBarrels.erase(it);
		}
		else
			it++;
	}
}

void StudentWorld::cleanSonars()
{
	for (vector<Sonars*>::iterator it = m_Sonars.begin(); it != m_Sonars.end();)
	{
		if ((*it)->isDead())
		{
			delete* it;
			it = m_Sonars.erase(it);
		}
		else
			it++;
	}
}

void StudentWorld::cleanWaterPool()
{
	for (vector<WaterPool*>::iterator it = m_WaterPool.begin(); it != m_WaterPool.end();)
	{
		if ((*it)->isDead())
		{
			delete* it;
			it = m_WaterPool.erase(it);
		}
		else
			it++;
	}
}

void StudentWorld::cleanRegularProtester()
{
	for (vector<Protester*>::iterator it = m_Protester.begin(); it != m_Protester.end();)
	{
		if ((*it)->isDead())
		{
			delete* it;
			it = m_Protester.erase(it);
		}
		else
			it++;
	}
}

bool StudentWorld::checkValid(int x, int y)
{
	//Check with the boulders
	for (int i = 0; i < m_Boulders.size(); i++)
	{
		//checks the distance between objects and the location if it's in the well
		int distance = sqrt(pow(x - m_Boulders[i]->getX(), 2) + pow(y - m_Boulders[i]->getY(), 2));
		if ((x > 26 && x < 34) || distance < 6)
			return false;
	}

	
	//check with the gold nuggets
	for (int i = 0; i < m_GoldNuggets.size(); i++)
	{
		//checks the distance between objects and the location if it's in the well
		int distance = sqrt(pow(x - m_GoldNuggets[i]->getX(), 2) + pow(y - m_GoldNuggets[i]->getY(), 2));
		if ((x > 26 && x < 34) || distance < 6)
			return false;
	}

	//check with the oil barrels
	for (int i = 0; i < m_OilBarrels.size(); i++)
	{
		//checks the distance between objects and the location if it's in the well
		int distance = sqrt(pow(x - m_OilBarrels[i]->getX(), 2) + pow(y - m_OilBarrels[i]->getY(), 2));
		if ((x > 26 && x < 34) || distance < 6)
			return false;
	}

	if (x > 26 && x < 34)
		return false;

	return true;
}

void StudentWorld::generateProtesters()
{
	if (AddProtesterTicksWaited == 0 && ProtestersToBeAdded > 0)
	{
		int probabilityOfHardCore = fmin(90, getLevel() * 10 + 30);
		if (rand() % 101 <= probabilityOfHardCore)
		{
			int M = 16 + getLevel() * 2;
			m_Protester.push_back(new HardcoreProtester(this, m_TunnelMan, 60, 60, 20, M));
		}
		else
			m_Protester.push_back(new RegularProtester(this, m_TunnelMan, 60, 60, 5));

		int T = fmax(25, 200 - getLevel());
		setAddProtesterTicksWaited(T);
		ProtestersToBeAdded--;
	}
	else if (AddProtesterTicksWaited != 0)
		decrementAddProtesterTicksWaited();
}

void StudentWorld::generateOilBarrels()
{
	int num_OilBarrels = fmin(2 + getLevel(), 21);
	for (int i = 0; i < num_OilBarrels; i++)
	{
		int loc_x = rand() % 61; //generates a random number between 0 and 60
		int loc_y = rand() % 57; //generates a random number between 0 and 56

		while (checkValid(loc_x, loc_y) == false)
		{
			loc_x = rand() % 61;
			loc_y = rand() % 57;
		}

		m_OilBarrels.push_back(new OilBarrels(loc_x, loc_y, this, m_TunnelMan));
		m_TunnelMan->addOil();
	}
}

void StudentWorld::generateGoldNuggets()
{
	int num_GoldNuggets = fmax(5-getLevel()/2,2);
	for (int i = 0; i < num_GoldNuggets; i++)
	{
		int loc_x = rand() % 61; //generates a random number between 0 and 60
		int loc_y = rand() % 57; //generates a random number between 0 and 56

		while (checkValid(loc_x, loc_y) == false)
		{
			loc_x = rand() % 61;
			loc_y = rand() % 57;
		}

		m_GoldNuggets.push_back(new GoldNuggets(loc_x, loc_y, this, false, true, true));
	}
}

void StudentWorld::generateBoulder()
{
	int num_Boulder = fmin(getLevel() / 2 + 2, 9);
	for (int i = 0; i < num_Boulder; i++)
	{
		int loc_x = rand() % 61; //generate a random number between 0 and 60
		int loc_y = rand() % 37 + 20; //generate a random number between 56 and 20
		{
			//Checks the distance with all distributed objects of the game.
			//"No distributed game object may be within a radius (Euclidian distance) of 6 squares
			//of any other distributed game object.

			//The while loop says that while a random location is within that radius,
			//it would regenerate the random x,y location until it meets the requirement.
			while (checkValid(loc_x, loc_y) == false)
			{
				loc_x = rand() % 60;
				loc_y = rand() % 36 + 20;
			}

			//Once the valid x,y location has been found, the Boulders would be added.
			removeEarth(loc_x, loc_y);
			m_Boulders.push_back(new Boulders(loc_x, loc_y, this));
		}
	}
}

void StudentWorld::generateTunnelMan()
{
	//Set up the player
	m_TunnelMan = new TunnelMan(this);
}

void StudentWorld::generateEarth()
{
	//Initialize the earths
	//j = 60 because row starts from 0, not 1 and we want it to be until row 59
	for (int i = 0; i < VIEW_WIDTH; i++)
	{
		m_Earth.push_back(std::vector<Earth*>());
		for (int j = 0; j < EARTH_HEIGHT; j++)
			m_Earth[i].push_back(new Earth(this,i, j));
	}

	//Trim for the tunnel
	for (int i = 30; i < 34; i++)
	{
		for (int j = 4; j < EARTH_HEIGHT; j++)
		{
			delete m_Earth[i][j];
			m_Earth[i][j] = nullptr;
		}
	}
}

void StudentWorld::generateSonar()
{
	int G = getLevel() * 25 + 300;
	int chanceAdd = rand() % G + 1;

	if (chanceAdd == 1)
	{
		m_Sonars.push_back(new Sonars(this, m_TunnelMan, 0, 60));
		return;
	}
	else
		return;
}

void StudentWorld::generateWaterPool()
{
	int G = getLevel() * 25 + 300;
	int chanceAdd = rand() % G + 1;

	if (chanceAdd == 1)
	{
		int loc_x = rand() % 61;
		int loc_y = rand() % 60;

		while (validWaterPoolPosition(loc_x, loc_y) == false)
		{
			loc_x = rand() % 61;
			loc_y = rand() % 60;
		}
		m_WaterPool.push_back(new WaterPool(this, m_TunnelMan, loc_x, loc_y));
	}
}

bool StudentWorld::validWaterPoolPosition(int x, int y)
{
	if (y > EARTH_HEIGHT) //there are no earths above this. can carry on!
		return true;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (EarthPresent(x+i, y+j) || BoulderPresent(x+i, y+j))
				return false;

	return true;
}
void StudentWorld::digEarth(int x, int y)
{
	if (y < EARTH_HEIGHT && m_Earth[x][y] != nullptr)
	{
		playSound(SOUND_DIG);
		//Should i delete the earth or not?
		delete m_Earth[x][y];
		m_Earth[x][y] = nullptr;
	}
}

void StudentWorld::removeEarth(int x, int y)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (m_Earth[x + i][y + j] != nullptr)
			{
				delete m_Earth[x + i][y + j];
				m_Earth[x + i][y + j] = nullptr;
			}
		}
}

bool StudentWorld::canMoveTowardsTunnelMan(int Protester_x, int Protester_y, int TunnelMan_x, int TunnelMan_y)
{
	//4 CASES : TunnelMan is above, below, right or left.
	if (Protester_y > 59 && TunnelMan_y > 59)
		return true;
	else if (Protester_x > TunnelMan_x) //TunnelMan is on the left
	{
		for (int i = 1; i < (Protester_x - TunnelMan_x); i++) //rows
			for (int j = 0; j < 4; j++) //columns
				for (int k = 0; k < m_Boulders.size(); k++)
					if (m_Earth[Protester_x - i][Protester_y + j] != nullptr || (m_Boulders[k]->getX() == Protester_x - i
						&& m_Boulders[k]->getY() == Protester_y + j))
						return false;
	}
	else if (Protester_x < TunnelMan_x) //TunnelMan is on the right
	{
		for (int i = 1; i < (TunnelMan_x - Protester_x); i++) //rows
			for (int j = 0; j < 4; j++) //columns
				for (int k = 0; k < m_Boulders.size(); k++)
					if (m_Earth[Protester_x + i][Protester_y + j] != nullptr || (m_Boulders[k]->getX() == Protester_x + i
						&& m_Boulders[k]->getY() == Protester_y + j))
						return false;
	}
	else if (Protester_y < TunnelMan_y) //TunnelMan is above
	{
		for (int i = 0; i < 4; i++) //rows
			for (int j = 1; j < (TunnelMan_y - Protester_y); j++) //columns
				for (int k = 0; k < m_Boulders.size(); k++)
					if (m_Earth[Protester_x + i][Protester_y + j] != nullptr || (m_Boulders[k]->getX() == Protester_x + i
						&& m_Boulders[k]->getY() == Protester_y + j))
						return false;
	}
	else if (Protester_y > TunnelMan_y) //TunnelMan is below
	{
		for (int i = 0; i < 4; i++) //rows
			for (int j = 1; j < (Protester_y - TunnelMan_y); j++) //columns
				for (int k = 0; k < m_Boulders.size(); k++)
					if (m_Earth[Protester_x + i][Protester_y - j] != nullptr || (m_Boulders[k]->getX() != Protester_x + i
						&& m_Boulders[k]->getY() != Protester_y - j))
						return false;
	}
	return true;
}

bool StudentWorld::isValidSpotToMove(int pos_x, int pos_y, int dest_x, int dest_y)
{
	if (pos_y > 59 && (dest_x > pos_x || dest_x < pos_x) && dest_x <= VIEW_WIDTH - 4)
		return true;
	if (dest_x >= 0 && dest_x <= VIEW_WIDTH - 4 && dest_y >= 0 && dest_y <= VIEW_HEIGHT - 4)
	{
		if (dest_x > pos_x) //want to move right
		{
			for (int i = 0; i < dest_x - pos_x; i++)
				for (int j = 0; j < 4; j++)
					for (int k = 0; k < m_Boulders.size(); k++)
						if (m_Earth[pos_x + 1 + i][j] != nullptr || (m_Boulders[k]->getX() - 4 == pos_x + i &&
							m_Boulders[k]->getY() == pos_y + j))
							return false;
		}
		else if (dest_x < pos_x) //want to move left
		{
			for (int i = 0; i < pos_x - dest_x; i++)
				for (int j = 0; j < 4; j++)
					for (int k = 0; k < m_Boulders.size(); k++)
						if (m_Earth[pos_x - 1 - i][pos_y + j] != nullptr || (m_Boulders[k]->getX() + 4 == pos_x - i &&
							m_Boulders[k]->getY() == pos_y + j))
							return false;
		}
		else if (dest_y > pos_y) //want to move up
		{
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < dest_y - pos_y; j++)
					for (int k = 0; k < m_Boulders.size(); k++)
						if (m_Earth[pos_x + i][pos_y + 1 + i] != nullptr || (m_Boulders[k]->getX() == pos_x + i &&
							m_Boulders[k]->getY() - 4 == pos_y + j))
							return false;
		}
		else if (dest_y < pos_y) //want to move down
		{
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < pos_y - dest_y; j++)
					for (int k = 0; k < m_Boulders.size(); k++)
						if (m_Earth[pos_x + i][pos_y - 1 - i] != nullptr || (m_Boulders[k]->getX() == pos_x + i &&
							m_Boulders[k]->getY() + 4 == pos_y - j))
							return false;
		}
	}
	else
		return false;

	return true;
}

bool StudentWorld::EarthPresent(int x, int y)
{
	if (x >= 0 && x < VIEW_WIDTH && y >= 0 && y < EARTH_HEIGHT && m_Earth[x][y] != nullptr)
		return true;
	else
		return false;
}

bool StudentWorld::BoulderPresent(int x, int y)
{
	for (int i = 0; i < m_Boulders.size(); i++)
	{
		double distance = sqrt(pow(x - m_Boulders[i]->getX(), 2) + pow(y - m_Boulders[i]->getY(), 2));
		if (distance == 0)
			return false;
		else if (distance < 4)
			return true;
	}
	return false;
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
