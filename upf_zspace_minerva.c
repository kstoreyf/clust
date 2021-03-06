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
/*#include "kdtree.h"
#include "kdtree_periodic.h"*/

#define PI 3.1415926535897932

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
	char *fn, *fnsave, *fn_ng;
	double rmin,rmax,L,px,py,pz,range,udens,meanngals,meandens,redshift;
  double Omega_m, w, E;
	double *x,*y,*z,*vx,*vy,*vz,*mh;
	int *idx;
	int i, j, dim, ngal, nspheres, nrs, reps=1000;
	char string[1000];
  unsigned int msec,start,tbuild,torig,tperiodic;
  void *kd, *set, *pset;
  FILE *fp, *fp_ng;

  nspheres = 1e6;
  redshift = 0.57;

  srand(time(NULL));
	if(argc==7) {
		fn = argv[1];
		fnsave = argv[5];
    fn_ng = argv[6];
    if (isdigit(argv[2][0]) && isdigit(argv[3][0]) && isdigit(argv[4][0])){
    		rmin = atof(argv[2]);
	    	rmax = atof(argv[3]);
	    	nrs = atoi(argv[4]);
	    }
	    else{
	        printf("Both rmin and rmax must be numbers\n");
	        printf("./upf [filename] [rmin] [rmax] [nbins] [savename] [meandensfile]\n");
	        exit(0);
	    }
	}
	else {
	    printf("Enter exactly 6 arguments\n");
	    printf("./upf [filename] [rmin] [rmax] [nbins] [savename] [meandensfile]\n");
	    exit(0);
	}

    printf("%f\n", rmin);
    printf("%f\n", rmax);
    printf("%s\n", fn);

    /* Boxsize - should this be a parameter? */
    L = 1500.0; /* Mpc/h */
    ngal = 0;
    dim = 3;

    Omega_m = 0.285;
    w = -1;
    /* E^2 = H^2/H0^2 */
    E = pow(Omega_m*pow(1+redshift, 3) + (1-Omega_m)*pow(1+redshift, 3*(1+w)), 0.5);

    /* get mean density */
    fp_ng = fopen(fn_ng,"r");
    if( !(fp_ng=fopen(fn_ng,"r")) ){
      printf("ERROR opening [%s]\n",fn_ng);
      exit(0);
    }

    fscanf(fp_ng,"%lf",&meanngals);
    meandens = meanngals/pow(L, dim);
    printf("mean number density of mocks: %f\n",meandens);
    udens = 0.2*meandens; /* (h/Mpc)^3 */
    fclose(fp_ng);
    
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

    printf("Reading file %s...\n", fn);

    for(i = 0; i < ngal; i++){
        fscanf(fp,"%lf %lf %lf %lf %lf %lf %*d %*lf %*d %*lf %*lf %*lf %*lf %*lf %*lf",&x[i],&y[i],&z[i],&vx[i],&vy[i],&vz[i]);
        fgets(string,1000,fp);
    }

    fclose(fp);

    /* convert to redshift space, with line of sight along z */
   
    double z_zspace; 
    for (i=0; i<ngal; i++){
      z_zspace = z[i]+vz[i]*(1+redshift)/(E*100);
      if (z_zspace < 0) { 
        z_zspace += L;
      }
      if (z_zspace >= L) {
        z_zspace -= L;
      }
      z[i] = z_zspace;
    }

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
      double vol = 4.0/3.0*PI*pow(radii[i], 3);
        for (j=0; j<nspheres; j++){
            /* random center of sphere */
            px = ((float)rand() / RAND_MAX) * L;
            py = ((float)rand() / RAND_MAX) * L;
            pz = ((float)rand() / RAND_MAX) * L;
            pset = kd_nearest_range3_periodic(kd, px, py, pz, radii[i], L);
            /* TODO: make work for underdensity, now just void */
            if (kd_res_size(pset)/vol <= udens) {
                nudens[i] += 1;
            }
        }
        msec = get_msec() - start;
	    printf("Found %d/%d underdense nspheres at radius %f in %.3f\n", nudens[i], nspheres, radii[i], (float)msec / 1000.0);

    }



    FILE *fptr;
    
    /*const char s[2] = "/";
    char *token;
    token = strtok(str, s);
    while( token != NULL ) {
      printf( " %s\n", token );
      token = strtok(NULL, s);
    }
    char *fnsave = ""    
    char *fnsave = "/home/users/ksf293/clust/results/vpf.txt";*/
    fptr = fopen(fnsave,"w");
    if(fptr == NULL){
        printf("Error! Could not open %s for writing.\n", fnsave);
        exit(1);
    }

    for (i=0; i<nrs; i++){
        /* TODO: make sure this works */
        double nfrac = (double)nudens[i] / (double)nspheres;
        fprintf(fptr,"%f,%f\n",radii[i],nfrac);

    }

    fclose(fptr);
    printf("Wrote results to %s\n", fnsave);
    kd_res_free(pset);

	kd_free(kd);
	return 0;


}


