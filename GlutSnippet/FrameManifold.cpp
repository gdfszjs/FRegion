#include "FrameManifold.h"
#include "CommonGL.h"
#include "ColorGradient.h"
#include "GLUTMain.h"
#include "FrameStructureBlend.h"
#include "FRStructure.h"
#include "SVGPattern.h"
#include "SVGElement.h"
#include "Geo2D.h"
#include "FRNode.h"
#include "heat_processing.h"
#include "windows.h"
#include "FRSTreeMRF.h"

FrameManifold::FrameManifold()
{
	this->manifold_x_ = 0;
	this->manifold_y_ = 0;
	this->offset_x_ = 0;
	this->offset_y_ = 0;
}

FrameManifold::FrameManifold(Frame * parent, int lx, int ly, int width, int height, string name,
	string manifold_filename, string var_filename, string anchor_point_filename)
	: Frame(parent, lx, ly, width, height, name)
{
	this->manifold_x_ = 0;
	this->manifold_y_ = 0;
	this->variance_fig_ = 0;
	this->offset_x_ = 50;
	this->offset_y_ = 50;

	this->LoadManifold(manifold_filename, var_filename, anchor_point_filename);
}

FrameManifold::~FrameManifold()
{
	if (this->variance_fig_ != 0)
		cvReleaseImage(&this->variance_fig_);
}

void ShowImageRegion(IplImage * img, const std::vector<v2i> & region, v3i color)
{
	for (size_t i = 0; i < region.size(); i++)
	{
		v2i v = region[i];
		LibIV::CppHelpers::Global::setImgData(color[0], color[1], color[2], img, v[0], v[1]);
	}
}

