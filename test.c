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
	int i, vcount = 538000;
	void *kd, *set, *pset;
	unsigned int msec, start;
    double L = 1050.0;
    double range = 100.0;
    double pos[3];
    double px, py, pz;

	if(argc > 1 && isdigit(argv[1][0])) {
		vcount = atoi(argv[1]);
	}
	printf("inserting %d random vectors... ", vcount);
	fflush(stdout);

	kd = kd_create(3);

	start = get_msec();
	for(i=0; i<vcount; i++) {
		float x, y, z;
		x = ((float)rand() / RAND_MAX) * L;
		y = ((float)rand() / RAND_MAX) * L;
		z = ((float)rand() / RAND_MAX) * L;
		assert(kd_insert3(kd, x, y, z, 0) == 0);

	}

    px = 50;
    py = 50;
    pz = 50;
    /*assert(kd_insert3(kd, 9.5, 4, 4, 0) == 0);
    assert(kd_insert3(kd, 2, 4, 4, 0) == 0);*/

	msec = get_msec() - start;
	printf("%.3f sec\n", (float)msec / 1000.0);

	start = get_msec();
	set = kd_nearest_range3(kd, px, py, pz, range);
	msec = get_msec() - start;
	printf("range query returned %d items in %.5f sec\n", kd_res_size(set), (float)msec / 1000.0);

    /*TODO: make sure dist returned is correct */
	start = get_msec();
	pset = kd_nearest_range3_periodic(kd, px, py, pz, range, L);
	msec = get_msec() - start;
	printf("range query returned %d items in %.5f sec\n", kd_res_size(pset), (float)msec / 1000.0);

    /*
    printf("Original results\n");
    while( !kd_res_end( set ) ) {
        kd_res_item( set, pos );

        printf( "node at (%.3f, %.3f, %.3f)\n",
            pos[0], pos[1], pos[2]);

        kd_res_next( set );
    }

    printf("Periodic results\n");
    while( !kd_res_end( pset ) ) {
        kd_res_item( pset, pos );

        printf( "node at (%.3f, %.3f, %.3f)\n",
            pos[0], pos[1], pos[2]);

        kd_res_next( pset );
    }
    */

    kd_res_free(set);
    kd_res_free(pset);

	kd_free(kd);
	return 0;
}
