// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoMatch:
//
// Apply histogram matching to I1. Output is in I2.
//
// Written by: Weifan Lin & Kaimin Jiang, 2016
//
void
HW_histoMatch(ImagePtr I1, ImagePtr Ilut, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
    int w = I1->width();
    int h = I1->height();
    int total = w * h;

    int i, type, Havg = 0;
    double scale;
    ChannelPtr<int> lut;

    IP_getChannel(Ilut, 0, lut, type);
    for(int i=0; i<MXGRAY; i++) { Havg += lut[i]; }

    scale = (double) total / Havg;
    if(scale != 1.0) {
        for(i = 0; i < MXGRAY; i++) {
            if(i%2) { lut[i] = floor(lut[i]*scale); }
            else    { lut[i] =  ceil(lut[i]*scale); }
        }
    }

    int left[MXGRAY], right[MXGRAY], histo[MXGRAY], lim[MXGRAY], index[MXGRAY];
    int p, R = 0, Hsum = 0;

    // reset histograms
    for(i=0; i<MXGRAY; i++) {
        histo[i] = 0;
        lim[i] = 0;
        index[i] = 0;
    }

    ChannelPtr<uchar> p1, p2, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
      for(endd = p1 + total; p1<endd; p1++) { histo[*p1]++; }
    }

    for(i=0; i<MXGRAY; i++) {
        left[i] = R; // left end of interval
        lim[i] = lut[R] - Hsum;
        Hsum += histo[i];
        while(Hsum > lut[R] && R < MXGRAY - 1) {
            Hsum -= lut[R];
            R++;
        }
        if(left[i] != R) { index[i] = left[i]; }
        right[i] = R;
    }

    for(i=0; i<MXGRAY; i++) histo[i] = 0;

    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd; p1++, p2++) {
            p = left[*p1];
            if(histo[p] < lut[p]) {
                if(left[*p1] != right[*p1]) {
                    if(left[*p1] == index[*p1]) {
                        if(lim[*p1] > 0) {
                            *p2 = p;
                            lim[*p1] -= 1;
                        } else { *p2 = p = left[*p1] = MIN(p+1, right[*p1]); }
                    } else { *p2 = p; }
                } else { *p2 = p; }
            } else { *p2 = p = left[*p1] = MIN(p+1, right[*p1]); }

            histo[p]++;
        }
    }
}
