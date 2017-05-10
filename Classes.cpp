#include "Classes.h"

//===============================================
//	Policy Class
//===============================================

void Policy::init(int wt) {
	w = wt;
	weights.clear();
	for (int i = 0; i < w; i++) {
		weights.push_back(ZERO_TO_ONE*2-1);
	}
	assert(weights.size() == w);
	change = w/5;
}

void Policy::mutate() {
	int index;

	for (int i = 0; i < change; i++) {
		index = rand() % w;
		if (rand()%100 > 10) {
			weights[index] += (ZERO_TO_ONE-0.5)*0.5;
		} else {
			weights[index] += (ZERO_TO_ONE-0.5);
		}

		if (weights[index] > 1) 		weights[index] = 1;
		else if (weights[index] < -1)	weights[index] = -1;
	}
}

//===============================================
//	Evolution Class
//===============================================

void Evolution::init (int size, int pop) {
	p = pop;
	change = (int)sqrt(size);
	Policy dummy;

	if (p%2 != 0) p++;

	for (int i = 0; i < p; i++) {
		dummy.init(size);
		population.push_back(dummy);
	}
}

void Evolution::execute(vector<double> fitness) {
	vector<int> choices;
	int a,b;
	int first, second;

	assert(fitness.size() == p);

	for (int i = 0; i < fitness.size(); i++) {
		choices.push_back(i);
	}
	while (choices.size() > 0) {
		first = rand()%choices.size();
		do {
			second = rand()%choices.size();
		} while (first == second);
		a = choices[first];
		b = choices[second];

		if (fitness[a] <= fitness[b]) {
			population[b] = population[a];
			population[a].mutate();
		} else if (fitness[b] < fitness[a]) {
			population[a] = population[b];
			population[b].mutate();
		}
		if (first > second) {
			choices.erase(choices.begin()+first);
			choices.erase(choices.begin()+second);
		} else {
			choices.erase(choices.begin()+second);
			choices.erase(choices.begin()+first);
		}
	}
}

vector<double> Evolution::get_policy(int n) {
	return population[n].weights;
}

int Evolution::population_size() {
	return p;
}

//===============================================
//	World Class
//===============================================

World::World() {	
	FILE *fp = fopen("assests//world.txt", "r");
	int count = 0;
	double c;

	while (!feof(fp)) {
		fscanf(fp, "%lf ", &c);
		count++;
	}
	size = sqrt(count);
	rewind(fp);

	original = new double*[size];
	food = new double*[size];

	for (int i = 0; i < size; i++) {
		original[i] = new double[size];
		food[i] = new double[size];

		for (int j = 0; j < size; j++) {
			fscanf(fp, "%lf ", &original[i][j]);
			food[i][j] = original[i][j];
		}
	}
	fclose(fp);
}

void World::run(Agent original, bool info) {
	FILE *path;
	double u,w,e;
	double distance, min;
	bool warp;
	float t;
	string num;
	char str[] = "assests//path000.txt\0";	//13,14,15


	for (int i = 0; i < policies; i++) {
		t = 0;
		a = update_nodes(original,size);
		update_stray();
		distance = dist(a.body.x,a.body.y,gx,gy);
		min = distance;
		bool warp;
		reset_world();

		if (info) {							//Write to files when requested
			num = int2str(i+1,3);
		
			str[13] = num[0];
			str[14] = num[1];
			str[15] = num[2];

			path = fopen(str,"w+");
		}

		do {
			update_stray();
			update_input();
			NN.set_vector_input(input);
			NN.set_weights(EA.get_policy(i),ASSERT);
			NN.execute();

			u = NN.get_output(0);
			w = NN.get_output(1);
			e = NN.get_output(2);

			warp = simulate(u,w,e);
			if (info) {
				if (warp) {
					fprintf(path, "\n");
				}
				fprintf(path, "%6f\t%6f\t%6f\t%6f\n", a.body.x, a.body.y, stray, a.energy);
			}

			distance = dist(a.body.x,a.body.y,gx,gy);
			if (distance < min) {
				min = distance;
			}
			t += TIME;
		} while (!found_goal() && t < DURATION && a.energy >= 0);
		if (info) fclose(path);
		//fitness
		//EA stuff
	}
	
}

// void World::run(Agent a_) {
// 	FILE *fp = fopen("assests//path.txt","w+");
// 	a = a_;
// 	old = a;
// 	double u;
// 	bool warp;
// 	// x = a.body.x;			y = a.body.y;
// 	// theta = a.theta;		omega = a.omega;
// 	// energy = a.energy;	v = a.v;

// 	for (int i = 0; i < 500; i++) {
// 		u = ((double)i-250)/250*3;
// 		// cout << u << "\t";
// 		printf("\nIteration #%d", i+1);
// 		warp = simulate(u,15,1);
// 		// cout << "update_input()" << endl;
// 		update_input();
// 		if (warp) {
// 			fprintf(fp,"\n");
// 		}
// 		fprintf(fp, "%f\t%f\n", a.body.x, a.body.y);
// 	}
// 	fclose(fp);
// 	cout << "\nLast energy read: " << a.energy << endl;
// }

void World::init(neural_network nn, Evolution ea) {
	NN = nn;
	EA = ea;
	policies = EA.population_size();
}

