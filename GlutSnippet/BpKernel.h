#ifndef BP_KERNEL_H_
#define BP_KERNEL_H_

#include <LibIV\libiv.h>

typedef LibIV::Memory::Array::Array1D<double> BpMessage;
typedef LibIV::Memory::Array::Array1D<double> BpBelief;
typedef LibIV::Memory::Array::Array1D<double> LocalEvidence;


struct BpNode
{
	int				adjacent_node_number;
	BpNode **		adjacent_nodes;

	BpMessage *		adjacent_node_message_to_me;
	BpMessage *		adjacent_node_new_message_to_me;

	BpBelief  		belief;
	
	LocalEvidence	local_evidence;
}; // end of BpNode

// 2015.05.16	BpKernel does not know the structure of MRF graph
//typedef LibIV::Memory::Array::FastArray2D<BpNode*> MRFGraph;

typedef LibIV::Memory::Array::FastArray2D<double>  LabelSimilarity;

class BpKernel
{
public:
	static void LoopyBp(const std::vector<BpNode*> & all_nodes, const LabelSimilarity & label_similarity, int num);
	static void ComputeBeliefofNodes(const std::vector<BpNode*> & all_nodes);
protected:
	static void ComputeNewMessageFromAdjacentnodeToNode(BpNode * adjacent_node, int index, BpNode * node, const LabelSimilarity & label_similarity);
	static void UpdateMessageOfNodes(const std::vector<BpNode*> & all_nodes, const LabelSimilarity & label_similarity);
};

#endif // end of BP_KERNEL_H_