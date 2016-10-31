void gammaCorrect(ImagePtr, double, ImagePtr);
void copyRowToBuffer(ChannelPtr<uchar>, int, short*, int);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_errDiffusion:
//
// Apply error diffusion algorithm to image I1.
//
// This procedure produces a black-and-white dithered version of I1.
// Each pixel is visited and if it + any error that has been diffused to it
// is greater than the threshold, the output pixel is white, otherwise it is black.
// The difference between this new value of the pixel from what it used to be
// (somewhere in between black and white) is diffused to the surrounding pixel
// intensities using different weighting systems.
//
// Use Floyd-Steinberg     weights if method=0.
// Use Jarvis-Judice-Ninke weights if method=1.
//
// Use raster scan (left-to-right) if serpentine=0.
// Use serpentine order (alternating left-to-right and right-to-left) if serpentine=1.
// Serpentine scan prevents errors from always being diffused in the same direction.
//
// A circular buffer is used to pad the edges of the image.
// Since a pixel + its error can exceed the 255 limit of uchar, shorts are used.
//
// Apply gamma correction to I1 prior to error diffusion.
// Output is saved in I2.
//
void
HW_errDiffusion(ImagePtr I1, int method, bool serpentine, double gamma, ImagePtr I2)
{
    ImagePtr Ig;
    IP_copyImageHeader(I1, I2);
    IP_copyImageHeader(I1, Ig);
    gammaCorrect(I1, gamma, Ig);

    int w = I1->width();
    int h = I1->height();

    int thr = MXGRAY/2;
    int type;
    ChannelPtr<uchar> p1, p2, endd;

    if(method == 0) { // Use Floyd-Steinberg
        short* in1;
        short* in2;
        short e;

        int bufSz = w + 2;
        short* buffer0 = new short[bufSz]; // top buffer
        short* buffer1 = new short[bufSz]; // buttom buffer

        for(int ch = 0; IP_getChannel(Ig, ch, p1, type); ch++) {
            IP_getChannel(I2, ch, p2, type);

            // copy first row to top buffer
            copyRowToBuffer(p1, w, buffer0, bufSz);
            p1 = p1 + w;

            for(int y=1; y<h; y++) {
                if(serpentine) { // serpentine scan
                    if (y % 2 == 0) { // on even rows
                        copyRowToBuffer(p1, w, buffer0, bufSz);
                        p1 = p1 + w;
                        in1 = buffer1 + w;
                        in2 = buffer0 + w;
                        p2 = p2 + w - 1;

                        for(int x=0; x<w; x++) {
                            *p2 = (*in1 < thr) ? 0 : 255;
                            e = *in1 - *p2;
                            in1[-1] += (e*7/16.0);
                            in2[1] += (e*3/16.0);
                            in2[0 ] += (e*5/16.0);
                            in2[-1] += (e*1/16.0);

                            in1--;
                            in2--;
                            p2--;
                        }
                        p2 = p2 + w + 1;

                    } else { // on add rows
                        copyRowToBuffer(p1, w, buffer1, bufSz);
                        p1 = p1 + w;
                        in1 = buffer0+1;
                        in2 = buffer1+1;

                        for(int x=0; x<w; x++) {
                            *p2 = (*in1 < thr) ? 0 : 255;
                            e = *in1 - *p2;
                            in1[1 ] += (e*7/16.0);
                            in2[-1] += (e*3/16.0);
                            in2[0 ] += (e*5/16.0);
                            in2[1 ] += (e*1/16.0);

                            in1++;
                            in2++;
                            p2++;
                        }
                    }
                } else { // raster scan
                    if (y % 2 == 0) {
                        copyRowToBuffer(p1, w, buffer0, bufSz);
                        in1 = buffer1+1; // +1 to skip the pad
                        in2 = buffer0+1;
                    } else {
                        copyRowToBuffer(p1, w, buffer1, bufSz);
                        in1 = buffer0+1;
                        in2 = buffer1+1;
                    }
                    p1 = p1 + w;

                    for(int x=0; x<w; x++) {
                        *p2 = (*in1 < thr) ? 0 : 255;
                        e = *in1 - *p2;
                        in1[1 ] += (e*7/16.0);
                        in2[-1] += (e*3/16.0);
                        in2[0 ] += (e*5/16.0);
                        in2[1 ] += (e*1/16.0);

                        in1++;
                        in2++;
                        p2++;
                    }
                }
            }
        }
        delete [] buffer0;
        delete [] buffer1;
    } else { // Use Jarvis-Judice-Ninke

    }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// gammaCorrect:
//
// Apply gamma correction to image I1.
// Save result in I2.
//
void
gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
    IP_copyImageHeader(I1, I2);
    int w = I1->width ();
    int h = I1->height();
    int total = w * h;

    gamma = 1.0 / gamma;

    // init lookup table
    int i, lut[MXGRAY];
    for(i=0; i<MXGRAY; ++i)
        lut[i] = (int)CLIP((MaxGray * (pow((double) i / MaxGray, gamma))), 0, MaxGray);

    // evaluate output: each input pixel indexes into lut[] to eval output
    int type;
    ChannelPtr<uchar> p1, p2, endd;
    for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
        IP_getChannel(I2, ch, p2, type);
        for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
    }
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// copyRowToBuffer:
//
// copy a row of pixels to buffer with padding size of (bufSz - w) / 2
//
void
copyRowToBuffer(ChannelPtr<uchar> p1, int width, short* buffer, int bufSz) {
    int padSz = (bufSz-width)/2; // padding size
    for (int i = 0          ; i < padSz      ; i++) buffer[i] = *p1; // left padding
    for (int i = padSz      ; i < bufSz-padSz; i++) buffer[i] = *p1++;
    for (int i = bufSz-padSz; i < bufSz      ; i++) buffer[i] = *p1; // right padding
}






