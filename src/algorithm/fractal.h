#pragma once

#include "compressionalgorithm.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

struct FractalTransform {
    int domainX;
    int domainY;
    int transformId; 
    double scale;
    double offset;
};

class Fractal : public CompressionAlgorithm {
public:
    Fractal(int rangeSize = 4, int domainStep = 4);
    cv::Mat compress(const cv::Mat& input) override;
    std::string algorithmname() const override { return "Fractal"; }
    double getCompression() const override { return compressionRatio; }

private:
    int rangeSize;
    int domainSize;
    int domainStep;
    double compressionRatio;

    cv::Mat encode(const cv::Mat& grayImage);
    cv::Mat decode(const cv::Mat& originalSizeImage, const std::vector<FractalTransform>& transforms);
    

    cv::Mat applyIsometry(const cv::Mat& block, int transformId);
};