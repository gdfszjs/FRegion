#include "FRStructure.h"
#include "SVGPattern.h"
#include "FRNode.h"
#include "CommonGl.h"
#include "SVGElement.h"
#include "Geo2D.h"

/* =============================================================================== */
/*	FRSNode																		   */
/* =============================================================================== */

FRSNode::FRSNode()
{
	this->partition_frnode_index_ = -1;
	this->left_child_ = 0;
	this->right_child_ = 0;
	this->parrent_ = 0;
	this->t1_ = 0;
	this->t2_ = 0;
	this->n1_ = -1;
	this->n2_ = -1;
}

FRSNode::~FRSNode()
{
	if (this->left_child_ != 0)
		delete this->left_child_;
	if (this->right_child_ != 0)
		delete this->right_child_;
}

double FRSNode::TotalBoundaryLength(FRSTree * tree)
{
	double total_length = 0;
	for (size_t i = 0; i < frnode_indices_.size(); i++)
	{
		FRNode * node = (FRNode *)(tree->all_frnodes_[frnode_indices_[i]]);
		total_length += node->Length(1);
	}
	return total_length;
}

double FRSNode::EndpointGlobalProportion(FRSTree * tree, int n, double t)
{
	double total_length = this->TotalBoundaryLength(tree);
	double end_length = 0;
	for (int i = 0; i < n; i++)
	{
		FRNode * node = (FRNode *)(tree->all_frnodes_[frnode_indices_[i]]);
		end_length += node->Length(1);
	}

	FRNode * cnode = (FRNode*)(tree->all_frnodes_[frnode_indices_[n]]);
	end_length += cnode->Length(t);

	return (end_length / total_length);
}

double FRSNode::FirstEndpointGlobalProportion(FRSTree * tree)
{
	return (EndpointGlobalProportion(tree, n1_, t1_));
}

double FRSNode::SecondEndpointGlobalProportion(FRSTree * tree)
{
	return (EndpointGlobalProportion(tree, n2_, t2_));
}

void FRSNode::Compute_n_and_t_ByGlobalProportion(FRSTree * tree, double p, int & n, double & t)
{
	if (p < 0) p = 0.001;
	if (p > 1) p = 0.999;

	double tl = this->TotalBoundaryLength(tree);
	double cl = p * tl;

	double l0 = 0;
	int    cn = 0;
	n = -1;

	for (size_t i = 0; i < this->frnode_indices_.size(); i++)
	{
		FRNode * node = tree->all_frnodes_[this->frnode_indices_[i]];
		double l1 = l0 + node->Length(1);
		if (cl >= l0 && cl < l1)
		{
			n = cn;
			break;
		}
		else
		{
			cn++;
			l0 = l1;
		}
	}

	if (n == -1)
	{
		cout << p << endl;
		cout << "error - FRSNode::Compute_n_and_t_ByGlobalProportion" << endl;
	}

	double l = cl - l0;
	FRNode * node = tree->all_frnodes_[this->frnode_indices_[cn]];
	t = node->LengthTo_t(l);
}

