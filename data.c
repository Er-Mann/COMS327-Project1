#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "data.h" 
//POI -> id    name    lat    lon

//Roads -> fromId    toId    length    latStart    lonStart    roadName


// split by tabs (\t)
static int split_fields(char *line, char *fields[], int max_fields) {
    int count = 0;
    char *p = line;
    while (*p && count < max_fields) {
        while (*p == '\t' || *p == '\n' || *p == '\r') p++; // skip blanks
        if (!*p) break;
        fields[count++] = p;
        while (*p && *p != '\t' && *p != '\n' && *p != '\r') p++; // move to next
        if (*p) { *p = '\0'; p++; }
    }
    return count;
}

// quick digit check
static int is_digits_only(const char *s) {
    if (*s == '\0') return 0;
    for (const unsigned char *p = (const unsigned char*)s; *p; ++p) {
        if (!isdigit(*p)) return 0;
    }
    return 1;
}

// parse ints safely
static int parse_long(const char *s, long *out) {
    char *end;
    if (*s == '\0') return 0;
    long v = strtol(s, &end, 10);
    if (*end != '\0') return 0;
    *out = v;
    return 1;
}

// parse doubles (allow NaN)
static int parse_double_allow_nan(const char *s, double *out) {
    if (strcasecmp(s, "NaN") == 0) { *out = 0.0; return 1; }
    char *end;
    double v = strtod(s, &end);
    if (*end != '\0') return 0;
    *out = v;
    return 1;
}

static int valid_lat(double lat) { return lat >= -90 && lat <= 90; }
static int valid_lon(double lon) { return lon > -180 && lon < 180; }

int validate(void) {
    char buf[4096];
    long line = 0;

    // read POI count
    if (!fgets(buf, sizeof buf, stdin)) return 1; // file empty
    line++;
    buf[strcspn(buf, "\r\n")] = '\0';

    long poi_count;
    char *f[4];
    if (split_fields(buf, f, 4) != 1 || !parse_long(f[0], &poi_count) || poi_count <= 0) {
        return line;
    }

    // read each POI 
    for (long i = 0; i < poi_count; i++) {
        if (!fgets(buf, sizeof buf, stdin)) return line + 1;
        line++;
        buf[strcspn(buf, "\r\n")] = '\0';

        char *fields[6];
        int n = split_fields(buf, fields, 6);
        if (n != 4) return line;

        if (!is_digits_only(fields[0])) return line;
        if (fields[1][0] == '\0') return line; // no blank name

        double lat, lon;
        if (!parse_double_allow_nan(fields[2], &lat)) return line;
        if (!parse_double_allow_nan(fields[3], &lon)) return line;
        if (!valid_lat(lat) || !valid_lon(lon)) return line;
    }

    // roads section
    if (!fgets(buf, sizeof buf, stdin)) return line + 1;
    line++;
    buf[strcspn(buf, "\r\n")] = '\0';

    long road_count;
    if (split_fields(buf, f, 4) != 1 || !parse_long(f[0], &road_count) || road_count <= 0) {
        return line;
    }

    for (long i = 0; i < road_count; i++) {
        if (!fgets(buf, sizeof buf, stdin)) return line + 1;
        line++;
        buf[strcspn(buf, "\r\n")] = '\0';

        char *fields[8];
        int n = split_fields(buf, fields, 8);
        if (n != 6) return line;

        if (!is_digits_only(fields[0]) || !is_digits_only(fields[1])) return line;

        double len, lat, lon;
        if (!parse_double_allow_nan(fields[2], &len)) return line;
        if (!parse_double_allow_nan(fields[3], &lat)) return line;
        if (!parse_double_allow_nan(fields[4], &lon)) return line;
        if (!valid_lat(lat) || !valid_lon(lon)) return line;

        if (fields[5][0] == '\0') return line; // no empty road names!
    }

    // i guess it’s valid
    return 0;
}