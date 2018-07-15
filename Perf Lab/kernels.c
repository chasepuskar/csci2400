/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/*
 * Please fill in the following student struct:
 */
const team_t team = {
    "chpu4633@colorado.edu", //Replace this with your email address.
    ""                   //Replace this with your partner's email address. Leave blank if working alone.
};

/***************
 * FLIP KERNEL
 ***************/

/******************************************************
 * Your different versions of the flip kernel go here
 ******************************************************/
 
/* 
 * naive_flip - The naive baseline version of flip 
 */
char naive_flip_descr[] = "naive_flip: Naive baseline implementation";
void naive_flip(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    for (i = 0; i < dim; i++){
        for (j = 0; j < dim; j++){
            dst[RIDX_F(i, j, dim)].red   = src[RIDX(i, j, dim)].red;
            dst[RIDX_F(i, j, dim)].green = src[RIDX(i, j, dim)].green;
            dst[RIDX_F(i, j, dim)].blue  = src[RIDX(i, j, dim)].blue;
        }
    }
}

/* 
 * flip - Your current working version of flip
 * IMPORTANT: This is the version you will be graded on
 */
char flip_descr[] = "flip: loop unrolls, reuses common expressions, prioitizes write hits to read hits";
void flip(int dim, pixel *src, pixel *dst) 
{
    int i, j, ridx_f;
    int counter = 0;
    int in = -1;
    for (i = 0; i < dim; i++){
    	in += dim;
        for (j = 0; j < dim; j+=32){
        	ridx_f = in - j;
    		dst[counter] = src[ridx_f];
    		dst[counter+1] = src[ridx_f-1];
    		dst[counter+2] = src[ridx_f-2];
    		dst[counter+3] = src[ridx_f-3];
    		dst[counter+4] = src[ridx_f-4];
    		dst[counter+5] = src[ridx_f-5];
    		dst[counter+6] = src[ridx_f-6];
    		dst[counter+7] = src[ridx_f-7];
    		dst[counter+8] = src[ridx_f-8];
    		dst[counter+9] = src[ridx_f-9];
    		dst[counter+10] = src[ridx_f-10];
    		dst[counter+11] = src[ridx_f-11];
    		dst[counter+12] = src[ridx_f-12];
    		dst[counter+13] = src[ridx_f-13];
    		dst[counter+14] = src[ridx_f-14];
    		dst[counter+15] = src[ridx_f-15];
    		dst[counter+16] = src[ridx_f-16];
    		dst[counter+17] = src[ridx_f-17];
    		dst[counter+18] = src[ridx_f-18];
    		dst[counter+19] = src[ridx_f-19];
    		dst[counter+20] = src[ridx_f-20];
    		dst[counter+21] = src[ridx_f-21];
    		dst[counter+22] = src[ridx_f-22];
    		dst[counter+23] = src[ridx_f-23];
    		dst[counter+24] = src[ridx_f-24];
    		dst[counter+25] = src[ridx_f-25];
    		dst[counter+26] = src[ridx_f-26];
    		dst[counter+27] = src[ridx_f-27];
    		dst[counter+28] = src[ridx_f-28];
    		dst[counter+29] = src[ridx_f-29];
			dst[counter+30] = src[ridx_f-30];
    		dst[counter+31] = src[ridx_f-31];
    		counter+=32;
        }

    }
}

/*********************************************************************
 * register_flip_functions - Register all of your different versions
 *     of the flip kernel with the driver by calling the
 *     add_flip_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_flip_functions() 
{
	//add_flip_function(&naive_flip, naive_flip_descr);
    add_flip_function(&flip, flip_descr);
    /* ... Register additional test functions here */
}


/***************
 * CONVOLVE KERNEL
 **************/
 
/***************************************************************
 * Various typedefs and helper functions for the convolve function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute a pixel value */
typedef struct {
    float red;
    float green;
    float blue;
    float weight;
} pixel_sum;

/******************************************************
 * Your different versions of the convolve kernel go here
 ******************************************************/

/*
 * naive_convolve - The naive baseline version of convolve 
 */