void FRSNode::codeFRSNode(FRSTree * tree, int treenum, int partnum)
{
	//计算区域最高点
	double h_x = 0;
	double h_y = 0;
	FRNode *l_b = NULL;
	for (size_t i = 0; i < this->frnode_indices_.size(); i++)
	{
		l_b = tree->all_frnodes_.at(this->frnode_indices_.at(i));
		//line == 0
		//bezier == 1
		if (l_b->NodeType() == 0)
		{
			if (l_b->pts(0)[1] > h_y)
			{
				h_x = l_b->pts(0)[0];
				h_y = l_b->pts(0)[1];
			}

			if (l_b->pts(1)[1] > h_y)
			{
				h_x = l_b->pts(1)[0];
				h_y = l_b->pts(1)[1];
			}
		}
		else
		{
			if (l_b->pts(0)[1] > h_y)
			{
				h_x = l_b->pts(0)[0];
				h_y = l_b->pts(0)[1];
			}

			if (l_b->pts(3)[1] > h_y)
			{
				h_x = l_b->pts(3)[0];
				h_y = l_b->pts(3)[1];
			}
		}

	}
	//计算区域最低点
	double l_x = 10000;
	double l_y = 10000;
	for (size_t i = 0; i < this->frnode_indices_.size(); i++)
	{
		l_b = tree->all_frnodes_.at(this->frnode_indices_.at(i));
		//line == 0
		//bezier == 1
		if (l_b->NodeType() == 0)
		{
			if (l_b->pts(0)[1] < l_y)
			{
				l_x = l_b->pts(0)[0];
				l_y = l_b->pts(0)[1];
			}

			if (l_b->pts(1)[1] < l_y)
			{
				l_x = l_b->pts(1)[0];
				l_y = l_b->pts(1)[1];
			}
		}
		else
		{
			if (l_b->pts(0)[1] < l_y)
			{
				l_x = l_b->pts(0)[0];
				l_y = l_b->pts(0)[1];
			}

			if (l_b->pts(3)[1] < l_y)
			{
				l_x = l_b->pts(3)[0];
				l_y = l_b->pts(3)[1];
			}
		}

	}
	cout << h_x << " " << h_y << endl;
	cout << l_x << " " << l_y << endl;
	//划分range
	double height = h_y - l_y;
	int rangeindex = 8;
	bool done = false;
	vector<vector<vector<v2d>>> range_vector;
	for (int i = 0; i < rangeindex; i++)
	{
		vector<vector<v2d>> point_vilage_vector;
		range_vector.push_back(point_vilage_vector);
	}
	vector<v4d> range_message;
	vector<int> point_vilage;
	for (int i = 0; i < rangeindex; i++)
	{
		double range_l = i * (1.0 / rangeindex);
		double range_u = (i + 1) * (1.0 / rangeindex);

		double height_l = l_y + i * (height / rangeindex);
		double height_u = l_y + (i + 1) * (height / rangeindex);

		range_message.push_back(_v4d_(range_l, range_u, height_l, height_u));
	}

	for (int i = 0; i < this->element_indices_.size(); i++)
	{
		vector<v3d> ele_vector;
		SVGElement * e = tree->pattern_->elements_.at(this->element_indices_.at(i));

		//concat all the segs
		for (int j = 0; j < e->segs_.size(); j++)
		{
			if (e->segs_.at(j)->SegType() == 0)
			{
				//is a line
				for (int k = 0; k < e->segs_.at(j)->samplepoints.size(); k++)
				{
					v2d geted_point = e->segs_.at(j)->samplepoints.at(k);
					ele_vector.push_back(_v3d_(geted_point[0], geted_point[1], 0));
				}

			}
			if (e->segs_.at(j)->SegType() == 1)
			{
				//is a bezier
				e->segs_.at(j)->getsamplepoint();
				for (int k = 0; k < e->segs_.at(j)->samplepoints.size(); k++)
				{
					int point_range = 0;
					v2d geted_point = e->segs_.at(j)->samplepoints.at(k);
					for (int l = 0; l < range_message.size(); l++)
					{
						double s_y = geted_point[1];
						double g_y_l = range_message.at(l)[2];
						double g_y_u = range_message.at(l)[3];
						if (s_y <= g_y_u)
						{
							if (s_y >= g_y_l)
							{
								point_range = l;
							}
						}
					}
					ele_vector.push_back(_v3d_(geted_point[0], geted_point[1], point_range));
				}
			}
		}

		if (abs(ele_vector.at(0)[0] - ele_vector.back()[0]) <= 0.001 && abs(ele_vector.at(0)[1] == ele_vector.back()[1]) <= 0.001)
		{
			int start_index = 0;
			cout << "is a circle!" << endl;
			for (int j = 0; j < ele_vector.size(); j++)
			{
				if (j != 0 && j != ele_vector.size() - 1)
				{
					if (ele_vector.at(j)[2] != ele_vector.at(j - 1)[2])
					{
						start_index = j;
						break;
					}
				}
			}	
			point_vilage.push_back(start_index);
			int k = start_index + 1;
			while (k != start_index)
			{
				if (k == ele_vector.size() - 1)
				{
					//the first point of a vector
					if (ele_vector.at(k)[2] != ele_vector.at(k - 1)[2])
					{
						point_vilage.push_back(k);

					}
					//the last point of a vector
					if (ele_vector.at(k)[2] != ele_vector.at(0)[2])
					{
						point_vilage.push_back(k);
						point_vilage.push_back(ele_vector.at(k)[2]);
					}
				}
				else if (k == 0)
				{
					//the first point of a vector
					if (ele_vector.at(k)[2] != ele_vector.back()[2])
					{
						point_vilage.push_back(k);

					}
					//the last point of a vector
					if (ele_vector.at(k)[2] != ele_vector.at(k + 1)[2])
					{
						point_vilage.push_back(k);
						point_vilage.push_back(ele_vector.at(k)[2]);
					}
				}
				else
				{
					if (ele_vector.at(k)[2] != ele_vector.at(k - 1)[2])
					{
						point_vilage.push_back(k);

					}
					//the last point of a vector
					if (ele_vector.at(k)[2] != ele_vector.at(k + 1)[2])
					{
						point_vilage.push_back(k);
						point_vilage.push_back(ele_vector.at(k)[2]);
					}
				}

				if (k == ele_vector.size() - 1)
				{
					k = 0;
				}
				else
				{
					k++;
				}
			}

			for (int j = 0; j < point_vilage.size(); j = j + 3)
			{
				vector<v2d> point_vector;
				for (int k = point_vilage.at(j); k < point_vilage.at(j + 1); k++)
				{
					point_vector.push_back(_v2d_(ele_vector.at(k)[0], ele_vector.at(k)[1]));
				}
				range_vector.at(ele_vector.at(point_vilage.at(j))[2]).push_back(point_vector);
			}

		}
		else
		{

			for (int j = 0; j < ele_vector.size(); j++)
			{
				if (j == 0)
				{
					point_vilage.push_back(j);
				}
				else if (j == ele_vector.size() - 1)
				{
					point_vilage.push_back(j);
					point_vilage.push_back(ele_vector.at(j)[2]);
				}
				else
				{
					//the first point of a vector
					if (ele_vector.at(j)[2] != ele_vector.at(j - 1)[2])
					{
						point_vilage.push_back(j);

					}
					//the last point of a vector
					if (ele_vector.at(j)[2] != ele_vector.at(j + 1)[2])
					{
						point_vilage.push_back(j);
						point_vilage.push_back(ele_vector.at(j)[2]);
					}

				}
			}

			for (int j = 0; j < point_vilage.size(); j = j + 3)
			{
				vector<v2d> point_vector;
				for (int k = point_vilage.at(j); k < point_vilage.at(j + 1); k++)
				{
					point_vector.push_back(_v2d_(ele_vector.at(k)[0], ele_vector.at(k)[1]));
				}
				range_vector.at(ele_vector.at(point_vilage.at(j))[2]).push_back(point_vector);
			}

		}

	}


	//存储进文件中
	for (int i = 0; i < rangeindex; i++)
	{
		stringstream ss;
		string line;
		ss << "D:\\MRGTrueFile\\" << treenum + 1 << "\\"<< partnum + 1 << "\\0 " << i << " " << i + 1 << ".txt";
		cout << ss.str() << endl;
		ofstream f1(ss.str());
		for (int j = 0; j < range_vector.at(i).size(); j++)
		{
			f1 << "one node" << endl;
			for (int k = 0; k < range_vector.at(i).at(j).size(); k++)
			{
				f1 << range_vector.at(i).at(j).at(k)[0] << " " << range_vector.at(i).at(j).at(k)[1] << endl;
			}
		}
		f1.close();
	}

	cout << "end coding" << endl;
}

void FRSNode::BinaryTreeTo1DArray(int * arr, int index)
{
	arr[index] = 1;
	if (this->left_child_ != 0)
		this->left_child_->BinaryTreeTo1DArray(arr, index * 2 + 1);
	if (this->right_child_ != 0)
		this->right_child_->BinaryTreeTo1DArray(arr, index * 2 + 2);
}

