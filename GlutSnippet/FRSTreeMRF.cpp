#include "FRSTreeMRF.h"
#include "CommonGL.h"

#include <sstream>
#include <fstream>
#include <vector>

#include "SVGPattern.h"
#include "SVGElement.h"
#include "FRNode.h"
#include "Geo2D.h"
#include "FRStructure.h"


using namespace std;

/*
Length = 100
const static double sigmaTemporal = 30;
const static double sigmaLength = 25;
i and is:
const static double sigmaImportance = 20;
its and it:
const static double sigmaImportance = 40;

*/


const static double sigmaImportance = 40;
const static double sigmaTemporal = 30;
const static double sigmaLength = 25;

void PartAssembling::bpLoop(FRSTree * e)
{
	int cnt = 200;
	while (cnt-- > 0)
	{
		cout << cnt << endl;
		updateMessagesAmongNodes(e);
	}
}

void PartAssembling::computeMessageFromNode1ToNode2(FRSTree e, BpNode * node1, int index, int indexInSynopsis, BpNode * node2)
{
	//need to be change
	cout << e.com_index_order.size() << endl;
	BpMessage msg = loadImportanceFromFRSTree(e,node1->nodeType);
	filterImportance(msg, indexInSynopsis);
	normalizeArray1D(msg);

	// 1. compute the belief of node1 from node1's adjacent nodes except node2
	for (int i = 0; i<node1->adjacentNodeNumber; i++)
	{
		BpNode * adjacentNode = node1->adjacentNodes[i];

		if (adjacentNode != NULL && node2 != adjacentNode)
			msg *= node1->adjacentNodeMessageToMe[i];
	}

	normalizeArray1D(msg);

	BpMessage msgTmp;
	msgTmp.set(SourceNum);
	msgTmp.fill(0);

	// 2.compute message from node1 to node2
	// here, consider spatial and temporal relations separately

	if (node2->adjacentType[index] > 0) // spatial compatibility term
	{
		for (int i = 0; i<SourceNum; i++)
		{
			for (int j = 0; j<SourceNum; j++)
			{
				//double spatialCompatibility = 1;
				//the most important part of the problem
				//compare the element which would be fill in the node1 and the element fill in the node2.
				double spatialCompatibility = lengthDiff[node2->adjacentType[index]].at(i, j);

				msgTmp[i] += msg[j] * spatialCompatibility;
			}
		}
	}

	normalizeArray1D(msgTmp);

	// 3. set new message from node1 to node2
	node2->adjacentNodeNewMessageToMe[index] = msgTmp;
}

void PartAssembling::updateMessagesAmongNodes(FRSTree * e)
{
	for (int i = 0; i < ConstructTreeNum; i++)
	{
		for (int j = 0; j < PartNum; j++)
		{
			BpNode * node2 = graph.at(i, j);
			for (int k = 0; k < node2->adjacentNodeNumber; k++)
			{
				BpNode * node1 = node2->adjacentNodes[k];
				if (node1 != NULL)
				{
					int frameNum = i;
					computeMessageFromNode1ToNode2(e[i], node1, k, frameNum/*i - node2->adjacentType[k]*/, node2);
				}
			}
		}
	}

	double alpha = 0.5;

	for (int i = 0; i < ConstructTreeNum; i++)
	{
		for (int j = 0; j < PartNum; j++)
		{
			BpNode * node = graph.at(i, j);
			for (int k = 0; k < node->adjacentNodeNumber; k++)
			{
				BpNode * adjacentNode = node->adjacentNodes[k];
				if (adjacentNode != NULL)
				{
					// node->adjacentNodeMessageToMe[k] = node->adjacentNodeNewMessageToMe[k];
					for (int m = 0; m < SourceNum; m++)
					{
						node->adjacentNodeMessageToMe[k][m] = (1 - alpha) * node->adjacentNodeMessageToMe[k][m] + alpha * node->adjacentNodeNewMessageToMe[k][m];
					}

				}
			}
		}
	}
}

void PartAssembling::computeBelief(FRSTree * e)
{
	for (int i = 0; i < ConstructTreeNum; i++)
	{
		for (int j = 0; j < PartNum; j++)
		{
			BpNode * node = graph.at(i, j);

			node->belief = this->loadImportanceFromFRSTree(e[i],PartNum);
			filterImportance(node->belief, i);

			for (int k = 0; k < node->adjacentNodeNumber; k++)
			{
				BpNode * adjacentNode = node->adjacentNodes[k];
				if (adjacentNode != NULL)
				{
					node->belief *= node->adjacentNodeMessageToMe[k];
				}
			}
		}
	}
}

void normalizeArray1D(LibIV::Memory::Array::Array1D<double> & arr)
{
	double maxVal = -100000000;
	for (uint i = 0; i < arr.size(); i++)
	{
		if (arr[i] > maxVal)
			maxVal = arr[i];
	}

	for (uint i = 0; i < arr.size(); i++)
	{
		arr[i] /= maxVal;
	}
}

