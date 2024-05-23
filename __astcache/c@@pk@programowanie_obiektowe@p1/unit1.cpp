﻿//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include <map>
#include <string>
#include <windows.h>
#include <Rpc.h>
#include <stdio.h>
#include <string.h>

TForm1 *Form1;


#define NO_COORDINATES 2
#define POS_X		   0
#define POS_Y 		   1

#define LANE_TOP    0
#define LANE_BOTTOM 1
#define LANE_SIZE   32

#define ROAD_TOP 88
#define ROAD_END 896

// VEHICLE

class Vehicle
{
	public:
    std::string id;
	TImage *obj;
	int velocity;
	unsigned int position[NO_COORDINATES];
	bool lane;
    int licenseplate;

	void move();

	void changeLane();

	Vehicle(int id);
	~Vehicle();
};

void Vehicle::move()
{
	this->position[POS_X] += velocity;
	this->obj->Left = position[POS_X];
}
void Vehicle::changeLane()
{
    this->position[POS_Y] += (lane == LANE_TOP ? -LANE_SIZE : LANE_SIZE);
}
Vehicle::Vehicle(int id)
{
	this->id = id;
	obj = new TImage(Form1);
	obj->Parent = Form1;

	int randomLane = random(2);
    int randomDirection = random(2);

	velocity = randomDirection ? 1 : -1;

	position[POS_X] = (1 - randomDirection) * ROAD_END;
	position[POS_Y] = ROAD_TOP + (randomLane * LANE_SIZE) + (randomDirection * 2 * LANE_SIZE);
	obj->Top = position[POS_Y];
	obj->Left = position[POS_X];
}
Vehicle::~Vehicle()
{
   delete this->obj;
}

// CAR : VEHICLE
class Car : public Vehicle
{
	public:

	Car(int id);
	~Car();
};

Car::Car(int id) : Vehicle(id)
{
	velocity *= 20;

	obj->Width  = 32;
	obj->Height = 32;

	if(velocity > 0)
	{
		obj->Picture->LoadFromFile("./media/car.png");
	}else
	{
		obj->Picture->LoadFromFile("./media/car_rev.png");
	}
}
Car::~Car()
{}

// TRUCK : VEHICLE
class Truck : public Vehicle
{
	public:
	Truck(int id);
	~Truck();
};

Truck::Truck(int id) : Vehicle(id)
{
	velocity *= 10;
	obj->Width  = 64;
	obj->Height = 32;

	if(velocity > 0)
	{
		obj->Picture->LoadFromFile("./media/truck.png");
	}else
	{
		obj->Picture->LoadFromFile("./media/truck_rev.png");
	}
}
Truck::~Truck()
{

}

// MOTORBIKE : VEHICLE
class Motorbike : public Vehicle
{
	public:

	Motorbike(int id);
	~Motorbike();
};

Motorbike::Motorbike(int id) : Vehicle(id)
{
	velocity *= 40;
	obj->Width  = 32;
	obj->Height = 32;
	if(velocity > 0)
	{
		obj->Picture->LoadFromFile("./media/motorbike.png");
	}else
	{
		obj->Picture->LoadFromFile("./media/motorbike_rev.png");
	}
}
Motorbike::~Motorbike()
{

}


class Game
{
	public:
    int roadMap[128][960];
    std::map<int, Vehicle*> vehicles;
	void updateMap();
	void clearMap();

	Game();
    ~Game();
};

void Game::updateMap()
{
    // update vehicles
	this->clearMap();

    for (auto const& it : this->vehicles)
	{
		int width = it.second->obj->Width;
		int height = it.second->obj->Height;
		int direction = it.second->velocity;

		for(size_t i = 0; i < height; i++)
		{
			for(size_t j = 0; j < width; j++)
			{
                this->roadMap[i][j] = 1;
            }
        }
	}
    // check for animal collisions
}

void Game::clearMap()
{
    for(size_t i = 0; i < 128; i++)
	{
		for(size_t j = 0; j < 960; j++)
		{
            this->roadMap[i][j] = 0;
        }
    }
}

Game::Game()
{

}

Game::~Game()
{

}



int counter = 0;
Game *game = new Game();

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Button1Click(TObject *Sender)
{
	int randomVehicle = random(3);

	switch(randomVehicle)
	{
		case 0:
			game->vehicles.insert(std::pair<int, Vehicle*>(counter, new Car(counter)));
			break;
		case 1:
			game->vehicles.insert(std::pair<int, Vehicle*>(counter, new Truck(counter)));
			break;
		case 2:
			game->vehicles.insert(std::pair<int, Vehicle*>(counter, new Motorbike(counter)));
			break;
		default:
			break;
	}



    counter++;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TForm1::GameClockTimer(TObject *Sender)
{

	for (auto const& it : game->vehicles)
	{
		it.second->move();
	}

    game->updateMap();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Button2Click(TObject *Sender)
{
    for (auto const& it : game->vehicles)
	{
		std::cout<<it.first <<std::endl;
	}
	//delete vehicles[0];
    //vehicles.erase(0);
}
//---------------------------------------------------------------------------