void FRSNode::SampleNPoints(std::vector<v2d> & pts, int N, FRSTree * tree)
{
	double d, dd = 0;
	for (size_t i = 0; i < this->frnode_indices_.size(); i++)
		dd += tree->all_frnodes_[this->frnode_indices_[i]]->Length(1);

	d = dd / N;

	for (int i = 0; i < N; i++)
	{
		double cd = i * d;

		dd = 0;
		for (size_t j = 0; j < this->frnode_indices_.size(); j++)
		{
			double ldd = dd;
			dd += tree->all_frnodes_[this->frnode_indices_[j]]->Length(1);

			if (cd >= ldd && cd <= dd)
			{
				double ccd = cd - ldd;
				double t = tree->all_frnodes_[this->frnode_indices_[j]]->LengthTo_t(ccd);
				v2d tmp;
				tree->all_frnodes_[this->frnode_indices_[j]]->tToPoint(tmp, t);
				pts.push_back(tmp);
				break;
			}
		}
	}
}

void FRSNode::SampleNPoints2(std::vector<v2d> & pts, int N, FRSTree * tree)
{
	double tt[4][3] = { {0,0.4,0.7}, {0.2,0.5,0.8}, {0.3,0.7,0},{0.3,0.7,0} };
	double nn[4] = { 3,3,2,2 };

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < nn[i]; j++)
		{
			double t = tt[i][j];

			v2d tmp;
			tree->all_frnodes_[this->frnode_indices_[i]]->tToPoint(tmp, t);

			pts.push_back(tmp);
		}
	}
}

/* =============================================================================== */
/*	FRSTree																		   */
/* =============================================================================== */

FRSTree::FRSTree()
{
	this->pattern_ = 0;
	this->frs_root_node_ = 0;
	this->current_node_ = 0;
	this->fr_angle_ = 0;
}

FRSTree::~FRSTree()
{
	if (this->pattern_ != 0)
		delete this->pattern_;
	if (this->frs_root_node_ != 0)
		delete this->frs_root_node_;
	for (size_t i = 0; i < this->all_frnodes_.size(); i++)
		delete this->all_frnodes_[i];
}

int FRSTree::IndexOfNode(FRNode * node)
{
	for (size_t i = 0; i < this->all_frnodes_.size(); i++)
	{
		if (node == this->all_frnodes_[i])
			return (int)(i);
	}

	return -1;
}

int FRSTree::IndexOfElement(SVGElement* element)
{
	for (size_t i = 0; i < this->pattern_->elements_.size(); i++)
	{
		if (element == this->pattern_->elements_[i])
			return (int)(i);
	}

	return -1;
}

void FRSTree::GetCurrentElements(std::vector<SVGElement*> & eles)
{
	if (this->current_node_ == 0) return;
	this->GetElementsFromFRSNode(this->current_node_, eles);
}

void FRSTree::GetCurrentFRNodes(std::vector<FRNode*> & nodes)
{
	if (this->current_node_ == 0) return;
	this->GetFRNodesFromFRSNode(this->current_node_, nodes);
}

void FRSTree::GetFRNodesFromFRSNode(const FRSNode * node, std::vector<FRNode*> & nodes)
{
	nodes.clear();
	for (size_t i = 0; i < node->frnode_indices_.size(); i++)
		nodes.push_back(this->all_frnodes_[node->frnode_indices_[i]]);
}

void FRSTree::GetElementsFromFRSNode(const FRSNode * node, std::vector<SVGElement*> & eles)
{
	eles.clear();
	for (size_t i = 0; i < node->element_indices_.size(); i++)
		eles.push_back(this->pattern_->elements_[node->element_indices_[i]]);
}

void FRSTree::DrawFRSNode(const FRSNode * node, bool decompose, bool draw_ele, double hoffset)
{
	/*if(node == this->current_node_)
		glColor3f(1,0,0);*/
		/*FRSNode * nn = this->FindFRSNodeByArrayIndex(6);
		if(nn == node)
		{
			glColor3d(1,0,0);
			glLineWidth(3);
		}*/
	for (size_t i = 0; i < node->frnode_indices_.size(); i++)
		this->all_frnodes_[node->frnode_indices_[i]]->Draw();
	/*if(nn == node)
	{
		glColor3f(0,0,0);
		glLineWidth(2);
	}*/

	if (draw_ele)
	{
		for (size_t i = 0; i < node->element_indices_.size(); i++)
			this->pattern_->elements_[node->element_indices_[i]]->drawElement();
	}

	//glColor3f(0,0,1);
	/*if(node->partition_frnode_index_ != -1)
		this->all_frnodes_[node->partition_frnode_index_]->Draw();*/
		//glColor3f(0,0,0);

		/*if(node == this->current_node_)
			glColor3f(1,1,1);*/

	glPushMatrix();
	if (decompose) glTranslated(-200 - hoffset / 2, -250, 0);
	if (node->left_child_ != 0) this->DrawFRSNode(node->left_child_, decompose, draw_ele, hoffset / 2);
	glPopMatrix();

	glPushMatrix();
	if (decompose) glTranslated(200 + hoffset / 2, -250, 0);
	if (node->right_child_ != 0) this->DrawFRSNode(node->right_child_, decompose, draw_ele, hoffset / 2);
	glPopMatrix();
}

void FRSTree::Draw(bool decompose, bool draw_ele)
{
	this->DrawFRSNode(this->frs_root_node_, decompose, draw_ele);
}

void FRSTree::SaveFRSNode(ofstream & outfile, const FRSNode * node)
{
	for (size_t i = 0; i < node->element_indices_.size(); i++)
		outfile << node->element_indices_[i] << ' ';
	outfile << '\n';

	for (size_t i = 0; i < node->frnode_indices_.size(); i++)
		outfile << node->frnode_indices_[i] << ' ';
	outfile << '\n';

	outfile << node->partition_frnode_index_ << '\n';

	outfile << node->n1_ << ' ' << node->n2_ << '\n';

	outfile << node->t1_ << ' ' << node->t2_ << '\n';

	if (node->left_child_)
	{
		outfile << "Left FRSNode\n";
		SaveFRSNode(outfile, node->left_child_);
		outfile << "End\n";
	}

	if (node->right_child_)
	{
		outfile << "Right FRSNode\n";
		SaveFRSNode(outfile, node->right_child_);
		outfile << "End\n";
	}
}

