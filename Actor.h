#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <sstream>
//Initiate a constant for the height of the earth.
const int EARTH_HEIGHT = 60;

class StudentWorld;

class Actor : public GraphObject
{
	public : 
		Actor(StudentWorld* studentWorld,int imageID, int startX, int startY, Direction startDirection, float size = 1.0, unsigned int depth = 0)
			: GraphObject(imageID, startX, startY, startDirection, size, depth)
		{
			m_StudentWorld = studentWorld;
			setVisible(true);
		};

		virtual ~Actor() {};

		virtual void doSomething() = 0;

		bool TunnelManNearby(int x, int y);
		bool isPickupAble(int x, int y, int x2, int y2);
		//bool canMove(int obj_x, int obj_y, int dir_x, int dir_y);
		bool inBound(int x, int y);

		StudentWorld* getWorld() { return m_StudentWorld; };

	private:
		StudentWorld* m_StudentWorld;
};

class Earth : public Actor
{
	public : 
		Earth(StudentWorld* studentWorld, int loc_x, int loc_y)
			: Actor(studentWorld, TID_EARTH, loc_x, loc_y, right, 0.25, 3) 
		{ 
			setVisible(true);
		};

		virtual ~Earth() {};
		virtual void doSomething() { return; };
};

class TunnelMan : public Actor
{
	public : 
		TunnelMan(StudentWorld* studentWorld)
			: Actor(studentWorld, TID_PLAYER, 30, 60, right, 1.0, 0) 
		{
			isAlive = true;
			m_Score = 0;
			m_hitPoints = 10;
			m_StudentWorld = studentWorld;
			m_GoldNuggets = 0;
			m_Oils = 0;
			m_Sonar = 1;
			m_Water = 5;
		};

		virtual ~TunnelMan() {};
		
		virtual void doSomething();

		//add or decrease
		void decreaseHitPoints(int num) { m_hitPoints -= num; }
		void addOil() { m_Oils++; };
		void addGoldNuggets() { m_GoldNuggets++; };
		void addSonars() { m_Sonar++; };
		void addWater() { m_Water += 5; };
		void addScore(int num) { m_Score += num; };
		void PickedUpOil() { m_Oils--; };

		void decreaseSonar() { m_Sonar--; };

		//getter functions
		int getScore() const { return m_Score; };
		int getWater() const { return m_Water; };
		int getOilLeft() const { return m_Oils; };
		int getSonar() const { return m_Sonar; };
		int getGoldNuggets() const { return m_GoldNuggets; };
		int getHitPoints() const { return m_hitPoints; };

		//Set the tunnelman to dead
		void setDead() { isAlive = false; };
		bool isDead() { return isAlive == false; };		

		void addSquirt(Direction dir);

	private:
		StudentWorld* m_StudentWorld;
		bool isAlive;
		int m_Score;
		int m_hitPoints; 
		int m_Water;
		int m_GoldNuggets;
		int m_Oils;
		int m_Sonar;
};

class Boulders : public Actor
{
	public :
		Boulders(int loc_x, int loc_y, StudentWorld* studentWorld)
			: Actor(studentWorld, TID_BOULDER, loc_x, loc_y, down, 1.0, 1)
		{
			isAlive = true;
			setVisible(true);
			m_StudentWorld = studentWorld;
		};
		virtual ~Boulders() {};

		virtual void doSomething();

		bool isDead() { return isAlive == false; };
		bool stillAlive() { return isAlive == true; };

		//Helper functions
		bool NoEarthBelow();
		bool NoBouldersBelow();
		bool killTunnelMan();
		void killProtesters();

	private:
		StudentWorld* m_StudentWorld;
		bool isSoundActivated = false;
		bool isAlive = false;
		int ticks_waited = 0;
		bool FallingState = false;
};

class Squirt : public Actor
{
	public:
		Squirt(StudentWorld* studentWorld, TunnelMan* tunnelMan, Direction dir, int loc_x, int loc_y)
			: Actor(studentWorld, TID_WATER_SPURT, loc_x, loc_y, dir, 1.0, 1)
		{
			setVisible(true);
			isAlive = true;
		}

		virtual ~Squirt() {};

		bool hitProtester();

		virtual void doSomething();
		bool isDead() { return isAlive == false; };
		void setDead() { isAlive = false; };
		void decrementDistanceTravelled() { distanceTravelled--; };

	private:
		bool isAlive = false;
		int distanceTravelled = 4;
};

class GoldNuggets : public Actor
{
	public :
		GoldNuggets(int loc_x, int loc_y, StudentWorld* studentWorld, bool visible, bool canTunnelManPickUp, bool isPermanentState)
			: Actor(studentWorld, TID_GOLD, loc_x, loc_y, right, 1.0, 2)
		{
			setVisible(visible);
			m_StudentWorld = studentWorld;
			TunnelManCanPickup = canTunnelManPickUp;
			permanent_state = isPermanentState;
			isAlive = true;
			ticks_waited = 0;
		};

		virtual ~GoldNuggets() {};

		virtual void doSomething();

		bool isDead() { return isAlive == false; };

	private :
		StudentWorld* m_StudentWorld;
		bool isAlive;
		bool permanent_state;
		bool TunnelManCanPickup;
		int ticks_waited;
};

