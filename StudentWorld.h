#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <vector>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
	public:
		StudentWorld(std::string assetDir)
			: GameWorld(assetDir)
		{
		}

		virtual ~StudentWorld() { cleanUp(); };

		virtual int init();
		virtual int move();
		virtual void cleanUp();

		////////////////////////////////////////////////////////////////////////////////
		//  The following functions are used to  generate the items in the oil field  //
		////////////////////////////////////////////////////////////////////////////////
		void generateEarth();
		void generateBoulder();
		void generateTunnelMan();
		void generateGoldNuggets();
		void generateOilBarrels();
		//With the exception of Water and Sonar, they are generated in the move() function (Protesters too)
		void generateWaterPool();
		void generateSonar();
		void generateProtesters();
		//This function checks for a valid x and y location
		//for a random x,y location for Oil, Nuggets and Boulders.
		bool checkValid(int x, int y);

		//////////////////////////////
		//Cleaner functions for move//
		//////////////////////////////
		void cleanBoulder();
		void cleanGoldNuggets();
		void cleanOils();
		void cleanSonars();
		void cleanWaterPool();
		void cleanRegularProtester();
		void cleanSquirts();

		TunnelMan* getTunnelMan() { return m_TunnelMan; };
		std::vector<Protester*>& getProtester() { return m_Protester; };
		std::vector<Squirt*>& getSquirts() { return m_Squirt; };
		std::vector<GoldNuggets*>& getGoldNuggets() { return m_GoldNuggets; };
		std::vector<Boulders*>& getBoulders() { return m_Boulders; };

		////////////////////
		//HELPER FUNCTIONS//
		////////////////////
		//Check if there's any earth in (x,y)
		bool EarthPresent(int x, int y);
		//Check if there's any boulder in (x,y)
		bool BoulderPresent(int x, int y);
		//Dig Earth if player overlaps it
		void digEarth(int x, int y);
		//remove Earth if boulder is overlapping it
		void removeEarth(int x, int y);
		//check for valid water pool position
		bool validWaterPoolPosition(int x, int y);
		//check if the destination is a valid spot to move.
		bool isValidSpotToMove(int pos_x, int pos_y, int dest_x, int dest_y);

		//Helper functions for tunnelman
		void activateSonar();
		void TunnelMandropGold();

		//Helper Functions for protesters
		bool canMoveTowardsTunnelMan(int Protester_x, int Protester_y, int TunnelMan_x, int TunnelMan_y);

		void setAddProtesterTicksWaited(int num) { AddProtesterTicksWaited = num; };
		void decrementAddProtesterTicksWaited() { AddProtesterTicksWaited--; };

	private:
		TunnelMan* m_TunnelMan;
		std::vector < std::vector<Earth*>> m_Earth;
		std::vector<Boulders*> m_Boulders;
		std::vector<GoldNuggets*> m_GoldNuggets;
		std::vector<OilBarrels*> m_OilBarrels;
		std::vector<Sonars*> m_Sonars;
		std::vector<WaterPool*> m_WaterPool;
		std::vector<Squirt*> m_Squirt;

		int ProtestersToBeAdded = fmin(15, 2 + getLevel() * 1.5);
		int AddProtesterTicksWaited = 0;
		std::vector<Protester*> m_Protester;
};

#endif // STUDENTWORLD_H_
