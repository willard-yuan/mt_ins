#include "fcnn.h"

int CnnFeature::nomalizeVector(std::vector<float> &v, const int feature_dim)
{
    if (int(v.size()) != feature_dim) return 0;
    std::vector<float> v_norm;
    float norm_v = sqrt(std::inner_product(v.begin(), v.end(), v.begin(), 0.0 ));
    float denorm_v = std::max(1e-12, (double)norm_v);
    for (auto it = v.begin(); it != v.end(); it++){
        *it = (*it)/denorm_v;
    }
    return 1;
}

int CnnFeature::initModel(const std::string& modelBin)
{
    std::ifstream modelBinFile(modelBin);
    if (!modelBinFile.good()) {
      return 0;
    }
    dnnNet = new cv::dnn::Net(cv::dnn::readNetFromONNX(modelBin));
    dnnNet->setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    cv::Mat img(netInputSize, netInputSize, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat inputBlob = cv::dnn::blobFromImage(img, 1.0f, cv::Size(netInputSize, netInputSize), cv::Scalar(0, 0, 0), false, false);
    dnnNet->setInput(inputBlob, "input");
    cv::Mat featMat = dnnNet->forward("output");
    return 1;
}

int CnnFeature::computeFeat(const cv::Mat& img, std::vector<float> &ft)
{
    if (img.empty()) {
      return 0;
    }
    bool swapRB = false;
    bool crop = false;
    ft.clear();
    cv::Mat imgResized;
    cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
    cv::resize(img, imgResized, cv::Size(288, 288));
    imgResized.convertTo(imgResized, CV_32FC3, 1.0f / 255.0f);
    cv::Scalar mean{0.485, 0.456, 0.406};
    cv::Scalar std{0.229, 0.224, 0.225};
    cv::Mat meanMat(288, 288, CV_32FC3, mean);
    imgResized = (imgResized - meanMat);
    if (std.val[0] != 0.0 && std.val[1] != 0.0 && std.val[2] != 0.0) {
      cv::divide(imgResized, std, imgResized);
    }
    cv::Mat inputBlob = cv::dnn::blobFromImage(imgResized, 1.0f, cv::Size(), cv::Scalar(0, 0, 0), swapRB, crop);
    dnnNet->setInput(inputBlob, "input");
    cv::Mat featMat = dnnNet->forward("output");

    for (int i = 0; i < featMat.cols; i++) {
        ft.push_back(featMat.at<float>(0, i));
    }
    //nomalizeVector(ft, (int)ft.size());
    return 1;
}