void FrameManifold::drawScene() {
	int gx, gy;
	_xyInGlobal(gx, gy);

	glEnable(GL_SCISSOR_TEST);
	glScissor(gx, gy, width, height);
	glClearColor(br / 255.0f, bg / 255.0f, bb / 255.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	glViewport(gx, gy, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw the variance by color
	/*if (this->variance_fig_ != 0)
		writeImage(this->offset_x_, this->offset_y_, this->variance_fig_, this->width, this->height);*/

	/*IplImage * heat_img = cvLoadImage("D:/heat.png");
	ShowImageRegion(heat_img,this->boundary_,_v3i_(0,255,0));
	cvCircle(heat_img,cvPoint(this->nearest_neighbor_[0],this->nearest_neighbor_[1]),5,cvScalar(255,0,0));
	if(heat_img)
		writeImage(this->offset_x_,this->offset_y_,heat_img,this->width,this->height,true);
	cvReleaseImage(&heat_img);*/



	// draw anchor points
	/*if (!this->gplvm_anchor_pts_.empty())
	{
		glColor3f(0, 1, 0);
		glPointSize(10);

		glBegin(GL_POINTS);
		for (size_t i = 0; i < this->gplvm_anchor_pts_.size(); i++)
			glVertex2d(this->gplvm_anchor_pts_[i][0] + this->offset_x_, this->gplvm_anchor_pts_[i][1] + this->offset_y_);
		glEnd();

		glPointSize(1);
		glColor3f(1, 1, 1);
	}*/

	//draw the selected points
	if (!this->center_position_FRSTree.empty())
	{
		//cout << "hello draw" << endl;
		int i = 0;
		while (i < center_position_FRSTree.size())
		{
			float j = i * 1.0 / center_position_FRSTree.size();
			glColor3f(j, 0, 0);
			glPointSize(20.0f);

			glBegin(GL_POINTS);
			for (int k = 0; k < center_position_FRSTree.at(i).size(); k++)
				glVertex2d(center_position_FRSTree.at(i).at(k)[0], center_position_FRSTree.at(i).at(k)[1]);
			glEnd();

			glPointSize(1);
			glColor3f(1, 1, 1);
			i++;
		}
		//this->draw_flag = 0;
		//cout << "complete draw" << endl;
	}

	// draw FRSTree of parent frame

	FrameStructureBlend * frb = (FrameStructureBlend*)(this->parentFrame);
	//for(size_t i = 0;i<frb->all_trees_.size();i++)
	//{
	//	//if(i != 3) continue;
	//	glPushMatrix();
	//	
	//	glTranslated(this->offset_x_ + this->gplvm_anchor_pts_[i][0], this->offset_y_ + this->gplvm_anchor_pts_[i][1],0);
	//	
	//	glScaled(0.3, 0.3, 0.3);
	//	glTranslated(-260, -260, 0);
	//	/*glTranslated(900,500,0);
	//	
	//	glTranslated(-260,-260,0);*/
	//	glColor3d(0.3,0.3,0.3);
	//	glLineWidth(2);
	//	frb->all_trees_[i]->Draw(false,false);
	//	glLineWidth(1);
	//	glColor3d(1,1,1);

	//	/*glColor3d(1,0,0);
	//	glLineWidth(3);
	//	FRSNode * nn = frb->all_trees_[i]->frs_root_node_->right_child_->right_child_;
	//	for(size_t j = 0;j<nn->frnode_indices_.size();j++)
	//	{
	//		frb->all_trees_[i]->all_frnodes_[nn->frnode_indices_[j]]->Draw();
	//	}
	//	glLineWidth(1);
	//	glColor3d(0,0,0);*/

	//	//this->DrawFR2Circle(frb->all_trees_[i]);

	//	glPopMatrix();
	//}


	// draw a single example
	/*int jjj = 0;
	for(size_t iii = 1;iii<frb->all_trees_.size();iii++)
	{
		if(iii == 2 || iii == 6 || iii == 8) continue;
		glPushMatrix();

		glTranslated(0 + (jjj++) * 150,400,0);
		glScaled(0.5,0.5,0.5);
		glTranslated(-260,-260,0);
		glColor3f(0,0,0);

		frb->all_trees_[iii]->Draw(false,false);
		FRSNode * nn = frb->all_trees_[iii]->frs_root_node_->right_child_->left_child_;
		glColor3f(1,0,0);
		glLineWidth(3);
		for(size_t i = 0;i<nn->frnode_indices_.size();i++)
			frb->all_trees_[iii]->all_frnodes_[nn->frnode_indices_[i]]->Draw();

		glLineWidth(1);
		glColor3f(1,1,1);

		glColor3f(0,0,0);
		glTranslated(0,-200,0);
		for(size_t i = 0;i<nn->frnode_indices_.size();i++)
			frb->all_trees_[iii]->all_frnodes_[nn->frnode_indices_[i]]->Draw();
		glColor3f(1,0,0);
		glPointSize(6);
		glBegin(GL_POINTS);
			for(size_t i = 0;i<nn->frnode_indices_.size();i++)
			{
				std::vector<v2d> tmppts;
				nn->SampleNPoints(tmppts,10,frb->all_trees_[iii]);

				glColor3f(0,0,1);
				for(size_t j = 1;j<tmppts.size();j++)
					glVertex2d(tmppts[j][0],tmppts[j][1]);
				glColor3f(1,0,0);
				glVertex2d(tmppts[0][0],tmppts[0][1]);
				glColor3f(0,1,0);
				glVertex2d(tmppts[5][0],tmppts[5][1]);
			}
		glEnd();

		glColor3f(0,0,0);

		glTranslated(0,-200,0);
		for(size_t i = 0;i<nn->frnode_indices_.size();i++)
			frb->all_trees_[iii]->all_frnodes_[nn->frnode_indices_[i]]->Draw();

		glColor3f(1,0,0);
		glPointSize(6);
		glBegin(GL_POINTS);
			for(size_t i = 0;i<nn->frnode_indices_.size();i++)
			{
				std::vector<v2d> tmppts;
				nn->SampleNPoints2(tmppts,10,frb->all_trees_[iii]);

				glColor3f(0,0,1);
				for(size_t j = 1;j<tmppts.size();j++)
					glVertex2d(tmppts[j][0],tmppts[j][1]);
				glColor3f(1,0,0);
				glVertex2d(tmppts[0][0],tmppts[0][1]);
				glColor3f(0,1,0);
				glVertex2d(tmppts[5][0],tmppts[5][1]);
			}
		glEnd();

		glColor3f(1,1,1);

		glPopMatrix();
	}*/

	// draw the generated tree
	if (frb->gen_frs_tree_ != 0)
	{
		glPushMatrix();

		glTranslated(this->offset_x_ + this->manifold_x_, this->offset_y_ + this->manifold_y_, 0);
		//glTranslated(300,300,0);

		glScaled(1, 1, 1);
		glTranslated(-260, -260, 0);

		//this->DrawFR2Circle(frb->gen_frs_tree_,false);
		glLineWidth(2);
		glColor3d(0.3, 0.3, 0.3);
		//frb->gen_frs_tree_->pattern_->drawPattern();
		frb->gen_frs_tree_->Draw(false, true);

		//save the generated tree
		ofstream outfile("D:/misc/result/1.txt");
		frb->gen_frs_tree_->Save(outfile);
		//cout << "Save completed!" << endl;

		//glScaled(0.25,0.25,1);
		//glTranslated(0,-200,0);

		//frb->gen_frs_tree_->Draw(false,true);


		glLineWidth(1);
		glColor3d(1, 1, 1);

		/*glLineWidth(3);
		glColor3f(1,0,0);
		frb->gen_frs_tree_->all_frnodes_[3]->Draw();
		glColor3f(0,0,1);
		frb->gen_frs_tree_->all_frnodes_[15]->Draw();
		glLineWidth(1);
		glColor3f(1,1,1);*/

		glPopMatrix();
	}

	/*if(frb->gen_frs_tree_)
	{
		SVGElement * e = frb->gen_frs_tree_->pattern_->elements_[3];
	glBegin(GL_LINES);
		for(size_t i = 0;i<1;i++)
		{
			v2d p0,p1;
			p0 = e->segs_[i]->FirstVertex();
			p1 = e->segs_[i]->LastVertex();

			glVertex3d(p0[0],p0[1],0);
			glVertex3d(p1[0],p1[1],0);
			cout<<p0[0]<<" "<<p0[1]<<"      "<<p1[0]<<" "<<p1[1]<<endl;
		}

	glEnd();
	}*/
}

void FrameManifold::LoadManifold(string manifold_filename, string var_filename, string anchor_point_filename)
{
	union foo
	{
		char c[sizeof(double)];
		double d;
	} bar;

	/* read the file of manifold */
	ifstream infile(manifold_filename, std::ios::binary);
	this->gplvm_manifold_.clear();

	infile.seekg(0, ios::end);
	long long int ss = infile.tellg();
	char * buf = new char[(unsigned int)ss];
	infile.seekg(0, ios::beg);
	infile.read(buf, ss);
	char * p = buf;
	infile.close();

	double line_num, dim_num;
	memcpy(bar.c, p, sizeof(double));
	line_num = bar.d;

	p += sizeof(double);
	memcpy(bar.c, p, sizeof(double));
	dim_num = bar.d;

	for (int i = 0; i < (int)line_num; i++)
	{
		std::vector<double> mm;
		for (int j = 0; j < (int)dim_num; j++)
		{
			p += sizeof(double);
			memcpy(bar.c, p, sizeof(double));

			mm.push_back(bar.d);
		}
		this->gplvm_manifold_.push_back(mm);
	}
	delete[] buf;

	/* read the file of anchorpoint */
	string line;
	ifstream infile2(anchor_point_filename);
	this->gplvm_anchor_pts_.clear();
	getline(infile2, line);
	double dim_x, dim_y;
	istringstream(line) >> dim_x >> dim_y;
	dim_x_ = (int)dim_x;
	dim_y_ = (int)dim_y;

	while (getline(infile2, line) && line != "")
	{
		double a, b;
		istringstream(line) >> a >> b;
		this->gplvm_anchor_pts_.push_back(_v2d_(a, b));
	}
	infile2.close();

	/* read the file of variance */
	ifstream infile3(var_filename, std::ios::binary);

	if (this->variance_fig_ != 0) cvReleaseImage(&this->variance_fig_);
	this->variance_fig_ = cvCreateImage(cvSize(this->dim_x_, this->dim_y_), 8, 3);
	cvZero(this->variance_fig_);

	infile3.seekg(0, ios::end);
	ss = infile3.tellg();
	buf = new char[(unsigned int)ss];
	infile3.seekg(0, ios::beg);
	infile3.read(buf, ss);
	p = buf;
	infile3.close();

	memcpy(bar.c, p, sizeof(double));
	line_num = bar.d;
	p += sizeof(double);
	memcpy(bar.c, p, sizeof(double));
	dim_num = bar.d;


	if (line_num != this->dim_x_ * this->dim_y_)
		cout << "error - FrameManifold::LoadManifold" << endl;

	variance_.set(dim_x_, dim_y_);

	ColorGradient heatmap;

	//IplImage * img = cvLoadImage("D:/heat.png");
	for (int i = 0; i < this->dim_x_; i++)
	{
		for (int j = 0; j < this->dim_y_; j++)
		{
			double b, c, d;
			p += sizeof(double);
			memcpy(bar.c, p, sizeof(double));
			heatmap.getColorAtValue(bar.d, b, c, d);
			variance_.at(j, i) = bar.d;

			//int rrr,ggg,bbb;
			//LibIV::CppHelpers::Global::imgData(rrr,ggg,bbb,img,i,j);
			//LibIV::CppHelpers::Global::setImgData(rrr,ggg,bbb,this->variance_fig_,i,dim_y_-1-j);
			LibIV::CppHelpers::Global::setImgData((int)(b * 255), (int)(c * 255), (int)(d * 255), this->variance_fig_, i, dim_y_ - j - 1);
		}
	}
	delete[] buf;

	//cvReleaseImage(&img);

	//cvSaveImage("D:/aaa.png",this->variance_fig_);
}

void FrameManifold::ComputeBoundaryAndNeighbor()
{
	IplImage * img = cvLoadImage("D:/heat.png");
	this->boundary_.clear();
	std::vector<v2i> region, inner;
	std::vector<v3i> color;
	HeatProcessing::ComputeImageRegionOfPoint(img, region, _v2i_(manifold_x_, manifold_y_));
	HeatProcessing::ComputeImageRegionBoundaryInner(img, region, boundary_, color, inner);
	v2i nearest_v;
	HeatProcessing::ComputeDistanceBetweenPointAndPointSet(_v2i_(manifold_x_, manifold_y_), boundary_, nearest_v);
	HeatProcessing::FindFirstDifferentColorNeighbor(img, nearest_v, nearest_neighbor_);
	cvReleaseImage(&img);
}

void FrameManifold::onMouseFunc(int button, int state, int x, int y)
{
	int lx, ly;
	this->globalPt2Local(lx, ly, x, y);
	this->manifold_x_ = lx - this->offset_x_;
	this->manifold_y_ = ly - this->offset_y_;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		//this->ComputeBoundaryAndNeighbor();

		Frame * main_frame = GLUTMain::Instance()->getMainFrame();
		FrameStructureBlend * fsb = (FrameStructureBlend*)(main_frame->getFrameByName("FrameView"));
		if (fsb != 0) fsb->RefreshData();
	}
}

void FrameManifold::onMotionFunc(int x, int y)
{
	int lx, ly;
	this->globalPt2Local(lx, ly, x, y);
	this->manifold_x_ = lx - this->offset_x_;
	this->manifold_y_ = ly - this->offset_y_;

	//this->ComputeBoundaryAndNeighbor();

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameStructureBlend * fsb = (FrameStructureBlend*)(main_frame->getFrameByName("FrameView"));
	if (fsb != 0) fsb->RefreshData();
}

void FrameManifold::onSpecialKeyboardFunc(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		this->GetOrderedFRSTrees();
		//this->GetOrderedIndex();
	}
}

