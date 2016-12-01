#include "FrameStructureBlend.h"
#include "CommonGL.h"

#include "FRNode.h"
#include "Geo2D.h"
#include "SVGPattern.h"
#include "FRStructure.h"
#include "SVGElement.h"
#include "Deform2D\RigidMeshDeformer2D.h"
#include "Deform2D\WmlExtTriangleUtils.h"
extern "C"
{
#include "Triangle\triangle.h"
}
#include "Deform2D\TriangleMesh.h"
#include "FrameManifold.h"
#include "GLUTMain.h"
#include "FrameSolidFlowerbed.h"

#define FRS_NODE_SAMPLE_NUM 50

FrameStructureBlend::FrameStructureBlend()
{
	this->tree_index_ = 0;
	this->gen_frs_tree_ = 0;
	for (int i = 0; i < 20; i++)
		arr[i] = 0;
}

FrameStructureBlend::FrameStructureBlend(Frame * parent, int lx, int ly, int width, int height, string name)
	: Frame(parent, lx, ly, width, height, name)
{
	this->tree_index_ = 0;
	this->gen_frs_tree_ = 0;

	for (int i = 0; i < 20; i++)
		arr[i] = 0;

	/* parameters that can be changed */
	// no

	FRSTree * tree;

	char filename[1024];

	for (int i = 1; i <= 10; i++)
	{
		sprintf_s(filename, 1024, "D:/misc/frstree/%d.txt", i);
		tree = LoadFRSTree(filename);
		this->all_trees_.push_back(tree);
	}

	this->CompleteFRSTree();

	for (size_t i = 0; i < this->all_trees_.size(); i++)
	{
		std::vector<FRSNode*>	frs_nodes;
		std::vector<int>		frs_node_codes;
		this->all_trees_[i]->GetLeafNodesAndArrayIndices(frs_nodes, frs_node_codes);

		for (size_t j = 0; j < frs_nodes.size(); j++)
		{
			std::vector<v2d> pts;
			FRSNode * frs_node = frs_nodes[j];
			frs_node->codeFRSNode(all_trees_[i]);
			this->SamplePointsOfFRSNodeBoundary(all_trees_[i], frs_node, pts, FRS_NODE_SAMPLE_NUM);

			this->tree_frsnodes_.push_back(frs_node);
			this->tree_frsnode_boundary_points_.push_back(pts);
			this->tree_frsnode_codes_.push_back(_v2i_(i, frs_node_codes[j]));
		}
	}

	//this->LoadManifoldSimilarCode();
	//this->LoadAlpha();

	//this->ExtractHighDimData();
}

FrameStructureBlend::~FrameStructureBlend()
{
	for (size_t i = 0; i < this->all_trees_.size(); i++)
		delete this->all_trees_[i];

	if (this->gen_frs_tree_ != 0) delete this->gen_frs_tree_;
}

void FrameStructureBlend::drawScene()
{
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

	writeText(GLUT_BITMAP_9_BY_15,
		0, 20,
		"F6: Load Manifold",
		255, 255, 255, width, height);

	/* draw structure tree examples */
	/*if(this->tree_index_ >= 0 && this->tree_index_ < (int)(this->all_trees_.size()))
	{
		glPushMatrix();
			glScaled(0.5,0.5,1);
			glTranslated(200,800,0);
			this->all_trees_[this->tree_index_]->Draw();
		glPopMatrix();
	}*/

	/* draw the generated tree */
	glPushMatrix();
	glTranslated(200, 250, 0);
	//if(this->gen_frs_tree_ != 0) this->gen_frs_tree_->Draw(false);


	glPopMatrix();

	/*glPushMatrix();
	glTranslated(50,150,0);
	this->DrawReflection();
	glPopMatrix();*/



	/* draw child frames */
	for (size_t i = 0; i < childFrames.size(); i++)
		if (childFrames[i])
			childFrames[i]->drawScene();
}

FRSTree* FrameStructureBlend::LoadFRSTree(char * frs_file_name)
{
	ifstream infile(frs_file_name);
	if (!infile) return 0;

	FRSTree * ans = new FRSTree();
	ans->Load(infile);

	return ans;
}

void FrameStructureBlend::onSpecialKeyboardFunc(int key, int x, int y)
{
	if (key == GLUT_KEY_F7)
	{
		this->tree_index_--;
		if (this->tree_index_ < 0)
			this->tree_index_ = (int)(this->all_trees_.size()) - 1;
	}
	else if (key == GLUT_KEY_F8)
	{
		this->tree_index_++;
		if (this->tree_index_ >= (int)(this->all_trees_.size()))
			this->tree_index_ = 0;
	}
	if (key == GLUT_KEY_LEFT)
	{
		FRSTree * tree = this->all_trees_[this->tree_index_];

		if (tree->current_node_->parrent_ == 0) return;
		if (tree->current_node_->parrent_->left_child_ == 0) return;
		tree->current_node_ = tree->current_node_->parrent_->left_child_;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		FRSTree * tree = this->all_trees_[this->tree_index_];

		if (tree->current_node_->parrent_ == 0) return;
		if (tree->current_node_->parrent_->right_child_ == 0) return;
		tree->current_node_ = tree->current_node_->parrent_->right_child_;
	}
	else if (key == GLUT_KEY_UP)
	{
		FRSTree * tree = this->all_trees_[this->tree_index_];

		if (tree->current_node_->parrent_ == 0) return;
		tree->current_node_ = tree->current_node_->parrent_;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		FRSTree * tree = this->all_trees_[this->tree_index_];

		if (tree->current_node_->left_child_ != 0)
			tree->current_node_ = tree->current_node_->left_child_;
		else if (tree->current_node_->right_child_ != 0)
			tree->current_node_ = tree->current_node_->right_child_;
	}
	else if (key == GLUT_KEY_F6)
	{
		FrameSolidFlowerbed * sfb = new FrameSolidFlowerbed(this, 620, 10, 600, 600, "SolidFlowerbed");
		sfb->setBkColor(255, 255, 255, 255);
		this->addFrame(sfb);

		FrameManifold * mani0 = new FrameManifold(this, 10, 10, 600, 600, "Manifold", "D:/misc/manifold/manifold.txt", "D:/misc/manifold/var.txt", "D:/misc/manifold/anchorpoint.txt");
		mani0->setBkColor(255, 255, 255, 255);
		this->addFrame(mani0);

		//this->BuildFRSTree();

		//this->ComputeHeatMapForElementFilling();
	}
	//else if(key == GLUT_KEY_F1)
	//{
		//this->RefreshData();
	//}
}

void FrameStructureBlend::DrawRmsTriangleMesh(const rmsmesh::TriangleMesh & mesh)
{
	unsigned int nTris = mesh.GetNumTriangles();
	for (unsigned int i = 0; i < nTris; ++i)
	{
		Wml::Vector3f vVerts[3];
		mesh.GetTriangle(i, vVerts);

		glBegin(GL_LINE_LOOP);
		glVertex3fv(vVerts[0]);
		glVertex3fv(vVerts[1]);
		glVertex3fv(vVerts[2]);
		glEnd();
	}
}

