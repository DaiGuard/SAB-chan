#ifndef RECORD_DATA_H_
#define RECORD_DATA_H_

typedef struct record_data {
    uint64_t time;
    float data[12];
} RecordData;

#endif