#include "BpKernel.h"

// 2015.05.16	node->adjacent_nodes[index] = adjacent_node
void BpKernel::ComputeNewMessageFromAdjacentnodeToNode(BpNode * adjacent_node, int index, BpNode * node, const LabelSimilarity & label_similarity)
{
	// Compute belief of adjacent_node
	LocalEvidence msg = adjacent_node->local_evidence;
	
	for(int i = 0;i<adjacent_node->adjacent_node_number;i++)
	{
		BpNode * adj_adj_node = adjacent_node->adjacent_nodes[i];

		if(adj_adj_node != 0 && adj_adj_node != node)
		{
			msg *= adjacent_node->adjacent_node_message_to_me[i];
		}
	}

	LibIV::Memory::Array::NormalizeArrayDouble1D(msg);

	// compute message from adjacent_node to node
	int label_num = (int)(msg.size());
	BpMessage msgTmp;
	msgTmp.set(label_num);
	msgTmp.fill(0);

	for(int i = 0;i<label_num;i++)
	{
		for(int j = 0;j<label_num;j++)
			msgTmp[i] += label_similarity.at(i,j) * msg[j];
	}

	LibIV::Memory::Array::NormalizeArrayDouble1D(msgTmp);

	// note: this is a new message sent
	node->adjacent_node_new_message_to_me[index] = msgTmp;
}

void BpKernel::UpdateMessageOfNodes(const std::vector<BpNode*> & all_nodes, const LabelSimilarity & label_similarity)
{
	for(size_t i = 0;i<all_nodes.size();i++)
	{
		BpNode * node = all_nodes[i];
		for(int j = 0;j<node->adjacent_node_number;j++)
		{
			BpNode * adjacent_node = node->adjacent_nodes[j];
			if(adjacent_node != 0)
				ComputeNewMessageFromAdjacentnodeToNode(adjacent_node,j,node, label_similarity);
		}
	}

	double alpha = 0.5;
	
	for(size_t i = 0;i<all_nodes.size();i++)
	{
		BpNode * node = all_nodes[i];
		for(int j = 0;j<node->adjacent_node_number;j++)
		{
			BpNode * adjacent_node = node->adjacent_nodes[j];
			if(adjacent_node != 0)
			{
				node->adjacent_node_message_to_me[j] = 
					(node->adjacent_node_message_to_me[j]) * (1-alpha) + 
					(node->adjacent_node_new_message_to_me[j]) * alpha;


				LibIV::Memory::Array::NormalizeArrayDouble1D(node->adjacent_node_message_to_me[j]);
			}
		}
	}
}

void BpKernel::ComputeBeliefofNodes(const std::vector<BpNode*> & all_nodes)
{
	for(size_t i = 0;i<all_nodes.size();i++)
	{
		BpNode * node = all_nodes[i];
		node->belief = node->local_evidence;
		for(int j = 0;j<node->adjacent_node_number;j++)
		{
			if(node->adjacent_nodes[j] != 0)
				node->belief *= node->adjacent_node_message_to_me[j];
		}
		LibIV::Memory::Array::NormalizeArrayDouble1D(node->belief);
	}
}

void BpKernel::LoopyBp(const std::vector<BpNode*> & all_nodes, const LabelSimilarity & label_similarity, int num)
{
	 while(num-->0)
		 UpdateMessageOfNodes(all_nodes,label_similarity);
}