#include <algorithm>
#include <vector>
#include <math.h>

extern void copyRowToBuffer(ChannelPtr<uchar>, short*, int, int); // see implementation in HW_errDiffusion.cpp
// int getSumWithKernel(std::vector<int>, ChannelPtr<float>);
int getSumWithKernel(std::vector<int>, std::vector<float>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_convolve:
//
// Apply image convolution on I1. Output is in I2.
//
void
HW_convolve(ImagePtr I1, ImagePtr Ikernel, ImagePtr I2)
{
    int sz = Ikernel->width();
    // if(sz % 2 == 0) sz++; // set sz to always be an odd number

    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w*h;

    int type;
    ChannelPtr<uchar> p1, p2, endd;
    ChannelPtr<float> pKernel;
    IP_getChannel(Ikernel, 0, pKernel, type);
    std::vector<float> vKernel(0);
    for (int i=0; i<sz; i++) {
      for (int j=0; j<sz; j++) {
        vKernel.push_back(pKernel[i+j*sz]);
        //qDebug() << pKernel[i+j*sz];
      }
    }

    if (sz == 1) {
        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
        }
    } else if (sz > 1) {
        int bufSz = sz+w-1; // size of buffer for each padded row
        short* buffers[sz]; // array of sz pointers
        for (int i=0; i<sz; i++) buffers[i] = new short[bufSz];

        for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);
            endd = p1+total;

            // copy first row to first sz/2 buffers, where are top padded rows
            for (int i=0; i<sz/2; i++) copyRowToBuffer(p1, buffers[i], w, sz);

            // continue to rest of buffers, note here we still begin copying from frist row of I1
            for (int i=sz/2; i<sz; i++) {
                copyRowToBuffer(p1, buffers[i], w, sz);
                p1+=w;
            }

            std::vector<int> v(0);  // vector for storing neighbors
            for (int y=0; y<h; y++) {  // visit each row
                for (int i=0; i<sz; i++) {  // visit each pixel value in neighborhood
                    for (int j=0; j<sz; j++) {
                        v.push_back(buffers[j][i]); // * storing column by column from buffers
                    }
                }

                // visit each pixel in row
                for (int x=0; x<w; x++) {
                    *p2++ = (int)CLIP(getSumWithKernel(v, vKernel), 0, MaxGray);
                    if (x<w-1) {
                        v.erase(v.begin(), v.begin()+sz);  // delete outgoing column
                        for (int i=0; i<sz; i++) v.push_back(buffers[i][x+sz]);  // add incoming column
                    }
                }
                v.clear(); // clear vector

                int nextRowIndex = y+sz-1;
                int nextBufferIndex = nextRowIndex%sz;
                copyRowToBuffer(p1, buffers[nextBufferIndex], w, sz);
                if (p1 < endd) p1+=w;
            }
        }
        for (int i=0; i<sz; i++) delete[] buffers[i];
    }
}


// int
// getSumWithKernel(std::vector<int> v, ChannelPtr<float> pKernel) {
//     int vSz = v.size();
//     float sum = 0.0;
//     for (int i=0; i<vSz; i++) {
//         sum += (v[i] * pKernel[i]);
//     }
//     return sum;
// }
int
getSumWithKernel(std::vector<int> v, std::vector<float> k) {
    int vSz = v.size();
    double sum = 0.0;
    for (int i=0; i<vSz; i++) sum += (v[i] * k[i]);
    return sum;
}