void FrameManifold::GetOrderedFRSTrees()
{
	//step1.find all the filling type
	int TimeStart = GetTickCount();
	if (!this->center_FRSTree.empty()) this->center_FRSTree.clear();
	if (!this->center_position_FRSTree.empty()) this->center_position_FRSTree.clear();

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameStructureBlend * fsb = (FrameStructureBlend*)(main_frame->getFrameByName("FrameView"));
	ofstream outfile("D:\\checkfile.txt");
	ofstream outfile2("D:\\checkfile2.txt");

	for (int i = 0; i < this->width; i = i + 60)
	{
		for (int j = 0; j < this->height; j = j + 60)
		{
			this->manifold_x_ = i;
			this->manifold_y_ = j;
			outfile2 << "gen_frs_tree:\n";
			outfile2 << "(" << this->manifold_x_ << "," << this->manifold_y_ << ")" << "\n";
			if (fsb != 0) fsb->BuildFRSTree();
		
			if (fsb->gen_frs_tree_ != 0)
			{	

				FRSTree *temp_tree = new FRSTree;
				*temp_tree = *(fsb->gen_frs_tree_);

				for (int l = 0; l < fsb->gen_frs_tree_->correc_indec.size(); l++)
					outfile2 << fsb->gen_frs_tree_->correc_indec.at(l) << " ";
				outfile2 << "\n";
				
				if (this->center_FRSTree.size() == 0)
				{
					std::vector<FRSTree*> inner_center_FRSTree;
					std::vector<v2i> inner_center_position_FRSTree;
					inner_center_FRSTree.push_back(temp_tree);
					inner_center_position_FRSTree.push_back(_v2i_(i, j));
					this->center_FRSTree.push_back(inner_center_FRSTree);
					this->center_position_FRSTree.push_back(inner_center_position_FRSTree);
				}
				else
				{
					outfile2 << "the com list:\n";
					outfile2 << this->center_FRSTree.size() << "\n";
					int k = 0;
					for (; k < this->center_FRSTree.size(); k++)
					{
						outfile2 << "(" << center_position_FRSTree.at(k).at(0)[0] << "," << center_position_FRSTree.at(k).at(0)[1] << ")" << " ";
						outfile2 << this->center_FRSTree.at(k).at(0)->correc_indec.size() << "\n";
						
						if (fsb->gen_frs_tree_->CompareWithFRSTree(this->center_FRSTree.at(k).at(0)))
						{//need to be changed into Element_compare
							outfile2 << "true\n";
							this->center_FRSTree.at(k).push_back(temp_tree);
							this->center_position_FRSTree.at(k).push_back(_v2i_(i, j));
							break;
						}
						else outfile2 << "false\n";
						
					}
					if(k >= this->center_FRSTree.size())
					{
						std::vector<FRSTree*> inner_center_FRSTree;
						std::vector<v2i> inner_center_position_FRSTree;
						inner_center_FRSTree.push_back(temp_tree);
						inner_center_position_FRSTree.push_back(_v2i_(i, j));
						this->center_FRSTree.push_back(inner_center_FRSTree);
						this->center_position_FRSTree.push_back(inner_center_position_FRSTree);
					}
				}
			}
		}
	}

	for (int i = 0; i < this->center_FRSTree.size(); i++)
	{	
		outfile << "one vector\n";
		outfile << "the number of vector: " << center_FRSTree.at(i).size() << "\n";
		for (int j = 0; j < this->center_FRSTree.at(i).size(); j++)
		{
			outfile << "(" << this->center_position_FRSTree.at(i).at(j)[0] << "," << this->center_position_FRSTree.at(i).at(j)[1] << ")" << "\n";
			for (int k = 0; k < this->center_FRSTree.at(i).at(j)->correc_indec.size();k++)
				outfile << this->center_FRSTree.at(i).at(j)->correc_indec.at(k) << " ";
			outfile << "\ncom_index:\n";
			for (int k = 0; k < this->center_FRSTree.at(i).at(j)->com_index_order.size(); k++)
			{
				outfile << k << ":";
				for (int l = 0; l < this->center_FRSTree.at(i).at(j)->com_index_order.at(k).size(); l++)
				{
					outfile << this->center_FRSTree.at(i).at(j)->com_index_order.at(k).at(l) << " ";
				}
				outfile << "\n";
			}
		}
	}

	cout << this->center_FRSTree.size() << endl;
	cout << this->center_position_FRSTree.size() << endl;

	for (int i = 0; i < this->center_FRSTree.size(); i++)
	{
		FRSTree *temp_tree = center_FRSTree.at(i).at(0);
		for (int j = 0; j < 4; j++)
		{
			std::vector<int> inner_correc_index_order;
			inner_correc_index_order.push_back(temp_tree->correc_indec.at(j));
		}
	}

	int TimeEnd = GetTickCount();
	int TimeUsed = TimeEnd - TimeStart;
	cout << "遍历用时： " << TimeUsed << " ms" << endl;

	FRSTree *compute_frs_tree = new FRSTree[this->center_FRSTree.size()];
	for (int i = 0; i < this->center_FRSTree.size(); i++)
	{
		compute_frs_tree[i] = *this->center_FRSTree.at(i).at(0);
		cout << i << endl;
	}

	
	//step2.use BP&MRF to get the most possible element
	/*int TimeStart2 = GetTickCount();

	PartAssembling pa(4,compute_frs_tree[0].com_index_order.size(), this->center_FRSTree.size());
	pa.initialize();
	pa.bpLoop(compute_frs_tree);
	pa.computeBelief(compute_frs_tree);
	pa.printResult(compute_frs_tree);

	int TimeEnd2 = GetTickCount();
	int TimeUsed2 = TimeEnd2 - TimeStart2;
	cout << "遍历用时： " << TimeUsed2 << " ms" << endl;*/

}