PartAssembling::PartAssembling(int PartNum, int SourceNum, int ConstructTreeNum)
{
	this->PartNum = PartNum;
	this->SourceNum = SourceNum;
	this->ConstructTreeNum = ConstructTreeNum;
	lookupTableForTemporalChorologicalTerm = new double[SourceNum];
	lookupTableForFilteringImportance = new double[SourceNum];
}

PartAssembling::~PartAssembling()
{
	delete[] partImportance;
	freeMemoryOfGraph();
}

void PartAssembling::initialize()
{
	allocateMemoryForGraph();
	setRelationsBetweenNodesOfGraph();
	initLookupTable();

	partImportance = new Importance[PartNum];

}

void PartAssembling::initLookupTable()
{
	for (int i = 0; i < SourceNum; i++)
	{
		lookupTableForTemporalChorologicalTerm[i] = exp(-0.5 * i * i / (sigmaTemporal * sigmaTemporal));
		lookupTableForFilteringImportance[i] = exp(-0.5 * i * i / (sigmaImportance * sigmaImportance));
	}
}

void PartAssembling::filterImportance(Importance & imp, int indexInSynopsis)
{
	double scale = (double)SourceNum / (double)ConstructTreeNum;
	int indexInVideo = (int)(scale * indexInSynopsis);

	for (int i = 0; i < SourceNum; i++)
	{
		int indexInLookupTable = indexInVideo > i ? indexInVideo - i : i - indexInVideo;
		imp[i] *= lookupTableForFilteringImportance[indexInLookupTable];
	}
}

void PartAssembling::allocateMemoryForGraph()
{
	graph.set(PartNum, ConstructTreeNum);
	graph.fill(0);

	for (int i = 0; i < ConstructTreeNum; i++)
	{
		for (int j = 0; j < PartNum; j++)
		{
			graph.at(i, j) = new BpNode;
		}
	}
}

void PartAssembling::freeMemoryOfGraph()
{
	for (int i = 0; i < ConstructTreeNum; i++)
	{
		for (int j = 0; j < PartNum; j++)
		{
			delete[] graph.at(i, j)->adjacentNodes;
			delete[] graph.at(i, j)->adjacentType;
			delete[] graph.at(i, j)->adjacentNodeMessageToMe;
			delete[] graph.at(i, j)->adjacentNodeNewMessageToMe;
			delete graph.at(i, j);
		}
	}
}

