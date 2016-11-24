#include "FrameFRStructure.h"
#include "CommonGL.h"

#include "FrameButton.h"
#include "FrameChecker.h"
#include "ButtonClick.h"
#include "CheckerClick.h"
#include "GLUTMain.h"

#include "SVGPattern.h"
#include "SVGElement.h"
#include "FRNode.h"
#include "Geo2D.h"
#include "FRStructure.h"

FrameFRStructure::FrameFRStructure() 
{
	this->frs_tree_				= 0;
}

FrameFRStructure::FrameFRStructure(Frame * parent, int lx, int ly, int width, int height,string name)
	: Frame(parent,lx,ly,width,height,name) 
{
	this->frs_tree_				= 0;

	/* parameters that can be changed */
	this->fregion_file_name_		= "D:/misc/fregion/drawing-1.txt";
	this->frstree_file_name_		= "D:/misc/frstree/2.txt";      

	/* create or load a fregion */
	/* load the fregion */
	LoadFR(this->fregion_file_name_.c_str());

	/* create buttons or sliders for this frame */
	FrameChecker * checker_select_node = new FrameChecker(this,width-190,height - 35, 180,25,"FRS Select Node",0,0,0,new CheckerFRStructureSelectNodeClick());
	checker_select_node->setBkColor(255,229,204,0);
	this->addFrame(checker_select_node);

	FrameChecker * checker_select_ele = new FrameChecker(this,width - 190,height - 70,180,25,"FRS Select Ele",0,0,0,new CheckerFRStructureSelectElementClick());
	checker_select_ele->setBkColor(255,229,204,0);
	this->addFrame(checker_select_ele);

	FrameChecker * checker_multiple = new FrameChecker(this,width - 190,height - 105,180,25,"FRS Multiple Sel",0,0,0);
	checker_multiple->setBkColor(255,229,204,0);
	this->addFrame(checker_multiple);

	FrameChecker * checker_add_line_node = new FrameChecker(this,width - 190,height - 140,180,25,"FRS Add Line",0,0,0, new CheckerFRStructureAddLineClick());
	checker_add_line_node->setBkColor(255,229,204,0);
	this->addFrame(checker_add_line_node);

	FrameChecker * checker_add_bezier_node = new FrameChecker(this,width - 190,height - 175,180,25,"FRS Add Bezier",0,0,0, new CheckerFRStructureAddBezierClick());
	checker_add_bezier_node->setBkColor(255,229,204,0);
	this->addFrame(checker_add_bezier_node);

	FrameChecker * checker_edit_node = new FrameChecker(this,width - 190,height - 210,180,25,"FRS Edit Node",0,0,0, new CheckerFRStructureEditNodeClick());
	checker_edit_node->setBkColor(255,229,204,0);
	this->addFrame(checker_edit_node);

	FrameButton * button_save_fr = new FrameButton(this,width - 190,height - 270,180,50,"FRS Save FRS", new ButtonFRSSaveFRClick());
	this->addFrame(button_save_fr);
}

FrameFRStructure::~FrameFRStructure() 
{
	if(this->frs_tree_ != 0)
		delete this->frs_tree_;
}

