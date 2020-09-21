
// MT, 2017jan24

#ifndef MT_DIM
#define MT_DIM

#ifdef __cplusplus
extern "C" {
#endif

struct Dim
{
    int w;
    int h;
};

double Dim_getAspectRatio(struct Dim const * const inDim);
struct Dim Dim_getScaledInto(struct Dim const * const inOuter, struct Dim const * const inInner);

#ifdef __cplusplus
}
#endif

#endif // MT_DIM
