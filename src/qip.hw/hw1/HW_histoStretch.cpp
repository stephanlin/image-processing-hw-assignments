// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_histoStretch:
//
// Apply histogram stretching to I1. Output is in I2.
// Stretch intensity values between t1 and t2 to fill the range [0,255].
//
// Written by: Weifan Lin & Kaimin Jiang, 2016
//
void
HW_histoStretch(ImagePtr I1, int t1, int t2, ImagePtr I2)
{
	IP_copyImageHeader(I1, I2);
  int w = I1->width();
  int h = I1->height();
  int total = w * h;

  int diff = 1;
  if(t1<t2) { diff = t2-t1; }

  // init lookup table
  int i, lut[MXGRAY];
  for(i=0; i<MXGRAY; ++i)
  lut[i] = CLIP((int)(MaxGray*(i-t1))/(diff), 0, MaxGray);

  // evaluate output: each input pixel indexes into lut[] to eval output
	int type;
	ChannelPtr<uchar> p1, p2, endd;
	for(int ch = 0; IP_getChannel(I1, ch, p1, type); ch++) {
		IP_getChannel(I2, ch, p2, type);
		for(endd = p1 + total; p1<endd;) *p2++ = lut[*p1++];
	}
}