void FrameStructureBlend::ShowRmsTriangleMeshInImage(const rmsmesh::TriangleMesh & mesh, char * filename)
{
	IplImage * img = cvCreateImage(cvSize(1000, 1000), 8, 3);
	cvZero(img);
	unsigned int nTris = mesh.GetNumTriangles();
	for (unsigned int i = 0; i < nTris; ++i)
	{
		Wml::Vector3f vVerts[3];
		mesh.GetTriangle(i, vVerts);

		int cnt = 1;
		vVerts[0][0] *= cnt;
		vVerts[0][1] *= cnt;
		vVerts[1][0] *= cnt;
		vVerts[1][1] *= cnt;
		vVerts[2][0] *= cnt;
		vVerts[2][1] *= cnt;

		/*vVerts[0][0] -= (cnt * 260 + 0);
		vVerts[0][1] -= (cnt * 260-50);
		vVerts[1][0] -= (cnt * 260 + 0);
		vVerts[1][1] -= (cnt * 260-50);
		vVerts[2][0] -= (cnt * 260 + 0);
		vVerts[2][1] -= (cnt * 260-50);*/

		cvLine(img, cvPoint((int)(vVerts[0][0]), (int)(vVerts[0][1])), cvPoint((int)(vVerts[1][0]), (int)(vVerts[1][1])), cvScalar(255, 255, 255));
		cvLine(img, cvPoint((int)(vVerts[1][0]), (int)(vVerts[1][1])), cvPoint((int)(vVerts[2][0]), (int)(vVerts[2][1])), cvScalar(255, 255, 255));
		cvLine(img, cvPoint((int)(vVerts[2][0]), (int)(vVerts[2][1])), cvPoint((int)(vVerts[0][0]), (int)(vVerts[0][1])), cvScalar(255, 255, 255));
	}
	cvSaveImage(filename, img);
	cvReleaseImage(&img);
}

v2d FrameStructureBlend::DeformPoint(const v2d & v, const rmsmesh::TriangleMesh * src_mesh, const rmsmesh::TriangleMesh * dst_mesh)
{
	v2d ans = v;

	bool flag_in_mesh = false;

	for (unsigned int i = 0; i < src_mesh->GetNumTriangles(); i++)
	{
		unsigned int nTriangle[3];
		src_mesh->GetTriangle(i, nTriangle);

		Wml::Vector3f v0, v1, v2;
		double w0, w1, w2;

		src_mesh->GetVertex(nTriangle[0], v0);
		src_mesh->GetVertex(nTriangle[1], v1);
		src_mesh->GetVertex(nTriangle[2], v2);

		Wml::BarycentricCoords(Wml::Vector2d(v0.X(), v0.Y()),
			Wml::Vector2d(v1.X(), v1.Y()),
			Wml::Vector2d(v2.X(), v2.Y()),
			Wml::Vector2d(v[0], v[1]),
			w0,
			w1,
			w2);


		if (w0 >= 0 && w0 <= 1 &&
			w1 >= 0 && w1 <= 1 &&
			w2 >= 0 && w2 <= 1)
		{
			dst_mesh->GetTriangle(i, nTriangle);

			Wml::Vector3f p0, p1, p2;

			dst_mesh->GetVertex(nTriangle[0], p0);
			dst_mesh->GetVertex(nTriangle[1], p1);
			dst_mesh->GetVertex(nTriangle[2], p2);

			ans[0] = p0.X() * w0 + p1.X() * w1 + p2.X() * w2;
			ans[1] = p0.Y() * w0 + p1.Y() * w1 + p2.Y() * w2;

			flag_in_mesh = true;

			break;
		}
	}

	// 2015.05.19	if not in mesh, find nearest triangle
	if (!flag_in_mesh)
	{
		double min_dist = 1e20;
		int min_idx = -1;

		Wml::Vector3f v0, v1, v2;
		unsigned int nTriangle[3];

		double w0, w1, w2;

		for (unsigned int i = 0; i < src_mesh->GetNumTriangles(); i++)
		{
			src_mesh->GetTriangle(i, nTriangle);

			src_mesh->GetVertex(nTriangle[0], v0);
			src_mesh->GetVertex(nTriangle[1], v1);
			src_mesh->GetVertex(nTriangle[2], v2);

			v2d p0 = _v2d_(v0.X(), v0.Y());
			v2d p1 = _v2d_(v1.X(), v1.Y());
			v2d p2 = _v2d_(v2.X(), v2.Y());

			double dist = Geo2D::DistanceBetweenPointTriangle(v, p0, p1, p2);
			if (dist < min_dist)
			{
				min_dist = dist;
				min_idx = i;
			}
		}

		src_mesh->GetTriangle(min_idx, nTriangle);

		src_mesh->GetVertex(nTriangle[0], v0);
		src_mesh->GetVertex(nTriangle[1], v1);
		src_mesh->GetVertex(nTriangle[2], v2);

		Wml::BarycentricCoords(Wml::Vector2d(v0.X(), v0.Y()),
			Wml::Vector2d(v1.X(), v1.Y()),
			Wml::Vector2d(v2.X(), v2.Y()),
			Wml::Vector2d(v[0], v[1]),
			w0,
			w1,
			w2);

		dst_mesh->GetTriangle(min_idx, nTriangle);

		dst_mesh->GetVertex(nTriangle[0], v0);
		dst_mesh->GetVertex(nTriangle[1], v1);
		dst_mesh->GetVertex(nTriangle[2], v2);

		ans[0] = v0.X() * w0 + v1.X() * w1 + v2.X() * w2;
		ans[1] = v0.Y() * w0 + v1.Y() * w1 + v2.Y() * w2;

		//cout<<w0<<","<<w1<<","<<w2<<endl;
	}

	return ans;
}

void FrameStructureBlend::DeformElements(std::vector<SVGElement*> & deformed_elements, const std::vector<SVGElement*> & src_elements, const rmsmesh::TriangleMesh * src_mesh, const rmsmesh::TriangleMesh * dst_mesh)
{
	for (size_t i = 0; i < src_elements.size(); i++)
	{
		SVGElement * src_ele = src_elements[i];

		SVGElement * new_ele = new SVGElement(*src_ele);

		for (size_t j = 0; j < new_ele->segs_.size(); j++)
		{
			ElementSeg * seg = new_ele->segs_[j];
			if (seg->SegType() == 0)
			{
				ElementSegLine * segl = (ElementSegLine*)seg;
				segl->pts_[0] = this->DeformPoint(segl->pts_[0], src_mesh, dst_mesh);
				segl->pts_[1] = this->DeformPoint(segl->pts_[1], src_mesh, dst_mesh);
			}
			else
			{

				ElementSegBezier * segb = (ElementSegBezier*)seg;
				segb->pts_[0] = this->DeformPoint(segb->pts_[0], src_mesh, dst_mesh);
				segb->pts_[1] = this->DeformPoint(segb->pts_[1], src_mesh, dst_mesh);
				segb->pts_[2] = this->DeformPoint(segb->pts_[2], src_mesh, dst_mesh);
				segb->pts_[3] = this->DeformPoint(segb->pts_[3], src_mesh, dst_mesh);
			}
		}

		deformed_elements.push_back(new_ele);
	}
}

