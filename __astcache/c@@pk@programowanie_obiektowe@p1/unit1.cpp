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
#include <memory.h>

TForm1 *Form1;


#define NO_COORDINATES 2
#define POS_X		   0
#define POS_Y 		   1

#define SURVIVORS 0
#define VICTIMS   1

#define LANE_TOP    0
#define LANE_BOTTOM 1
#define LANE_SIZE   32

#define ROAD_TOP 88
#define ROAD_END 896

/******************************************************************************

								  ANIMALS

******************************************************************************/

// ANIMAL

class Animal
{
	public:
	int id;
	TImage *obj;
	int velocity;
	unsigned int position[NO_COORDINATES];
	bool alive;

	void move();
	void die();
	Animal(int id);
    ~Animal();
};

void Animal::die()
{
	this->alive	= false;
	this->velocity = 0;
	this->obj->Width  = 20;
	this->obj->Height = 20;

	this->obj->Picture->LoadFromFile("./media/dead.png");
}	

void Animal::move()
{
	position[POS_Y] -= velocity;
	obj->Top -= velocity;
}

Animal::Animal(int id)
{
	this->id = id;
	obj = new TImage(Form1);
	obj->Parent = Form1;

	alive = true;

	int randomPosition = random(ROAD_END/2);

	position[POS_X] = randomPosition + ROAD_END/4;
	position[POS_Y] = Form1->Height - 50;

	obj->Top = position[POS_Y];
	obj->Left = position[POS_X];
}

Animal::~Animal()
{
	delete this->obj;
}

// FROG

class Frog : public Animal
{
	public:
	Frog(int id);
    ~Frog();
};

Frog::Frog(int id) : Animal(id)
{
	velocity *= 1;

	obj->Width  = 10;
	obj->Height = 10;

	obj->Picture->LoadFromFile("./media/frog.png");
}

Frog::~Frog() {}

// ZWIERZ

class Zwierz : public Animal
{
	public:
	Zwierz(int id);
	~Zwierz();
};

Zwierz::Zwierz(int id) : Animal(id)
{
	velocity *= 2;

	obj->Width  = 20;
	obj->Height = 20;

	obj->Picture->LoadFromFile("./media/zwierz.png");
}

Zwierz::~Zwierz() {}

/******************************************************************************

								  VEHICLES

******************************************************************************/

// VEHICLE

class Vehicle
{
	public:
	int id;
	TImage *obj;
	int velocity;
	unsigned int position[NO_COORDINATES];
	bool lane;

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
	this->position[POS_Y] += (velocity > 0 ? (-1*(int)LANE_SIZE) : LANE_SIZE);
    this->obj->Top = this->position[POS_Y];
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
Car::~Car() {}

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
Truck::~Truck() {}

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
Motorbike::~Motorbike() {}


/******************************************************************************

									 GAME

******************************************************************************/
typedef struct
{
	unsigned int val;
	int          id;
} RoadMap_t;

class Game
{
	public:
	RoadMap_t roadMap[128][960];
	Graphics::TBitmap *bitmap;
	TImage *Image1;

	int stats[2];

	std::map<int, Vehicle*> vehicles;
	std::map<int, Animal*>  animals;
	void updateMap();
	void clearMap();
	void printMap();
	void checkForCollisions();
	void clearVehiclesOutOfMap();
	void clearAnimalsOutOfMap();
	void updateStats();
	void checkForAnimalCollisions();