void FrameManifold::GetOrderedIndex()
{
	int TimeStart = GetTickCount();

	//comparejust on index
	if (!this->all_select_index.empty()) this->all_select_index.clear();
	if (!this->center_position_FRSTree.empty()) this->center_position_FRSTree.clear();

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameStructureBlend * fsb = (FrameStructureBlend*)(main_frame->getFrameByName("FrameView"));
	vector<int> temp_index;
	/*ofstream outfile2("D:\\checkfile2.txt");
	ofstream outfile("D:\\checkfile.txt");*/
	for (int i = 0; i < this->width; i = i + 20)
	{
		for (int j = 0; j < this->height; j = j + 20)
		{
			this->manifold_x_ = i;
			this->manifold_y_ = j;
			//outfile2 << "gen_frs_tree:\n";
			//outfile2 << "(" << this->manifold_x_ << "," << this->manifold_y_ << ")" << "\n";
			if (fsb != 0)
				temp_index = fsb->BuildFRSTreeforseletindex();

			/*outfile2 << "the temp_index:\n";
			outfile2 << "(" << i << "," << j << ")\n[";
			for (int l = 0; l < temp_index.size(); l++)
			outfile2 << temp_index.at(l) << " ";
			outfile2 << "]\n";*/

			if (temp_index.size() != 0)
			{
				if (this->all_select_index.size() == 0)
				{
					std::vector<std::vector<int>> inner_index;
					std::vector<v2i> inner_center_position_FRSTree;
					inner_index.push_back(temp_index);
					inner_center_position_FRSTree.push_back(_v2i_(i, j));
					this->all_select_index.push_back(inner_index);
					this->center_position_FRSTree.push_back(inner_center_position_FRSTree);
				}
				else
				{
					int k = 0;
					/*outfile2 << "the com list:\n";*/
					for (; k < this->all_select_index.size(); k++)
					{
						std::vector<int> e = this->all_select_index.at(k).at(0);
						v2i temp_pos = _v2i_(this->center_position_FRSTree.at(k).at(0)[0], this->center_position_FRSTree.at(k).at(0)[1]);

						/*outfile2 << "(" << temp_pos[0] << "," << temp_pos[1] << ")\n[";
						for (int l = 0; l < temp_index.size(); l++)
						outfile2 << e.at(l) << " ";
						outfile2 << "]";*/

						if (temp_index == e)
						{
							/*outfile2 << " true\n";*/
							this->all_select_index.at(k).push_back(temp_index);
							this->center_position_FRSTree.at(k).push_back(_v2i_(i, j));
							break;
						}
						else /*outfile2 << " false\n"*/;

					}
					if (k >= this->all_select_index.size())
					{
						std::vector<std::vector<int>> inner_index;
						std::vector<v2i> inner_center_position_FRSTree;
						inner_index.push_back(temp_index);
						inner_center_position_FRSTree.push_back(_v2i_(i, j));
						this->all_select_index.push_back(inner_index);
						this->center_position_FRSTree.push_back(inner_center_position_FRSTree);
					}
				}
			}
		}
	}

	/*for (int i = 0; i < this->all_select_index.size(); i++)
	{
	outfile << "one vector\n";
	outfile << "the number of vector: " << all_select_index.at(i).size() << "\n";
	for (int j = 0; j < this->all_select_index.at(i).size(); j++)
	{
	outfile << "(" << this->center_position_FRSTree.at(i).at(j)[0] << "," << this->center_position_FRSTree.at(i).at(j)[1] << ")" << "\n";
	for (int k = 0; k < this->all_select_index.at(i).at(j).size();k++)
	outfile << this->all_select_index.at(i).at(j).at(k) << " ";
	outfile << "\n";
	}
	}*/

	//cout << this->all_select_index.size() << endl;
	//cout << this->center_position_FRSTree.size() << endl;

	int TimeEnd = GetTickCount();
	int TimeUsed = TimeEnd - TimeStart;
	cout << "遍历用时： " << TimeUsed << " ms" << endl;
}

