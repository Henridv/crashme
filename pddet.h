#ifndef __PDDET_H__
#define __PDDET_H__


struct v
{
    long n;
    double *a;
};

struct apair
{
    const char *name;
    unsigned long addr;
};

struct adelt
{
    long delta;
    long count;
};

struct v *vcons(long n, double x);
struct v *vprod(struct v *x, struct v *y);
double vsume(struct v *x);
double viprod(struct v *x, struct v *y);
double vnorm(struct v *x);

void sayarch(const char *x);
int apairl(const void *a, const void *b);
int adeltl(const void *a, const void *b);

double case10(void);
double case20(void);
double case30(void);
double case40(void);
double case50(void);

#endif /* __PDDET_H__ */
