#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.1415927

typedef struct {
    int len;      /* length of complex number list */
    float *real;  /* pointer to real number list */
    float *imag;  /* pointer to imaginary number list */
} complexS, *complexP;

void dft(FILE *in, int dir, FILE *out);
int write_output(complexS in, FILE *out);

int main(int argc, char *argv[]) {
    FILE *in, *out;

    if(argc == 4) {
        char *in_fname = argv[1];
        int dir = atoi(argv[2]);
        char *out_fname = argv[3];
        in = fopen(in_fname, "r");
        out = fopen(out_fname, "w");
        dft(in, dir, out);
    } else printf("Expected 3 arguments\n");

    fclose(in);
    fclose(out);
    return 0;
}

void dft(FILE *in, int dir, FILE *out) {
    int width, height;
    fscanf(in, "%d %d", &width, &height);

    int N = height;
    int success;
    float c, s, real, imag;
    complexS f, F;
    f.len = height;
    f.real = malloc(sizeof(float) * N);
    f.imag = malloc(sizeof(float) * N);

    F.len = height;
    F.real = malloc(sizeof(float) * N);
    F.imag = malloc(sizeof(float) * N);


    switch (dir) {
        case 0: // dft
            for (int i = 0; i < N; i++) fscanf(in, "%f %f", &f.real[i], &f.imag[i]);

            // compute spectrum
            for(int n = 0; n < N; n++) {
                real = 0;
                imag = 0;

                // visiting each input pixel
                for(int k = 0; k < N; k++) {
                    c =  cos(2.* PI * n * k / N);
                    s = -sin(2.* PI * n * k / N);
                    real += (f.real[k] * c - f.imag[k] * s);
                    imag += (f.real[k] * s + f.imag[k] * c);
                }
                F.real[n] = real / N;
                F.imag[n] = imag / N;
            }

            success = write_output(F, out);
            if(success > 0) printf("Successfully write to output file.\n");
            else printf("Failed to write to output file.\n");
            break;

        case 1: // idft
            for (int i = 0; i < N; i++) fscanf(in, "%f %f", &F.real[i], &F.imag[i]);

            // compute output pixel
            for(int n = 0; n < N; n++) {
                real = 0;
                imag = 0;
                int k;
                for(k = 0; k < N; k++) {
                    c = cos(2. * PI * n * k / N);
                    s = sin(2. * PI * n * k / N);
                    real += (F.real[k]*c - F.imag[k]*s);
                    imag += (F.real[k]*s + F.imag[k]*c);
                }
                f.real[k] = real;
                f.imag[k] = imag;
            }
            success = write_output(f, out);
            if(success > 0) printf("Successfully write to output file!\n");
            else printf("Failed to write to output file!\n");
            break;
    }
    return;
}


int write_output(complexS in, FILE *out) {
    int success = fprintf(out, "%d %d\n", 2, in.len);
    if(success < 0) return success;
    for(int i = 0; i < in.len; i++) {
        success = fprintf(out, "%f %f\n", in.real[i], in.imag[i]);
        if(success < 0) return success;
    }
    return 1;
}