void FrameStructureBlend::DeformFRSNode(FRSTree * src_tree, FRSNode * src_node, FRSTree * dst_tree, FRSNode * dst_node, std::vector<SVGElement*> & eles)
{
	std::vector<v2d> src_pts, dst_pts, src_frs_node_boundary_points, dst_frs_node_boundary_points;

	/*this->SamplePointsOfFRSNodeBoundary(src_tree,src_node,src_frs_node_boundary_points,20);
	this->SamplePointsOfFRSNodeBoundary(dst_tree,dst_node,dst_frs_node_boundary_points,20);

	size_t point_num = src_frs_node_boundary_points.size() < dst_frs_node_boundary_points.size() ?
		src_frs_node_boundary_points.size() : dst_frs_node_boundary_points.size();

	for(size_t i = 0;i<point_num;i++)
	{
		src_pts.push_back(src_frs_node_boundary_points[i]);
		dst_pts.push_back(dst_frs_node_boundary_points[i]);
	}*/

	this->SamplePointsOfFRSNodeBoundaryBySeg(src_tree, src_node, src_frs_node_boundary_points);
	this->SamplePointsOfFRSNodeBoundaryBySeg(dst_tree, dst_node, dst_frs_node_boundary_points);

	size_t point_num, less_point_num;

	if (src_frs_node_boundary_points.size() < dst_frs_node_boundary_points.size())
	{
		point_num = src_frs_node_boundary_points.size();

		for (size_t i = 0; i < point_num; i++)
		{
			src_pts.push_back(src_frs_node_boundary_points[i]);
			dst_pts.push_back(dst_frs_node_boundary_points[i]);
		}
	}
	else
	{
		point_num = src_frs_node_boundary_points.size();
		less_point_num = dst_frs_node_boundary_points.size();

		for (size_t i = 0; i < less_point_num; i++)
		{
			src_pts.push_back(src_frs_node_boundary_points[i]);
			dst_pts.push_back(dst_frs_node_boundary_points[i]);
		}

		for (size_t i = less_point_num; i < point_num; i++)
		{
			dst_pts.push_back(dst_frs_node_boundary_points[less_point_num - 1]);
			src_pts.push_back(src_frs_node_boundary_points[i]);
		}
	}

	//this->ShowPointsInImage(src_pts,dst_pts);

		/* get segs for the source polygon */
	std::vector<v2i> src_segs;

	for (int i = 0; i < (int)(point_num); i++)
		src_segs.push_back(_v2i_(i, (i + 1) % (int)(point_num)));

	/* get source mesh */
	triangulateio * input = Geo2D::InputToTriangulateio(src_pts, src_segs);
	triangulateio * output = Geo2D::ComputeMeshByTriangle(input);

	rmsmesh::TriangleMesh * src_mesh = new rmsmesh::TriangleMesh();	// should be released
	rmsmesh::TriangleMesh * dst_mesh = new rmsmesh::TriangleMesh(); // should be released

	Geo2D::TriangulateioToOutput(output, *src_mesh);

	Geo2D::FreeTriangulateio(&input, true);
	Geo2D::FreeTriangulateio(&output);

	/* deform the source mesh to get dst mesh*/

	rmsmesh::RigidMeshDeformer2D deformer;

	deformer.InitializeFromMesh(src_mesh); // initial mesh

	for (size_t i = 0; i < point_num; i++)
		deformer.SetDeformedHandle(i, Wml::Vector2f((float)(dst_pts[i][0]), (float)(dst_pts[i][1])));

	deformer.ForceValidation(); // compute matrices

	unsigned int nVerts = src_mesh->GetNumVertices();
	for (unsigned int i = 0; i < nVerts; ++i) {
		Wml::Vector3f vVertex(0, 0, 0);
		dst_mesh->AppendVertexData(vVertex);
	}

	unsigned int nTris = src_mesh->GetNumTriangles(); // topology relationship is not changed 
	for (unsigned int i = 0; i < nTris; ++i) {
		unsigned int nTriangle[3];
		src_mesh->GetTriangle(i, nTriangle);
		dst_mesh->AppendTriangleData(nTriangle);
	}

	deformer.UpdateDeformedMesh(dst_mesh, true);	// get vertex coordinates after deformation

	/*this->ShowRmsTriangleMeshInImage(*src_mesh,"F:/src.png");
	this->ShowRmsTriangleMeshInImage(*dst_mesh,"F:/dst.png");*/

	/* deform elements of the source FRS node */
	std::vector<SVGElement*> src_elements;
	for (size_t i = 0; i < src_node->element_indices_.size(); i++)
		src_elements.push_back(src_tree->pattern_->elements_[src_node->element_indices_[i]]);

	/*std::vector<v2d> ppp;
	for(size_t i = 0;i<src_elements.size();i++)
	{
		for(size_t j = 0;j<src_elements[i]->segs_.size();j++)
		{
			src_elements[i]->segs_[j]->collectPoints(ppp,0.1);
		}
	}*/
	//this->ShowPointsInImage(src_pts,ppp);
	//this->ShowPointsInImage(dst_pts,ppp);

	int num_before = (int)(eles.size());
	this->DeformElements(eles, src_elements, src_mesh, dst_mesh);
	int num_after = (int)(eles.size());

	for (int i = num_before; i < num_after; i++) dst_node->element_indices_.push_back(i);

	delete src_mesh;
	delete dst_mesh;
}



void FrameStructureBlend::SubdivideFRSTreeShell(FRSTree * tree, std::vector<double> hddata)
{
	int cnt = 0;
	for (int i = 0; i < 20; i++)
	{
		if (arr[i] == 1)
		{
			if (arr[2 * i + 1] == 1 && arr[2 * i + 2] == 1)
			{
				v6d curve;
				for (int j = 0; j < 6; j++)
					curve[j] = hddata[cnt * 6 + j];

				FRSNode * node = tree->FindFRSNodeByArrayIndex(i);

				tree->SplitFRSNode(node, curve);
				cnt++;
			}
		}
	}
}

v3i FrameStructureBlend::Candidate2Color(const std::vector<int> & cand)
{
	if (cand.size() != 4)
	{
		cout << "FrameStructureBlend::Candidate2Color - error" << endl;
		return _v3i_(0, 0, 0);
	}


	int a = cand[0] / 4;
	int b = cand[1] / 4;
	int c = cand[2] / 4;
	int d = cand[3] / 4;


	int r = a * 15 + b * 5;
	int g = c * 20;
	int bb = d * 20;

	return _v3i_(r, g, bb);
}

void FrameStructureBlend::ComputeHeatMapForElementFilling()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));

	int manifold_dim_x = mani->dim_x();
	int manifold_dim_y = mani->dim_y();

	IplImage * img = cvCreateImage(cvSize(manifold_dim_x, manifold_dim_y), 8, 3);
	cvZero(img);

	union foo
	{
		char c[sizeof(int)];
		int d;
	} bar;

	ofstream outfile("D:/test.txt", std::ios::binary);
	bar.d = manifold_dim_x;
	outfile.write(bar.c, sizeof(int));
	bar.d = manifold_dim_y;
	outfile.write(bar.c, sizeof(int));

	for (int x = 0; x < manifold_dim_x; x++)
	{
		for (int y = 0; y < manifold_dim_y; y++)
		{
			//double variance = mani->variance(x,y);
			//if(variance < 0.75) continue;

			FRSTree * tree = this->BuildTreeStructureForManifoldPosition(x, y);


			std::vector<int> corre;
			this->ComputeMostSimilarCandidatesForTree(tree, corre);

			for (size_t i = 0; i < corre.size(); i++)
			{
				bar.d = corre[i];
				outfile.write(bar.c, sizeof(int));
			}

			v3i color = this->Candidate2Color(corre);
			LibIV::CppHelpers::Global::setImgData(color[0], color[1], color[2], img, x, y);

			delete tree;
		}
		cout << x << endl;
	}
	outfile.close();
	cvSaveImage("D:/heat.png", img);
	cvReleaseImage(&img);
}

FRSTree* FrameStructureBlend::BuildFRSTreeShell(double shell_angle, double shell_radius, v2d shell_centre)
{
	FRSTree * frst = new FRSTree();

	frst->set_fr_angle(shell_angle);

	double  theta1_radian = shell_angle * _PI_ / 180;
	double	theta2_radian = 0;

	v2d p0, p1, p2;

	p0 = shell_centre;

	p1[0] = shell_centre[0] + shell_radius * cos(theta1_radian);
	p1[1] = shell_centre[1] + shell_radius * sin(theta1_radian);

	p2[0] = shell_centre[0] + shell_radius * cos(theta2_radian);
	p2[1] = shell_centre[1] + shell_radius * sin(theta2_radian);

	FRNodeLine * n0 = new FRNodeLine();
	n0->set_pts(0, p0);
	n0->set_pts(1, p1);

	FRNodeLine * n1 = new FRNodeLine();
	n1->set_pts(0, p1);
	n1->set_pts(1, p2);

	FRNodeLine * n2 = new FRNodeLine();
	n2->set_pts(0, p2);
	n2->set_pts(1, p0);

	frst->all_frnodes_.push_back(n0);
	frst->all_frnodes_.push_back(n1);
	frst->all_frnodes_.push_back(n2);

	frst->frs_root_node_ = new FRSNode();
	frst->frs_root_node_->frnode_indices_.push_back(0);
	frst->frs_root_node_->frnode_indices_.push_back(1);
	frst->frs_root_node_->frnode_indices_.push_back(2);

	frst->current_node_ = frst->frs_root_node_;

	return frst;
}