	Game(TImage* Image);
	~Game();
};

void Game::checkForAnimalCollisions()
{
	for (auto const& it : this->animals)
	{
		int width = it.second->obj->Width;
		int height = it.second->obj->Height;

		int x = it.second->position[POS_X];
		int y = it.second->position[POS_Y] - ROAD_TOP;

		if((y < (4 * LANE_SIZE)) && (y > 0))
		{
			if((this->roadMap[y][x].val == 255) && (it.second->alive == true))
			{
				it.second->die();
				this->stats[VICTIMS] += 1;
            }
		}
	}
}

void Game::updateStats()
{
	std::string survivors_str = "Survivors: " + std::to_string(this->stats[SURVIVORS]);
	std::string victims_str   = "Victims: "   + std::to_string(this->stats[VICTIMS]);
	
	System::String survivors = *(new String(survivors_str.c_str()));
	System::String victims   = *(new String(victims_str.c_str()));

	Form1->SurvivorsLabel->Caption = survivors;
	Form1->VictimsLabel->Caption   = victims;
}

void Game::clearAnimalsOutOfMap()
{
	for(auto const& it : this->animals)
	{
		if((it.second->alive == true) && (it.second->position[POS_Y] <= 10))
		{
			//Animal *tempPointer = this->animals.at(it.second->id);
			//this->animals.erase(it.second->id);
			//delete tempPointer;
			this->stats[SURVIVORS] += 1;
			it.second->alive = false;
		}
	}
}

void Game::clearVehiclesOutOfMap()
{
	for (auto const& it : this->vehicles)
	{
		if(it.second->position[POS_X] > 960)
		{
			Vehicle *tempPointer = this->vehicles.at(it.second->id);
			this->vehicles.erase(it.second->id);
			delete tempPointer;
        }
    }
}

void Game::checkForCollisions()
{
	static const unsigned int X_MARGIN = 50;
	static const unsigned int Y_MARGIN = 128 / 8;

	for (auto const& it : this->vehicles)
	{
		int dir = it.second->velocity;

		unsigned int y_position = it.second->position[POS_Y] - ROAD_TOP;
		unsigned int x_position = it.second->position[POS_X];

		int x_position_shifted_right = x_position + it.second->obj->Width + X_MARGIN;
        int x_position_shifted_left  = x_position - X_MARGIN;

		if(dir > 0 && (x_position_shifted_right < 960))
		{
			// Check if something is in front of vehicle
			if((this->roadMap[y_position][x_position_shifted_right]).val == 255)
			{

				// Check if something is above vehicle
				if(((this->roadMap[y_position - Y_MARGIN][x_position]).val != 255) && ((2 * LANE_SIZE) != y_position))
				{
					// Chane lane
					it.second->changeLane();
				}else
				{
					it.second->velocity = (this->vehicles.at((this->roadMap[y_position][x_position_shifted_right]).id))->velocity;
				}
			}
		}else if(dir < 0 && (x_position_shifted_left > 0))
		{
			// Check if something is in front of vehcile
			if((this->roadMap[y_position][x_position_shifted_left]).val == 255)
			{
                // Check if something is below vehicle
				if(((this->roadMap[y_position + it.second->obj->Height + Y_MARGIN][x_position - (it.second->obj->Width / 2)]).val != 255)
					&& (LANE_SIZE != y_position))
				{
					it.second->changeLane();
				}else
				{
					it.second->velocity = (this->vehicles.at((this->roadMap[y_position][x_position_shifted_left]).id))->velocity;
				}
			}
        }
	}
}

void Game::updateMap()
{
	// update vehicles
	this->clearMap();
	this->clearVehiclesOutOfMap();
	this->clearAnimalsOutOfMap();

	for (auto const& it : this->vehicles)
	{
		int width = it.second->obj->Width;
		int height = it.second->obj->Height;
		int direction = it.second->velocity;
		int x = it.second->position[POS_X];
		int y = it.second->position[POS_Y] - ROAD_TOP;

		TColor color = (TColor)RGB(255, 255, 255);

		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				// Ensure the coordinates are within bounds
				if (y + i >= 0 && y + i < 128 && x + j >= 0 && x + j < 960) {
					(this->roadMap[y + i][x + j]).val = 255; // Mark the vehicle position
					(this->roadMap[y + i][x + j]).id = it.second->id;
				}
			}
		}
	}

	this->checkForAnimalCollisions();

	this->checkForCollisions();
	this->updateStats();
	//this->printMap(); // Update the bitmap display
}

void Game::clearMap()
{
    for(int i = 0; i < 128; i++)
	{
		for(int j = 0; j < 960; j++)
		{
            (this->roadMap[i][j]).val = 0;
        }
    }
}

void Game::printMap()
{
    // Create a bitmap from the roadMap array
    for(int y = 0; y < 128; y++)
    {
        for(int x = 0; x < 960; x++)
        {
            int grayValue = (this->roadMap[y][x]).val;
            TColor color = (TColor)RGB(grayValue, grayValue, grayValue);
            this->bitmap->Canvas->Pixels[x][y] = color;
        }
    }

    // Display the bitmap in the TImage component
    this->Image1->Picture->Bitmap = this->bitmap;
}

Game::Game(TImage* Image)
{
	this->Image1 = Image;
	this->bitmap = new Graphics::TBitmap();
	this->bitmap->PixelFormat = pf24bit; // Set the pixel format
	this->bitmap->Width = 960;
	this->bitmap->Height = 128;

	this->Image1->Width = 0;
    this->Image1->Height = 0;

    this->clearMap(); // Initialize the road map to all zeros
	this->printMap(); // Initial display

	this->stats[SURVIVORS] = 0;
	this->stats[VICTIMS]   = 0;
}

Game::~Game()
{
	delete this->bitmap;
	delete this->Image1;

	for(auto const &it : this->vehicles)
	{
		Vehicle *tempPointer = this->vehicles.at(it.second->id);
		this->vehicles.erase(it.second->id);
		delete tempPointer;
	}

	for(auto const& it : this->animals)
	{
		Animal *tempPointer = this->animals.at(it.second->id);
		this->animals.erase(it.second->id);
		delete tempPointer;
	}
}



/******************************************************************************

								UI AND CLOCK

******************************************************************************/

int counter = 0;
int animalCounter = 0;
Game *game;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	game = new Game(ImgRoadBitmap); // Pass the TImage component to the Game constructor
}
//---------------------------------------------------------------------------

void __fastcall TForm1::GameClockTimer(TObject *Sender)
{

	for (auto const& it : game->vehicles)
	{
		it.second->move();
	}

	for (auto const& it : game->animals) {
		it.second->move();
	}

    game->updateMap();
}

//---------------------------------------------------------------------------


void __fastcall TForm1::VehicleSpawnerTimer(TObject *Sender)
{
	int randomVehicle = random(3);
	int randomAnimal  = random(2);
	int randomSpawn   = random(20);

	if(randomSpawn <= 9)
	{
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

	if(randomSpawn <= 4)
	{
		switch(randomAnimal)
		{
			case 0:
				game->animals.insert(std::pair<int, Animal*>(animalCounter, new Frog(counter)));
				break;
			case 1:
				game->animals.insert(std::pair<int, Animal*>(animalCounter, new Zwierz(counter)));
				break;
			default:
				break;
		}
		
		animalCounter++;
    }
}

//---------------------------------------------------------------------------

