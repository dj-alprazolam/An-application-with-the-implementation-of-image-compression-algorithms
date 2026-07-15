#pragma once

#include <opencv2/opencv.hpp>
#include<string>

class CompressionAlgorithm {
public:
    CompressionAlgorithm() = default;
	virtual ~CompressionAlgorithm() = default; 
	virtual cv::Mat compress(const cv::Mat& input) = 0;
	virtual std::string algorithmname() const = 0; 
	virtual double getCompression() const = 0; 
};