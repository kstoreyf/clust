/*! gcc -Wall -g -o test test.c libkdtree.a */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include "kdtree-0.5.6/kdtree.h"
#include "kdtree-0.5.6/kdtree_periodic.h"



unsigned int get_msec(void)
{
	static struct timeval timeval, first_timeval;

	gettimeofday(&timeval, 0);

	if(first_timeval.tv_sec == 0) {
		first_timeval = timeval;
		return 0;
	}
	return (timeval.tv_sec - first_timeval.tv_sec) * 1000 + (timeval.tv_usec - first_timeval.tv_usec) / 1000;
}


int main(int argc, char **argv)
{

    unsigned int tbuild, torig, tperiodic;
    int i, ntrees = 7;
    int treesizes[7] = {1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7};

    FILE *fptr;
    fptr = fopen("results/times/timekd_center.txt","w");

    if(fptr == NULL){
        printf("Error!\n");
        exit(1);
    }


    for (i=0; i<ntrees; i++){
        timekd(&treesizes[i], &tbuild, &torig, &tperiodic);
        fprintf(fptr,"%d,%d,%d,%d\n",treesizes[i],tbuild,torig,tperiodic);
    }

    fclose(fptr);
    return 0;
}


void timekd(int* treesize, unsigned int* tbuild, unsigned int* torig, unsigned int* tperiodic){

	int i, j, reps;
	void *kd, *set, *pset;
	unsigned int msec, start;
    double L, range, ndens;
    double px, py, pz;

    reps = 1000;
    ndens = 0.00047; /* #/Mpc^3, number density of actual sim */
    L = pow((double)*treesize/ndens,1.0/3.0);
    range = L/10.0;
    /*corner*/
    /*px = range/3.0;
    py = range/3.0;
    pz = range/3.0;*/
    /*center*/
    px = L/2.0;
    py = L/2.0;
    pz = L/2.0;

    printf("L=%f, r=%f, px=%f\n", L, range, px);

	printf("inserting %d random vectors...\n", *treesize);
	fflush(stdout);
	kd = kd_create(3);

	start = get_msec();
	for(i=0; i<*treesize; i++) {
		float x, y, z;
		x = ((float)rand() / RAND_MAX) * L;
		y = ((float)rand() / RAND_MAX) * L;
		z = ((float)rand() / RAND_MAX) * L;
		assert(kd_insert3(kd, x, y, z, 0) == 0);
	}
    msec = get_msec() - start;
    *tbuild = msec;
    printf("Build time: %.3f sec\n", (float)msec / 1000.0);


    start = get_msec();
    for (j=0;j<reps;j++){
        set = kd_nearest_range3(kd, px, py, pz, range);
    }
    msec = get_msec() - start;
    *torig = msec;
    printf("range query returned %d items in %.3f sec\n", kd_res_size(set), (float)msec / 1000.0);

    start = get_msec();
    for (j=0;j<reps;j++){
        /*TODO: make sure dist returned is correct */
        pset = kd_nearest_range3_periodic(kd, px, py, pz, range, L);
    }
    msec = get_msec() - start;
    *tperiodic = msec;
    printf("range query returned %d items in %.3f sec\n", kd_res_size(pset), (float)msec / 1000.0);


    kd_res_free(set);
    kd_res_free(pset);

	kd_free(kd);
}