void FRSTree::Save(ofstream  & outfile)
{
	/* save the elements */
	if (this->pattern_ != 0) this->pattern_->Save(outfile);

	/* save the angle of the strcutre */
	outfile << "FR Angle\n";
	outfile << this->fr_angle() << '\n';
	outfile << "End\n";

	/* save the structure */
	for (size_t i = 0; i < this->all_frnodes_.size(); i++)
	{
		if (this->all_frnodes_[i]->NodeType() == 0)
		{
			outfile << "Line\n";

			v2d pt0 = this->all_frnodes_[i]->pts(0);
			v2d pt1 = this->all_frnodes_[i]->pts(1);

			outfile << pt0[0] << ' ' << pt0[1] << ' ' << pt1[0] << ' ' << pt1[1] << '\n';
		}
		else
		{
			outfile << "Bezier\n";

			v2d pt0 = this->all_frnodes_[i]->pts(0);
			v2d pt1 = this->all_frnodes_[i]->pts(1);
			v2d pt2 = this->all_frnodes_[i]->pts(2);
			v2d pt3 = this->all_frnodes_[i]->pts(3);

			outfile << pt0[0] << ' ' << pt0[1] << ' ' << pt1[0] << ' ' << pt1[1] << ' ' << pt2[0] << ' ' << pt2[1] << ' ' << pt3[0] << ' ' << pt3[1] << '\n';
		}

		int left_idx = IndexOfNode(this->all_frnodes_[i]->left_node());
		int right_idx = IndexOfNode(this->all_frnodes_[i]->right_node());

		outfile << left_idx << ' ' << right_idx << '\n';
	}

	if (this->frs_root_node_ != 0)
	{
		outfile << "Root FRSNode\n";
		this->SaveFRSNode(outfile, this->frs_root_node_);
		outfile << "End\n";
	}
}

void FRSTree::LoadFRSNode(ifstream & infile, FRSNode * node)
{
	string line;
	int index;

	getline(infile, line);
	istringstream iss1(line);
	while (iss1 >> index)
		node->element_indices_.push_back(index);

	getline(infile, line);
	istringstream iss2(line);
	while (iss2 >> index)
		node->frnode_indices_.push_back(index);

	getline(infile, line);
	istringstream(line) >> node->partition_frnode_index_;

	getline(infile, line);
	istringstream(line) >> node->n1_ >> node->n2_;

	getline(infile, line);
	istringstream(line) >> node->t1_ >> node->t2_;

	while (getline(infile, line) && line != "End")
	{
		if (line == "Left FRSNode")
		{
			FRSNode * node_left = new FRSNode;
			node_left->parrent_ = node;
			node->left_child_ = node_left;
			this->LoadFRSNode(infile, node_left);
		}
		else if (line == "Right FRSNode")
		{
			FRSNode * node_right = new FRSNode;
			node_right->parrent_ = node;
			node->right_child_ = node_right;
			this->LoadFRSNode(infile, node_right);
		}
	}
}

void FRSTree::Load(ifstream & infile)
{
	string line;

	while (getline(infile, line) && line != "")
	{
		if (line == "Pattern")
		{
			this->pattern_ = new SVGPattern();
			this->pattern_->Load(infile);
		}
		if (line == "Line")
		{
			string line1, line2;
			getline(infile, line1);
			getline(infile, line2);

			double a, b, c, d;
			istringstream(line1) >> a >> b >> c >> d;
			int e, f;
			istringstream(line2) >> e >> f;

			FRNodeLine * node = new FRNodeLine();
			node->set_pts(0, _v2d_(a, b));
			node->set_pts(1, _v2d_(c, d));

			this->all_frnodes_.push_back(node);
		}
		else if (line == "Bezier")
		{
			string line1, line2;
			getline(infile, line1);
			getline(infile, line2);

			double a, b, c, d, e, f, g, h;
			istringstream(line1) >> a >> b >> c >> d >> e >> f >> g >> h;
			int aa, bb;
			istringstream(line2) >> aa >> bb;

			FRNodeBezier * node = new FRNodeBezier();
			node->set_pts(0, _v2d_(a, b));
			node->set_pts(1, _v2d_(c, d));
			node->set_pts(2, _v2d_(e, f));
			node->set_pts(3, _v2d_(g, h));

			this->all_frnodes_.push_back(node);
		}
		else if (line == "Root FRSNode")
		{
			this->frs_root_node_ = new FRSNode();
			this->LoadFRSNode(infile, this->frs_root_node_);
			this->current_node_ = this->frs_root_node_;
		}
		else if (line == "FR Angle")
		{
			string line1;
			getline(infile, line1);
			istringstream(line1) >> this->fr_angle_;
		}
		else if (line == "End")
		{
		}
	}
}

bool FRSTree::IsElementInFRSNode(FRSNode * frs_node, SVGElement * ele)
{
	std::vector<FRNode*> poly;
	for (size_t i = 0; i < frs_node->frnode_indices_.size(); i++)
		poly.push_back(this->all_frnodes_[frs_node->frnode_indices_[i]]);

	std::vector<v2d> pts;
	ele->collectPoints(pts, 0.1);

	int num = 0;

	for (size_t i = 0; i < pts.size(); i++)
	{
		if (Geo2D::PointInPolygon(pts[i], poly))
			num++;
	}

	if (num > (int)(pts.size()) / 2)
		return true;
	else
		return false;
}

