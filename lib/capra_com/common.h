#pragma once

#ifdef ARDUINO
    #include <Arduino.h>
#endif


// Define types with known lengths for encoding
#ifdef ARDUINO
using euint8_t  = uint8_t;
using eint8_t   = int8_t;
using euint16_t = uint16_t;
using eint16_t  = int16_t;
using euint32_t = uint32_t;
using eint32_t  = int32_t;
using euint64_t = uint64_t;
using eint64_t  = int64_t;
using efloat    = float;
using eboolean  = boolean;
#endif


// Verify type lengths
static_assert(sizeof(uint8_t) == 1);

static_assert(sizeof(euint8_t)   == 1);
static_assert(sizeof(eint8_t)    == 1);
static_assert(sizeof(euint16_t)  == 2);
static_assert(sizeof(eint16_t)   == 2);
static_assert(sizeof(euint32_t)  == 4);
static_assert(sizeof(eint32_t)   == 4);
static_assert(sizeof(euint64_t)  == 8);
static_assert(sizeof(eint64_t)   == 8);
static_assert(sizeof(efloat)     == 4);
static_assert(sizeof(eboolean)   == 1);