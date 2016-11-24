#include "Grid4ConnectedUndirectMRF.h"

Grid4ConnectedUndirectMRF::Grid4ConnectedUndirectMRF()
{
	
}

Grid4ConnectedUndirectMRF::~Grid4ConnectedUndirectMRF()
{
	this->Free();
}

void Grid4ConnectedUndirectMRF::Free()
{
	for(uint i = 0;i<mrf_.rows();i++)
	{
		for(uint j = 0;j<mrf_.cols();j++)
		{
			if(this->mrf_.at(i,j))
			{
				delete [] mrf_.at(i,j)->adjacent_nodes;
				delete [] mrf_.at(i,j)->adjacent_node_message_to_me;
				delete [] mrf_.at(i,j)->adjacent_node_new_message_to_me;
				delete [] mrf_.at(i,j);
			}
		}
	}
}

void Grid4ConnectedUndirectMRF::Initialize(int w, int h, int labels)
{
	Free();
	
	mrf_.set(w,h);
	
	for(int j = 0;j<w;j++)
	{
		for(int i = 0;i<h;i++)
			mrf_.at(i,j) = new BpNode();
	}

	for(int j = 0;j<w;j++)
	{
		for(int i = 0;i<h;i++)
		{
			mrf_.at(i,j)->adjacent_node_number = 4;
			
			// set the connections 
			//             0
			//         3       1
			//             2
			mrf_.at(i,j)->adjacent_nodes = new BpNode*[4];
			if(i == 0) 
				mrf_.at(i,j)->adjacent_nodes[0] = 0;
			else
				mrf_.at(i,j)->adjacent_nodes[0] = mrf_.at(i-1,j);

			if(j == w-1)
				mrf_.at(i,j)->adjacent_nodes[1] = 0;
			else
				mrf_.at(i,j)->adjacent_nodes[1] = mrf_.at(i,j+1);

			if(i == h-1)
				mrf_.at(i,j)->adjacent_nodes[2] = 0;
			else
				mrf_.at(i,j)->adjacent_nodes[2] = mrf_.at(i+1,j);

			if(j == 0)
				mrf_.at(i,j)->adjacent_nodes[3] = 0;
			else
				mrf_.at(i,j)->adjacent_nodes[3] = mrf_.at(i,j-1);

			mrf_.at(i,j)->adjacent_node_message_to_me = new BpMessage[4];
			mrf_.at(i,j)->adjacent_node_message_to_me[0].set(labels);
			mrf_.at(i,j)->adjacent_node_message_to_me[1].set(labels);
			mrf_.at(i,j)->adjacent_node_message_to_me[2].set(labels);
			mrf_.at(i,j)->adjacent_node_message_to_me[3].set(labels);
			mrf_.at(i,j)->adjacent_node_message_to_me[0].fill(1.0/labels);
			mrf_.at(i,j)->adjacent_node_message_to_me[1].fill(1.0/labels);
			mrf_.at(i,j)->adjacent_node_message_to_me[2].fill(1.0/labels);
			mrf_.at(i,j)->adjacent_node_message_to_me[3].fill(1.0/labels);


			mrf_.at(i,j)->adjacent_node_new_message_to_me = new BpMessage[4];
			mrf_.at(i,j)->adjacent_node_new_message_to_me[0].set(labels);
			mrf_.at(i,j)->adjacent_node_new_message_to_me[1].set(labels);
			mrf_.at(i,j)->adjacent_node_new_message_to_me[2].set(labels);
			mrf_.at(i,j)->adjacent_node_new_message_to_me[3].set(labels);
			mrf_.at(i,j)->adjacent_node_new_message_to_me[0].fill(1.0/labels);
			mrf_.at(i,j)->adjacent_node_new_message_to_me[1].fill(1.0/labels);
			mrf_.at(i,j)->adjacent_node_new_message_to_me[2].fill(1.0/labels);
			mrf_.at(i,j)->adjacent_node_new_message_to_me[3].fill(1.0/labels);

			mrf_.at(i,j)->belief.set(labels);
			mrf_.at(i,j)->belief.fill(0);

			mrf_.at(i,j)->local_evidence.set(labels);
			mrf_.at(i,j)->local_evidence.fill(0);
		}
	}
}

/*void Grid4ConnectedUndirectMRF::Destroy()
{
	for(uint i = 0;i<this->mrf_.rows();i++)
	{
		for(uint j = 0;j<this->mrf_.cols();j++)
		{
			if(this->mrf_.at(i,j))
			{
				delete [] this->mrf_.at(i,j)->adjacent_nodes;
				delete [] this->mrf_.at(i,j)->adjacent_node_message_to_me;
				delete [] this->mrf_.at(i,j)->adjacent_node_new_message_to_me;
				delete [] this->mrf_.at(i,j);
			}
		}
	}
}*/