void FRSTree::SplitFRNode(FRNode *& n1, FRNode *& n2, double & t, FRNode * node, const v2d & pt)
{
	if (node->NodeType() == 0)//the node is a line
	{
		n1 = new FRNodeLine();
		n1->set_pts(0, node->GetFirstEnd());
		n1->set_pts(1, pt);

		n2 = new FRNodeLine();
		n2->set_pts(0, pt);
		n2->set_pts(1, node->GetLastEnd());

		t = Geo2D::tOfPointOnLine(pt, node->GetFirstEnd(), node->GetLastEnd());
	}
	else//the node is a bezier curve
	{
		v2d p0 = node->pts(0);
		v2d p1 = node->pts(1);
		v2d p2 = node->pts(2);
		v2d p3 = node->pts(3);

		t = Geo2D::tOfPointOnBezier(pt, p0, p1, p2, p3);

		std::vector<v2d> b1, b2;
		Geo2D::SplitBezier(b1, b2, t, p0, p1, p2, p3);

		n1 = new FRNodeBezier();
		n1->set_pts(0, b1[0]);
		n1->set_pts(1, b1[1]);
		n1->set_pts(2, b1[2]);
		n1->set_pts(3, b1[3]);

		n2 = new FRNodeBezier();
		n2->set_pts(0, b2[0]);
		n2->set_pts(1, b2[1]);
		n2->set_pts(2, b2[2]);
		n2->set_pts(3, b2[3]);
	}
}

void FRSTree::SplitFRNode(FRNode *& n1, FRNode *& n2, FRNode *& n3, double & t1, double & t2, FRNode * node, const v2d & p0, const v2d & p1)
{
	if (node->NodeType() == 0)
	{
		n1 = new FRNodeLine();
		n1->set_pts(0, node->GetFirstEnd());
		n1->set_pts(1, p0);

		n2 = new FRNodeLine();
		n2->set_pts(0, p0);
		n2->set_pts(1, p1);

		n3 = new FRNodeLine();
		n3->set_pts(0, p1);
		n3->set_pts(1, node->GetLastEnd());

		t1 = Geo2D::tOfPointOnLine(p0, node->GetFirstEnd(), node->GetLastEnd());
		t2 = Geo2D::tOfPointOnLine(p1, node->GetFirstEnd(), node->GetLastEnd());
	}
	else
	{
		t1 = Geo2D::tOfPointOnBezier(p0, node->pts(0), node->pts(1), node->pts(2), node->pts(3));
		t2 = Geo2D::tOfPointOnBezier(p1, node->pts(0), node->pts(1), node->pts(2), node->pts(3));

		std::vector<std::vector<v2d>> beziers;
		std::vector<v2d> pts;
		pts.push_back(p0);
		pts.push_back(p1);

		Geo2D::SplitBezier(beziers, pts, node->pts(0), node->pts(1), node->pts(2), node->pts(3));

		if (beziers.size() != 3)
		{
			cout << "Error - FrameFRStructure::SplitFRNode - Split Bezier Wrong!" << endl;
			return;
		}

		n1 = new FRNodeBezier();
		n1->set_pts(0, beziers[0][0]);
		n1->set_pts(1, beziers[0][1]);
		n1->set_pts(2, beziers[0][2]);
		n1->set_pts(3, beziers[0][3]);

		n2 = new FRNodeBezier();
		n2->set_pts(0, beziers[1][0]);
		n2->set_pts(1, beziers[1][1]);
		n2->set_pts(2, beziers[1][2]);
		n2->set_pts(3, beziers[1][3]);

		n3 = new FRNodeBezier();
		n3->set_pts(0, beziers[2][0]);
		n3->set_pts(1, beziers[2][1]);
		n3->set_pts(2, beziers[2][2]);
		n3->set_pts(3, beziers[2][3]);
	}
}


void FRSTree::SplitFRSNodeOnIdenticalFRNode(FRSNode * frs_node, FRNode * node, int idx)
{
	/* n1, n2, n3 are split nodes
		ni is the inverse selected node */

	FRNode *n1, *n2, *n3, *ni;
	double t1, t2;

	int fr_node_idx = frs_node->frnode_indices_[idx];
	FRNode * split_fr_node = this->all_frnodes_[fr_node_idx];


	this->SplitFRNode(n1, n2, n3, t1, t2, split_fr_node, node->GetFirstEnd(), node->GetLastEnd());

	if (node->NodeType() == 0)
	{
		ni = new FRNodeLine();
		ni->set_pts(0, node->pts(1));
		ni->set_pts(1, node->pts(0));
	}
	else
	{
		ni = new FRNodeBezier();
		ni->set_pts(0, node->pts(3));
		ni->set_pts(1, node->pts(2));
		ni->set_pts(2, node->pts(1));
		ni->set_pts(3, node->pts(0));
	}

	this->all_frnodes_.push_back(ni);
	this->all_frnodes_.push_back(n1);
	this->all_frnodes_.push_back(n2);
	this->all_frnodes_.push_back(n3);

	/* get split FRS nodes */
	FRSNode * frsn_left = new FRSNode();
	FRSNode * frsn_right = new FRSNode();

	frsn_left->parrent_ = frs_node;
	frsn_right->parrent_ = frs_node;

	frs_node->left_child_ = frsn_left;
	frs_node->right_child_ = frsn_right;

	for (int i = 0; i < idx; i++)
		frsn_left->frnode_indices_.push_back(frs_node->frnode_indices_[i]);
	frsn_left->frnode_indices_.push_back(this->IndexOfNode(n1));
	frsn_left->frnode_indices_.push_back(this->IndexOfNode(node));
	frsn_left->frnode_indices_.push_back(this->IndexOfNode(n3));
	for (size_t i = idx + 1; i < frs_node->frnode_indices_.size(); i++)
		frsn_left->frnode_indices_.push_back(frs_node->frnode_indices_[i]);

	frsn_right->frnode_indices_.push_back(this->IndexOfNode(n2));
	frsn_right->frnode_indices_.push_back(this->IndexOfNode(ni));

	/* split elements */
	for (size_t i = 0; i < frs_node->element_indices_.size(); i++)
	{
		SVGElement * ele = this->pattern_->elements_[frs_node->element_indices_[i]];
		if (this->IsElementInFRSNode(frsn_left, ele))
			frsn_left->element_indices_.push_back(frs_node->element_indices_[i]);
		else if (this->IsElementInFRSNode(frsn_right, ele))
			frsn_right->element_indices_.push_back(frs_node->element_indices_[i]);
	}

	/* set partition index and split t */
	frs_node->partition_frnode_index_ = this->IndexOfNode(node);
	frs_node->t1_ = t1;
	frs_node->t2_ = t2;
	frs_node->n1_ = idx;
	frs_node->n2_ = idx;
}