std::vector<double> FrameManifold::GetManifoldValue()
{
	std::vector<double> val;
	if (this->manifold_x_ >= 0 && this->manifold_x_ < dim_x_ && this->manifold_y_ >= 0 && this->manifold_y_ < dim_y_)
		val = this->gplvm_manifold_[this->manifold_x_ * dim_y_ + this->manifold_y_];
	else
		val = this->gplvm_manifold_[0];
	return val;
}

std::vector<double> FrameManifold::GetManifoldValue(int x, int y)
{
	std::vector<double> val;
	if (x >= 0 && x < dim_x_ && y >= 0 && y < dim_y_)
		val = this->gplvm_manifold_[x * dim_y_ + y];
	else
		val = this->gplvm_manifold_[0];
	return val;
}

/*
	FRSTree is a sector
	Reflect the sector around its edges to get a complete circular pattern
*/
void FrameManifold::DrawFR2Circle(FRSTree * tree, bool draw_structure, bool draw_ele, bool draw_reflection)
{
	v2d		centre = _v2d_(260, 260);
	v2d		line_to = _v2d_(500, 260);
	double	theta_angle = tree->fr_angle();

	if (tree->pattern_ == 0) return;

	/* get relection pattern */
	SVGPattern * reflect_pattern = Geo2D::PatternReflectionOverLine(tree->pattern_, centre, line_to);

	/* draw */
	glPushMatrix();

	if (draw_structure)
	{
		glColor3f(1, 0, 0);
		tree->Draw(false, false);
		glColor3f(1, 1, 1);
	}
	glColor3f(0, 0, 0);
	if (draw_ele)
		tree->pattern_->drawPattern();

	if (draw_reflection)
	{
		reflect_pattern->drawPattern();

		int num = (int)(360 / (theta_angle * 2)) - 1;
		while (num-- > 0)
		{
			glTranslated(centre[0], centre[1], 0);
			glRotated(theta_angle * 2, 0, 0, 1);
			glTranslated(-centre[0], -centre[1], 0);
			//tree->Draw(false,false);
			tree->pattern_->drawPattern();
			reflect_pattern->drawPattern();
		}

		/*glTranslated(centre[0],centre[1],0);
		glRotated(theta_angle * 2,0,0,1);
		glTranslated(-centre[0],-centre[1],0);
		//tree->Draw(false,false);
		tree->pattern_->drawPattern();
		reflect_pattern->drawPattern();

		glTranslated(centre[0],centre[1],0);
		glRotated(theta_angle * 2,0,0,1);
		glTranslated(-centre[0],-centre[1],0);
		//tree->Draw(false,false);
		tree->pattern_->drawPattern();
		reflect_pattern->drawPattern();

		glTranslated(centre[0],centre[1],0);
		glRotated(theta_angle * 2,0,0,1);
		glTranslated(-centre[0],-centre[1],0);
		//tree->Draw(false,false);
		tree->pattern_->drawPattern();
		reflect_pattern->drawPattern();*/
	}

	glPopMatrix();

	delete reflect_pattern;
}