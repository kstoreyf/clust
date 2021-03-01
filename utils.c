#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


int count_lines_in_file(char *fn){
    int n_lines = 0;
    char buffer[1000];
    FILE *fp;
    fp = fopen(fn,"r");
	if( !(fp=fopen(fn,"r")) ){
        printf("ERROR opening [%s]\n",fn);
        exit(0);
    }
    while(fgets(buffer,1000,fp)){
        n_lines++;
    }
    rewind(fp);
    fclose(fp);
    return n_lines;
}

int load_mock(char *fn, 
               double *x, double *y, double *z,
               double *vx, double *vy, double *vz,
               int n_galaxies){

    int i;
    char buffer[1000];
    FILE *fp;

    fp = fopen(fn,"r");
	if( !(fp=fopen(fn,"r")) ){
        printf("ERROR opening [%s]\n",fn);
        exit(0);
    }
    for(i = 0; i < n_galaxies; i++){
        fscanf(fp,"%lf %lf %lf %lf %lf %lf",&x[i],&y[i],&z[i],&vx[i],&vy[i],&vz[i]);
        fgets(buffer,1000,fp);
    }
    fclose(fp);
    return 0;
}

/* Overwrites position array with redshift-space position. */
int real_to_redshift_space(double *position, double *velocity,
                           int n_galaxies, double L, double redshift,
                           double Omega_m, double w){

    int i;
    double E;

    /* Compute E=H/H_0 for given cosmology & redshift */
    E = pow(Omega_m*pow(1+redshift, 3) + (1-Omega_m)*pow(1+redshift, 3*(1+w)), 0.5);

    double pos_zspace; 
    for (i=0; i<n_galaxies; i++){
      pos_zspace = position[i]+velocity[i]*(1+redshift)/(E*100);
      if (pos_zspace < 0) { 
        pos_zspace += L;
      }
      if (pos_zspace >= L) {
        pos_zspace -= L;
      }
      position[i] = pos_zspace;
    }
    return 0;             
}

void logspace(double xmin, double xmax, int xnum, double *xarr){
    double logspace = (log10(xmax) - log10(xmin))/(xnum-1);
    int i;
    for (i=0; i<xnum; i++){
        double logx = log10(xmin) + i*logspace;
        xarr[i] = pow(10, logx);
    }
}

void linspace(double xmin, double xmax, int xnum, double *xarr){
    double space = (xmax - xmin)/(xnum-1); 
    int i;
    for (i=0; i<xnum; i++){
        xarr[i] = xmin + i*space;
    }
}

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