void FRSTree::SplitFRSNodeOnDifferentFRNode(FRSNode * frs_node, FRNode * node, int left_idx, int right_idx)
{
	/* n1 and n2 are two split nodes of the left node
		n3 and n4 are two split nodes of the right node
		ni is the inverse selected node */

	FRNode *n1, *n2, *n3, *n4, *ni;
	double t1, t2;

	int			left_node_idx = frs_node->frnode_indices_[left_idx];
	int			right_node_idx = frs_node->frnode_indices_[right_idx];
	FRNode *	left_node = this->all_frnodes_[left_node_idx];
	FRNode *	right_node = this->all_frnodes_[right_node_idx];

	this->SplitFRNode(n1, n2, t1, left_node, node->GetFirstEnd());
	this->SplitFRNode(n3, n4, t2, right_node, node->GetLastEnd());

	if (node->NodeType() == 0)
	{
		ni = new FRNodeLine();
		ni->set_pts(0, node->pts(1));
		ni->set_pts(1, node->pts(0));
	}
	else
	{
		ni = new FRNodeBezier();
		ni->set_pts(0, node->pts(3));
		ni->set_pts(1, node->pts(2));
		ni->set_pts(2, node->pts(1));
		ni->set_pts(3, node->pts(0));
	}

	this->all_frnodes_.push_back(ni);
	this->all_frnodes_.push_back(n1);
	this->all_frnodes_.push_back(n2);
	this->all_frnodes_.push_back(n3);
	this->all_frnodes_.push_back(n4);

	/* get split FRS nodes */
	FRSNode * frsn_left = new FRSNode();
	FRSNode * frsn_right = new FRSNode();

	frsn_left->parrent_ = frs_node;
	frsn_right->parrent_ = frs_node;

	frs_node->left_child_ = frsn_left;
	frs_node->right_child_ = frsn_right;

	for (int i = 0; i < left_idx; i++)
		frsn_left->frnode_indices_.push_back(frs_node->frnode_indices_[i]);
	frsn_left->frnode_indices_.push_back(this->IndexOfNode(n1));
	frsn_left->frnode_indices_.push_back(this->IndexOfNode(node));
	frsn_left->frnode_indices_.push_back(this->IndexOfNode(n4));
	for (size_t i = right_idx + 1; i < frs_node->frnode_indices_.size(); i++)
		frsn_left->frnode_indices_.push_back(frs_node->frnode_indices_[i]);

	frsn_right->frnode_indices_.push_back(this->IndexOfNode(n2));
	for (int i = left_idx + 1; i < right_idx; i++)
		frsn_right->frnode_indices_.push_back(frs_node->frnode_indices_[i]);
	frsn_right->frnode_indices_.push_back(this->IndexOfNode(n3));
	frsn_right->frnode_indices_.push_back(this->IndexOfNode(ni));

	/* split elements */
	for (size_t i = 0; i < frs_node->element_indices_.size(); i++)
	{
		SVGElement * ele = this->pattern_->elements_[frs_node->element_indices_[i]];
		if (this->IsElementInFRSNode(frsn_left, ele))
			frsn_left->element_indices_.push_back(frs_node->element_indices_[i]);
		else if (this->IsElementInFRSNode(frsn_right, ele))
			frsn_right->element_indices_.push_back(frs_node->element_indices_[i]);
	}

	/* set partition index and split t */
	frs_node->partition_frnode_index_ = this->IndexOfNode(node);
	frs_node->t1_ = t1;
	frs_node->t2_ = t2;
	frs_node->n1_ = left_idx;
	frs_node->n2_ = right_idx;
}

void FRSTree::SplitFRSNode(FRSNode * frs_node, FRNode * node)
{
	if (node == 0)
	{
		cout << "Error - FRSTree::SplitFRSNode - node is NULL" << endl;
		return;
	}

	if (frs_node == 0)
	{
		cout << "Error - FRSTree::SplitFRSNode - frs_node is NULL" << endl;
		return;
	}

	if (node->left_node() == 0 || node->right_node() == 0)
	{
		cout << "Error - FRSTree::SplitFRSNode - node->left_node() or node->right_node() is NULL" << endl;
		return;
	}

	if (frs_node->left_child_ != 0 || frs_node->right_child_ != 0)
	{
		cout << "Error - FRSTree::SplitFRSNode - has been split" << endl;
		return;
	}

	int left_idx = -1, right_idx = -1;

	for (size_t i = 0; i < frs_node->frnode_indices_.size(); i++)
	{
		if (this->all_frnodes_[frs_node->frnode_indices_[i]] == node->left_node())
			left_idx = (int)i;
		if (this->all_frnodes_[frs_node->frnode_indices_[i]] == node->right_node())
			right_idx = (int)i;
	}

	if (left_idx == -1 || right_idx == -1)
	{
		cout << "Error - FRSTree::SplitFRSNode - no connection" << endl;
		return;
	}

	if (left_idx > right_idx)
	{
		cout << "Error- FRSTree::SplitFRSNode - connection is reverse" << endl;
		return;
	}

	if (left_idx < right_idx)
	{
		this->SplitFRSNodeOnDifferentFRNode(frs_node, node, left_idx, right_idx);
	}
	else if (left_idx == right_idx)
	{
		this->SplitFRSNodeOnIdenticalFRNode(frs_node, node, left_idx);
	}
}