FRSTree * FrameStructureBlend::BuildTreeStructureForManifoldPosition(int x, int y)
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));
	std::vector<double> hddata = mani->GetManifoldValue(x, y);

	/*for(size_t i = 0;i<hddata.size();i++)
	{
		cout<<hddata[i]<<",";
	}
	cout<<endl;*/

	/*double data[19] = {36.6707,0.281555,428.625,354.251,451.154,296.419,0.717979,0.292052,369.231,320.293,367.914,294.838,0.787156,0.128414,454.347,370.012,459.407,306.137,0.740453};
	std::vector<double> hddata;
	for(int i = 0;i<19;i++)
		hddata.push_back(data[i]);*/

	double angle = FRSTree::BestAngle(hddata[0]);
	std::vector<double> hddata_structure;
	for (size_t i = 1; i < hddata.size(); i++)
		hddata_structure.push_back(hddata[i]);

	FRSTree * ans = this->BuildFRSTreeShell(angle, 280, _v2d_(260, 260));
	this->SubdivideFRSTreeShell(ans, hddata_structure);

	return ans;
}

void FrameStructureBlend::ComputeMostSimilarCandidatesForTree(FRSTree * tree, std::vector<int> & corre)
{
	std::vector<FRSNode*> leaf_nodes;
	std::vector<int> leaf_node_codes;
	std::vector<std::vector<v2d>> leaf_node_boundary_points;

	tree->GetLeafNodesAndArrayIndices(leaf_nodes, leaf_node_codes);
	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		std::vector<v2d> pts;
		this->SamplePointsOfFRSNodeBoundary(tree, leaf_nodes[i], pts, FRS_NODE_SAMPLE_NUM);
		leaf_node_boundary_points.push_back(pts);
	}

	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		int code = leaf_node_codes[i];
		// find all possible examples
		std::vector<int> possible_example_id;
		for (size_t j = 0; j < this->tree_frsnode_codes_.size(); j++)
		{
			if (code == this->tree_frsnode_codes_[j][1])
				possible_example_id.push_back(j);
		}

		// find the most similar example
		double min_dist = 1e20;
		int min_idx = -1;
		for (size_t j = 0; j < possible_example_id.size(); j++)
		{
			std::vector<v2d> dst_points;
			this->ResizeFRSNodeBoundaryPointsByAngle(tree->fr_angle(),
				this->all_trees_[this->tree_frsnode_codes_[possible_example_id[j]][0]]->fr_angle(),
				_v2d_(260, 260),
				this->tree_frsnode_boundary_points_[possible_example_id[j]],
				dst_points);

			//this->ShowPointsInImage(leaf_node_boundary_points[i],dst_points);

			double dist = this->DistanceBetweenTwoPointSets(leaf_node_boundary_points[i], dst_points);

			if (dist < min_dist)
			{
				min_dist = dist;
				min_idx = possible_example_id[j];
			}
		}

		corre.push_back(min_idx);
	}
}

void FrameStructureBlend::DeformCandidates(FRSTree * tree, const std::vector<int> & corre)
{
	std::vector<FRSNode*> leaf_nodes;
	std::vector<int> leaf_node_codes;
	std::vector<std::vector<v2d>> leaf_node_boundary_points;

	tree->GetLeafNodesAndArrayIndices(leaf_nodes, leaf_node_codes);
	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		std::vector<v2d> pts;
		this->SamplePointsOfFRSNodeBoundary(tree, leaf_nodes[i], pts, FRS_NODE_SAMPLE_NUM);
		leaf_node_boundary_points.push_back(pts);
	}

	if (tree->pattern_ != 0)
		delete tree->pattern_;
	tree->pattern_ = new SVGPattern();

	for (size_t i = 0; i < leaf_nodes.size(); i++)
		//for(size_t i = 3;i<4;i++)
	{
		FRSTree * corre_tree = this->all_trees_[this->tree_frsnode_codes_[corre[i]][0]];
		FRSNode * corre_node = this->tree_frsnodes_[corre[i]];

		/*char filename[1024];
		sprintf_s(filename,1024,"F:/test%d_src.png",leaf_node_codes[i]);
		ShowFRSNodeInImage(filename,corre_tree,leaf_node_codes[i],true);
		sprintf_s(filename,1024,"F:/test%d_dst.png",leaf_node_codes[i]);
		ShowFRSNodeInImage(filename,tree,leaf_node_codes[i],true);*/

		if (corre_node->element_indices_.empty()) continue;
		// to be added ...
		// if the leaf_node is very small, continue

		this->DeformFRSNode(corre_tree, corre_node, tree, leaf_nodes[i],
			tree->pattern_->elements_);

		/*sprintf_s(filename,1024,"F:/test%d_dst_after.png",leaf_node_codes[i]);
		ShowFRSNodeInImage(filename,tree,leaf_node_codes[i],true);*/
	}
}

void FrameStructureBlend::BuildFRSTreeTest()
{
	if (this->gen_frs_tree_ != 0) delete this->gen_frs_tree_;

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));

	this->gen_frs_tree_ = this->BuildTreeStructureForManifoldPosition(mani->manifold_x(), mani->manifold_y());

	std::vector<int> corre;
	this->ComputeMostSimilarCandidatesForTree(this->gen_frs_tree_, corre);

	this->DeformCandidates(this->gen_frs_tree_, corre);
}

void FrameStructureBlend::BuildTreeByCode()
{
	if (this->gen_frs_tree_ != 0) delete this->gen_frs_tree_;

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));

	this->gen_frs_tree_ = this->BuildTreeStructureForManifoldPosition(mani->manifold_x(), mani->manifold_y());

	v4i code = this->manifold_similar_codes_.at(mani->manifold_y(), mani->manifold_x());

	std::vector<int> corre;
	corre.push_back(code[0]);
	corre.push_back(code[1]);
	corre.push_back(code[2]);
	corre.push_back(code[3]);

	this->DeformCandidates(this->gen_frs_tree_, corre);
}