// this function must be manually specified for each input video
void PartAssembling::setRelationsBetweenNodesOfGraph()
{
	// for Fig.3 in the work, the FRSTree
	//				    0.first area
	//                     |
	//                  1.second area
	//                     |
	//					2.third area
	//					   |
	//					3.forth area
	//

	for (int i = 0; i < ConstructTreeNum; i++)
	{
		// initialize the first area
		graph.at(i, 0)->belief.set(SourceNum);
		graph.at(i, 0)->nodeType = 0;
		graph.at(i, 0)->adjacentNodeNumber = 1;
		graph.at(i, 0)->adjacentNodes = new BpNode*[1];

		graph.at(i, 0)->adjacentNodes[0] = graph.at(i, 1);

		//notice these labels£¬it's important.
		graph.at(i, 0)->adjacentType = new int[1];
		graph.at(i, 0)->adjacentType[0] = 0;

		graph.at(i, 0)->adjacentNodeMessageToMe = new BpMessage[1];
		graph.at(i, 0)->adjacentNodeMessageToMe[0].set(SourceNum);
		graph.at(i, 0)->adjacentNodeMessageToMe[0].fill(1.0 / SourceNum);

		graph.at(i, 0)->adjacentNodeNewMessageToMe = new BpMessage[1];
		graph.at(i, 0)->adjacentNodeNewMessageToMe[0].set(SourceNum);
		graph.at(i, 0)->adjacentNodeNewMessageToMe[0].fill(1.0 / SourceNum);


		// initialize the second area
		graph.at(i, 1)->belief.set(SourceNum);
		graph.at(i, 1)->nodeType = 1;
		graph.at(i, 1)->adjacentNodeNumber = 2;
		graph.at(i, 1)->adjacentNodes = new BpNode*[2];

		graph.at(i, 1)->adjacentNodes[0] = graph.at(i, 0);
		graph.at(i, 1)->adjacentNodes[1] = graph.at(i, 2);


		graph.at(i, 1)->adjacentType = new int[2];
		graph.at(i, 1)->adjacentType[1] = 3;
		graph.at(i, 1)->adjacentType[2] = 3;

		graph.at(i, 1)->adjacentNodeMessageToMe = new BpMessage[2];
		graph.at(i, 1)->adjacentNodeMessageToMe[0].set(SourceNum);
		graph.at(i, 1)->adjacentNodeMessageToMe[0].fill(1.0 / SourceNum);
		graph.at(i, 1)->adjacentNodeMessageToMe[1].set(SourceNum);
		graph.at(i, 1)->adjacentNodeMessageToMe[1].fill(1.0 / SourceNum);

		graph.at(i, 1)->adjacentNodeNewMessageToMe = new BpMessage[2];
		graph.at(i, 1)->adjacentNodeNewMessageToMe[0].set(SourceNum);
		graph.at(i, 1)->adjacentNodeNewMessageToMe[0].fill(1.0 / SourceNum);
		graph.at(i, 1)->adjacentNodeNewMessageToMe[1].set(SourceNum);
		graph.at(i, 1)->adjacentNodeNewMessageToMe[1].fill(1.0 / SourceNum);


		// initialize the third area
		graph.at(i, 2)->belief.set(SourceNum);
		graph.at(i, 2)->nodeType = 2;
		graph.at(i, 2)->adjacentNodeNumber = 2;
		graph.at(i, 2)->adjacentNodes = new BpNode*[2];

		graph.at(i, 2)->adjacentNodes[0] = graph.at(i, 1);
		graph.at(i, 2)->adjacentNodes[1] = graph.at(i, 3);

		graph.at(i, 2)->adjacentType = new int[2];
		graph.at(i, 2)->adjacentType[1] = 4;
		graph.at(i, 2)->adjacentType[2] = 4;;

		graph.at(i, 2)->adjacentNodeMessageToMe = new BpMessage[2];
		graph.at(i, 2)->adjacentNodeMessageToMe[0].set(SourceNum);
		graph.at(i, 2)->adjacentNodeMessageToMe[0].fill(1.0 / SourceNum);
		graph.at(i, 2)->adjacentNodeMessageToMe[1].set(SourceNum);
		graph.at(i, 2)->adjacentNodeMessageToMe[1].fill(1.0 / SourceNum);

		graph.at(i, 2)->adjacentNodeNewMessageToMe = new BpMessage[2];
		graph.at(i, 2)->adjacentNodeNewMessageToMe[0].set(SourceNum);
		graph.at(i, 2)->adjacentNodeNewMessageToMe[0].fill(1.0 / SourceNum);
		graph.at(i, 2)->adjacentNodeNewMessageToMe[1].set(SourceNum);
		graph.at(i, 2)->adjacentNodeNewMessageToMe[1].fill(1.0 / SourceNum);



		// initialize the right arm part
		graph.at(i, 3)->belief.set(SourceNum);
		graph.at(i, 3)->nodeType = 3;
		graph.at(i, 3)->adjacentNodeNumber = 1;
		graph.at(i, 3)->adjacentNodes = new BpNode*[1];

		graph.at(i, 3)->adjacentNodes[0] = graph.at(i, 2);


		graph.at(i, 3)->adjacentType = new int[1];
		graph.at(i, 3)->adjacentType[0] = 5;

		graph.at(i, 3)->adjacentNodeMessageToMe = new BpMessage[1];
		graph.at(i, 3)->adjacentNodeMessageToMe[0].set(SourceNum);
		graph.at(i, 3)->adjacentNodeMessageToMe[0].fill(1.0 / SourceNum);

		graph.at(i, 3)->adjacentNodeNewMessageToMe = new BpMessage[1];
		graph.at(i, 3)->adjacentNodeNewMessageToMe[0].set(SourceNum);
		graph.at(i, 3)->adjacentNodeNewMessageToMe[0].fill(1.0 / SourceNum);
	}
}

Importance PartAssembling::loadImportanceFromFRSTree(FRSTree e,int PartNum)
{
	cout << e.com_index_order.size() << endl;
	for (int i = 0; i < e.com_index_order.size(); i++)
	{
		for (int j = 0; j < e.com_index_order.at(PartNum).size(); j++)
		{
			cout << e.com_index_order.at(PartNum).at(j) << " ";
		}
		cout << endl;
	}
	Importance innerImportance;
	innerImportance.set(e.com_index_order.size());
	for (int k = 0; k < e.com_index_order.at(PartNum).size(); k++)
	{
		cout << e.com_index_order.at(PartNum).at(k) << " ";
		innerImportance[k] = e.com_index_order.at(PartNum).at(k);
	}

	return innerImportance;
}

void PartAssembling::printResult(FRSTree * e)
{
	for (int i = 0; i<ConstructTreeNum; i++)
	{
		e[i].correc_indec.clear();
		for (int j = 0; j<PartNum; j++)
		{
			BpNode * node = graph.at(i, j);
			double maxBelief = -1000000;
			double maxIndex = 0;
			for (int k = 0; k<SourceNum; k++)
			{
				if (node->belief[k] > maxBelief)
				{
					maxBelief = node->belief[k];
					maxIndex = k;
				}
			}
			e[i].correc_indec.push_back(maxBelief);
		}
	}
	cout << "end BP!" << endl;
}
