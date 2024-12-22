#ifndef METRICS_H
#define METRICS_H

#include <time.h>


typedef struct Metric {
    char name[50];
    char tag[100];
    double value;
    time_t timestamp;
} Metric;


void add_or_update_metric(const char *name, const char *tag, double value, time_t timestamp);
void format_metrics(char *output, size_t max_size);
void clear_metrics();

#endif
