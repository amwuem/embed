#ifndef WMPROC_H
#define WMPROC_H

#include <vector>
#include <stdint.h>
#include <memory>

#include <opencv2/opencv.hpp>

class WmProc {
public:
    virtual ~WmProc() {}
    virtual void encode(const cv::Mat& img, cv::Mat &dstimg) = 0;
    virtual std::vector<uint8_t> decode(cv::Mat& img) = 0;
};

typedef std::unique_ptr<WmProc> WMProcPtr;

#endif // WMPROC_H
