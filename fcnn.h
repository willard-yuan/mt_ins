#ifndef CNNFEATURE_H
#define CNNFEATURE_H

#include <stdio.h>
#include <numeric>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

class CnnFeature {
public:
    CnnFeature(const int inputSize_, const std::string blobName_, const std::string modelBin_) {
        dnnNet = nullptr;
        netInputSize = inputSize_;
        blobName = blobName_;
        initModel(modelBin_);
    }

    ~CnnFeature() {
        delete dnnNet;
    }
    int computeFeat(const cv::Mat& img, std::vector<float> &ft);

private:
    int nomalizeVector(std::vector<float> &v, const int feature_dim);
    int initModel(const std::string& modelBin);

protected:
    int netInputSize;
    cv::dnn::Net* dnnNet;
    std::string blobName;
};

#endif // CNNFEATURE_H
