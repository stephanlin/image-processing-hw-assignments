extern void HW_blur  (ImagePtr, int, int, ImagePtr);
extern void HW_blur1D(ChannelPtr<uchar>, int, int, int, ChannelPtr<uchar>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// HW_sharpen:
//
// Sharpen image I1. Output is in I2.
//
void
HW_sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
}
