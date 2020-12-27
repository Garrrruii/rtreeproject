#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <queue>
using namespace std;

int MAXINF = 123456789;
int MININF = -123456789;

int MAXNN = 41, MINNN =21; //169,85  84,42  41,21
int LV = 2;
//        R					 //128MB 16MB 2MB
//       . .		(R.lv-N.lv)=1  
//     . . . .		(R.lv-N.lv)=2
// . . . . . . . 	(R.lv-N.lv)=3

typedef long long ll;

struct Point {
	int x, y;
};
struct Rec {
	int x1, y1, x2, y2;
};
Rec InitRec = { MAXINF,MAXINF,MININF,MININF };
struct Node {
	Rec mbr;				//int*4			=16
	int level;				//int*1			=4
	bool isLeaf, isRoot;	//bool*2		=2
	vector<Node*> child;	//(ptr+int*4)*M =24M
	Node* parent;			//ptr*1			=8
							//30+24M < node => M < ?
};
vector<Node*> nullvec(0);
Node InitNode = { InitRec,-1,false,false,nullvec,nullptr };

class RTree {
protected:
	Node* Root;
	ll CNT = 0;
public:
	RTree() {
		Root = new Node;
		Root->mbr = InitRec;
		Root->level = 0;
		Root->isLeaf = true;
		Root->isRoot = true;
		Root->child = nullvec;
		Root->parent = nullptr;
		CNT = 0;
	}
	~RTree() {}

	void InsertObjTree(int, int);
	void InsertObjNode(Node*, Node*);
	void InsertObjLeaf(Node*, Node*);
	void InsertNode(Node*, Node*);
	Node* ChooseNode(Node*, Node*);
	void SetNewRoot(Node*, Node*);

	void SplitAdjust(Node*);
	void ResizeMbr(Node*);
	void ResizeMbr2(Node*, Rec);

	int GetArea(Node*);
	int AreaBtw(Node*, Node*);
	int DistBtw(Node*, Node*);
	int Dist(Node*, Point*);
	int MinDist(Node*, Point*);
	int MaxDist(Node*, Point*);
	int MinMaxDist(Node*, Point*);

	void SplitInto(Node*, Node*);
	void Split(Node*, Node*, vector<Node*>);

	void PrintTree();
	void PrintNode(Node*);
	ll GetCNT();

	//vector<Point> range(int, Point, ll*);
	void range(int, Point, ll*);
	//vector<Point> KNN(int, Point, ll*);
	void KNN(int, Point, ll*);
	ll NodeInDisk();

	//sort
	struct sortpoints {
		bool operator()(Node* A, Node* B) {
			if (A->mbr.y1 == B->mbr.y1) return A->mbr.x1 < B->mbr.x1;
			return A->mbr.y1 < B->mbr.y1;
		}
	};
	struct sortmbrs {
		bool operator()(Node* A, Node* B) {
			if (A->mbr.y2 == B->mbr.y2) {
				if (A->mbr.y1 == B->mbr.y1) {
					if (A->mbr.x2 == B->mbr.x2) return A->mbr.x1 > B->mbr.x1;
					return A->mbr.x2 < B->mbr.x2;
				}
				return A->mbr.y1 > B->mbr.y1;
			}
			return A->mbr.y2 < B->mbr.y2;
		}
	};
	struct smallfirst {
		bool operator()(pair<int, Node*>A, pair<int, Node*>B) {
			return A.first > B.first;
		}
	};
	struct nearfirst {
		bool operator()(pair<int, Node> A, pair<int, Node> B) {
			return A.first > B.first;
		}
	};
	struct farfirst {
		bool operator()(pair<int, Node> A, pair<int, Node> B) {
			return A.first < B.first;
		}
	};
};