/*void Grid4ConnectedUndirectMRF::Initialize(int manifold_x, int manifold_y, int grid_size, int frs_region_num, int example_num) 
{
	int a  = manifold_x / grid_size;
	int aa = manifold_x % grid_size;
	if(aa != 0) a+=1;

	int b  = manifold_y / grid_size;
	int bb = manifold_y % grid_size;
	if(bb != 0) b+=1;

	this->MRF_node_pos_.set(a, b);

	for(int j = 0;j<a;j++)
	{
		for(int i = 0;i<b;i++)
			this->MRF_node_pos_.at(i,j) = _v2d_(j * grid_size, i * grid_size);
	}

	this->mrf_.set(a, b);
	this->mrf_.fill(0);

	for(int j = 0;j<a;j++)
	{
		for(int i = 0;i<b;i++)
			this->mrf_.at(i,j) = new BpNode;
	}

	for(int j = 0;j<a;j++)
	{
		for(int i = 0;i<b;i++)
		{
			this->mrf_.at(i,j)->belief.set(frs_region_num * example_num);
			this->mrf_.at(i,j)->adjacent_node_number = 4;
			this->mrf_.at(i,j)->adjacent_nodes = new BpNode*[4];
			
			if(i == 0) 
				this->mrf_.at(i,j)->adjacent_nodes[0] = 0;
			else
				this->mrf_.at(i,j)->adjacent_nodes[0] = this->mrf_.at(i-1,j);

			if(j == a-1)
				this->mrf_.at(i,j)->adjacent_nodes[1] = 0;
			else
				this->mrf_.at(i,j)->adjacent_nodes[1] = this->mrf_.at(i,j+1);

			if(i == b-1)
				this->mrf_.at(i,j)->adjacent_nodes[2] = 0;
			else
				this->mrf_.at(i,j)->adjacent_nodes[2] = this->mrf_.at(i+1,j);

			if(j == 0)
				this->mrf_.at(i,j)->adjacent_nodes[3] = 0;
			else
				this->mrf_.at(i,j)->adjacent_nodes[3] = this->mrf_.at(i,j-1);

			
		}
	}
}

void Grid4ConnectedUndirectMRF::ShowSomething()
{
	for(uint i = 0;i<this->MRF_node_pos_.rows();i++)
	{
		for(uint j = 0;j<this->MRF_node_pos_.cols();j++)
			std::cout<<"("<<this->MRF_node_pos_.at(i,j)[0]<<","<<this->MRF_node_pos_.at(i,j)[1]<<") ";
		std::cout<<std::endl;
	}
}*/

void Grid4ConnectedUndirectMRF::SetLocalEvidence()
{
	mrf_.at(0,0)->local_evidence[0] = 0.9;
	mrf_.at(0,0)->local_evidence[1] = 0.1;
	
	mrf_.at(0,1)->local_evidence[0] = 0.6;
	mrf_.at(0,1)->local_evidence[1] = 0.4;
	
	mrf_.at(1,0)->local_evidence[0] = 0.1;
	mrf_.at(1,0)->local_evidence[1] = 0.9;

	mrf_.at(1,1)->local_evidence[0] = 0.3;
	mrf_.at(1,1)->local_evidence[1] = 0.7;
}

void Grid4ConnectedUndirectMRF::SetLabelSimilarity()
{
	this->ls_.set(2,2);
	ls_.at(0,0) = 1;
	ls_.at(0,1) = 0.01;
	ls_.at(1,0) = 0.01;
	ls_.at(1,1) = 1;
}

void Grid4ConnectedUndirectMRF::ComputeBeliefForMRF()
{
	this->Initialize(2,2,2);
	this->SetLocalEvidence();
	this->SetLabelSimilarity();

	std::vector<BpNode*> all_nodes;
	for(uint i = 0;i<mrf_.rows();i++)
	{
		for(uint j = 0;j<mrf_.cols();j++)
			all_nodes.push_back(mrf_.at(i,j));
	}

	BpKernel::LoopyBp(all_nodes,ls_,10);
	BpKernel::ComputeBeliefofNodes(all_nodes);

	for(uint i = 0;i<mrf_.rows();i++)
	{
		for(uint j = 0;j<mrf_.cols();j++)
		{
			for(int k = 0;k<2;k++)
			{
				std::cout<<mrf_.at(i,j)->belief[k]<<",";
			}
			std::cout<<std::endl;
		}
	}
}