void FrameStructureBlend::BuildFRSTreeforfiletest(ofstream  & outfile)
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));
	std::vector<double> hddata = mani->GetManifoldValue();

	/*for(size_t i = 0;i<hddata.size();i++)
	{
	cout<<hddata[i]<<" ";
	}
	cout<<endl;*/

	/*double hddata_[19] = {24.3385, 0.140191, 387.491, 280.975, 425.65, 275.01, 0.719131, 0.266481, 355.745, 276.456, 379.443, 272.124, 0.590594, 0.160899, 453.179, 305.8, 482.751, 291.348, 0.724278};
	std::vector<double> hddata;
	for(int ii = 0;ii<19;ii++)
	hddata.push_back(hddata_[ii]);*/

	double angle = FRSTree::BestAngle(hddata[0]);
	std::vector<double> hddata_structure;
	for (size_t i = 1; i < hddata.size(); i++)
		hddata_structure.push_back(hddata[i]);

	if (this->gen_frs_tree_ != 0)		delete this->gen_frs_tree_;

	// generate a new structure
	gen_frs_tree_ = this->BuildFRSTreeShell(angle, 280, _v2d_(260, 260));
	this->SubdivideFRSTreeShell(this->gen_frs_tree_, hddata_structure);

	// fill the structure
	// 2015.05.13	Filling structure should be carefully considered
	//		For ajdacent pixels, their structures should be filled in similar
	//		elements

	std::vector<FRSNode*> leaf_nodes;
	std::vector<int> leaf_node_codes;
	std::vector<std::vector<v2d>> leaf_node_boundary_points;


	std::vector<int> corre_indices;

	this->gen_frs_tree_->GetLeafNodesAndArrayIndices(leaf_nodes, leaf_node_codes);
	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		std::vector<v2d> pts;
		this->SamplePointsOfFRSNodeBoundary(this->gen_frs_tree_, leaf_nodes[i], pts, FRS_NODE_SAMPLE_NUM);
		leaf_node_boundary_points.push_back(pts);
	}

	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		int code = leaf_node_codes[i];
		// find all possible examples
		std::vector<int> possible_example_id;
		for (size_t j = 0; j < this->tree_frsnode_codes_.size(); j++)
		{
			if (code == this->tree_frsnode_codes_[j][1])
				possible_example_id.push_back(j);
		}


		// find the most similar example
		double min_dist = 1e20;
		int min_idx = -1;
		for (size_t j = 0; j < possible_example_id.size(); j++)
		{
			std::vector<v2d> dst_points;
			this->ResizeFRSNodeBoundaryPointsByAngle(this->gen_frs_tree_->fr_angle(),
				this->all_trees_[this->tree_frsnode_codes_[possible_example_id[j]][0]]->fr_angle(),
				_v2d_(260, 260),
				this->tree_frsnode_boundary_points_[possible_example_id[j]],
				dst_points);

			//this->ShowPointsInImage(leaf_node_boundary_points[i],dst_points);

			double dist = this->DistanceBetweenTwoPointSets(leaf_node_boundary_points[i], dst_points);

			if (dist < min_dist)
			{
				min_dist = dist;
				min_idx = possible_example_id[j];
			}
		}

		corre_indices.push_back(min_idx);
		
	}
	
	for (int i = 0; i < corre_indices.size(); i++)
		this->gen_frs_tree_->correc_indec.push_back(corre_indices.at(i));

	if (this->gen_frs_tree_->pattern_ != 0)
		delete this->gen_frs_tree_->pattern_;

	this->gen_frs_tree_->pattern_ = new SVGPattern();

	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		FRSTree * corre_tree = this->all_trees_[this->tree_frsnode_codes_[corre_indices[i]][0]];
		FRSNode * corre_node = this->tree_frsnodes_[corre_indices[i]];

		if (corre_node->element_indices_.empty()) continue;
		// to be added ...
		// if the leaf_node is very small, continue
		/*this->ShowPointsInImage(this->tree_frsnode_boundary_points_[corre_indices[i]],
		leaf_node_boundary_points[i]);*/

		this->DeformFRSNode(corre_tree, corre_node,
			this->gen_frs_tree_, leaf_nodes[i],
			this->gen_frs_tree_->pattern_->elements_);
	}
}

vector<int> FrameStructureBlend::BuildFRSTreeforseletindex()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));
	std::vector<double> hddata = mani->GetManifoldValue();

	/*for(size_t i = 0;i<hddata.size();i++)
	{
	cout<<hddata[i]<<" ";
	}
	cout<<endl;*/

	/*double hddata_[19] = {24.3385, 0.140191, 387.491, 280.975, 425.65, 275.01, 0.719131, 0.266481, 355.745, 276.456, 379.443, 272.124, 0.590594, 0.160899, 453.179, 305.8, 482.751, 291.348, 0.724278};
	std::vector<double> hddata;
	for(int ii = 0;ii<19;ii++)
	hddata.push_back(hddata_[ii]);*/

	double angle = FRSTree::BestAngle(hddata[0]);
	std::vector<double> hddata_structure;
	for (size_t i = 1; i < hddata.size(); i++)
		hddata_structure.push_back(hddata[i]);

	if (this->gen_frs_tree_ != 0)		delete this->gen_frs_tree_;

	// generate a new structure
	gen_frs_tree_ = this->BuildFRSTreeShell(angle, 280, _v2d_(260, 260));
	this->SubdivideFRSTreeShell(this->gen_frs_tree_, hddata_structure);

	// fill the structure
	// 2015.05.13	Filling structure should be carefully considered
	//		For ajdacent pixels, their structures should be filled in similar
	//		elements

	std::vector<FRSNode*> leaf_nodes;
	std::vector<int> leaf_node_codes;
	std::vector<std::vector<v2d>> leaf_node_boundary_points;


	std::vector<int> corre_indices;

	this->gen_frs_tree_->GetLeafNodesAndArrayIndices(leaf_nodes, leaf_node_codes);
	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		std::vector<v2d> pts;
		this->SamplePointsOfFRSNodeBoundary(this->gen_frs_tree_, leaf_nodes[i], pts, FRS_NODE_SAMPLE_NUM);
		leaf_node_boundary_points.push_back(pts);
	}

	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		int code = leaf_node_codes[i];
		// find all possible examples
		std::vector<int> possible_example_id;
		for (size_t j = 0; j < this->tree_frsnode_codes_.size(); j++)
		{
			if (code == this->tree_frsnode_codes_[j][1])
				possible_example_id.push_back(j);
		}

		// find the most similar example
		double min_dist = 1e20;
		int min_idx = -1;
		for (size_t j = 0; j < possible_example_id.size(); j++)
		{
			std::vector<v2d> dst_points;
			this->ResizeFRSNodeBoundaryPointsByAngle(this->gen_frs_tree_->fr_angle(),
				this->all_trees_[this->tree_frsnode_codes_[possible_example_id[j]][0]]->fr_angle(),
				_v2d_(260, 260),
				this->tree_frsnode_boundary_points_[possible_example_id[j]],
				dst_points);

			//this->ShowPointsInImage(leaf_node_boundary_points[i],dst_points);

			double dist = this->DistanceBetweenTwoPointSets(leaf_node_boundary_points[i], dst_points);

			if (dist < min_dist)
			{
				min_dist = dist;
				min_idx = possible_example_id[j];
			}
		}

		corre_indices.push_back(min_idx);
	}
	
	return corre_indices;
}