bool World::simulate(double u, double w, double e) {
	double eat = EAT_AMOUNT*TIME/e;
	double sense = sense_world(a.body);

	if (sense < eat) eat = sense;
	food[(int)a.body.x][(int)a.body.y] -= eat;

	double energy_spend = eat+(abs(a.v)*ENERGY_MOVE + abs(a.omega)*ENERGY_ROTATE)*TIME;
	bool warp = false;
	// cout << energy_spend << endl;

	a.body.x += a.v*cos(a.theta*RADIANS)*TIME;
	a.body.y += a.v*sin(a.theta*RADIANS)*TIME;
	a.v += (u*e-a.v)*TIME/STICK;
	a.theta += confine(a.omega*TIME,true);
	a.omega += (w*e-a.omega)*TIME/STICK;
	a.energy -= energy_spend;

	while (a.body.x < 0 || a.body.x >= size) {		//Get the x and y position of the Agent within bounds
		if (a.body.x < 0) 	a.body.x += size;
		else						a.body.x -= size;
		warp = true;
	}
	while (a.body.y < 0 || a.body.y >= size) {
		if (a.body.y < 0)		a.body.y += size;
		else						a.body.y -= size;
		warp = true;
	}
	if (a.energy > 120) a.energy = 120;
	if (ASSERT) {
		assert(a.body.x >= 0 && a.body.x < size);
		assert(a.body.y >= 0 && a.body.y < size);
		assert(a.v >= -3 && a.v <= 3);
		assert(a.theta >= 0 && a.theta < 360);
		assert(a.omega >= -15 && a.omega <= 15);
	}
	return warp;
}

void World::reset_world() {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			food[i][j] = original[i][j];
		}
	}
}

World::~World() {
	end();
}

void World::end() {
	for (int i = 0; i < size; i++) {
		delete [] original[i];
		delete [] food[i];
	}
	delete [] original;
	delete [] food;
}

double World::sense_world(Node n) {
	int i = n.x, j = n.y;
	if (i < 0 || i >= size || j < 0 || j >= size) {
		cout << "SOMEHOW STILL OUT OF BOUNDS" << endl;
		return 0;
	}
	cout << food[i][j] << ' ';

	return food[i][j];
}

void World::display() {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			cout << food[i][j];
		}
		cout << endl;
	}
}

void World::update_input() {
	input.clear();
	a = update_nodes(a,size);
	update_stray();

	cout << '\t';
	input.push_back(sense_world(a.l_sensor));
	input.push_back(sense_world(a.body));
	input.push_back(sense_world(a.r_sensor));
	// cout << "Finished sense_world()\n" << endl;
	input.push_back(a.energy);
	input.push_back(stray);

	// for (int i = 0; i< input.size(); i++) {
	// 	printf("%4.2f", input[i]);
	// 	if (i != input.size()-1)	printf("\t");
	// 	else								printf("\n");

	// }
}

void World::update_stray() {
	double beta = atan((a.body.y-gy)/(a.body.x-gx))/RADIANS;
	if (a.body.x > gx) {
		beta += 180;
	} else if (a.body.x < gx && a.body.y > gx) {
		beta += 360;
	}
	stray = abs(confine(beta-a.theta,false));
}

bool World::found_goal() {
	if (dist(a.body.x,a.body.y,gx,gy) < 2) return true;
	return false;
}

//===============================================
//	Functions
//===============================================

double dist(double x1, double y1, double x2, double y2) {
	return (double)sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

string int2str(int num, int size) {
	string dummy, output;
	int val;

	for (int i = 0; i < size; i++) {
		val = num%10;
		dummy.push_back('0'+val);
		num = (num-val)/10;
	}

	for (int i = 0; i < size; i++) {
		output.push_back(dummy[dummy.size()-1-i]);
	}

	return output;
}

double confine(double angle, bool full_rotation) {
	//Gets angle between 0 and 360
	while (angle < 0) {
		angle += 360;
	}
	while (angle >= 360) {
		angle -= 360;
	}

	//Gets angle between -180 and 180 if "full_rotation" is false
	if (!full_rotation && angle >= 180) {
		angle = angle-360;
	}

	return angle;
}

Agent update_nodes(Agent a, int size) {
	double aL = confine(a.theta + a.alpha,true);		//Angle to the left node
	double aR = confine(a.theta - a.alpha,true);		//Angle to the right node
	// cout << aL << ", " << aR << endl;

	//Update left node location
	a.l_sensor.x = cos(aL*RADIANS)*a.L + a.body.x;
	a.l_sensor.y = sin(aL*RADIANS)*a.L + a.body.y;

	while (a.l_sensor.x < 0 || a.l_sensor.x >= size) {		//Get the x and y position of the Agent within bounds
		if (a.l_sensor.x < 0) 	a.l_sensor.x += size;
		else							a.l_sensor.x -= size;
	}
	while (a.l_sensor.y < 0 || a.l_sensor.y >= size) {
		if (a.l_sensor.y < 0)		a.l_sensor.y += size;
		else								a.l_sensor.y -= size;
	}

	//Update right node location
	a.r_sensor.x = cos(aR*RADIANS)*a.L + a.body.x;
	a.r_sensor.y = sin(aR*RADIANS)*a.L + a.body.y;
	
	while (a.r_sensor.x < 0 || a.r_sensor.x >= size) {		//Get the x and y position of the Agent within bounds
		if (a.r_sensor.x < 0) 	a.r_sensor.x += size;
		else							a.r_sensor.x -= size;
	}
	while (a.r_sensor.y < 0 || a.r_sensor.y >= size) {
		if (a.r_sensor.y < 0)	a.r_sensor.y += size;
		else							a.r_sensor.y -= size;
	}

	return a;
}
