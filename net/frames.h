#ifndef FRAMES_H
#define FRAMES_H

#include <stdint.h>
#include "requestactions.h"

typedef struct
{
    FrameType frameType;
    uint8_t actionType;
    uint32_t frameSize;
    uint16_t frameParameters;
    uint32_t data0;
    uint32_t data1;
}FrameHeader;

union FrameHeader_uni
{
    FrameHeader structData;
    char rawData[sizeof(FrameHeader)];
};

#endif // FRAMES_H
