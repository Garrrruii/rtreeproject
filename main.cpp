#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <random>
#include <time.h>
using namespace std;
#include "RTreeHeader.h"

//void _range(RTree, int, Point);
//void _KNN(int, RTree, int, Point);
int main() {
	int X, Y;
	int K[4] = { 5, 10, 15, 20};
	int ksize = 4;
	clock_t start, end;
	double TIME = 0;
	ll INDISK = 0;
	
	// 실행하기 전에 확인할 것
	int N = 10000000;
	FILE* fp_u = fopen("data_uniformed1.txt", "r");
	FILE* fp_g = fopen("data_gaussian1.txt", "r");
	FILE* fp_c = fopen("data_clustered1.txt", "r");
	Point P = { 7000,7000 };
	FILE* fp_res = fopen("result.txt", "w");
	bool U = true;
	bool G = false;
	bool C = false;

	printf("N=%d\tMAX=%d\tLV=%d\tPoint={%d,%d}\n", N, MAXNN,LV, P.x, P.y);
	fprintf(fp_res, "N=%d\tMAX=%d\tLV=%d\tPoint={%d,%d}\n", N, MAXNN,LV, P.x, P.y);

	// 1) uniformed
	if (U) {
		printf("\n1) Uniformed\n");
		fprintf(fp_res, "\n1) Uniformed\n");
		
		start = clock();
		RTree* RTREE_U = new RTree();
		for (int i = 0; i < N; i++) {
			fscanf(fp_u, "%d %d", &X, &Y);
			RTREE_U->InsertObjTree(X, Y);

//			RTREE_U->PrintTree(); printf("\nCNT=%d-----------------------------------------------------------\n",RTREE_U->GetCNT());

		}
		RTREE_U->PrintTree(); printf("\n");

		end = clock();
		TIME = (double)(end - start) / 1000.0;
		INDISK = RTREE_U->NodeInDisk();
		printf("BUILD TREE\t%16lld IO COUNT, %8.2f SECS %16lld NODES IN DISK\n\n", RTREE_U->GetCNT(), TIME, INDISK);
		fprintf(fp_res, "BUILD TREE\t%16lld IO COUNT, %8.2f SECS %16lld NODES IN DISK\n\n", RTREE_U->GetCNT(), TIME, INDISK);

		for (int i = 0; i < ksize; i++) {
			ll CNT_KNN = 0;
			start = clock();
			vector<Point> RES = RTREE_U->KNN(K[i], P, &CNT_KNN);
			end = clock();
			TIME = (double)(end - start) / 1000.0;
			printf("KNN, K=%d \t%16lld IO COUNT, %8.2f SECS\n", K[i], CNT_KNN, TIME);
			fprintf(fp_res, "KNN, K=%d \t%16lld IO COUNT, %8.2f SECS\n", K[i], CNT_KNN, TIME);
		}
		fclose(fp_u); delete RTREE_U;
	}
	// 2) gaussian
	if (G) {
		printf("\n2) Gaussian\n");
		fprintf(fp_res, "\n2) Gaussian\n");
		
		start = clock();
		RTree* RTREE_G = new RTree();
		for (int i = 0; i < N; i++) {
			fscanf(fp_g, "%d %d", &X, &Y);
			RTREE_G->InsertObjTree(X, Y);
		}
//		RTREE_G->PrintTree(); printf("\n");

		end = clock();
		TIME = (double)(end - start) / 1000.0;
		INDISK = RTREE_G->NodeInDisk();
		printf("BUILD TREE\t%16lld IO COUNT, %8.2f SECS %16lld NODES IN DISK\n\n", RTREE_G->GetCNT(), TIME, INDISK);
		fprintf(fp_res, "BUILD TREE\t%16lld IO COUNT, %8.2f SECS %16lld NODES IN DISK\n\n", RTREE_G->GetCNT(), TIME, INDISK);

		for (int i = 0; i < ksize; i++) {
			ll CNT_KNN = 0;
			start = clock();
			vector<Point> RES = RTREE_G->KNN(K[i], P, &CNT_KNN);
			end = clock();
			TIME = (double)(end - start) / 1000.0;
			printf("KNN, K=%d \t%16lld IO COUNT, %8.2f SECS\n", K[i], CNT_KNN, TIME);
			fprintf(fp_res, "KNN, K=%d \t%16lld IO COUNT, %8.2f SECS\n", K[i], CNT_KNN, TIME);
		}
		fclose(fp_g); delete RTREE_G;
	}
	// 3 clustered
	if (C) {
		printf("\n3) Clustered\n");
		fprintf(fp_res, "\n3) Clustered\n");
		
		start = clock();
		RTree* RTREE_C = new RTree();
		for (int i = 0; i < N; i++) {
			fscanf(fp_c, "%d %d", &X, &Y);
			RTREE_C->InsertObjTree(X, Y);
		}
//		RTREE_C->PrintTree(); printf("\n");

		end = clock();
		TIME = (double)(end - start) / 1000.0;
		INDISK = RTREE_C->NodeInDisk();
		printf("BUILD TREE\t%16lld IO COUNT, %8.2f SECS %16lld NODES IN DISK\n\n", RTREE_C->GetCNT(), TIME, INDISK);
		fprintf(fp_res, "BUILD TREE\t%16lld IO COUNT, %8.2f SECS %16lld NODES IN DISK\n\n", RTREE_C->GetCNT(), TIME, INDISK);

		for (int i = 0; i < ksize; i++) {
			ll CNT_KNN = 0;
			start = clock();
			vector<Point> RES = RTREE_C->KNN(K[i], P, &CNT_KNN);
			end = clock();
			TIME = (double)(end - start) / 1000.0;
			printf("KNN, K=%d \t%16lld IO COUNT, %8.2f SECS\n", K[i], CNT_KNN, TIME);
			fprintf(fp_res, "KNN, K=%d \t%16lld IO COUNT, %8.2f SECS\n", K[i], CNT_KNN, TIME);
		}
		fclose(fp_c); delete RTREE_C;
	}
	
	fclose(fp_res);

	return 0;
}
/*
void _range(RTree T, int R, Point P) {

	int CNT = 0;
	clock_t start = clock();
	vector<Point> RES = T.range(R, P, &CNT);
	clock_t end = clock();

	printf("\nRange R=%d P(%d,%d)------------ ", R, P.x, P.y);
	printf("%10d IO COUNT, %8.2f SECS\n", CNT, (double)(end - start)/1000.0);

	for (int i = 0; i < RES.size(); i++) {
		printf(" (%2d,%2d)", RES[i].x, RES[i].y);
		if (i % 15 == 14) printf("\n");
	}
	printf("\n");
}
void _KNN(int ver, RTree T, int K, Point P) {
	int CNT = 0;
	clock_t start = clock();
	vector<Point> RES;
	if (ver == 0) RES = T.KNN_BF(K, P, &CNT);
	else if (ver == 1) RES = T.KNN(K, P, &CNT);
	clock_t end = clock();

	if(ver==0) printf("\nBF,  K=%d P(%d,%d)------------ ", K, P.x, P.y);
	else printf("\nKNN, K=%d P(%d,%d)------------ ", K, P.x, P.y);
	printf("%10d IO COUNT, %8.2f SECS\n", CNT, (double)(end - start)/1000.0);

	for (int i = 0; i < RES.size(); i++) {
		printf(" (%2d,%2d)", RES[i].x, RES[i].y);
		if (i % 15 == 14) printf("\n");
	}
	printf("\n");
}*/