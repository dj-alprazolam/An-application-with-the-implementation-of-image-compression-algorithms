#include"lwz.h"
#include"opencv2/opencv.hpp"
#include<vector>
#include<string>
#include <cstring>

cv::Mat lwz::compress(const cv::Mat& input){
    compressdata.clear();
    dictionary.clear();

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

    uint16_t currentpre = data[0];

    for(size_t i = 1; i < totalbytes; ++i){
        uint8_t nextsuf = data[i];
        bool flag = false;
        int code = 0;
        for(size_t j = 0; j < dictionary.size(); ++j){
            if(dictionary[j].prefix == currentpre && dictionary[j].suffix == nextsuf){
                flag = true;
                code = j + 256;
                break;
            }
        }
        if(flag){
            currentpre = code;

        }else{
            compressdata.push_back(currentpre);
            if (dictionary.size() < 65535 - 256)
            {
                dictionary.push_back({currentpre,nextsuf});
            }

            currentpre = nextsuf;
            
        }

    }
    compressdata.push_back(currentpre);

    double originalSize = static_cast<double>(totalbytes);
    double compressedSize = static_cast<double>(compressdata.size() * sizeof(uint16_t));
    
    compressratio = (compressedSize > 0) ? (compressedSize / originalSize) : 1.0;

    std::vector<uint8_t> restoredData;
    restoredData.reserve(totalbytes);
    
    std::vector<std::vector<uint8_t>> decDictionary;

    if (!compressdata.empty()) {

        uint16_t firstCode = compressdata[0];
        std::vector<uint8_t> currentEntry = {static_cast<uint8_t>(firstCode)};
        restoredData.push_back(firstCode);

        for (size_t i = 1; i < compressdata.size(); ++i) {
            uint16_t code = compressdata[i];
            std::vector<uint8_t> entry;

            if (code < decDictionary.size() + 256) {

                entry = decDictionary[code - 256];
            } else if (code == decDictionary.size() + 256) {

                entry = currentEntry;
                entry.push_back(currentEntry[0]);
            }

            for (uint8_t b : entry) {
                restoredData.push_back(b);
            }


            if (decDictionary.size() < 65535 - 256) {
                std::vector<uint8_t> newEntry = currentEntry;
                newEntry.push_back(entry[0]);
                decDictionary.push_back(newEntry);
            }
            
            currentEntry = entry;
        }
    }

    cv::Mat restored(contInput.rows, contInput.cols, contInput.type());
    std::memcpy(restored.data, restoredData.data(), std::min(totalbytes, restoredData.size()));

    return restored.reshape(contInput.channels(), contInput.rows);

}