void FrameStructureBlend::BuildFRSTree()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));
	std::vector<double> hddata = mani->GetManifoldValue();

	/*for(size_t i = 0;i<hddata.size();i++)
	{
		cout<<hddata[i]<<" ";
	}
	cout<<endl;*/

	/*double hddata_[19] = {24.3385, 0.140191, 387.491, 280.975, 425.65, 275.01, 0.719131, 0.266481, 355.745, 276.456, 379.443, 272.124, 0.590594, 0.160899, 453.179, 305.8, 482.751, 291.348, 0.724278};
	std::vector<double> hddata;
	for(int ii = 0;ii<19;ii++)
		hddata.push_back(hddata_[ii]);*/

	double angle = FRSTree::BestAngle(hddata[0]);
	std::vector<double> hddata_structure;
	for (size_t i = 1; i < hddata.size(); i++)
		hddata_structure.push_back(hddata[i]);

	if (this->gen_frs_tree_ != 0)		delete this->gen_frs_tree_;

	// generate a new structure
	gen_frs_tree_ = this->BuildFRSTreeShell(angle, 280, _v2d_(260, 260));
	this->SubdivideFRSTreeShell(this->gen_frs_tree_, hddata_structure);

	// fill the structure
	// 2015.05.13	Filling structure should be carefully considered
	//		For ajdacent pixels, their structures should be filled in similar
	//		elements

	std::vector<FRSNode*> leaf_nodes;
	std::vector<int> leaf_node_codes;
	std::vector<std::vector<v2d>> leaf_node_boundary_points;


	std::vector<int> corre_indices;

	this->gen_frs_tree_->GetLeafNodesAndArrayIndices(leaf_nodes, leaf_node_codes);
	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		std::vector<v2d> pts;
		this->SamplePointsOfFRSNodeBoundary(this->gen_frs_tree_, leaf_nodes[i], pts, FRS_NODE_SAMPLE_NUM);
		leaf_node_boundary_points.push_back(pts);
	}

	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		int code = leaf_node_codes[i];
		// find all possible examples
		std::vector<int> possible_example_id;
		for (size_t j = 0; j < this->tree_frsnode_codes_.size(); j++)
		{
			if (code == this->tree_frsnode_codes_[j][1])
			{
				possible_example_id.push_back(j);
			}
		}

		// find the most similar example
		double min_dist = 1e20;
		int min_idx = -1;
		std::vector<int> temp_com_index;
		for (size_t j = 0; j < possible_example_id.size(); j++)
		{
			std::vector<v2d> dst_points;
			this->ResizeFRSNodeBoundaryPointsByAngle(this->gen_frs_tree_->fr_angle(),
				this->all_trees_[this->tree_frsnode_codes_[possible_example_id[j]][0]]->fr_angle(),
				_v2d_(260, 260),
				this->tree_frsnode_boundary_points_[possible_example_id[j]],
				dst_points);

			//this->ShowPointsInImage(leaf_node_boundary_points[i],dst_points);

			double dist = this->DistanceBetweenTwoPointSets(leaf_node_boundary_points[i], dst_points);

			temp_com_index.push_back(dist);

			if (dist < min_dist)
			{
				min_dist = dist;
				min_idx = possible_example_id[j];
			}
		}
		this->gen_frs_tree_->com_index_order.push_back(temp_com_index);

		corre_indices.push_back(min_idx);
	}

	for (int i = 0; i < corre_indices.size(); i++)
		this->gen_frs_tree_->correc_indec.push_back(corre_indices.at(i));

	if (this->gen_frs_tree_->pattern_ != 0)
		delete this->gen_frs_tree_->pattern_;

	this->gen_frs_tree_->pattern_ = new SVGPattern();

	for (size_t i = 0; i < leaf_nodes.size(); i++)
	{
		FRSTree * corre_tree = this->all_trees_[this->tree_frsnode_codes_[corre_indices[i]][0]];
		FRSNode * corre_node = this->tree_frsnodes_[corre_indices[i]];

		if (corre_node->element_indices_.empty()) continue;
		// to be added ...
		// if the leaf_node is very small, continue
		/*this->ShowPointsInImage(this->tree_frsnode_boundary_points_[corre_indices[i]],
			leaf_node_boundary_points[i]);*/

		this->DeformFRSNode(corre_tree, corre_node,
			this->gen_frs_tree_, leaf_nodes[i],
			this->gen_frs_tree_->pattern_->elements_);
	}
}

void FrameStructureBlend::SamplePointsOfFRSNodeBoundary(FRSTree * tree, FRSNode * node, std::vector<v2d> & points, int num)
{
	// the total length of the node boundary
	double total_length = 0;

	for (size_t i = 0; i < node->frnode_indices_.size(); i++)
	{
		FRNode * fr_node = tree->all_frnodes_[node->frnode_indices_[i]];
		total_length += fr_node->Length(1);
	}

	double gap_len = total_length / num;

	for (size_t i = 0; i < node->frnode_indices_.size(); i++)
	{
		FRNode * fr_node = tree->all_frnodes_[node->frnode_indices_[i]];
		std::vector<v2d> pts;
		fr_node->SamplePointsByGap(pts, gap_len, num * 10);
		for (size_t j = 0; j < pts.size(); j++)
			points.push_back(pts[j]);
	}
}

void FrameStructureBlend::SamplePointsOfFRSNodeBoundaryBySeg(FRSTree * tree, FRSNode * node, std::vector<v2d> & points)
{
	for (size_t i = 0; i < node->frnode_indices_.size(); i++)
	{
		FRNode * fr_node = tree->all_frnodes_[node->frnode_indices_[i]];
		std::vector<v2d> pts;
		fr_node->SamplePoints(pts, 0.2);
		for (size_t j = 0; j < pts.size(); j++)
			points.push_back(pts[j]);
	}
}

void FrameStructureBlend::ResizeFRSNodeBoundaryPointsByAngle(double new_angle, double old_angle, v2d centre, const std::vector<v2d> & src_points, std::vector<v2d> & dst_points)
{
	double por = new_angle / old_angle;

	for (size_t i = 0; i < src_points.size(); i++)
	{
		v2d pt = src_points[i];
		v2d ve = _v2d_(pt[0] - centre[0], pt[1] - centre[1]);

		double stheta = atan2(ve[1], ve[0]);
		double dtheta = stheta * por;
		double sradius = sqrt(ve[0] * ve[0] + ve[1] * ve[1]);

		v2d pt1;

		pt1[0] = centre[0] + sradius * cos(dtheta);
		pt1[1] = centre[1] + sradius * sin(dtheta);
		dst_points.push_back(pt1);
	}
}

double FrameStructureBlend::DistanceBetweenTwoPointSets(const std::vector<v2d> & pts1, const std::vector<v2d> & pts2)
{
	size_t real_num = pts1.size() < pts2.size() ? pts1.size() : pts2.size();

	double diff = 0;

	for (size_t i = 0; i < real_num; i++)
	{
		v2d p0 = pts1[i];
		v2d p1 = pts2[i];

		double diff_x = p0[0] - p1[0];
		double diff_y = p0[1] - p1[1];

		diff += sqrt(diff_x * diff_x + diff_y * diff_y);
	}

	return diff;
}

void FrameStructureBlend::ShowPointsInImage(const std::vector<v2d> & pts1, const std::vector<v2d> & pts2)
{
	double x0, x1, y0, y1, xx0, xx1, yy0, yy1;

	x0 = y0 = xx0 = yy0 = 1e20;
	x1 = y1 = xx1 = yy1 = -1e20;

	for (size_t i = 0; i < pts1.size(); i++)
	{
		v2d pt = pts1[i];
		if (pt[0] < x0) x0 = pt[0];
		if (pt[0] > x1) x1 = pt[0];
		if (pt[1] < y0) y0 = pt[1];
		if (pt[1] > y1) y1 = pt[1];
	}

	for (size_t i = 0; i < pts2.size(); i++)
	{
		v2d pt = pts2[i];
		if (pt[0] < xx0) xx0 = pt[0];
		if (pt[0] > xx1) xx1 = pt[0];
		if (pt[1] < yy0) yy0 = pt[1];
		if (pt[1] > yy1) yy1 = pt[1];
	}

	IplImage * img = cvCreateImage(cvSize(800, 800), 8, 3);
	cvZero(img);

	for (size_t i = 0; i < pts1.size(); i++)
	{
		LibIV::CppHelpers::Global::setImgData(255 - (int)(i * 2), 0, 0, img, (int)(pts1[i][0] - 0 + 10), (int)(800 - pts1[i][1] + 1));
	}

	for (size_t i = 0; i < pts2.size(); i++)
	{
		LibIV::CppHelpers::Global::setImgData(0, 0, 255 - (int)(i * 2), img, (int)(pts2[i][0] - 0 + 10), (int)(800 - pts2[i][1] + 1));
	}

	static int j = 0;
	char filename[1000];
	sprintf_s(filename, 1000, "F:/ttt%d.png", j);
	cvSaveImage(filename, img);
	cvReleaseImage(&img);
	j++;
}

void FrameStructureBlend::ExtractHighDimData()
{
	for (size_t i = 0; i < this->all_trees_.size(); i++)
	{
		this->all_trees_[i]->ExtractHighDimensionalDataBreadthFirst("D:/misc/HDData/data.txt");
		ofstream outfile("D:/misc/HDData/data.txt", std::ios::app);
		outfile << '\n';
		outfile.close();
	}
}

