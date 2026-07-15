#include"rle.h"
#include<vector>
#include<opencv2/opencv.hpp>
#include<string>

cv::Mat rle::compress(const cv::Mat& input){
    compressData.clear();

    if(input.empty()){
        compressratio = 1.0;
        return cv::Mat();
    }
    cv::Mat contInput = input.isContinuous() ? input : input.clone();

    size_t totalbytes = contInput.total() * contInput.elemSize();
    const uint8_t* data = contInput.ptr<uint8_t>(0);

    if(totalbytes == 0){
        compressratio = 1.0;
        return cv::Mat();
    }

    compressData.reserve(totalbytes/2);
    
    uint16_t currcount = 1;
    uint8_t currValue = data[0];
    for (size_t i = 1; i < totalbytes; ++i)
    {
        uint8_t nextValue = data[i];
        if(nextValue == currValue && currcount < 65535){
            currcount++;
        }else{
            compressData.push_back({currcount,currValue});
            currValue = nextValue;
            currcount = 1;
        }
    }
    compressData.push_back({currcount,currValue});
    

    double originalSize = static_cast<double>(totalbytes);
    double compressedSize = static_cast<double>(compressData.size() * sizeof(rleznach));
    
    compressratio = (compressedSize > 0) ? (compressedSize / originalSize) : 1.0;

    cv::Mat restored = contInput.clone();
    uint8_t* outData = restored.ptr<uint8_t>(0);
    size_t pos = 0;

    for (const auto& pair : compressData) {
        std::memset(outData + pos, pair.value, pair.count);
        pos += pair.count;
    }


    return restored.reshape(contInput.channels(), contInput.rows);
}

