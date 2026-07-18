#pragma once
#include"compressionalgorithm.h"
#include <opencv2/opencv.hpp>
#include<vector>
#include<stdint.h>

class lwz:public CompressionAlgorithm {
public:
    lwz() = default;
	cv::Mat compress(const cv::Mat& input) override;
	std::string algorithmname() const override { return "LWZ"; }
	double getCompression() const override { return compressratio; }

private:
	double compressratio = 1.0;
	
    struct entry
    {
        uint16_t prefix;
        uint8_t suffix;
    };
    
    std::vector<entry> dictionary;
    std::vector<uint16_t> compressdata;
};