char naive_convolve_descr[] = "naive_convolve: Naive baseline implementation";
void naive_convolve(int dim, pixel *src, pixel *dst) 
{
    int i, j, ii, jj, curI, curJ;
    pixel_sum ps;
    
    for (j = 0; j < dim; j++){
        for (i = 0; i < dim; i++){
            ps.red    = 0.0;
            ps.green  = 0.0;
            ps.blue   = 0.0;
            ps.weight = 0.0;
            for (jj = -2; jj <= 2; jj++){
                for (ii = -2; ii <= 2; ii++){
                    curJ = j+jj;
                    if(curJ<0 || curJ>=dim){
                        continue;
                    }
                    curI = i+ii;
                    if(curI<0 || curI>=dim){
                        continue;
                    }
                    ps.red   += src[RIDX(curI, curJ, dim)].red *   kernel[ii+2][jj+2];
                    ps.green += src[RIDX(curI, curJ, dim)].green * kernel[ii+2][jj+2];
                    ps.blue  += src[RIDX(curI, curJ, dim)].blue *  kernel[ii+2][jj+2];
                    ps.weight += kernel[ii+2][jj+2];
                }
            }
            dst[RIDX(i,j,dim)].red   = (unsigned short)(ps.red/ps.weight);
            dst[RIDX(i,j,dim)].green = (unsigned short)(ps.green/ps.weight);
            dst[RIDX(i,j,dim)].blue  = (unsigned short)(ps.blue/ps.weight);
        }
    }
}


void unroll(int in, int i, int j, int dim, pixel *src, pixel *dst){
	int ii, jj, curI, curJ, iii, jjj, ridx1, ridx2, curIn;
	pixel_sum ps;
	ps.red    = 0.0;
    ps.green  = 0.0;
    ps.blue   = 0.0;
    ps.weight = 0.0;
    for (ii = -2; ii <= 2; ii++){
        curI = i+ii;
        if(curI<0 || curI>=dim){
            continue;
        }
        curIn = curI*dim;
        iii = ii+2;
        for (jj = -2; jj <= 2; jj++){
        	curJ = j+jj;
        	if(curJ<0 || curJ>=dim){
        	    continue;
        	}
        	jjj = jj+2;
        	ridx1 = curIn+curJ;
        	ps.red   += src[ridx1].red *   kernel[iii][jjj];
            ps.green += src[ridx1].green * kernel[iii][jjj];
            ps.blue  += src[ridx1].blue *  kernel[iii][jjj];
            ps.weight += kernel[iii][jjj];
        }
    }
    ridx2 = in + j; 
    dst[ridx2].red   = (unsigned short)(ps.red/ps.weight);
    dst[ridx2].green = (unsigned short)(ps.green/ps.weight);
    dst[ridx2].blue  = (unsigned short)(ps.blue/ps.weight);
}

/*
 * convolve - Your current working version of convolve. 
 * IMPORTANT: This is the version you will be graded on
 */
char convolve_descr[] = "convolve: reuses common expressions, unrolling";
void convolve(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int in = -dim;
    
    for (i = 0; i < dim; i++){
    	in += dim;
        for (j = 0; j < dim; j+=32){
            unroll(in,i,j,dim,src,dst);
            unroll(in,i,j+1,dim,src,dst);
            unroll(in,i,j+2,dim,src,dst);
            unroll(in,i,j+3,dim,src,dst);
            unroll(in,i,j+4,dim,src,dst);
            unroll(in,i,j+5,dim,src,dst);
            unroll(in,i,j+6,dim,src,dst);
            unroll(in,i,j+7,dim,src,dst);
            unroll(in,i,j+8,dim,src,dst);
            unroll(in,i,j+9,dim,src,dst);
            unroll(in,i,j+10,dim,src,dst);
            unroll(in,i,j+11,dim,src,dst);
            unroll(in,i,j+12,dim,src,dst);
            unroll(in,i,j+13,dim,src,dst);
            unroll(in,i,j+14,dim,src,dst);
            unroll(in,i,j+15,dim,src,dst);
            unroll(in,i,j+16,dim,src,dst);
            unroll(in,i,j+17,dim,src,dst);
            unroll(in,i,j+18,dim,src,dst);
            unroll(in,i,j+19,dim,src,dst);
            unroll(in,i,j+20,dim,src,dst);
            unroll(in,i,j+21,dim,src,dst);
            unroll(in,i,j+22,dim,src,dst);
            unroll(in,i,j+23,dim,src,dst);
            unroll(in,i,j+24,dim,src,dst);
            unroll(in,i,j+25,dim,src,dst);
            unroll(in,i,j+26,dim,src,dst);
            unroll(in,i,j+27,dim,src,dst);
            unroll(in,i,j+28,dim,src,dst);
            unroll(in,i,j+29,dim,src,dst);
            unroll(in,i,j+30,dim,src,dst);
            unroll(in,i,j+31,dim,src,dst);
        }
    }
}

/********************************************************************* 
 * register_convolve_functions - Register all of your different versions
 *     of the convolve kernel with the driver by calling the
 *     add_convolve_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_convolve_functions() {
	//add_convolve_function(&naive_convolve, naive_convolve_descr);
    add_convolve_function(&convolve, convolve_descr);
    /* ... Register additional test functions here */
}

