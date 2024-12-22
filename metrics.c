#include "metrics.h"
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct MetricNode {
    Metric metric;
    struct MetricNode *next;
} MetricNode;

static MetricNode *metrics_head = NULL;
static pthread_mutex_t metrics_mutex = PTHREAD_MUTEX_INITIALIZER;


void add_or_update_metric(const char *name, const char *tag, double value, time_t timestamp) {
    pthread_mutex_lock(&metrics_mutex);

    MetricNode *current = metrics_head;
    while (current != NULL) {
        if (strcmp(current->metric.name, name) == 0 &&
            strcmp(current->metric.tag, tag) == 0) {
            // Update existing metric
            current->metric.value = value;
            current->metric.timestamp = timestamp;
            pthread_mutex_unlock(&metrics_mutex);
            return;
        }
        current = current->next;
    }

   
    MetricNode *new_node = (MetricNode *)malloc(sizeof(MetricNode));
    if (new_node == NULL) {
        fprintf(stderr, "Failed to allocate memory for new metric.\n");
        pthread_mutex_unlock(&metrics_mutex);
        return;
    }

    strncpy(new_node->metric.name, name, sizeof(new_node->metric.name));
    strncpy(new_node->metric.tag, tag, sizeof(new_node->metric.tag));
    new_node->metric.value = value;
    new_node->metric.timestamp = timestamp;
    new_node->next = metrics_head;
    metrics_head = new_node;

    pthread_mutex_unlock(&metrics_mutex);
}


void format_metrics(char *output, size_t max_size) {
    pthread_mutex_lock(&metrics_mutex);

    MetricNode *current = metrics_head;
    size_t offset = 0;

    while (current != NULL) {
        int written = snprintf(
            output + offset, max_size - offset,
            "%s{tag=\"%s\"} %.2f %ld\n",
            current->metric.name,
            current->metric.tag,
            current->metric.value,
            current->metric.timestamp
        );
        if (written < 0 || (size_t)written >= max_size - offset) {
            break;  
        }
        offset += written;
        current = current->next;
    }

    pthread_mutex_unlock(&metrics_mutex);
}


void clear_metrics() {
    pthread_mutex_lock(&metrics_mutex);

    MetricNode *current = metrics_head;
    while (current != NULL) {
        MetricNode *to_free = current;
        current = current->next;
        free(to_free);
    }
    metrics_head = NULL;

    pthread_mutex_unlock(&metrics_mutex);
}
