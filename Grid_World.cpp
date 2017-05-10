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

	Agent bug;
	bug.body.x = 0;		bug.body.y = 0;
	bug.theta = 0;			bug.omega = 0;
	bug.energy = 100;		bug.v = 0;
	bug.alpha = 10;		bug.L = 1.5;

	neural_network NN; 
	NN.setup(5,5,3);

	NN.set_in_min_max(0,6);				//Left Sensor
	NN.set_in_min_max(0,6);				//Body Sensor
	NN.set_in_min_max(0,6);				//Right Sensor
	NN.set_in_min_max(-15,15);			//Omega
	NN.set_in_min_max(0,120);			//Energy
	NN.set_out_min_max(-3,3);				//Velocity Change
	NN.set_out_min_max(-15.0,15.0);		//Omega Change
	NN.set_out_min_max(0,1);				//Move:eat ratio
	wt = NN.get_number_of_weights();
	cout << wt << endl;

	World w;
	// for (int i = 0; i < 100; i++) {
		w.run(bug,false);
	// }
	
	return 0;
}