void FrameStructureBlend::CompleteFRSTree()
{
	for (size_t i = 0; i < this->all_trees_.size(); i++)
		this->all_trees_[i]->BinaryTreeTo1DArray(arr);

	for (size_t i = 0; i < this->all_trees_.size(); i++)
	{
		int l_arr[20] = { 0 };
		this->all_trees_[i]->BinaryTreeTo1DArray(l_arr);

		for (int j = 0; j < 20; j++)
		{
			if (arr[j] != l_arr[j])
			{
				if (j % 2 != 1)
				{
					cout << "error - FrameStructureBlend::CompleteFRSTree j % 2 != 1" << endl;
					continue;
				}

				FRSNode * frs_node_to_be_split = this->all_trees_[i]->FindParentFRSNodeByArrayIndex(j);

				if (frs_node_to_be_split->parrent_ == 0)
				{
					cout << "error - FrameStructureBlend::CompleteFRSTree " << j << endl;
					continue;
				}

				int	split_fr_node_idx = frs_node_to_be_split->parrent_->partition_frnode_index_;

				if (((j - 1) / 2) % 2 == 0)
					split_fr_node_idx++;

				FRNode * split_fr_node = this->all_trees_[i]->all_frnodes_[split_fr_node_idx];

				int kk = -1, kk1, kk2;
				for (int k = 0; k < (int)(frs_node_to_be_split->frnode_indices_.size()); k++)
				{
					if (frs_node_to_be_split->frnode_indices_[k] == split_fr_node_idx)
					{
						kk = k;
						break;
					}
				}
				if (kk == -1)
				{
					cout << "error - FrameStructureBlend::CompleteFRSTree kk == -1" << endl;
					continue;
				}
				kk1 = kk - 1;
				kk2 = kk + 1;
				if (kk1 < 0) kk1 = (int)(frs_node_to_be_split->frnode_indices_.size()) - 1;
				if (kk2 >= (int)(frs_node_to_be_split->frnode_indices_.size())) kk2 = 0;

				FRNode * split_fr_node_copy;

				if (((j - 1) / 2) % 2 == 1)
				{
					split_fr_node_copy = split_fr_node->CopySelf();
					split_fr_node_copy->set_left_node(this->all_trees_[i]->all_frnodes_[frs_node_to_be_split->frnode_indices_[kk1]]);
					split_fr_node_copy->set_right_node(this->all_trees_[i]->all_frnodes_[frs_node_to_be_split->frnode_indices_[kk2]]);
				}
				else
				{
					split_fr_node_copy = split_fr_node->CopyInverseSelf();
					split_fr_node_copy->set_left_node(this->all_trees_[i]->all_frnodes_[frs_node_to_be_split->frnode_indices_[kk2]]);
					split_fr_node_copy->set_right_node(this->all_trees_[i]->all_frnodes_[frs_node_to_be_split->frnode_indices_[kk1]]);
				}

				this->all_trees_[i]->all_frnodes_.push_back(split_fr_node_copy);
				this->all_trees_[i]->SplitFRSNode(frs_node_to_be_split, split_fr_node_copy);

				l_arr[j] = 1;
				l_arr[j + 1] = 1;
			}
		}
	}
}

void FrameStructureBlend::RefreshData()
{
	/*if(this->manifold_similar_codes_.empty())
		this->BuildFRSTreeTest();
	else
		this->BuildTreeByCode();*/

		//this->BuildFRSTreeTest();
		//this->BlendFRSTree();
	this->BuildFRSTree();

	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameSolidFlowerbed * fsfb = (FrameSolidFlowerbed*)(main_frame->getFrameByName("SolidFlowerbed"));
	//fsfb->RefreshPattern(this->gen_frs_tree_);
	//fsfb->RefreshPatternJustOneSector(this->gen_frs_tree_);
}

void FrameStructureBlend::LoadManifoldSimilarCode()
{
	union foo
	{
		char c[sizeof(int)];
		int d;
	}bar;

	ifstream infile("D:/test.txt", std::ios::binary);

	if (!infile) return;

	infile.seekg(0, ios::end);
	long long int ss = infile.tellg();
	infile.seekg(0, ios::beg);

	char * buf = new char[(unsigned int)ss];
	infile.read(buf, ss);

	char * p = buf;

	int mani_width, mani_height;
	memcpy(bar.c, p, sizeof(int));
	mani_width = bar.d;

	p += sizeof(int);
	memcpy(bar.c, p, sizeof(int));
	mani_height = bar.d;

	this->manifold_similar_codes_.set(mani_width, mani_height);

	for (int x = 0; x < mani_width; x++)
	{
		for (int y = 0; y < mani_height; y++)
		{
			int aa, bb, cc, dd;

			p += sizeof(int);
			memcpy(bar.c, p, sizeof(int));
			aa = bar.d;

			p += sizeof(int);
			memcpy(bar.c, p, sizeof(int));
			bb = bar.d;

			p += sizeof(int);
			memcpy(bar.c, p, sizeof(int));
			cc = bar.d;

			p += sizeof(int);
			memcpy(bar.c, p, sizeof(int));
			dd = bar.d;

			this->manifold_similar_codes_.at(y, x) = _v4i_(aa, bb, cc, dd);
		}
	}

	delete[] buf;
}

void ShowAlpha(char * filename, IplImage * img, const LibIV::Memory::Array::FastArray2D<double> & alpha)
{
	for (uint i = 0; i < alpha.rows(); i++)
	{
		for (uint j = 0; j < alpha.cols(); j++)
		{
			double a = alpha.at(i, j);
			int r = (int)(a * 255);
			LibIV::CppHelpers::Global::setImgData(r, 0, 0, img, j, i);
		}
	}

	cvSaveImage(filename, img);
}

void FrameStructureBlend::LoadAlpha()
{
	union foo
	{
		char c[sizeof(double)];
		double d;
	}bar;

	ifstream infile("D:/alpha.txt", ios::binary);

	if (!infile) return;

	infile.seekg(0, ios::end);
	long long int ss = infile.tellg();
	infile.seekg(0, ios::beg);

	char * buf = new char[(unsigned int)ss];
	infile.read(buf, ss);

	infile.close();

	char * p = buf;

	double rows, cols;
	memcpy(bar.c, p, sizeof(double));
	p += sizeof(double);
	rows = bar.d;
	memcpy(bar.c, p, sizeof(double));
	p += sizeof(double);
	cols = bar.d;

	alpha_.set((int)cols, (int)rows);

	for (int i = 0; i < (int)rows; i++)
	{
		for (int j = 0; j < (int)cols; j++)
		{
			memcpy(bar.c, p, sizeof(double));
			p += sizeof(double);
			alpha_.at(i, j) = bar.d;
		}
	}

	/*IplImage * img = cvCreateImage(cvSize((int)cols,(int)rows),8,3);
	cvZero(img);

	ShowAlpha("D:/test2.png",img,alpha_);

	cvReleaseImage(&img);*/
}

void FrameStructureBlend::ShowFRSNodeInImage(char * filename, FRSTree * tree, int frs_node_code, bool flag_show_ele)
{
	FRSNode * frs_node = tree->GetLeafNodeByIndex(frs_node_code);
	if (frs_node == 0) return;

	IplImage * img = cvCreateImage(cvSize(800, 800), 8, 3);
	cvZero(img);

	for (size_t i = 0; i < frs_node->frnode_indices_.size(); i++)
	{
		FRNode * fr_node = tree->all_frnodes_[frs_node->frnode_indices_[i]];

		std::vector<v2d> pts;
		fr_node->SamplePoints(pts, 0.1);

		for (size_t j = 0; j < pts.size() - 1; j++)
		{
			v2d pp, ss;
			pp = pts[j];
			ss = pts[j + 1];

			cvLine(img, cvPoint((int)(pp[0]), (int)(pp[1])), cvPoint((int)(ss[0]), (int)(ss[1])), cvScalar(255 - j * 20, 0, 0));
		}
	}

	if (flag_show_ele)
	{
		for (size_t i = 0; i < frs_node->element_indices_.size(); i++)
		{
			SVGElement * e = tree->pattern_->elements_[frs_node->element_indices_[i]];

			std::vector<v2d> pts;
			e->collectPoints(pts, 0.1);

			for (size_t j = 0; j < pts.size() - 1; j++)
			{
				v2d pp, ss;
				pp = pts[j];
				ss = pts[j + 1];

				cvLine(img, cvPoint((int)(pp[0]), (int)(pp[1])), cvPoint((int)(ss[0]), (int)(ss[1])), cvScalar(255 - j * 10, 0, 0));
			}

			for (size_t j = 0; j < e->segs_.size(); j++)
			{
				ElementSeg * seg = e->segs_[j];
				if (seg->SegType() == 1)
				{
					v2d pppp0 = ((ElementSegBezier*)(seg))->pts_[1];
					v2d pppp1 = ((ElementSegBezier*)(seg))->pts_[2];
					cvCircle(img, cvPoint((int)(pppp0[0]), (int)(pppp0[1])), 3, cvScalar(255, 255, 0));
					cvCircle(img, cvPoint((int)(pppp1[0]), (int)(pppp1[1])), 3, cvScalar(255, 255, 0));
				}
			}
		}
	}

	cvSaveImage(filename, img);
	cvReleaseImage(&img);
}

