#include <algorithm>
#include <vector>
using std::vector;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_median:
//
// Apply median filter of size sz x sz to I1.
// Clamp sz to 9.
// Output is in I2.
//

void
copyRowToBuffer(ChannelPtr<uchar> &p1, short* bufFor1Row, int w, int sz);

int
getMedianWithK(std::vector<int> v, int k);

int
getMedianHisto(std::vector<int> histo, int total, int k);

void
HW_median(ImagePtr I1, int sz, ImagePtr I2)
{
    int method = 1;
    int k = 0;
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w*h;

    if (sz == 1) {
      int type;
      ChannelPtr<uchar> p1, p2, endd;
      for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
          IP_getChannel(I2, ch, p2, type);
          for(endd = p1 + total; p1<endd;) *p2++ = *p1++;
      }
    } else if (sz > 1) {
      int bufSz = sz+w-1; // size of buffer for each padded row
      short* buffers[sz]; // array of sz pointers, sz refers to # of pointers
      for (int i=0; i<sz; i++) { buffers[i] = new short[bufSz]; }

      int type;
      ChannelPtr<uchar> p1, p2, endd;
      for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
          IP_getChannel(I2, ch, p2, type);
          endd = p1+total;

          // copy first row to first sz/2 buffers, where are top padded rows
          for (int i=0; i<sz/2; i++) {
              copyRowToBuffer(p1, buffers[i], w, sz);
              p1=p1-w; // we dont want to go to the next row, continue copying the first row
          }

          // continue to rest of buffers, note here we still begin copying from frist row of I1
          for (int i=sz/2; i<sz; i++) {
              copyRowToBuffer(p1, buffers[i], w, sz);
          }

          clock_t t;
          if (method == 0) {  // if sorting method is selected
              t = clock();  // start clock
              std::vector<int> v(0);  // vector for storing neighbors

              for (int y=0; y<h; y++) {  // visit each row
                  for (int i=0; i<sz; i++) {  // visit each pixel value in neighborhood
                      for (int j=0; j<sz; j++) {
                          v.push_back(buffers[j][i]);
                      }
                  }

                  for (int x=0; x<w; x++) {  // visit each pixel in row
                      *p2++ = getMedianWithK(v, k);  // use sorting to find median

                      if (x<w-1) {
                          v.erase(v.begin(), v.begin()+sz);  // delete outgoing column
                          for (int i=0; i<sz; i++) {
                              v.push_back(buffers[i][x+sz]);  // add incoming column
                          }
                      }
                  }
                  v.clear(); // clear vector

                  int nextRowIndex = y+sz-1;
                  int nextBufferIndex = nextRowIndex%sz;
                  copyRowToBuffer(p1, buffers[nextBufferIndex], w, sz);
                  if (p1>endd) p1-=w; // if have passed last pix, go back to the first pix of last row
              }
              t = clock() - t;  // clock ends
              printf("Median filter %dx%d using sorting method is: %f seconds\n", sz, sz, ((float)t)/CLOCKS_PER_SEC);

          } else {   // if histogram based method is selected
              t = clock();   // start clock
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
                          histo[buffers[i][x]]--;      // delete outgoing column (x)
                          histo[buffers[i][x+sz]]++;  // add incoming column (x+sz)
                      }
                  }
                  *p2++ = getMedianHisto(histo, sz*sz, k);  // use histogram to find median

                  std::fill(histo.begin(), histo.end(), 0); // reset histogram

                  // move to the next row
                  int nextRowIndex = y+sz-1; // index of next row that is needed to copy to buffer
                  int nextBufferIndex = nextRowIndex%sz; // index of buffers that next row needs to copy to
                  copyRowToBuffer(p1, buffers[nextBufferIndex], w, sz);
                  if (p1>endd) p1-=w; // if have passed last pix, go back to the first pix of last row
              }
              t = clock() - t;
              printf("Median filter %dx%d using histogram based is: %f seconds\n", sz, sz, ((float)t)/CLOCKS_PER_SEC);
          }
      }
    }
}

void
copyRowToBuffer(ChannelPtr<uchar> &p1, short* bufFor1Row, int w, int sz) {
    /* |..sz/2..|..w..|..sz/2..|  => bufSz=sz+w-1 */

    int bufSz = sz+w-1;
    for (int i=0; i<sz/2; i++) { bufFor1Row[i]=*p1; }
    for (int i=sz/2; i<sz/2+w; i++) {
        bufFor1Row[i] = *p1;
        p1++;
    }
    p1-=1;
    for (int i=sz/2+w; i<bufSz; i++) { bufFor1Row[i]=*p1; }
    p1+=1;
}

int
getMedianWithK(std::vector<int> v, int k) {
    int vSz = v.size(); // vector size, also equal to kernel size, always odd
    unsigned short sum;

    if (vSz==1) { return v[0]; }  // no need to sort if there is only 1 ele
    else {
        std::sort(v.begin(), v.end());  // sort the vector
        int middleIndex = vSz/2;    // middle index
        if (k==0) { return v[middleIndex]; }  // if k is 0 return the value at the middle index
        else {
            sum=v[middleIndex]; // initialize sum

            // add left k values and right k values to sum
            for (int i=1; i<=k; i++) { sum+= (v[middleIndex-i]+v[middleIndex+i]); }
            return sum/(k*2+1); // returns the average
        }
    }
}

int
getMedianHisto(std::vector<int> histo, int total, int k) { // total is total # of neighbors
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