void FrameFRStructure::drawScene() 
{
	int gx,gy;
	_xyInGlobal(gx,gy);

	glEnable(GL_SCISSOR_TEST);
	glScissor(gx,gy,width,height);
	glClearColor(br/255.0f,bg/255.0f,bb/255.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	
	glViewport(gx,gy,width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,width,0,height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	writeText(GLUT_BITMAP_9_BY_15,
		0,20,
		"F1: Split",
		255,255,255,width,height);

	/* draw fr structure tree */
	if(this->frs_tree_ != 0)
	{
		glPushMatrix();
			glScaled(0.4,0.4,1);
			glTranslated(50,1000,0);
			this->frs_tree_->Draw();	
		glPopMatrix();
	}
	
	/* draw current nodes and elements */
	for(size_t i = 0;i<this->current_elements_.size();i++)
		this->current_elements_[i]->drawElement();

	for(size_t i = 0;i<this->current_nodes_.size();i++)
		this->current_nodes_[i]->Draw();

	/* draw selected nodes and elements */
	glColor3f(1,0,1);
	glLineWidth(1);
	for(size_t i = 0;i<this->selected_fr_nodes_.size();i++)
	{
		this->selected_fr_nodes_[i]->Draw();
	}

	for(size_t i = 0;i<this->selected_elements_.size();i++)
	{
		this->selected_elements_[i]->drawElement();
	}

	glLineWidth(1);
	glColor3f(1,1,1); 
		
	/* draw control points of selected one node 
		and its left and right node */
	if(this->selected_fr_nodes_.size() == 1)
	{
		glColor3f(1,0,0);
		glPointSize(10);
		this->selected_fr_nodes_[0]->DrawControlPoints();
		glPointSize(1);
		glColor3f(1,1,1);

		if(this->selected_fr_nodes_[0]->left_node())
		{
			glColor3f(0,1,0);
			glLineWidth(2);
			this->selected_fr_nodes_[0]->left_node()->Draw();
			glLineWidth(1);
			glColor3f(1,1,1);
		}

		if(this->selected_fr_nodes_[0]->right_node())
		{
			glColor3f(0,0,1);
			glLineWidth(2);
			this->selected_fr_nodes_[0]->right_node()->Draw();
			glLineWidth(1);
			glColor3f(1,1,1);
		}
	}

	/* control points when add node */
	glColor3f(1,0,0);
	glPointSize(10);

	glBegin(GL_POINTS);

		for(size_t i = 0;i<this->line_control_points_.size();i++)
			glVertex2d(this->line_control_points_[i][0],this->line_control_points_[i][1]);

		for(size_t i = 0;i<this->bezier_control_points_.size();i++)
			glVertex2d(this->bezier_control_points_[i][0],this->bezier_control_points_[i][1]);

	glEnd();

	glPointSize(1);
	glColor3f(1,1,1);

	/* draw child frames */
	for(size_t i = 0;i<childFrames.size();i++)
		if(childFrames[i])
			childFrames[i]->drawScene();
}

void FrameFRStructure::LoadFR(const char * filename)
{
	/* initialize FR Structure Tree (FRSTree) from FRegion file */
	ifstream infile(filename);

	if(!infile) return;

	this->frs_tree_ = new FRSTree();

	this->frs_tree_->Load(infile);

	/* set current fr nodes and elements */
	this->SetCurrentNodeAndElements();
}

void FrameFRStructure::SaveFRSTree()
{
	ofstream outfile(this->frstree_file_name_.c_str());
	if(this->frs_tree_ != 0) this->frs_tree_->Save(outfile);
}

void FrameFRStructure::onMouseFunc(int button,int state,int x,int y)
{
	int lx,ly;
	this->globalPt2Local(lx,ly,x,y);
	
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();	
	FrameChecker * checker_select_node = (FrameChecker*)(main_frame->getFrameByName("FRS Select Node"));
	FrameChecker * checker_add_line = (FrameChecker*)(main_frame->getFrameByName("FRS Add Line"));
	FrameChecker * checker_add_bezier = (FrameChecker*)(main_frame->getFrameByName("FRS Add Bezier"));
	FrameChecker * checker_edit_node = (FrameChecker*)(main_frame->getFrameByName("FRS Edit Node"));
	FrameChecker * checker_multi = (FrameChecker*)(main_frame->getFrameByName("FRS Multiple Sel"));
	FrameChecker * checker_sel_ele = (FrameChecker*)(main_frame->getFrameByName("FRS Select Ele"));
	

	if(checker_select_node->getStatus())
	{
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			std::vector<double> dist;
			for(size_t i = 0;i<this->current_nodes_.size();i++)
			{
				if(this->current_nodes_[i]->NodeType() == 0)
				{
					v2d tmpv;
					v2d p0 = this->current_nodes_[i]->pts(0);
					v2d p1 = this->current_nodes_[i]->pts(1);

					dist.push_back(Geo2D::DistanceBetweenPointLineseg(tmpv,_v2d_(lx,ly),p0,p1));
				}
				else
				{
					v2d tmpv;
					v2d p0 = this->current_nodes_[i]->pts(0);
					v2d p1 = this->current_nodes_[i]->pts(1);
					v2d p2 = this->current_nodes_[i]->pts(2);
					v2d p3 = this->current_nodes_[i]->pts(3);

					dist.push_back(Geo2D::DistanceBetweenPointBezier(tmpv,_v2d_(lx,ly),p0,p1,p2,p3));
				}
			}

			double mind = dist[0];
			int minidx = 0;
			for(size_t i = 1;i<dist.size();i++)
			{
				if(dist[i] < mind)
				{
					mind = dist[i];
					minidx = (int)i;
				}
			}

			if(checker_multi->getStatus())
				this->selected_fr_nodes_.push_back(this->current_nodes_[minidx]);
			else
			{
				this->selected_fr_nodes_.clear();
				this->selected_fr_nodes_.push_back(this->current_nodes_[minidx]);
			}
		}
	}
	else if(checker_add_line->getStatus())
	{
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			if(this->line_control_points_.empty())
			{
				this->line_control_points_.push_back(_v2d_(lx,ly));
			}
			else if(this->line_control_points_.size() == 1)
			{
				FRNodeLine * node = new FRNodeLine();
				node->set_pts(0,this->line_control_points_[0]);
				node->set_pts(1,_v2d_(lx,ly));
				this->AddNode(node);
				this->line_control_points_.clear();
			}
		}
	}
	else if(checker_add_bezier->getStatus())
	{
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			if(this->bezier_control_points_.size() < 3)
			{
				this->bezier_control_points_.push_back(_v2d_(lx,ly));
			}
			else
			{
				FRNodeBezier * node = new FRNodeBezier();
				node->set_pts(0,bezier_control_points_[0]);
				node->set_pts(1,bezier_control_points_[1]);
				node->set_pts(2,bezier_control_points_[2]);
				node->set_pts(3,_v2d_(lx,ly));
				this->AddNode(node);
				this->bezier_control_points_.clear();
			}
		}
	}
	else if(checker_edit_node->getStatus() && this->selected_fr_nodes_.size() == 1)
	{
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && 
			this->ComputeControlPtIndexByMousePos(this->edit_control_point_idx_,*(this->selected_fr_nodes_[0]),_v2d_(lx,ly)))
		{
			this->isLeftButtonDown = true;
			this->mouseOrgX = lx;
			this->mouseOrgY = ly;
		}
		else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP && this->isLeftButtonDown)
		{
			this->isLeftButtonDown = false;
			double dx = lx - this->mouseOrgX;
			double dy = ly - this->mouseOrgY;
			v2d last_pt = this->selected_fr_nodes_[0]->pts(this->edit_control_point_idx_);
			this->selected_fr_nodes_[0]->set_pts(this->edit_control_point_idx_,_v2d_(last_pt[0] + dx, last_pt[1] + dy));
			this->NodeStickWhenEdit(this->current_nodes_,this->selected_fr_nodes_[0],this->edit_control_point_idx_);
		}
	}
	else if(checker_sel_ele->getStatus())
	{
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			double mind = 1e10;
			int minidx = -1;
			for(size_t i = 0;i<this->current_elements_.size();i++)
			{
				double d = this->current_elements_[i]->DistanceFromPoint(_v2d_(lx,ly));
				if(d < mind)
				{
					mind = d;
					minidx = (int)i;
				}
			}

			if(minidx != -1)
			{
				if(checker_multi->getStatus())
					this->selected_elements_.push_back(this->current_elements_[minidx]);
				else
				{
					this->selected_elements_.clear();
					this->selected_elements_.push_back(this->current_elements_[minidx]);
				}
			}
		}
	}
}