/// RTree functions
///
void RTree::InsertObjTree(int x, int y) {

	Node* PointNode = new Node();
	PointNode->mbr = { x,y,x,y };
	PointNode->level = -1;
	PointNode->isLeaf = false;
	PointNode->isRoot = false;
	PointNode->child = nullvec;
	PointNode->parent = nullptr;
	InsertObjNode(Root, PointNode);
	//IOCOUNT 데이터 한 개 블록체인에 넣을 때
	//CNT++;
}
void RTree::InsertObjNode(Node* N, Node* InputObj) {
	ResizeMbr2(N, InputObj->mbr);

	if (N->isLeaf) InsertObjLeaf(N, InputObj);
	else {
		Node* NewN = ChooseNode(N, InputObj);
		InsertObjNode(NewN, InputObj);
	}
}
void RTree::InsertObjLeaf(Node* N, Node* InputObj) {
	//IOCOUNT 내 정보가 바뀔 때
	if (Root->level - N->level > LV) CNT++; 

	InputObj->level = -1;
	InputObj->isLeaf = false;
	InputObj->isRoot = false;
	InputObj->parent = N;
	N->child.push_back(InputObj);

	if (N->child.size() > MAXNN) SplitAdjust(N);
	else ResizeMbr(N);
}
void RTree::InsertNode(Node* Par, Node* NewNode) {
	NewNode->level = Par->level - 1;
	NewNode->isLeaf = (NewNode->level == 0);
	NewNode->isRoot = false;
	NewNode->parent = Par;
	Par->child.push_back(NewNode);

	if (Par->child.size() > MAXNN) SplitAdjust(Par);
}
Node* RTree::ChooseNode(Node* N, Node* InputObj) {
	
	vector<Node*> NodeList;
	int MinArea = AreaBtw(N->child[0], InputObj);
	NodeList.push_back(N->child[0]);

	for (int i = 1; i < N->child.size(); i++) {
		int NewArea = AreaBtw(N->child[i], InputObj);
		if (MinArea < NewArea) continue;
		if (MinArea > NewArea) MinArea = NewArea, NodeList.clear();

		NodeList.push_back(N->child[i]);
	}

	Node* NewN = NodeList[0];
	MinArea = GetArea(NewN);
	for (int i = 0; i < NodeList.size(); i++) {
		int NewArea = GetArea(NodeList[i]);
		if (MinArea > NewArea) MinArea = NewArea, NewN = NodeList[i];
	}
	return NodeList[0];
}
void RTree::SplitAdjust(Node* N) {
	Node* NewNode = new Node;
	SplitInto(N, NewNode);

	//IOCOUNT 새 노드 씀
	if (Root->level - N->level > LV) CNT++;

	if (N->isRoot) SetNewRoot(N, NewNode);
	else InsertNode(N->parent, NewNode);
}
void RTree::SplitInto(Node* N, Node* NewNode) {

	vector<Node*> Allchild = N->child;
	N->child.clear();

	if (N->isLeaf) sort(Allchild.begin(), Allchild.end(), sortpoints());
	else sort(Allchild.begin(), Allchild.end(), sortmbrs());

	// STEP 1. Pick Seeds
	int MaxArea = AreaBtw(Allchild[0], Allchild.back());
	int ai = 0, bi = (int)Allchild.size() - 1;
	N->child.push_back(Allchild[ai]);
	NewNode->child.push_back(Allchild[bi]);
	for (int i = 0; i < Allchild.size() - 1; i++) {
		for (int j = (int)Allchild.size() - 1; j > i; j--) {
			if (i == 0 && j == Allchild.size() - 1) j--;

			int NewArea = AreaBtw(Allchild[i], Allchild[j]);
			if (NewArea > MaxArea) {
				MaxArea = NewArea;
				ai = i; bi = j;
				N->child[0] = Allchild[i];
				NewNode->child[0] = Allchild[j];
			}
		}
	}
	Allchild.erase(Allchild.begin() + bi);
	Allchild.erase(Allchild.begin() + ai);
	ResizeMbr(N); ResizeMbr(NewNode);

	// STEP 2. Split
	Split(N, NewNode, Allchild);
}
void RTree::Split(Node* N, Node* NewNode, vector<Node*> Allchild) {
	bool addtoN;

	while (!Allchild.empty()) {
		if (N->child.size() > MINNN) { addtoN = false; break; }
		if (NewNode->child.size() > MINNN) { addtoN = true; break; }

		int ti = 0;
		int Area1 = AreaBtw(N, Allchild[0]);
		int Area2 = AreaBtw(NewNode, Allchild[0]);
		int MinArea = min(Area1, Area2);
		addtoN = (Area1 <= Area2);

		for (int i = 1; i < Allchild.size(); i++) {
			Node* Target = Allchild[i];
			Area1 = AreaBtw(N, Target);
			Area2 = AreaBtw(NewNode, Target);
			int NewArea = min(Area1, Area2);
			if (NewArea < MinArea) {
				addtoN = (Area1 <= Area2); ti = i;
			}
		}
		if (addtoN) {
			Allchild[ti]->parent = N; N->child.push_back(Allchild[ti]); ResizeMbr(N);
		}
		else {
			Allchild[ti]->parent = NewNode; NewNode->child.push_back(Allchild[ti]); ResizeMbr(NewNode);
		}
		Allchild.erase(Allchild.begin() + ti);
	}

	if (!Allchild.empty()) {
		if (addtoN) {
			for (int i = 0; i < Allchild.size(); i++) {
				Allchild[i]->parent = N; N->child.push_back(Allchild[i]); ResizeMbr(N);
			}
		}
		else {
			for (int i = 0; i < Allchild.size(); i++) {
				Allchild[i]->parent = NewNode; NewNode->child.push_back(Allchild[i]); ResizeMbr(NewNode);
			}
		}
	}
}
void RTree::SetNewRoot(Node* A, Node* B) { // Change Root A to NewRoot whose children are A and B
	Node* NewRoot = new Node;

	B->level = A->level;
	B->isLeaf = (B->level == 0);
	A->isRoot = false; B->isRoot = false;
	A->parent = NewRoot; B->parent = NewRoot;

	NewRoot->child.push_back(A); ResizeMbr(NewRoot);
	NewRoot->child.push_back(B); ResizeMbr(NewRoot);
	NewRoot->level = A->level + 1;
	NewRoot->isLeaf = false;
	NewRoot->isRoot = true;
	Root = NewRoot;

	//IOCOUNT
	queue<Node*> Q;
	Q.push(A); Q.push(B);
	while (!Q.empty()) {
		Node* Cur = Q.front(); Q.pop();
		if (Cur->isLeaf)  break;
		if (Root->level - Cur->level < LV) {
			for (int i = 0; i < Cur->child.size(); i++) Q.push(Cur->child[i]);
		}
		else {
			CNT += (ll)Cur->child.size();
		}
	}
	//printf("level down,\tCNT=%d\n", CNT); //IOCOUNT
}
void RTree::ResizeMbr(Node* N) {	//Since new child inserted, resize mbr
	Rec Newmbr = N->child.back()->mbr;
	if (N->child.size() == 1) {
		N->mbr.x1 = Newmbr.x1;
		N->mbr.y1 = Newmbr.y1;
		N->mbr.x2 = Newmbr.x2;
		N->mbr.y2 = Newmbr.y2;
	}
	else {
		N->mbr.x1 = min(N->mbr.x1, Newmbr.x1);
		N->mbr.y1 = min(N->mbr.y1, Newmbr.y1);
		N->mbr.x2 = max(N->mbr.x2, Newmbr.x2);
		N->mbr.y2 = max(N->mbr.y2, Newmbr.y2);
	}
}
void RTree::ResizeMbr2(Node* N, Rec R) {
	N->mbr.x1 = min(N->mbr.x1, R.x1);
	N->mbr.y1 = min(N->mbr.y1, R.y1);
	N->mbr.x2 = max(N->mbr.x2, R.x2);
	N->mbr.y2 = max(N->mbr.y2, R.y2);
}
int RTree::GetArea(Node* N) {
	int dx = N->mbr.x2 - N->mbr.x1;
	int dy = N->mbr.y2 - N->mbr.y1;

	return dy * dy;
}
int RTree::AreaBtw(Node* A, Node* B) {
	int dx = max(A->mbr.x2, B->mbr.x2) - min(A->mbr.x1, B->mbr.x1);
	int dy = max(A->mbr.y2, B->mbr.y2) - min(A->mbr.y1, B->mbr.y1);

	return dx * dy - GetArea(A) - GetArea(B);
}
int RTree::DistBtw(Node* A, Node* B) {
	int dx = A->mbr.x1 - B->mbr.x1;
	int dy = A->mbr.y1 - B->mbr.y1;
	return dx * dx + dy * dy;
}
int RTree::Dist(Node* A, Point* P) {
	int dx1 = P->x - A->mbr.x1; int dy1 = P->y - A->mbr.y1;
	return dx1 * dx1 + dy1 * dy1;
}
int RTree::MinDist(Node* A, Point* P) {
	int dx1 = P->x - A->mbr.x1; int dy1 = P->y - A->mbr.y1;
	int dx2 = P->x - A->mbr.x2; int dy2 = P->y - A->mbr.y2;

	int result = 0;
	if (dx1 < 0) result += (dx1 * dx1);
	else if (dx2 > 0) result += (dx2 * dx2);
	if (dy1 < 0) result += (dy1 * dy1);
	else if (dy2 > 0) result += (dy2 * dy2);
	return result;
}
int RTree::MaxDist(Node* A, Point* P) {
	int dx1 = P->x - A->mbr.x1; int dy1 = P->y - A->mbr.y1;
	int dx2 = P->x - A->mbr.x2; int dy2 = P->y - A->mbr.y2;

	return max(dx1 * dx1, dx2 * dx2) + max(dy1 * dy1, dy2 * dy2);
}
int RTree::MinMaxDist(Node* A, Point* P) {
	int dx1 = P->x - A->mbr.x1; int dy1 = P->y - A->mbr.y1;
	int dx2 = P->x - A->mbr.x2; int dy2 = P->y - A->mbr.y2;

	int dist1 = min(dx1 * dx1, dx2 * dx2) + max(dy1 * dy1, dy2 * dy2);
	int dist2 = max(dx1 * dx1, dx2 * dx2) + min(dy1 * dy1, dy2 * dy2);
	return min(dist1, dist2);
}
void RTree::PrintTree() {
	PrintNode(Root);
}
void RTree::PrintNode(Node* N) {
	/*
	for (int i = 0; i < 4 - (N->level); i++) printf("  ");
	printf("%2d...", (Root->level) - (N->level));
	printf("(%5d,%5d | %5d,%5d) ", N->mbr.x1, N->mbr.x2, N->mbr.y1, N->mbr.y2);

	if (N->level > 0) {
		for (int i = 0; i < N->child.size(); i++) printf("\n"), PrintNode(N->child[i]);
	}
	else {
		printf(": ");
		for (int i = 0; i < N->child.size(); i++) printf("(%5d,%5d) ", N->child[i]->mbr.x1, N->child[i]->mbr.y1);
	}*/
	
	for (int i = 0; i < 4 - (N->level); i++) printf("  ");
	printf("%2d...", (Root->level) - (N->level));
	printf("(%4d,%4d | %4d,%4d) ", N->mbr.x1, N->mbr.x2, N->mbr.y1, N->mbr.y2);

	if (N->level > 0) {
		printf("\n"), PrintNode(N->child[0]);
	}
	else {
		printf(": ");
		printf("(%4d,%4d) ", N->child[0]->mbr.x1, N->child[0]->mbr.y1);
	}
}
ll RTree::GetCNT() {
	return CNT;
}
//vector<Point> RTree::range(int R, Point P, ll* CNT) {
void RTree::range(int R, Point P, ll * CNT) {
	vector<Point> V;
	queue<Node> Q;

	Q.push(*Root);
	while (!Q.empty()) {
		Node Cur = Q.front(); Q.pop();

		//IO COUNT
		if (Root->level - Cur.level > LV) (*CNT)++;
		if (Cur.isLeaf) {
			for (int i = 0; i < (int)Cur.child.size(); i++) {
				if (Dist(Cur.child[i], &P) <= R * R) V.push_back({ Cur.child[i]->mbr.x1,Cur.child[i]->mbr.y1 });
			}
		}
		else {
			for (int i = 0; i < (int)Cur.child.size(); i++) {
				if (MinDist(Cur.child[i], &P) <= R * R) Q.push(*Cur.child[i]);
			}
		}
	}
	//return V;
}
//vector<Point> RTree::KNN(int K, Point P, ll* CNT) {
void RTree::KNN(int K, Point P, ll* CNT) {
	priority_queue<pair<int, Node>, vector<pair<int, Node>>, nearfirst> QN;
	priority_queue<pair<int, Node>, vector<pair<int, Node>>, farfirst> QF;
	vector<Point> V;

	int R = MaxDist(Root, &P);
	QN.push({ MinDist(Root,&P),*Root });

	while (!QN.empty()) {
		if (QN.top().first > R) break;

		Node Cur = QN.top().second; QN.pop();
		
		//IO COUNT
		if (Root->level - Cur.level > LV) (*CNT)++;
		
		if (Cur.isLeaf) {
			for (int i = 0; i < (int)Cur.child.size(); i++) QF.push({ Dist(Cur.child[i], &P),*Cur.child[i] });

			while (QF.size() > K) QF.pop();
			if (QF.size() == K) R = QF.top().first;
		}
		else {
			for (int i = 0; i < (int)Cur.child.size(); i++) QN.push({ MinDist(Cur.child[i], &P),*Cur.child[i] });
		}
	}
	while (!QF.empty()) {
		V.push_back({ QF.top().second.mbr.x1,QF.top().second.mbr.y1 }); QF.pop();
	}
	/* use MINMAXDIST => 의미X
	priority_queue<int> QR;
	int R = MaxDist(Root, &P);
	QN.push({ MinDist(Root,&P),*Root });
	QR.push(MinMaxDist(Root, &P));
	while (!QN.empty()) {
		if (QN.top().first > R) break;

		Node Cur = QN.top().second; QN.pop();

		//IO COUNT
		if (Root->level - Cur.level > LV) (*CNT)++;

		if (Cur.isLeaf) {
			for (int i = 0; i < (int)Cur.child.size(); i++) QF.push({ Dist(Cur.child[i], &P),*Cur.child[i] });

			while (QF.size() > K) QF.pop();
			if (QF.size() == K) R = min(R,QF.top().first);
		}
		else {
			for (int i = 0; i < (int)Cur.child.size(); i++) QN.push({ MinDist(Cur.child[i], &P),*Cur.child[i] }), QR.push(MinMaxDist(Cur.child[i],&P));
			while (QR.size() > K) QR.pop();
			if (QR.size() == K) R = QR.top();
		}
	}
	while (!QF.empty()) {
		V.push_back({ QF.top().second.mbr.x1,QF.top().second.mbr.y1 }); QF.pop();
	}*/
	//return V;
}
ll RTree::NodeInDisk() {
	ll CNT = 0;
	queue<Node> Q;
	Q.push(*Root);
	while (!Q.empty()) {
		Node Cur = Q.front(); Q.pop();
		
		int curchildsize = Cur.child.size();
		if (Root->level - Cur.level >= LV) CNT += (ll)curchildsize;
		if (!Cur.child[0]->isLeaf) {
			for (int i = 0; i < curchildsize; i++) Q.push(*(Cur.child[i]));
		}
	}

	return CNT;
}