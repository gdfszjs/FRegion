#pragma once
#pragma once
#include "CommonGL.h"
#include <LibIV/libiv.h>
#include <string>
#include <vector>

#include "SVGPattern.h"
#include "SVGElement.h"
#include "FRNode.h"
#include "Geo2D.h"
#include "FRStructure.h"

using namespace std;

typedef LibIV::Memory::Array::Array1D<double> BpMessage;
typedef LibIV::Memory::Array::Array1D<double> Importance;
typedef LibIV::Memory::Array::Array1D<double> Belief;

struct BpNode
{
	Belief belief;
	int nodeType;
	int adjacentNodeNumber;
	BpNode ** adjacentNodes;
	int * adjacentType;
	BpMessage * adjacentNodeMessageToMe;
	BpMessage * adjacentNodeNewMessageToMe;
};

typedef LibIV::Memory::Array::FastArray2D<BpNode*> MRFGraph;

void normalizeArray1D(LibIV::Memory::Array::Array1D<double> & arr);

class PartAssembling
{
public:
	PartAssembling() {}
	PartAssembling(int PartNum, int SourceNum, int ConstructTreeNum);
	~PartAssembling();

	void initialize();
	void allocateMemoryForGraph();
	void initLookupTable();
	void freeMemoryOfGraph();
	void setRelationsBetweenNodesOfGraph();
	Importance loadImportanceFromFRSTree(FRSTree e, int PartNum);
	void loadImportanceFromTxt(char * filename, Importance & imp);


	void computeMessageFromNode1ToNode2(FRSTree e,BpNode * node1, int index, int indexInSynopsis, BpNode * node2);
	void updateMessagesAmongNodes(FRSTree * e);
	void computeBelief(FRSTree * e);
	void bpLoop(FRSTree * e);

	void printResult(FRSTree * e);

	void filterImportance(Importance & imp, int indexInSynopsis);

	// load boundaries from files, and compute compatibility between boundaries
	// the relations between boundaries are hard-coded in the MRF graph
	void loadTwoBoundariesFromFile(string folder, string hubPart, string part2, int part2Index);

	void loadBoundary(vector<pair<int, int>> & boundary, char * filename);
	double computeLengthDiff(const vector<pair<int, int>> & boundary1, const vector<pair<int, int>> & boundary2);
	//double computeCurvatureDiff(const vector<pair<int,int>> & boundary1, const vector<pair<int,int>> & boundary2);
	//double computeFLowDiff(const vector<pair<int,int>> & boundary1, const vector<pair<int,int>> & boundary2);


private:
	MRFGraph graph;
	Importance * partImportance;
	double * lookupTableForTemporalChorologicalTerm;
	double * lookupTableForFilteringImportance;
	std::vector<TensorDb> lengthDiff;
	std::vector<TensorDb> curvatureDiff;
	std::vector<TensorDb> velocityDiff;
	int PartNum;
	int SourceNum;
	int ConstructTreeNum;
};