void FrameFRStructure::onMotionFunc(int x,int y)
{
	int lx,ly;
	this->globalPt2Local(lx,ly,x,y);

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();	
	FrameChecker * checker_edit_node = (FrameChecker*)(main_frame->getFrameByName("FRS Edit Node"));

	if(checker_edit_node->getStatus() && this->selected_fr_nodes_.size() == 1 && this->isLeftButtonDown)
	{
		double dx = lx - this->mouseOrgX;
		double dy = ly - this->mouseOrgY;
		v2d last_pt = this->selected_fr_nodes_[0]->pts(this->edit_control_point_idx_);
		this->selected_fr_nodes_[0]->set_pts(this->edit_control_point_idx_,_v2d_(last_pt[0] + dx, last_pt[1] + dy));
		this->mouseOrgX = lx;
		this->mouseOrgY = ly;
	}
}

void FrameFRStructure::onSpecialKeyboardFunc(int key,int x,int y)
{
	if(key == GLUT_KEY_F1)
	{
		if(this->selected_fr_nodes_.size() == 1)
			this->frs_tree_->SplitFRSNode(this->frs_tree_->current_node_,this->selected_fr_nodes_[0]);
	}
	else if(key == GLUT_KEY_LEFT)
	{
		if(this->frs_tree_->current_node_->parrent_ == 0) return;
		if(this->frs_tree_->current_node_->parrent_->left_child_ == 0) return;
		this->frs_tree_->current_node_ = this->frs_tree_->current_node_->parrent_->left_child_;
		this->SetCurrentNodeAndElements();
	}
	else if(key == GLUT_KEY_RIGHT)
	{
		if(this->frs_tree_->current_node_->parrent_ == 0) return;
		if(this->frs_tree_->current_node_->parrent_->right_child_ == 0) return;	
		this->frs_tree_->current_node_ = this->frs_tree_->current_node_->parrent_->right_child_;
		this->SetCurrentNodeAndElements();
	}
	else if(key == GLUT_KEY_UP)
	{
		if(this->frs_tree_->current_node_->parrent_ == 0) return;
		else this->frs_tree_->current_node_ = this->frs_tree_->current_node_->parrent_;
		this->SetCurrentNodeAndElements();
	}
	else if(key == GLUT_KEY_DOWN)
	{
		if(this->frs_tree_->current_node_->left_child_ != 0)
		{
			this->frs_tree_->current_node_ = this->frs_tree_->current_node_->left_child_;
			this->SetCurrentNodeAndElements();
		}
		else if(this->frs_tree_->current_node_->right_child_ != 0)
		{
			this->frs_tree_->current_node_ = this->frs_tree_->current_node_->right_child_;
			this->SetCurrentNodeAndElements();
		}
	}
}