void FRSTree::SplitFRSNode(FRSNode * frs_node, v6d curve)
{
	int left_idx, right_idx;
	double t1, t2;

	frs_node->Compute_n_and_t_ByGlobalProportion(this, curve[0], left_idx, t1);
	frs_node->Compute_n_and_t_ByGlobalProportion(this, curve[5], right_idx, t2);

	FRNode *	left_node = this->all_frnodes_[frs_node->frnode_indices_[left_idx]];
	FRNode *	right_node = this->all_frnodes_[frs_node->frnode_indices_[right_idx]];

	v2d p0, p1;

	if (left_node->NodeType() == 0)
		Geo2D::PointOnLine(p0, t1, left_node->GetFirstEnd(), left_node->GetLastEnd());
	else
		Geo2D::PointOnBezier(p0, t1, left_node->pts(0), left_node->pts(1), left_node->pts(2), left_node->pts(3));

	if (right_node->NodeType() == 0)
		Geo2D::PointOnLine(p1, t2, right_node->GetFirstEnd(), right_node->GetLastEnd());
	else
		Geo2D::PointOnBezier(p1, t2, right_node->pts(0), right_node->pts(1), right_node->pts(2), right_node->pts(3));

	FRNodeBezier * node = new FRNodeBezier();
	node->set_pts(0, p0);
	node->set_pts(1, _v2d_(curve[1], curve[2]));
	node->set_pts(2, _v2d_(curve[3], curve[4]));
	node->set_pts(3, p1);

	node->set_left_node(left_node);
	node->set_right_node(right_node);

	this->all_frnodes_.push_back(node);

	this->SplitFRSNode(frs_node, node);
}


void FRSTree::ExtractHighDimensionalData(FRSNode * node, string filepath)
{
	if (node->partition_frnode_index_ == -1) return; // the node is not split, thus there is no HDData

	/* save current partition curve */
	ofstream outfile(filepath, std::ios::app);

	FRNodeBezier * bezier = (FRNodeBezier*)(this->all_frnodes_[node->partition_frnode_index_]);	 // must use a bezier to partition a FRSNode
	v2d p1, p2;
	p1 = bezier->pts(1);
	p2 = bezier->pts(2);
	outfile << node->FirstEndpointGlobalProportion(this) << ' ' << p1[0] << ' ' << p1[1] << ' ' << p2[0] << ' ' << p2[1] << ' ' << node->SecondEndpointGlobalProportion(this) << ' ';

	outfile.close();

	if (node->left_child_ != 0)
		this->ExtractHighDimensionalData(node->left_child_, filepath);

	if (node->right_child_ != 0)
		this->ExtractHighDimensionalData(node->right_child_, filepath);
}

void FRSTree::ExtractHighDimensionalData(FRSNode * node, string str, int tree_no)
{
	if (node->partition_frnode_index_ == -1) return; // the node is not split, thus there is no HDData

	/* save current partition curve */
	ofstream outfile(str + ".txt", std::ios::app);
	FRNodeBezier * bezier = (FRNodeBezier*)(this->all_frnodes_[node->partition_frnode_index_]);	 // must use a bezier to partition a FRSNode
	v2d p1, p2;
	p1 = bezier->pts(1);
	p2 = bezier->pts(2);
	outfile << tree_no << ' ' << (node->n1_ + node->t1_) << ' ' << p1[0] << ' ' << p1[1] << ' ' << p2[0] << ' ' << p2[1] << ' ' << (node->n2_ + node->t2_) << '\n';
	outfile.close();

	if (node->left_child_ != 0)
	{
		string str_left = str + '-'
			+ LibIV::CppHelpers::Global::num2str(node->n1_) + '-'
			+ LibIV::CppHelpers::Global::num2str(node->n2_) + '-'
			+ 'L';

		this->ExtractHighDimensionalData(node->left_child_, str_left, tree_no);
	}

	if (node->right_child_ != 0)
	{
		string str_right = str + '-'
			+ LibIV::CppHelpers::Global::num2str(node->n1_) + '-'
			+ LibIV::CppHelpers::Global::num2str(node->n2_) + '-'
			+ 'R';

		this->ExtractHighDimensionalData(node->right_child_, str_right, tree_no);
	}
}

void FRSTree::ExtractHighDimensionalDataBreadthFirst(string filepath)
{
	if (this->frs_root_node_ == 0) return;

	ofstream outfile(filepath, std::ios::app);

	std::vector<FRSNode *> vec;
	vec.push_back(this->frs_root_node_);

	size_t i = 0;
	while (i < vec.size())
	{
		FRSNode * node = vec[i];
		if (node->left_child_ != 0)
			vec.push_back(node->left_child_);
		if (node->right_child_ != 0)
			vec.push_back(node->right_child_);
		i++;
	}

	outfile << this->fr_angle() << ' ';

	for (i = 0; i < vec.size(); i++)
	{
		FRSNode * node = vec[i];
		if (node->partition_frnode_index_ != -1)
		{
			FRNodeBezier * bezier = (FRNodeBezier*)(this->all_frnodes_[node->partition_frnode_index_]);	 // must use a bezier to partition a FRSNode
			v2d p1, p2;
			p1 = bezier->pts(1);
			p2 = bezier->pts(2);
			outfile << node->FirstEndpointGlobalProportion(this) << ' ' << p1[0] << ' ' << p1[1] << ' ' << p2[0] << ' ' << p2[1] << ' ' << node->SecondEndpointGlobalProportion(this) << ' ';
		}
	}

	outfile.close();
}

void FRSTree::ExtractHighDimensionalDataBreadthFirst(std::vector<double> & hddata)
{
	if (this->frs_root_node_ == 0) return;
	hddata.clear();

	std::vector<FRSNode *> vec;
	vec.push_back(this->frs_root_node_);

	size_t i = 0;
	while (i < vec.size())
	{
		FRSNode * node = vec[i];
		if (node->left_child_ != 0)
			vec.push_back(node->left_child_);
		if (node->right_child_ != 0)
			vec.push_back(node->right_child_);
		i++;
	}

	hddata.push_back(this->fr_angle());

	for (i = 0; i < vec.size(); i++)
	{
		FRSNode * node = vec[i];
		if (node->partition_frnode_index_ != -1)
		{
			FRNodeBezier * bezier = (FRNodeBezier*)(this->all_frnodes_[node->partition_frnode_index_]);	 // must use a bezier to partition a FRSNode
			v2d p1, p2;
			p1 = bezier->pts(1);
			p2 = bezier->pts(2);

			hddata.push_back(node->FirstEndpointGlobalProportion(this));
			hddata.push_back(p1[0]);
			hddata.push_back(p1[1]);
			hddata.push_back(p2[0]);
			hddata.push_back(p2[1]);
			hddata.push_back(node->SecondEndpointGlobalProportion(this));
		}
	}
}

