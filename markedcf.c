#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include "kdtree-0.5.6/kdtree.h"
#include "kdtree-0.5.6/kdtree_periodic.h"
#include "include/nrutil.h"


/*int *ivector(long nl, long nh);
double *dvector(long nl, long nh);*/

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
	char *fn;
	double rmin,rmax,L,px,py,pz,range;
	double *x,*y,*z,*vx,*vy,*vz,*mh;
	int *idx;
	int i, j, dim, ngal, reps=1000;
	char string[1000];
    unsigned int msec,start,tbuild,torig,tperiodic;
    void *kd, *set, *pset;
    FILE *fp;


	if(argc == 4) {
		fn = argv[1];
		if (isdigit(argv[2][0]) && isdigit(argv[3][0])){
    		rmin = atof(argv[2]);
	    	rmax = atof(argv[3]);
	    }
	    else{
	        printf("Both rmin and rmax must be numbers\n");
	        printf("./ufp.c [filename] [rmin] [rmax]\n");
	        exit(0);
	    }
	}
	else {
	    printf("Enter exactly 3 arguments\n");
	    printf("./ufp.c [filename] [rmin] [rmax]\n");
	    exit(0);
	}

    printf("%f\n", rmin);
    printf("%f\n", rmax);
    printf("%s\n", fn);

    ngal = 0;
    dim = 3;

    fp = fopen(fn,"r");
	if( !(fp=fopen(fn,"r")) ){
        printf("ERROR opening [%s]\n",fn);
        exit(0);
    }

    while(fgets(string,1000,fp)){
        ngal++;
    }
    rewind(fp);
    printf("ngals: %d\n", ngal);

    /*
    x = dvector(1,ngal);
    y = dvector(1,ngal);
    z = dvector(1,ngal);
    vx = dvector(1, ngal);
    vy = dvector(1, ngal);
    vz = dvector(1, ngal);
    mh = dvector(1, ngal);
    idx = ivector(1, ngal);*/

    x = malloc(sizeof(double)*ngal);
    y = malloc(sizeof(double)*ngal);
    z = malloc(sizeof(double)*ngal);
    vx = malloc(sizeof(double)*ngal);
    vy = malloc(sizeof(double)*ngal);
    vz = malloc(sizeof(double)*ngal);
    mh = malloc(sizeof(double)*ngal);
    idx = malloc(sizeof(int)*ngal);

    printf("Reading file...\n");

    for(i = 0; i < ngal; i++){
        fscanf(fp,"%lf %lf %lf %lf %lf %lf %*d %lf",&x[i],&y[i],&z[i],&vx[i],&vy[i],&vz[i],&mh[i]);
        idx[i] = i;
        fgets(string,1000,fp);
    }

    fclose(fp);

    /* Boxsize - should this be a parameter? */
    L = 1050.0;

    /* Build kdtree */
    kd = kd_create(dim);

	start = get_msec();
	for(i=0; i<ngal; i++) {
	/* 0 is data - eventually want to input index but don't think
	i've gotten it to work with integers */
		assert(kd_insert3(kd, x[i], y[i], z[i], 0) == 0);

	}

    /* Point to query */
    range = L/10.0;
    /*corner*/
    /*px = range/3.0;
    py = range/3.0;
    pz = range/3.0;*/
    /*center*/
    px = L/2.0;
    py = L/2.0;
    pz = L/2.0;

	msec = get_msec() - start;
	printf("Built tree in %.3f sec\n", (float)msec / 1000.0);
    tbuild = msec;

	start = get_msec();
	for (j=0;j<reps;j++){
	    set = kd_nearest_range3(kd, px, py, pz, range);
	}
	msec = get_msec() - start;
	printf("Range query returned %d items in %.3f sec\n", kd_res_size(set), (float)msec / 1000.0);
    torig = msec;

    /*TODO: make sure dist returned is correct */
	start = get_msec();
	for (j=0;j<reps;j++){
	    pset = kd_nearest_range3_periodic(kd, px, py, pz, range, L);
	}
	msec = get_msec() - start;
	printf("Range query returned %d items in %.3f sec\n", kd_res_size(pset), (float)msec / 1000.0);
    tperiodic = msec;

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

    FILE *fptr;
    fptr = fopen("results/times/timekd_center_mock.txt","w");
    if(fptr == NULL){
        printf("Error!\n");
        exit(1);
    }
    fprintf(fptr,"%d,%d,%d,%d\n",ngal,tbuild,torig,tperiodic);
    fclose(fptr);

    kd_res_free(set);
    kd_res_free(pset);

	kd_free(kd);
	return 0;


}
