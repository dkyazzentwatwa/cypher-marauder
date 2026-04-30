// flock_detector.h - Flock Camera Detector Module for Project Starbeam V2

#ifndef FLOCK_DETECTOR_H
#define FLOCK_DETECTOR_H

#include <Arduino.h>

class FlockDetector {
public:
    static void init();
    static void deinit();
    static void runDetector();
};

#endif // FLOCK_DETECTOR_H
