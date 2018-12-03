#include<string>
#include<iostream>
#include<vector>
using namespace std;
typedef vector<int > arr;
typedef vector<arr> matrix;


int RESOURCES_COUNT;//the number of resources
int PROCESSES_COUNT;//the number of processes
int bounded_rand(int, int);

struct RequestRelease {
	int pid;
	arr resources;
	RequestRelease() {
		//links the request or release to one of the processess
		pid = rand() % PROCESSES_COUNT;
	}

	virtual void initiate(const matrix&, const matrix&) = 0;
	virtual void printResources() = 0;
	virtual void updateState(matrix& allo, arr& available) = 0;
protected:
	virtual void print(string type) {
		cout << "process " << pid << " made the following " + type + ": " << endl;
		for (int i = 0; i < RESOURCES_COUNT; i++) {
			cout << resources[i] << " ";
		}
		cout << endl;
	}


};
struct Request :RequestRelease {
	virtual void printResources() {
		print("request");
	}
	virtual void initiate(const matrix& max, const matrix& alloc) {
		for (int i = 0; i < RESOURCES_COUNT; i++) {
			resources.push_back(bounded_rand(0, max[pid][i] - alloc[pid][i]));
		}
	}
	virtual void updateState(matrix& alloc, arr& available) {
		for (int i = 0; i < RESOURCES_COUNT; i++) {
			alloc[pid][i] += resources[i];
			available[i] -= resources[i];
		}
	}
};
struct Release :RequestRelease {
	virtual void printResources() {
		print("release");
	}
	virtual void initiate(const matrix& max, const matrix& alloc) {
		for (int i = 0; i < RESOURCES_COUNT; i++) {
			resources.push_back(bounded_rand(0, alloc[pid][i]));
		}
	}
	virtual void updateState(matrix& alloc, arr& available) {
		for (int i = 0; i < RESOURCES_COUNT; i++) {
			alloc[pid][i] -= resources[i];
			available[i] -= resources[i];
		}
	}
};
int bounded_rand(int lower_bound, int upper_bound) {
	//returns a possible random number of resources, it means between max and min proc count
	return rand() % (upper_bound - lower_bound + 1) + lower_bound;
}
void fillAvailableArr(arr& available) {
	cout << "please enter the number of resources in the system followed by thecount of their instances" << endl;
	cin >> RESOURCES_COUNT;
	for (int i = 0; i < RESOURCES_COUNT; i++)
	{
		//fills the available resources with user input
		int input;
		cin >> input;
		available.push_back(input);
	}
}

void initiateMatrix(matrix& m) {
	for (int i = 0; i < PROCESSES_COUNT; i++) {
		m.push_back(arr());
		for (int j = 0; j < RESOURCES_COUNT; j++)
		{
			//this funcion initializes our 2d arrays to zeroes
			m[i].push_back(0);
		}
	}
}
void fillMaxMatrix(matrix& max, const arr& available) {
	for (int i = 0; i < PROCESSES_COUNT; i++) {
		max.push_back(arr());
		for (int j = 0; j < RESOURCES_COUNT; j++)
		{
			max[i].push_back(bounded_rand(0, available[j]));
		}
	}
}
bool isrequest() {
	return rand() % 2 == 0;
}
bool try_to_finish(int pid, matrix& alloc, arr& available, const matrix& max) {
	//try to see if the system could finish with the current configuration without preemptuing
	for (int i = 0; i < RESOURCES_COUNT; i++)
	{
		//if no enough resources then can not finish
		if (alloc[pid][i] + available[i] < max[pid][i]) {
			return false;
		}

	}
	//if reached this far then can finish and must free resources
	for (int i = 0; i < RESOURCES_COUNT; i++)
	{
		available[i] += alloc[pid][i];
		alloc[pid][i] = 0;
	}
	return true;
}
bool isSafe(matrix alloc, arr available, const matrix& max, RequestRelease& req) {
	//this function recieves a COPY of the system state and does some changes on this copy
	//using banker's check to decide if the requeest leave the system in a safe state or not
	req.updateState(alloc, available);
	vector<bool> finished(PROCESSES_COUNT, false);//if false then process i did not finish yet
	for (int i = 0; i < PROCESSES_COUNT; i++) {
		bool made_changes = false;
		for (int j = 0; j < PROCESSES_COUNT; j++)
		{
			if (finished[i])
				continue;
			finished[i] = try_to_finish(j, alloc, available, max);
			if (finished[i])
				made_changes = true;
		}
		if (made_changes == false)
			break;
	}
	for (int i = 0; i < RESOURCES_COUNT; i++)
	{
		//if after the above loop one process could not be finished then it is not a safe sate
		if (!try_to_finish(i, alloc, available, max))
			return false;
	}
	return true;

}

void print_arr(const arr& a, string name) {
	cout << "the " << name << " array has the following: " << endl;
	for (int i = 0; i < a.size(); i++)
	{
		cout << a[i] << " ";
	}
	cout << endl;
}
void print_matrix(const matrix& m, string name) {
	cout << "the " << name << " matrix has the following: " << endl;
	for (int i = 0; i < m.size(); i++)
	{
		for (int j = 0; j < m[i].size(); j++)
		{
			cout << m[i][j] << " ";
		}
		cout << endl;
	}
}
void updateState(matrix& alloc, arr& available, RequestRelease& requestRelease) {
	requestRelease.updateState(alloc, available);
	print_arr(available, "Available");
	print_matrix(alloc, "Allocation");

}
int main() {
	//declare data
	arr available;//available arr changes as processes successfully request or release from system
	matrix max;// max is always constant
	matrix alloc;//alloc changes according to current state of process
	//initialize data
	cout << "please enter the number of processes in the system : ";
	cin >> PROCESSES_COUNT;
	fillAvailableArr(available);
	initiateMatrix(alloc);
	fillMaxMatrix(max, available);

	//print initial values
	print_arr(available, "available");
	print_matrix(max, "maximum");

	//simulation part
	string input = "";
	RequestRelease* test;
	while (input != "0") {
		if (!(isrequest() * 0)) {//in case the random choice was a request
			test = new Request();
			test->initiate(max, alloc);
			test->printResources();
			if (isSafe(alloc, available, max, *test)) {
				cout << "and the request was granted" << endl;
				updateState(alloc, available, *test);
			}
			else {
				cout << "and the request was refused" << endl;
			}
		}
		else {//in case the random choice was a release
			test = new Release();
			test->initiate(max, alloc);
			test->printResources();
			updateState(alloc, available, *test);
		}
		delete test;
		cout << endl;
		cout << "enter 0 to exit, any other number to continue : ";
		cin >> input;
	}

	cout << "0 entered simulation finished" << endl;


	cin.get();
	cin.get();
	return 0;

}

