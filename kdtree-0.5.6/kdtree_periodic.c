/* This is a wrapper around / addition to the kdtree code by John Tsiombikas.
The algorithm is inspired by the python implementation by Patrick Varilly,
which can be found at https://github.com/patvarilly/periodic_kdtree.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "kdtree.h"



struct res_node {
	struct kdnode *item;
	double dist_sq;
	struct res_node *next;
};


#define alloc_resnode()		malloc(sizeof(struct res_node))

struct kdres *kd_nearest_range3_periodic(struct kdtree *tree,
                double x, double y, double z, double range, double L)
{

    /*TODO: safeguards for range & L (see patrick's code) */
    int i, j, ii;
    int dim = 3; /* #magic */
    int rsize = 0;
    int num_ims = 1;
    int num_ims_max = pow(2, dim); /* 2^dim possible locations of image points */

	double pos[dim];
	double positions[num_ims_max][dim];
    struct kdres *rset;

	pos[0] = x;
	pos[1] = y;
	pos[2] = z;

    for(ii=0; ii<dim; ii++) {
        positions[0][ii] = pos[ii];
    }

    /* Get images of point */
    for(i=0; i<dim; i++) {
        int num_ims_new = num_ims;

        if (L-pos[i] < range) {
            for(j=0; j<num_ims; j++) {
                double im[dim];
                int ii;

                for(ii=0; ii<dim; ii++) {
                    im[ii] = positions[j][ii];
                }
                im[i] -= L;
                for(ii=0; ii<dim; ii++) {
                    positions[num_ims][ii] = im[ii];
                }
                num_ims_new += 1;
            }
            num_ims = num_ims_new;
        }
        if (pos[i] < range) {
            for(j=0; j<num_ims; j++) {
                double im[dim];
                int ii;
                for(ii=0; ii<dim; ii++) {
                    im[ii] = positions[j][ii];
                }
                im[i] += L;

                for(ii=0; ii<dim; ii++) {
                    positions[num_ims][ii] = im[ii];
                }

                num_ims_new += 1;
            }
            num_ims = num_ims_new;

        }
    }

    /* set up result struct */
	if(!(rset = malloc(sizeof *rset))) {
		return 0;
	}
	if(!(rset->rlist = alloc_resnode())) {
		free(rset);
		return 0;
	}
	rset->rlist->next = 0;
	rset->tree = tree;
    rset->size = rsize;

    /* Query kdtree for each image */
    for (j=0; j<num_ims; j++){

        struct kdres *res;

        res = kd_nearest_range(tree, positions[j], range);

        rsize = kd_res_size(res);
        rset->size += kd_res_size(res);

        /* Insert the results into our new result set, ordered by dist_sq */
        while( !kd_res_end(res) ) {

            /* copied from rlist_insert in kdtree.c, but it's a static function */
            /* TODO: figure out how to use that function or write my own */
            double dist_sq = res->riter->dist_sq;
            struct res_node *rnode;

            if(!(rnode = alloc_resnode())) {
                return -1;
            }
            rnode->item = res->riter->item;
            rnode->dist_sq = dist_sq;

            if(dist_sq >= 0.0) {
                while(rset->rlist->next && rset->rlist->next->dist_sq < dist_sq) {
                    rset->rlist = rset->rlist->next;
                }
            }
            rnode->next = rset->rlist->next;
            rset->rlist->next = rnode;

            kd_res_next( res );
        }
        kd_res_free(res);
    }

    kd_res_rewind(rset);

	return rset;
}
