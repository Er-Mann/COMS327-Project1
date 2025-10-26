#include <math.h>
#include "citydata.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384
#endif

static double d2r(double d){ return d * (M_PI / 180.0); }

double geoDist(double lat1, double lon1, double lat2, double lon2){  //haversine formula
    const double R = 6371000.0; /* meters */
    double p1 = d2r(lat1), p2 = d2r(lat2);
    double dlat = d2r(lat2 - lat1), dlon = d2r(lon2 - lon1);
    double a = sin(dlat/2)*sin(dlat/2) + cos(p1)*cos(p2)*sin(dlon/2)*sin(dlon/2);
    return 2 * R * atan2(sqrt(a), sqrt(1 - a));
}
