// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_quantize:
//
// Quantize I1 to specified number of levels. Apply dither if flag is set.
// Output is in I2.
//
// Written by: Weifan Lin & Kaimin Jiang, 2016
//
void
HW_quantize(ImagePtr I1, int levels, bool dither, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);
	int w = I1->width ();
	int h = I1->height();
	int total = w * h;

  int scale = MXGRAY / levels;
  double bias = scale/2.0;

	// init lookup table
	int i, lut[MXGRAY];
	for(i=0; i<MXGRAY; ++i)
		lut[i] = CLIP(int(scale * (i/scale) + bias), 0, 255);

	int type;
	ChannelPtr<uchar> p1, p2, endd;

  if (!dither){ // if dither checkbox is unchecked
  	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
    	IP_getChannel(I2, ch, p2, type);
      for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
    }
  } else {
  	int pixelPoint = 0;
  	int k;
  	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
    	IP_getChannel(I2, ch, p2, type);
      for(endd = p1 + total; p1<endd; p1++, pixelPoint++) {
        int noise = (double)rand() / RAND_MAX * bias;
        if(pixelPoint % 2) { k = CLIP(*p1 - noise, 0, 255); }
        else 							 { k = CLIP(*p1 + noise, 0, 255); }
        *p2++ = lut[k];
      }
    }
  }
}