class Goodies : public Actor
{
	public:
		Goodies(int imageID, int loc_x, int loc_y, Direction dir, float size, unsigned int depth, StudentWorld* studentWorld, TunnelMan* tunnelMan)
			: Actor(studentWorld, imageID, loc_x, loc_y, dir, size, depth)
		{
			isAlive = true;
			m_StudentWorld = studentWorld;
			m_TunnelMan = tunnelMan;
		}
		virtual ~Goodies() {};

		virtual void doSomething() = 0;

		void setDead() { isAlive = false; }

		bool isDead() { return isAlive == false; };

		StudentWorld* getWorld() { return m_StudentWorld; };
		TunnelMan* getTunnelMan() { return m_TunnelMan; };

	private:
		StudentWorld* m_StudentWorld;
		TunnelMan* m_TunnelMan;
		bool isAlive;
};

class Sonars : public Goodies
{
	public:
		Sonars(StudentWorld* studentWorld, TunnelMan* tunnelMan, int loc_x, int loc_y)
			: Goodies(TID_SONAR, loc_x, loc_y, right, 1.0, 2, studentWorld, tunnelMan)
		{
			setVisible(true);
		}
		virtual ~Sonars() {};

		virtual void doSomething();

	private:
		int ticks_waited = 0;
};

class OilBarrels : public Goodies
{
	public : 
		OilBarrels(int loc_x, int loc_y, StudentWorld* studentWorld, TunnelMan* tunnelMan)
			: Goodies(TID_BARREL, loc_x, loc_y, right, 1.0, 2, studentWorld, tunnelMan)
		{
			setVisible(false);
		}
		virtual ~OilBarrels() {};

		virtual void doSomething();
		
	private:
};

class WaterPool : public Goodies
{
	public: 
		WaterPool(StudentWorld* studentWorld, TunnelMan* tunnelMan, int loc_x, int loc_y)
			: Goodies(TID_WATER_POOL, loc_x, loc_y, right, 1.0, 2, studentWorld, tunnelMan)
		{
			setVisible(true);
		}
		virtual ~WaterPool() {};
		virtual void doSomething();

	private:
		int ticks_waited = 0;
};

class Protester : public Actor
{
	public:
		Protester(StudentWorld* studentWorld, TunnelMan* tunnelMan, int imageID, int startX, int startY, int hitpoints, Direction dir, float size, unsigned int depth)
			: Actor(studentWorld, imageID, startX, startY, dir, size, depth)
		{
			isAlive = true;
			m_StudentWorld = studentWorld;
			m_TunnelMan = tunnelMan;
			m_HitPoints = hitpoints;
		}

		virtual ~Protester() {}
		virtual void doSomething();

		bool isDead() { return isAlive == false; };

		void setTickWaited(int num_ticks) { ticksWaited = num_ticks; };
		void setShoutingInterval(int num_ticks) { ShoutingInterval = num_ticks; };
		void setnumSquaresToMoveInCurrentDirection(int num) { numSquaresToMoveInCurrentDirection = num; };
		void setLeaveOilState() { leaveOilState = true; };
		void setTicksWaitedAfterShot(int num) { ticksWaitedAfterShot = num; };
		void setDead() { isAlive = false; };
		void setTicksPerpen(int num) { ticksPerpen = num; };

		void decrementNumSquaresToMoveInCurrentDirection() { numSquaresToMoveInCurrentDirection--; }
		void decrementHitPoints(int num) { m_HitPoints -= num; };
		void decrementTicksWaitedAfterShot() { ticksWaitedAfterShot--; };
		void decrementTicksPerpen() { ticksPerpen--; };

		StudentWorld* getWorld() { return m_StudentWorld; };
		TunnelMan* getTunnelMan() { return m_TunnelMan; };

		//Helper Functions
		bool isFacingTunnelMan(int TunnelMan_x, int TunnelMan_y);
		bool inLineWithTunnelMan(int TunnelMan_x, int TunnelMan_y);
		void chaseTunnelMan(int TunnelMan_x, int TunnelMan_y);

		bool canPickUpGoldNuggets();
		bool isShotBySquirt();

		void pickDirection();
		void GoToCurrDirection();

		bool checkPerpen(Direction dir);
		void movePerpen(Direction dir);

	private:
		StudentWorld* m_StudentWorld;
		TunnelMan* m_TunnelMan;
		bool isAlive;
		bool leaveOilState = false;
		int m_HitPoints;
		int ticksWaited = 0;
		int ShoutingInterval = 0;
		int ticksWaitedAfterShot = 0;
		int ticksPerpen = 0;
		int numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
};

class RegularProtester :public Protester
{
	public:
		RegularProtester(StudentWorld* studentWorld, TunnelMan* tunnelMan, int loc_x, int loc_y, int hitpoints)
			: Protester(studentWorld, tunnelMan, TID_PROTESTER, loc_x, loc_y, hitpoints, left, 1.0, 0)
		{
			setVisible(true);
		}
		virtual ~RegularProtester() {};
};

class HardcoreProtester : public Protester
{	
	public:
		HardcoreProtester(StudentWorld* studentWorld, TunnelMan* tunnelMan, int loc_x, int loc_y, int hitpoints, int total_distance)
			: Protester(studentWorld, tunnelMan, TID_HARD_CORE_PROTESTER, loc_x, loc_y, hitpoints, left, 1.0, 0)
		{
			setVisible(true);
			totalDistanceFromPlayer = total_distance;
		}

		void setTotalDistanceFromPlayer(int num) { totalDistanceFromPlayer = num; };

	private:
		int totalDistanceFromPlayer;
};

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

#endif // ACTOR_H_