bool FrameFRStructure::ComputeControlPtIndexByMousePos(int & idx, const FRNode & node, const v2d & pt)
{
	idx = node.SelectControlPoint(pt);
	if(idx == -1) return false;
	else return true;
}

void FrameFRStructure::NodeStickWhenAdd(const std::vector<FRNode*> & all_nodes, FRNode * node)
{
	v2d			fe				= node->GetFirstEnd();	
	FRNode *	fe_near_node;
	v2d			fe_near_pt;
	double		fe_near_dist	= this->FindNearestPointOnNodeToPoint(fe_near_pt,fe_near_node,all_nodes,fe);

	if(fe_near_dist < 20)
	{
		node->SetFirstEnd(fe_near_pt);
		node->set_left_node(fe_near_node);
	}

	v2d			le				= node->GetLastEnd();	
	FRNode *	le_near_node;
	v2d			le_near_pt;
	double		le_near_dist	= this->FindNearestPointOnNodeToPoint(le_near_pt,le_near_node,all_nodes,le);

	if(le_near_dist < 20)
	{
		node->SetLastEnd(le_near_pt);
		node->set_right_node(le_near_node);
	}
}

double FrameFRStructure::FindNearestPointOnNodeToPoint(v2d & near_pt, FRNode * & near_node, const std::vector<FRNode*> & nodes, const v2d & pt)
{
	double near_dist	= 1e10;
	near_node			= 0;
	
	for(size_t i = 0;i<nodes.size();i++)
	{
		v2d		local_near_pt;
		double	d;

		if(nodes[i]->NodeType() == 0) // line
			d = Geo2D::DistanceBetweenPointLineseg(local_near_pt,pt,nodes[i]->GetFirstEnd(),nodes[i]->GetLastEnd());
		else // bezier
			d = Geo2D::DistanceBetweenPointBezier(local_near_pt,pt,nodes[i]->pts(0),nodes[i]->pts(1),nodes[i]->pts(2),nodes[i]->pts(3));

		if(d < near_dist)
		{
			near_dist	= d;
			near_node	= nodes[i];
			near_pt		= local_near_pt;
		}
	}

	return near_dist;
}

void FrameFRStructure::AddNode(FRNode * node)
{
	NodeStickWhenAdd(this->current_nodes_,node);
	this->frs_tree_->all_frnodes_.push_back(node);
	this->current_nodes_.push_back(node);
}

void FrameFRStructure::NodeStickWhenEdit(const std::vector<FRNode*> & all_nodes, FRNode * node, int idx)
{
	std::vector<FRNode*> nodes;
	for(size_t i = 0;i<all_nodes.size();i++)
	{
		if(all_nodes[i] != node)
			nodes.push_back(all_nodes[i]);
	}

	if(node->NodeType() == 0)
	{
		v2d			pt				= node->pts(idx);
		FRNode *	pt_near_node;
		v2d			pt_near_pt;
		double		pt_near_dist	= this->FindNearestPointOnNodeToPoint(pt_near_pt,pt_near_node,nodes,pt);

		if(pt_near_dist < 20)
		{
			node->set_pts(idx,pt_near_pt);
			if(idx == 0) node->set_left_node(pt_near_node);
			else node->set_right_node(pt_near_node);
		}
		else
		{
			if(idx == 0) node->set_left_node(0);
			else	node->set_right_node(0);
		}
	}
	else
	{
		if(idx == 0 || idx == 3)
		{
			v2d			pt				= node->pts(idx);
			FRNode *	pt_near_node;
			v2d			pt_near_pt;
			double		pt_near_dist	= this->FindNearestPointOnNodeToPoint(pt_near_pt,pt_near_node,nodes,pt);

			if(pt_near_dist < 20)
			{
				node->set_pts(idx,pt_near_pt);
				if(idx == 0) node->set_left_node(pt_near_node);
				else node->set_right_node(pt_near_node);
			}
			else
			{
				if(idx == 0) node->set_left_node(0);
				else	node->set_right_node(0);
			}
		}
	}
}

void FrameFRStructure::SetCurrentNodeAndElements()
{
	if(this->frs_tree_ == 0) return;
	
	this->frs_tree_->GetCurrentElements(this->current_elements_);
	this->frs_tree_->GetCurrentFRNodes(this->current_nodes_);

	this->selected_elements_.clear();
	this->selected_fr_nodes_.clear();
}