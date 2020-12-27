#define _CRT_SECURE_NO_WARNIGS
#include <iostream> 
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <time.h>
using namespace std;
#include "RTreeHeader.h"

int main() {
	int X, Y;
	int R[4] = { 10, 50, 100, 500 };
	int K[4] = { 5, 10, 15, 20};
	int ksize = 4, rsize = 4;
	Point P[10] = { {6140,12330},{10434,11934},{5459,4875},{12067,4014},{4185,8556},
					{8564,9902},{13602,14075},{3185,3054},{9987,7221},{2007,9510} };
	int psize = 10;
	clock_t start, end;
	double TIME = 0;
	ll INDISK = 0;
	
	// 실행하기 전에 확인할 것
	int N = 50000000;
	FILE* fp_u = fopen("data_uniformed.txt", "r");
	FILE* fp_c = fopen("data_clustered.txt", "r");

	FILE* fp_res = fopen("output_2mb_5천만.txt", "w");
	int U = 1;
	int C = 1;

	printf("N=%d\tMAX=%d\tLV=%d\n", N, MAXNN,LV);
	fprintf(fp_res, "N=%d\tMAX=%d\tLV=%d\n", N, MAXNN,LV);

	// 1) uniformed
	if (U) {
		printf("\n1) Uniformed\n");
		fprintf(fp_res, "\n1) Uniformed\n");

		start = clock();
		RTree* RTREE_U = new RTree();
		for (int i = 1; i <= N; i++) {
			fscanf(fp_u, "%d %d", &X, &Y);
			RTREE_U->InsertObjTree(X, Y);
			if (i % 10000000 == 0) {
				printf("%9d-points inserted, %16lld IO COUNT\n", i, RTREE_U->GetCNT());
				fprintf(fp_res, "%9d-points inserted, %16lld IO COUNT\n", i, RTREE_U->GetCNT());
			}
		}

		end = clock();
		TIME = (double)(end - start) / 1000.0;
		INDISK = RTREE_U->NodeInDisk();
		printf("BUILD TREE,\t%8.2f SECS %10lld NODES IN DISK\n\n", TIME, INDISK);
		fprintf(fp_res, "BUILD TREE,\t%8.2f SECS %10lld NODES IN DISK\n\n", TIME, INDISK);

		for (int i = 0; i < rsize; i++) {
			ll CNT = 0;
			printf("R=%d\n",R[i]);
			fprintf(fp_res,"R=%d\n", R[i]);
			for (int p = 0; p < psize; p++) {
				ll CNTtmp = 0;
				RTREE_U->range(R[i], P[p], &CNTtmp);
				printf("%9lld", CNTtmp), fprintf(fp_res, "%9lld+", CNTtmp);
				if (p == 4) printf("\n"), fprintf(fp_res, "\n");
				CNT += CNTtmp;
			}
			printf("\tTOTAL %10lld IO COUNT\n\n", CNT);
			fprintf(fp_res, "\tTOTAL %10lld IO COUNT\n\n", CNT);
		}
		for (int i = 0; i < ksize; i++) {
			ll CNT = 0;
			printf("K=%d\n", K[i]);
			fprintf(fp_res, "K=%d\n", K[i]);
			for (int p = 0; p < psize; p++) {
				ll CNTtmp = 0;
				RTREE_U->KNN(K[i], P[p], &CNTtmp);
				printf("%9lld", CNTtmp), fprintf(fp_res, "%9lld+", CNTtmp);
				if (p == 4) printf("\n"), fprintf(fp_res, "\n");
				CNT += CNTtmp;
			}
			printf("\tTOTAL %10lld IO COUNT\n\n", CNT);
			fprintf(fp_res, "\tTOTAL %10lld IO COUNT\n\n", CNT);
		}

		fclose(fp_u); delete RTREE_U;
	}
	// 2) clustered
	if (C) {
		printf("\n2) Clustered\n");
		fprintf(fp_res, "\n2) Clustered\n");
		
		start = clock();
		RTree* RTREE_C = new RTree();
		for (int i = 1; i <= N; i++) {
			fscanf(fp_c, "%d %d", &X, &Y);
			RTREE_C->InsertObjTree(X, Y);
			if (i % 10000000 == 0) {
				printf("%9d-points inserted, %16lld IO COUNT\n", i, RTREE_C->GetCNT());
				fprintf(fp_res, "%9d-points inserted, %16lld IO COUNT\n", i, RTREE_C->GetCNT());
			}
		}

		end = clock();
		TIME = (double)(end - start) / 1000.0;
		INDISK = RTREE_C->NodeInDisk();
		printf("BUILD TREE,\t%8.2f SECS %10lld NODES IN DISK\n\n", TIME, INDISK);
		fprintf(fp_res, "BUILD TREE,\t%8.2f SECS %10lld NODES IN DISK\n\n", TIME, INDISK);

		for (int i = 0; i < rsize; i++) {
			ll CNT = 0;
			printf("R=%d\n", R[i]);
			fprintf(fp_res, "R=%d\n", R[i]);
			for (int p = 0; p < psize; p++) {
				ll CNTtmp = 0;
				RTREE_C->range(R[i], P[p], &CNTtmp);
				printf("%9lld", CNTtmp), fprintf(fp_res, "%9lld+", CNTtmp);
				if (p == 4) printf("\n"), fprintf(fp_res, "\n");
				CNT += CNTtmp;
			}
			printf("\tTOTAL %10lld IO COUNT\n\n", CNT);
			fprintf(fp_res, "\tTOTAL %10lld IO COUNT\n\n", CNT);
		}
		for (int i = 0; i < ksize; i++) {
			ll CNT = 0;
			printf("K=%d\n", K[i]);
			fprintf(fp_res, "K=%d\n", K[i]);
			for (int p = 0; p < psize; p++) {
				ll CNTtmp = 0;
				RTREE_C->KNN(K[i], P[p], &CNTtmp);
				printf("%9lld", CNTtmp), fprintf(fp_res, "%9lld+", CNTtmp);
				if (p == 4) printf("\n"), fprintf(fp_res, "\n");
				CNT += CNTtmp;
			}
			printf("\tTOTAL %10lld IO COUNT\n\n", CNT);
			fprintf(fp_res, "\tTOTAL %10lld IO COUNT\n\n", CNT);
		}
		fclose(fp_c); delete RTREE_C;
	}

	fclose(fp_res);

	return 0;
}
