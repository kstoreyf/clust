#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
/*#include "kdtree-0.5.6/kdtree.h"
#include "kdtree-0.5.6/kdtree_periodic.h"*/
#include "include/kdtree.h"
#include "include/kdtree_periodic.h"
/*#include "include/nrutil.h"*/


int main(int argc, char **argv);
void linspace(double xmin, double xmax, int xnum, double* xarr);
void logspace(double xmin, double xmax, int xnum, double* xarr);
unsigned int get_msec(void);

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
	double rmin,rmax,L,px,py,pz,range,udens;
	double *x,*y,*z,*vx,*vy,*vz,*mh;
	int *idx;
	int i, j, dim, ngal, nspheres, nrs, reps=1000;
	char string[1000];
    unsigned int msec,start,tbuild,torig,tperiodic;
    void *kd, *set, *pset;
    FILE *fp;

    nspheres = 1e6;
    udens = 0.0;

    srand(time(NULL));

	if(argc == 5) {
		fn = argv[1];
		if (isdigit(argv[2][0]) && isdigit(argv[3][0])){
    		rmin = atof(argv[2]);
	    	rmax = atof(argv[3]);
	    	nrs = atoi(argv[4]);

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

    /* Boxsize - should this be a parameter? */
    L = 1050.0;
    ngal = 0;
    dim = 3;

    int nudens[nrs];
    for(i=0;i<nrs;i++){
        nudens[i] = 0;
    }
    double radii[nrs];
    /*logspace(rmin, rmax, nrs, &radii);*/
    linspace(rmin, rmax, nrs, &radii);
    printf("radii:");
    for(i=0;i<nrs;i++){
        printf(" %f", radii[i]);
    }
    printf("\n");

    /* Load data */
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


    /* Build kdtree */
    kd = kd_create(dim);

	start = get_msec();
	for(i=0; i<ngal; i++) {
		assert(kd_insert3(kd, x[i], y[i], z[i], 0) == 0);
	}
	msec = get_msec() - start;
	printf("Built tree in %.3f sec\n", (float)msec / 1000.0);
    tbuild = msec;

    /* query at radii */
    for (i=0; i<nrs; i++){
    	start = get_msec();
        for (j=0; j<nspheres; j++){
            /* random center of sphere */
            px = ((float)rand() / RAND_MAX) * L;
            py = ((float)rand() / RAND_MAX) * L;
            pz = ((float)rand() / RAND_MAX) * L;
            pset = kd_nearest_range3_periodic(kd, px, py, pz, radii[i], L);
            /* TODO: make work for underdensity, now just void */
            if (kd_res_size(pset) <= udens) {
                nudens[i] += 1;
            }
        }
        msec = get_msec() - start;
	    printf("Found %d/%d underdense nspheres at radius %f in %.3f\n", nudens[i], nspheres, radii[i], (float)msec / 1000.0);

    }



    FILE *fptr;
    fptr = fopen("results/vpf.txt","w");
    if(fptr == NULL){
        printf("Error!\n");
        exit(1);
    }

    for (i=0; i<nrs; i++){
        /* TODO: make sure this works */
        double nfrac = (double)nudens[i] / (double)nspheres;
        fprintf(fptr,"%f,%f\n",radii[i],nfrac);

    }

    fclose(fptr);

    kd_res_free(pset);

	kd_free(kd);
	return 0;


}


void logspace(double xmin, double xmax, int xnum, double* xarr){
    double logspace = (log10(xmax) - log10(xmin))/(xnum-1);
    int i;
    for (i=0; i<xnum; i++){
        double logx = log10(xmin) + i*logspace;
        xarr[i] = pow(10, logx);
    }
}

void linspace(double xmin, double xmax, int xnum, double* xarr){
    double space = (xmax - xmin)/(xnum-1);
    int i;
    for (i=0; i<xnum; i++){
        xarr[i] = xmin + i*space;
    }
}