void FRSTree::GetLeafNodesAndStringIds(FRSNode * frs_node, string cur_str_id, std::vector<FRSNode*> & leaf_nodes, std::vector<string> & str_ids)
{
	if (frs_node->left_child_ == 0 && frs_node->right_child_ == 0) // a leaf node
	{
		leaf_nodes.push_back(frs_node);
		str_ids.push_back(cur_str_id);
		return;
	}

	if (frs_node->left_child_ != 0)
	{

		string left_str = cur_str_id +
			'-' +
			LibIV::CppHelpers::Global::num2str(frs_node->n1_) +
			'-' +
			LibIV::CppHelpers::Global::num2str(frs_node->n2_) +
			"-L";
		this->GetLeafNodesAndStringIds(frs_node->left_child_, left_str, leaf_nodes, str_ids);
	}

	if (frs_node->right_child_ != 0)
	{
		string right_str = cur_str_id +
			'-' +
			LibIV::CppHelpers::Global::num2str(frs_node->n1_) +
			'-' +
			LibIV::CppHelpers::Global::num2str(frs_node->n2_) +
			"-R";
		this->GetLeafNodesAndStringIds(frs_node->right_child_, right_str, leaf_nodes, str_ids);
	}
}

void FRSTree::GetLeafNodesAndArrayIndices(std::vector<FRSNode *> & leaf_nodes, std::vector<int> & indices)
{
	if (this->frs_root_node_ == 0) return;

	std::vector<FRSNode *> vec;
	std::vector<int> idx;
	vec.push_back(this->frs_root_node_);
	idx.push_back(0);

	int i = 0;
	while (i < (int)(vec.size()))
	{
		FRSNode * node = vec[i];
		if (node->left_child_ != 0)
		{
			vec.push_back(node->left_child_);
			idx.push_back(2 * i + 1);
		}
		if (node->right_child_ != 0)
		{
			vec.push_back(node->right_child_);
			idx.push_back(2 * i + 2);
		}
		i++;
	}

	for (size_t j = 0; j < vec.size(); j++)
	{
		if (vec[j]->partition_frnode_index_ == -1)
		{
			leaf_nodes.push_back(vec[j]);
			indices.push_back(idx[j]);
		}
	}
}

FRSNode* FRSTree::GetLeafNodeByIndex(int code)
{
	std::vector<FRSNode*> nodes;
	std::vector<int>	indices;

	this->GetLeafNodesAndArrayIndices(nodes, indices);

	FRSNode * ans = 0;
	for (size_t i = 0; i < indices.size(); i++)
	{
		if (indices[i] == code)
		{
			ans = nodes[i];
			break;
		}
	}

	return ans;
}

void FRSTree::GetFRSNodeByName(FRSNode * frs_node, string cur_name, string name, FRSNode *& ans)
{
	if (name == cur_name)
	{
		ans = frs_node;
		return;
	}

	if (frs_node->left_child_ != 0)
	{
		string left_name = cur_name +
			'-' +
			LibIV::CppHelpers::Global::num2str(frs_node->n1_) +
			'-' +
			LibIV::CppHelpers::Global::num2str(frs_node->n2_) +
			"-L";

		this->GetFRSNodeByName(frs_node->left_child_, left_name, name, ans);

		if (ans != 0) return;
	}

	if (frs_node->right_child_ != 0)
	{
		string right_name = cur_name +
			'-' +
			LibIV::CppHelpers::Global::num2str(frs_node->n1_) +
			'-' +
			LibIV::CppHelpers::Global::num2str(frs_node->n2_) +
			"-R";

		this->GetFRSNodeByName(frs_node->right_child_, right_name, name, ans);

		if (ans != 0) return;
	}
}

void FRSTree::BinaryTreeTo1DArray(int * arr)
{
	if (this->frs_root_node_ != 0)
		this->frs_root_node_->BinaryTreeTo1DArray(arr, 0);
}

FRSNode * FRSTree::FindParentFRSNodeByArrayIndex(int n)
{
	if (n <= 0)
	{
		cout << "error - FRSTree::FindParentFRSNodeByArrayIndex" << endl;
		return 0;
	}

	std::vector<int> a;
	while (n > 0)
	{
		a.push_back(n);
		n = (n - 1) / 2;
	}


	FRSNode * node = this->frs_root_node_;
	if (node == 0) return 0;

	for (int i = (int)(a.size()) - 1; i > 0; i--)
	{
		int b = a[i];
		if (b % 2 == 1)
			node = node->left_child_;
		else
			node = node->right_child_;
	}

	return node;
}

FRSNode * FRSTree::FindFRSNodeByArrayIndex(int n)
{
	if (n < 0)
	{
		cout << "error - FRSTree::FindFRSNodeByArrayIndex" << endl;
		return 0;
	}

	std::vector<int> a;
	while (n > 0)
	{
		a.push_back(n);
		n = (n - 1) / 2;
	}

	FRSNode * node = this->frs_root_node_;
	if (node == 0) return 0;

	for (int i = (int)(a.size()) - 1; i >= 0; i--)
	{
		int b = a[i];
		if (b % 2 == 1)
			node = node->left_child_;
		else
			node = node->right_child_;
	}

	return node;
}

double FRSTree::BestAngle(double angle)
{
	//double m[16] = {360,180,120,90,72,60,45,40,36,30,24,22.5,20,18,15,12};
	double m[13] = { 360,180,120,90,60,45,36,30,22.5,20,18,15,12 };
	double v[13];

	for (int i = 0; i < 13; i++)
		v[i] = fabs(angle - m[i]);

	int j = 0;
	for (int i = 1; i < 13; i++)
	{
		if (v[i] < v[j])
			j = i;
	}

	return m[j];
}

bool FRSTree::CompareWithFRSTree(FRSTree  *e)
{
	if (this->correc_indec.size() != e->correc_indec.size())
	{
		return false;
	}
	else
	{
		for (int i = 0; i < this->correc_indec.size(); i++)
		{
			if (this->correc_indec[i] != e->correc_indec[i])
			{
				return false;
			}
		}
	}
	return true;
}