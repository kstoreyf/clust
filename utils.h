#define PI 3.1415926535897932

int count_lines_in_file(char *fn);
int load_mock(char *fn, 
              double *x, double *y, double *z,
              double *vx, double *vy, double *vz,
              int n_galaxies);
int real_to_redshift_space(double *position, double *velocity,
                           int n_galaxies, double L, double redshift,
                           double Omega_m, double w);
void linspace(double xmin, double xmax, int xnum, double* xarr);
void logspace(double xmin, double xmax, int xnum, double* xarr);
unsigned int get_msec(void);