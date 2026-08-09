#include "fractal.h"
#include <cmath>
#include <algorithm>

Fractal::Fractal(int rangeSize, int domainStep) 
    : rangeSize(rangeSize), domainSize(rangeSize * 2), domainStep(domainStep), compressionRatio(1.0) {}

cv::Mat Fractal::applyIsometry(const cv::Mat& block, int transformId) {
    cv::Mat result;
    switch (transformId) {
        case 0: result = block; break; 
        case 1: cv::rotate(block, result, cv::ROTATE_90_CLOCKWISE); break;
        case 2: cv::rotate(block, result, cv::ROTATE_180); break;
        case 3: cv::rotate(block, result, cv::ROTATE_90_COUNTERCLOCKWISE); break;
        case 4: cv::flip(block, result, 1); break; 
        case 5: cv::flip(block, result, 0); break; 
        case 6: { 
            cv::Mat flipped;
            cv::flip(block, flipped, 1);
            cv::rotate(flipped, result, cv::ROTATE_90_CLOCKWISE);
            break;
        }
        case 7: { 
            cv::Mat flipped;
            cv::flip(block, flipped, 1);
            cv::rotate(flipped, result, cv::ROTATE_90_COUNTERCLOCKWISE);
            break;
        }
    }
    return result.clone();
}

cv::Mat Fractal::encode(const cv::Mat& grayImage) {
    std::vector<FractalTransform> transforms;
    
    int rows = grayImage.rows;
    int cols = grayImage.cols;

    std::vector<cv::Mat> shrunkDomains;
    std::vector<std::pair<int, int>> domainCoords;

    for (int y = 0; y <= rows - domainSize; y += domainStep) {
        for (int x = 0; x <= cols - domainSize; x += domainStep) {
            cv::Mat domainBlock = grayImage(cv::Rect(x, y, domainSize, domainSize));
            cv::Mat shrunk;

            cv::resize(domainBlock, shrunk, cv::Size(rangeSize, rangeSize), 0, 0, cv::INTER_AREA);
            shrunk.convertTo(shrunk, CV_64F); 
            shrunkDomains.push_back(shrunk);
            domainCoords.push_back({x, y});
        }
    }

    int numDomains = shrunkDomains.size();
    int totalRanges = (rows / rangeSize) * (cols / rangeSize);
    transforms.reserve(totalRanges);


    for (int ry = 0; ry <= rows - rangeSize; ry += rangeSize) {
        for (int rx = 0; rx <= cols - rangeSize; rx += rangeSize) {
            cv::Mat rangeBlock = grayImage(cv::Rect(rx, ry, rangeSize, rangeSize));
            cv::Mat rDouble;
            rangeBlock.convertTo(rDouble, CV_64F);

            double bestError = 1e20;
            FractalTransform bestTransform;

            double rMean = cv::mean(rDouble)[0];
            cv::Mat rCentered = rDouble - rMean;

            for (int d = 0; d < numDomains; ++d) {
                cv::Mat dBlock = shrunkDomains[d];
                
                for (int t = 0; t < 8; ++t) {
                    cv::Mat dTrans = applyIsometry(dBlock, t);
                    double dMean = cv::mean(dTrans)[0];
                    cv::Mat dCentered = dTrans - dMean;

                    // Метод наименьших квадратов для поиска scale
                    double num = cv::sum(rCentered.mul(dCentered))[0];
                    double den = cv::sum(dCentered.mul(dCentered))[0];
                    
                    double s = 0.0;
                    if (den > 1e-10) {
                        s = num / den;
                    }

                    s = std::max(-0.9, std::min(0.9, s));
                    double o = rMean - s * dMean;

                    cv::Mat diff = rDouble - (s * dTrans + o);
                    double error = cv::sum(diff.mul(diff))[0];

                    if (error < bestError) {
                        bestError = error;
                        bestTransform.domainX = domainCoords[d].first;
                        bestTransform.domainY = domainCoords[d].second;
                        bestTransform.transformId = t;
                        bestTransform.scale = s;
                        bestTransform.offset = o;
                    }
                }
            }
            transforms.push_back(bestTransform);
        }
    }


    double origSize = rows * cols;
    double compSize = transforms.size() * (sizeof(int)*3 + sizeof(double)*2);
    compressionRatio = (origSize > 0) ? (compSize / origSize) : 1.0;


    cv::Mat dummyImage(rows, cols, CV_8UC1, cv::Scalar(128));
    return decode(dummyImage, transforms);
}

cv::Mat Fractal::decode(const cv::Mat& originalSizeImage, const std::vector<FractalTransform>& transforms) {
    cv::Mat current = originalSizeImage.clone();
    current.convertTo(current, CV_64F);

    int rows = current.rows;
    int cols = current.cols;
    int iterations = 10; 

    for (int iter = 0; iter < iterations; ++iter) {
        cv::Mat nextImg(rows, cols, CV_64F);
        
        int idx = 0;
        for (int ry = 0; ry <= rows - rangeSize; ry += rangeSize) {
            for (int rx = 0; rx <= cols - rangeSize; rx += rangeSize) {
                const FractalTransform& tr = transforms[idx++];
                
                cv::Mat domBlock = current(cv::Rect(tr.domainX, tr.domainY, domainSize, domainSize));
                cv::Mat shrunkDom;
                cv::resize(domBlock, shrunkDom, cv::Size(rangeSize, rangeSize), 0, 0, cv::INTER_AREA);
                
                cv::Mat transformed = applyIsometry(shrunkDom, tr.transformId);
                cv::Mat reconstructed = tr.scale * transformed + tr.offset;
                
                reconstructed.copyTo(nextImg(cv::Rect(rx, ry, rangeSize, rangeSize)));
            }
        }
        current = nextImg;
    }

    cv::Mat result;
    current.convertTo(result, CV_8UC1);
    return result;
}

cv::Mat Fractal::compress(const cv::Mat& input) {
    if (input.empty()) {
        compressionRatio = 1.0;
        return cv::Mat();
    }


    cv::Mat gray;
    if (input.channels() == 3) {
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = input.clone();
    }

    int padRows = (rangeSize - (gray.rows % rangeSize)) % rangeSize;
    int padCols = (rangeSize - (gray.cols % rangeSize)) % rangeSize;
    cv::Mat padded;
    cv::copyMakeBorder(gray, padded, 0, padRows, 0, padCols, cv::BORDER_REPLICATE);

    cv::Mat reconstructedPadded = encode(padded);

    return reconstructedPadded(cv::Rect(0, 0, gray.cols, gray.rows)).clone();
}