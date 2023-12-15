#pragma once
#include<vector>
#include"Utils.h"
#include"Controlls.h"
#include"CollisionSystem.h"
#include<fstream>
#include<iostream>
using namespace std;
/*
Level class represents each connection betwenn columns of neurons in neural network
*/
class Level {
public:
	int numInputs, numOutputs;
	vector<float> biases;
	vector<float> weights;//flatened 2d array, weight represents connection with one neuron from input to one from output
	vector<float> outputs;
	Level() {};
	Level(int numInputs,int numOutputs,bool init=true) {
		outputs.resize(numOutputs);
		biases.resize(numOutputs);
		weights.resize(numInputs * numOutputs);
		this->numInputs = numInputs;
		this->numOutputs = numOutputs;
		if(init)
			randomize();
	}
	//just to make it quick
	Level(ifstream& fs) {
		fs.read((char*)&numInputs, sizeof(int));
		fs.read((char*)&numOutputs, sizeof(int));
		outputs.resize(numOutputs);
		biases.resize(numOutputs);
		weights.resize(numInputs * numOutputs);
		fs.read((char*)biases.data(), sizeof(float) * biases.size());
		fs.read((char*)weights.data(), sizeof(float) * weights.size());
	}
	void randomize() {
		//same reserch say that it is good to init weigths and biases to random double between -1 to 1
		for (int i = 0; i < numOutputs; i++) {
			biases[i] = Utils::randomFloat01() * 2.0f - 1.0f;
		}
		for (int i = 0; i < numOutputs; i++) {
			for (int j = 0; j < numInputs; j++) {
				weights[i * numInputs + j] = Utils::randomFloat01() * 2.0f - 1.0f;;
			}
		}

	}
	
	void calculateOutput(vector<float>* inputs) {
		//simple forward pass
		for (int i = 0; i < numOutputs; i++) {
			outputs[i] = 0.0f;
			for (int j = 0; j < numInputs; j++) {
			
				outputs[i] += inputs->at(j) * weights[i * numInputs + j];
			}
			//as this is a geneteic newtwork step function can be used
			if (outputs[i] + biases[i] > 0.0f) {
				outputs[i] = 1.0f;
			}
			else {
				outputs[i] = 0.0f;
			}
		}
	}
	void print() {
		cout << numInputs << endl;
		cout << numOutputs << endl;
		for (int i = 0; i < numOutputs; i++) {
			cout << biases[i] << endl;
		}
		for (int i = 0; i < numOutputs; i++) {
			for (int j = 0; j < numInputs; j++) {
				cout << weights[i * numInputs + j] << endl;
			}
		}
	}
	//take level from base network and randomnly adjust its values , still keep them in -1 to 1 range
	void mutateFrom(Level& l,float mutationStrength) {
		numInputs = l.numInputs;
		numOutputs = l.numOutputs;
		outputs.resize(numOutputs);
		biases.resize(numOutputs);
		weights.resize(numInputs * numOutputs);
		for (int i = 0; i < numOutputs; i++) {
			biases[i] = Utils::lerp(l.biases[i], Utils::randomFloat01() * 2.0f - 1.0f,mutationStrength);
		}
		for (int i = 0; i < numOutputs; i++) {
			for (int j = 0; j < numInputs; j++) {
				weights[i * numInputs + j] = Utils::lerp(l.weights[i * numInputs + j], Utils::randomFloat01() * 2.0f - 1.0f, mutationStrength);
			}
		}
	}
	//simple saving of level
	void saveToFile(ofstream& fs) {
		fs.write((char*)&numInputs, sizeof(int));
		fs.write((char*)&numOutputs, sizeof(int));
		fs.write((char*)biases.data(), sizeof(float) * biases.size());
		fs.write((char*)weights.data(), sizeof(float) * weights.size());
	}
	void clear() {
		weights.clear();
		biases.clear();
		outputs.clear();
	}
	~Level() { clear(); }
};
class NeuralNetwork {
public:
	int numLevels;
	int numInputs;
	Level* levels;
	NeuralNetwork(){}
	NeuralNetwork(int numInputs,int numOutputs,const vector<int> l) {
		numLevels = l.size()+1;
		this->numInputs = numInputs;
		levels = new Level[numLevels];
		for (int i = 0; i < numLevels-1; i++)
		{
			levels[i] = Level(numInputs, l[i]);
			numInputs = l[i];
		}
		levels[numLevels - 1] = Level(numInputs, numOutputs);
	}
	//simple file saving
	void saveToFile(string file="Resources/Saves/car.nn") {
		ofstream fs(file, ios::out | ios::binary);
		fs.write((char*) &numInputs, sizeof(int));
		fs.write((char*)&numLevels, sizeof(int));
		for (int i = 0; i < numLevels; i++) {
			levels[i].saveToFile(fs);
		}
		fs.close();
	}
	//simple loding
	static NeuralNetwork* loadFromFile(string file = "Resources/Saves/car.nn") {
		ifstream fs(file, ios::in | ios::binary);
		int numLevels, numInputs;
		fs.read((char*)&numInputs, sizeof(int));
		fs.read((char*)&numLevels, sizeof(int));
		NeuralNetwork* nn = new NeuralNetwork();
		nn->numInputs = numInputs;
		nn->numLevels = numLevels;
		nn->levels = new Level[numLevels];
		for (int i = 0; i < numLevels; i++) {
			nn->levels[i] = Level(fs);
		}
		fs.close();
		return nn;
	}
	//output of one Level is the input for the next one
	void calculateOutputs(vector<float>* inputs) {
		for (int i = 0; i < numLevels; i++) {
			levels[i].calculateOutput(inputs);
			inputs=&levels[i].outputs;
		}
	}
	vector<float>& getOutputs() {
		return levels[numLevels - 1].outputs;
	}
	void print() {
		for (int i = 0; i < numLevels; i++) {
			levels[i].print();
		}
	}
	void mutateFrom(NeuralNetwork* nn,float muatationStrength) {
		for (int i = 0; i < numLevels; i++){
			levels[i].mutateFrom(nn->levels[i],muatationStrength);
		}
	}
	void randomize() {
		for (int i = 0; i < numLevels; i++) {
			levels[i].randomize();
		}
	}
	~NeuralNetwork() {
		for (int i = 0; i < numLevels; i++) {
			levels[i].clear();
		}
		delete[] levels;
	}
};
class AIControlls : public CarControlls {
public:
	NeuralNetwork* nn;
	vector<float> inputs;
	AIControlls() {
	}
	AIControlls(NeuralNetwork* nn) {
		inputs.resize(nn->numInputs);
		this->nn = nn;

	}
	boolx4 virtual getMovementDirections() override {
		//take ray length from start to wall and pass it to neural network
		for (int i = 0; i < raysData->size(); i++)
		{
			inputs[i] =raysData->at(i).offset;
		}
		nn->calculateOutputs(&inputs);
		vector<float>& outputs= nn->getOutputs();
		return{
			outputs[0]>0.5f,outputs[1]>0.5f,outputs[2]>0.5f,outputs[3]>0.5f

		};
	}
	~AIControlls() {
		inputs.clear();
	}
};