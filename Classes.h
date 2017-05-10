#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include <random>
#include <iomanip>
#include <cmath>
#include "LY_NN.h"

using namespace std;

#define ZERO_TO_ONE ((double)rand()/RAND_MAX)
#define RADIANS ((double)3.1415926535897/180)
#define TIME 0.2
#define SIZE 500
#define ASSERT true
#define STICK 1
#define DURATION 350
#define SPACING 2.5
#define ENERGY_MOVE 1.2
#define ENERGY_ROTATE 0.05
#define EAT_AMOUNT 1

#ifndef CLASSES_H
#define CLASSES_H

//===============================
//	Policy Class
//===============================

class Policy {
private:
	vector<double> weights;
	int w;
	int change;
	friend class Evolution;
public:
	void init(int);
	void mutate();
};

//===============================
//	EA Class
//===============================

class Evolution {
private:
	vector<Policy> population;	//[-1,1]
	int p;			//Number of policies
	int change; 	//how many weights to change per mutation
public:
	void init(int,int);
	void execute(vector<double>);	//A combination of downselecting and repopulating, will also call the mutate function
	void mutate(int);
	int population_size();
	vector<double> get_policy(int);// {return population[n]};

};

//===============================
// Node Struct
//===============================

struct Node {
	double x, y;
};

//===============================
// Agent Struct
//===============================

struct Agent {
	Node body, l_sensor, r_sensor;
	double v;
	double theta, omega;
	double energy;
	double alpha;
	double L;
};

//===============================
// World Class
//===============================

class World {
private:
	double **food, **original;
	double gx, gy;
	int policies;
	// double x, y;
	// double energy;
	// double theta, omega;
	// double v;
	double stray;
	int size;
	vector<double> fitness;
	vector<double> input;
	neural_network NN;
	Evolution EA;
	Agent a;
public:
	World();
	~World();
	void init(neural_network, Evolution);
	bool simulate(double,double,double);
	void reset_world();
	void end();
	void run(Agent,bool);
	double sense_world(Node);
	void display();
	void update_input();
	void update_stray();
	bool found_goal();
};

//===============================
//	Functions
//===============================

double dist(double,double,double,double);
string int2str(int,int);
double confine(double,bool);
Agent update_nodes(Agent,int);		//Nodes are the sensor locations

#endif