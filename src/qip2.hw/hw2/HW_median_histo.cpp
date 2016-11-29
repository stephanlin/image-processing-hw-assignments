// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median (Extra Credit - histogram approach):
//
// Apply median filter of size sz x sz to I1.
// Output is in I2.
//
// Written by: Weifan Lin & Kaimin Jiang, 2016
//
#include <algorithm>
#include <vector>

void copyRowToBuffer(ChannelPtr<uchar>, short*, int, int);
int getMedianHisto(std::vector<int>, int, int);

void
HW_median(ImagePtr I1, int sz, ImagePtr I2)
{
    if(sz % 2 == 0) sz++; // set sz to always be an odd number
    int k = 0; // k nearest neighbors, hard coded k to be 0 for our homework, but also works if k is other than 0
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w*h;

    int type;
    ChannelPtr<uchar> p1, p2, endd;

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
            std::vector<int> histo(MXGRAY);
            for (int y=0; y<h; y++) {
                for (int i=0; i<sz; i++) {
                    for (int j=0; j<sz; j++) {
                        histo[buffers[j][i]]++;
                    }
                }

                for (int x=0; x<w-1; x++) {   //
                    *p2++ = getMedianHisto(histo, sz*sz, k);  // use histogram to find median

                    for (int i = 0; i < sz; i++) {
                        histo[buffers[i][x]]--;     // delete outgoing column (x)
                        histo[buffers[i][x+sz]]++;  // add incoming column (x+sz)
                    }
                }
                *p2++ = getMedianHisto(histo, sz*sz, k);  // use histogram to find median

                std::fill(histo.begin(), histo.end(), 0); // reset histogram

                // move to the next row
                int nextBufferIndex = (y+sz-1)%sz; // index of buffers that next row needs to copy to
                copyRowToBuffer(p1, buffers[nextBufferIndex], w, sz);
                if (p1 < endd-w) p1+=w;
            }
        }
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// copyRowToBuffer:
//
// copy a row of pixels to buffer with padding size of sz
// |..sz/2..|..w..|..sz/2..|
//
void
copyRowToBuffer(ChannelPtr<uchar> p1, short* buffer, int w, int sz) {
    int bufSz = sz+w-1;
    for (int i = 0       ; i < sz/2    ; i++) buffer[i] = *p1  ;
    for (int i = sz/2    ; i < sz/2+w-1; i++) buffer[i] = *p1++;
    for (int i = sz/2+w-1; i < bufSz   ; i++) buffer[i] = *p1  ;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getMedianHisto:
//
// input: vector histo, int total (total # of neighbors), int k
// output: return median of all elements in histo with k nearest neighbors
//
int
getMedianHisto(std::vector<int> histo, int total, int k) {
    int mid = total/2+1; // middle location of values in histo
    int count = 0;  // count number of pixels
    int i;
    for (i=0; i<MXGRAY; i++) {
        count += histo[i];
        if (count >= mid) {  // when count reaches to mid or exceeds, median is found
            break; // for loop breaks, now i is median
        }
    }
    if (k==0) { return i; }
    else {
        int sum = i;
        int left = mid-(count-histo[i])-1;  // how many on the left can give
        int right = count-mid;              // how many on the right can give
        int leftIndex=i, rightIndex=i;
        for (int j=1; j<=k; j++) {
            while (left == 0) {  // while left has none to give
                --leftIndex;    // move to one index to the left
                left=histo[leftIndex];  // left now has histo[leftIndex] to give
            }
            sum+=leftIndex; // give one to the sum
            --left; // left has one less to give
            while (right == 0) {
                ++rightIndex;
                right = histo[rightIndex];
            }
            sum+=rightIndex;
            --right;
        }

        return sum/(k*2+1);
    }
}
