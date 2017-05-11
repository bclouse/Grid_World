//
//	Bryant Clouse
//	Grid-World
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include <random>
#include <cmath>
#include <fstream>
#include <string>
#include "Classes.cpp"

using namespace std;

int main() {
	int wt;
	int pop = 10;
	int generations = 10;

	cout << "How large is the population? ";
	cin >> pop;
	cout << "How many generations? ";
	cin >> generations;

	Agent bug;																	//SET UP AGENT
	bug.body.x = 0;		bug.body.y = 0;
	bug.theta = 0;			bug.omega = 0;
	bug.energy = 100;		bug.v = 3;
	bug.alpha = 10;		bug.L = 1.5;
	bug = update_nodes(bug,50);
	// disp_Agent(bug,false);

	neural_network NN; 														//SET UP NEURAL NETWORK
	NN.setup(5,5,2);

	NN.set_in_min_max(0,6);				//Left Sensor
	NN.set_in_min_max(0,6);				//Body Sensor
	NN.set_in_min_max(0,6);				//Right Sensor
	NN.set_in_min_max(-15,15);			//Omega
	NN.set_in_min_max(0,120);			//Energy
	// NN.set_out_min_max(-3,3);				//Velocity Change
	NN.set_out_min_max(-15.0,15.0);		//Omega Change
	NN.set_out_min_max(0,1);				//Move:eat ratio
	wt = NN.get_number_of_weights();

	Evolution EA;																//SET UP EVOLUTIONARY ALGORITHM
	EA.init(pop, wt);

	World w;																		//SET UP WORLD CLASS
	w.init(NN,EA);
	for (int i = 0; i < generations; i++) {
		if ((i+1)%10 == 0) {
			cout << "Generation #" << i+1 << endl;
			if (i == generations-1) {
				w.run(bug,true);
			} else {
				w.run(bug,false);
			}
		} else {
			w.run(bug,false);
		}
	}
	
	return 0;
}