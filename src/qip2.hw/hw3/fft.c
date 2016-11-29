#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.1415927

typedef struct {
    int len;      /* length of complex number list */
    float *real;  /* pointer to real number list */
    float *imag;  /* pointer to imaginary number list */
} complexS, *complexP;

void fft1D(FILE *in, int dir, FILE *out);
void fft(complexP I1, int dir, complexP I2);
int write_output(complexP in, FILE *out);

int main(int argc, char *argv[]) {
    FILE *in, *out;

    if(argc == 4) {
        char *in_fname = argv[1];
        int dir = atoi(argv[2]);
        char *out_fname = argv[3];
        in = fopen(in_fname, "r");
        out = fopen(out_fname, "w");
        fft1D(in, dir, out);
    } else printf("Expected 3 arguments\n");

    fclose(in);
    fclose(out);
    return 0;
}

void fft1D(FILE *in, int dir, FILE *out) {
    int width, height;
    fscanf(in, "%d %d", &width, &height);

    int N = height;
    int success;
    float c, s, real, imag;

    complexP I1 = malloc(sizeof(*I1));
    complexP I2 = malloc(sizeof(*I2));
    I1->len = N;
    I1->real = malloc(sizeof(float)*N);
    I1->imag = malloc(sizeof(float)*N);
    I2->len = height;
    I2->real = malloc(sizeof(float)*N);
    I2->imag = malloc(sizeof(float)*N);

    for (int i = 0; i < N; i++) fscanf(in, "%f %f", &I1->real[i], &I1->imag[i]);

    fft(I1, dir, I2);
    if(!dir) {
        for(int i = 0; i < N; i++) {
            I2->real[i] /= N;
            I2->imag[i] /= N;
        }
    }
    success = write_output(I2, out);
    if(success > 0) printf("Successfully write to output file.\n");
    else printf("Failed to write to output file.\n");
    return;
}

int write_output(complexP in, FILE *out) {
    int success = fprintf(out, "%d %d\n", 2, in->len);
    if(success < 0) return success;

    for(int i = 0; i < in->len; i++) {
        success = fprintf(out, "%f %f\n", in->real[i], in->imag[i]);
        if(success < 0) return success;
    }
    return 1;
}

void fft(complexP I1, int dir, complexP I2) {
    int i, N, N2;
    float *r1, *i1, *r2, *i2, *ra, *ia, *rb, *ib;
    double FCTR, fctr, a, b, c, s;
    complexP Ia, Ib;

    N = I1->len;
    r1 = I1->real;
    i1 = I1->imag;
    r2 = I2 ->real;
    i2 = I2 ->imag;

    if(N == 2) {
    		a = r1[0] + r1[1];
    		b = i1[0] + i1[1];
    		r2[1] = r1[0] - r1[1];
    		i2[1] = i1[0] - i1[1];
    		r2[0] = a;
    		i2[0] = b;
  	} else {
        Ia = malloc(sizeof(*Ia));
        Ib = malloc(sizeof(*Ib));
  	    N2 = N / 2;
        Ia->len = N2;
        Ia->real = malloc(sizeof(float)*N2);
        Ia->imag = malloc(sizeof(float)*N2);
        Ib->len = N2;
        Ib->real = malloc(sizeof(float)*N2);
        Ib->imag = malloc(sizeof(float)*N2);
  	    ra = Ia->real;
        ia = Ia->imag;
  	    rb = Ib->real;
        ib = Ib->imag;

    		for(i=0; i<N2; i++) {
    		    ra[i] = *r1++;
            ia[i] = *i1++;
    		    rb[i] = *r1++;
            ib[i] = *i1++;
    		}

    		fft(Ia, dir, Ia);
    		fft(Ib, dir, Ib);

    		if(!dir) FCTR = -PI*2 / N; // forward
    		else FCTR = PI*2 / N;

    		for(fctr = i = 0; i < N2; i++, fctr += FCTR) {
      			c = cos(fctr);
      			s = sin(fctr);
      			a = c * rb[i] - s * ib[i];
      			r2[i]	= ra[i] + a;
      			r2[i+N2] = ra[i] - a;
      			a = s * rb[i] + c * ib[i];
      			i2[i]	= ia[i] + a;
      			i2[i+N2] = ia[i] - a;
    		}
  	}
}