SVGElement* FrameStructureBlend::ChangeElementLineToBezier(const SVGElement * e)
{
	SVGElement * new_ele = new SVGElement(*e);

	for (size_t j = 0; j < new_ele->segs_.size(); j++)
	{
		ElementSeg * seg = new_ele->segs_[j];
		if (seg->SegType() == 0)
		{
			v2d p0 = ((ElementSegLine*)seg)->FirstVertex();
			v2d p3 = ((ElementSegLine*)seg)->LastVertex();

			v2d p1;
			v2d p2;

			Geo2D::InterpLinear(p1, 0.3333333333, p0, p3);
			Geo2D::InterpLinear(p2, 0.6666666666, p0, p3);
			ElementSegBezier * new_seg = new ElementSegBezier(p0, p1, p2, p3);

			delete new_ele->segs_[j];
			new_ele->segs_[j] = new_seg;
		}
	}

	return new_ele;
}

SVGElement * FrameStructureBlend::BlendTwoElements(const SVGElement * ee, const SVGElement * ff, double alpha)
{
	SVGElement * eee = this->ChangeElementLineToBezier(ee);
	SVGElement * fff = this->ChangeElementLineToBezier(ff);

	std::vector<v2i> seg_corre;

	double eee_i = (double)(eee->segs_.size());
	double fff_i = (double)(fff->segs_.size());

	for (size_t i = 0; i < eee->segs_.size(); i++)
	{
		v2i cc;
		cc[0] = i;
		cc[1] = (int)(i / eee_i * fff_i);

		seg_corre.push_back(cc);
	}

	double dist1 = 0, dist2 = 0;
	for (size_t i = 0; i < seg_corre.size(); i++)
	{
		v2i cc = seg_corre[i];
		v2i cc1 = seg_corre[i];
		ElementSegBezier * eseg = (ElementSegBezier*)(eee->segs_[cc[0]]);
		ElementSegBezier * fseg = (ElementSegBezier*)(fff->segs_[cc1[1]]);
		for (int j = 0; j < 4; j++)
		{
			v2d pp0, pp1;
			pp0 = eseg->pts_[j];
			pp1 = fseg->pts_[j];

			double xx = pp0[0] - pp1[0];
			double yy = pp0[1] - pp1[1];

			dist1 += (xx*xx + yy*yy);
		}
	}

	for (size_t i = 0; i < seg_corre.size(); i++)
	{
		v2i cc = seg_corre[i];
		v2i cc1 = seg_corre[seg_corre.size() - i - 1];

		ElementSegBezier * eseg = (ElementSegBezier*)(eee->segs_[cc[0]]);
		ElementSegBezier * fseg = (ElementSegBezier*)(fff->segs_[cc1[1]]);

		for (int j = 0; j < 4; j++)
		{
			v2d pp0, pp1;
			pp0 = eseg->pts_[j];
			pp1 = fseg->pts_[j];

			double xx = pp0[0] - pp1[0];
			double yy = pp0[1] - pp1[1];

			dist2 += (xx*xx + yy*yy);
		}
	}

	SVGElement * ans = new SVGElement();

	for (size_t i = 0; i < seg_corre.size(); i++)
	{
		v2i cc = seg_corre[i];
		v2i cc1;
		if (dist1 < dist2)
			cc1 = seg_corre[i];
		else
			cc1 = seg_corre[seg_corre.size() - i - 1];

		ElementSegBezier * eseg = (ElementSegBezier*)(eee->segs_[cc[0]]);
		ElementSegBezier * fseg = (ElementSegBezier*)(fff->segs_[cc1[1]]);

		ElementSegBezier * new_bezier = new ElementSegBezier();

		for (int j = 0; j < 4; j++)
		{
			new_bezier->pts_[j][0] = eseg->pts_[j][0] * alpha + (1 - alpha) * fseg->pts_[j][0];
			new_bezier->pts_[j][1] = eseg->pts_[j][1] * alpha + (1 - alpha) * fseg->pts_[j][1];
		}
		ans->segs_.push_back(new_bezier);
	}

	delete eee;
	delete fff;

	return ans;
}

void FrameStructureBlend::BlendFRSTree()
{
	Frame * main_frame = GLUTMain::Instance()->getMainFrame();
	FrameManifold * mani = (FrameManifold*)(main_frame->getFrameByName("Manifold"));

	v2i mouse, neighbor;

	mouse[0] = mani->manifold_x();
	mouse[1] = mani->manifold_y();
	neighbor = mani->nearest_neighbor();

	double alpha = this->alpha_.at(mouse[1], mouse[0]);

	if (this->manifold_similar_codes_.empty()) return;

	v4i mouse_similar_code = this->manifold_similar_codes_.at(mouse[1], mouse[0]);
	v4i neighbor_similar_code = this->manifold_similar_codes_.at(neighbor[1], neighbor[0]);
	std::vector<int> mouse_code, neighbor_code;

	for (int i = 0; i < 4; i++)
	{
		mouse_code.push_back(mouse_similar_code[i]);
		neighbor_code.push_back(neighbor_similar_code[i]);
	}

	FRSTree * new_tree_mouse = this->BuildTreeStructureForManifoldPosition(mouse[0], mouse[1]);
	FRSTree * new_tree_neighbor = this->BuildTreeStructureForManifoldPosition(mouse[0], mouse[1]);
	if (this->gen_frs_tree_ != 0) delete this->gen_frs_tree_;
	this->gen_frs_tree_ = this->BuildTreeStructureForManifoldPosition(mouse[0], mouse[1]);
	this->gen_frs_tree_->pattern_ = new SVGPattern();

	this->DeformCandidates(new_tree_mouse, mouse_code);
	this->DeformCandidates(new_tree_neighbor, neighbor_code);

	for (int i = 3; i < 7; i++)
	{
		FRSNode * aa = new_tree_mouse->GetLeafNodeByIndex(i);
		FRSNode * bb = new_tree_neighbor->GetLeafNodeByIndex(i);
		FRSNode * cc = this->gen_frs_tree_->GetLeafNodeByIndex(i);

		if (aa->element_indices_.size() == bb->element_indices_.size())
		{
			for (size_t j = 0; j < aa->element_indices_.size(); j++)
			{
				SVGElement * new_ele = this->BlendTwoElements(new_tree_mouse->pattern_->elements_[aa->element_indices_[j]],
					new_tree_neighbor->pattern_->elements_[bb->element_indices_[j]], alpha);
				cc->element_indices_.push_back(this->gen_frs_tree_->pattern_->elements_.size());
				this->gen_frs_tree_->pattern_->elements_.push_back(new_ele);
			}
		}
	}

	delete new_tree_mouse;
	delete new_tree_neighbor;
}

