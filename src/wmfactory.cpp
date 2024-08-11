#include "wmfactory.h"

// #include "constants.h"
// #include "blindwm.h"
// #include "ndslwm.h"
// #include "ndslwm_v2.h"
// #include "./dftwater/dftwm.h"

// #include "ndslwm_v2.h"

// using namespace re;

using namespace cv;
using namespace std;


class NdslWmV2 : virtual public WmProc {
public:
    NdslWmV2(int procWidth, int procHeight, int wmBitlen, const std::string& secret);
    NdslWmV2(int procWidth, int procHeight, int wmBitlen, const std::vector<uint8_t>& wmBits);
    virtual ~NdslWmV2() {}
    virtual void encode(const cv::Mat& img, cv::Mat &dstimg) override;
    virtual std::vector<uint8_t> decode(cv::Mat& img) override;
private:
    std::vector<uint8_t> generateWmBits(const char* secret);
private:
    int procWidth;
    int procHeight;
    int wmBitlen;
    std::vector<uint8_t> wmBits;
    int bitmask;
};


WMProcPtr CreateWmInstance(const std::string& secret, WM_ALGO_TYPE type) {

    switch (type) {
    case NDSL_V2:
        return std::make_unique<NdslWmV2>(0, 0, 128, secret);
    // case BLIND:
    //     return std::make_unique<BlindWm>( DEAL_IMG_WIDTH, DEAL_IMG_HEIGHT, MAX_SEC_BUFF_LEN, secret);
    // case NDSL:
    //     return std::make_unique<NdslWm>(DEAL_IMG_WIDTH, DEAL_IMG_HEIGHT, MAX_SEC_BUFF_LEN, secret);
    // case NDSL_V2:
    //     return std::make_unique<NdslWmV2>(DEAL_IMG_WIDTH, DEAL_IMG_HEIGHT, MAX_SEC_BUFF_LEN, secret);
    // case DFT_NDSL:
    //     return std::make_unique<DFTWm>(secret); //DEAL_IMG_WIDTH, DEAL_IMG_HEIGHT, MAX_SEC_BUFF_LEN, wmbits);
    }

    return nullptr;
}

WMProcPtr CreateWmInstance(const std::vector<uint8_t>& wmbits, WM_ALGO_TYPE type) {

    switch (type) {
    case NDSL_V2:
        return std::make_unique<NdslWmV2>(0, 0, 128, wmbits);

    // case BLIND:
    //     return std::make_unique<BlindWm>(DEAL_IMG_WIDTH, DEAL_IMG_HEIGHT, MAX_SEC_BUFF_LEN, wmbits);
    // case NDSL:
    //     return std::make_unique<NdslWm>(DEAL_IMG_WIDTH, DEAL_IMG_HEIGHT, MAX_SEC_BUFF_LEN, wmbits);
    // case NDSL_V2:
    //     return std::make_unique<NdslWmV2>(DEAL_IMG_WIDTH, DEAL_IMG_HEIGHT, MAX_SEC_BUFF_LEN, wmbits);
    // case DFT_NDSL:
    //     return std::make_unique<DFTWm>(std::string(wmbits.begin(), wmbits.end())); //DEAL_IMG_WIDTH, DEAL_IMG_HEIGHT, MAX_SEC_BUFF_LEN, wmbits);
    }

    return nullptr;
}




NdslWmV2::NdslWmV2(int procWidth, int procHeight, int wmBitlen, const std::string &secret)
    : procWidth(procWidth), procHeight(procHeight), bitmask(8)
{

    this->wmBitlen = wmBitlen;
    wmBits = std::vector<uint8_t>(wmBitlen, 0);
    if (!secret.empty()) {
        auto gen = generateWmBits(secret.data());
        std::cout << "generate bits size: " << gen.size() << std::endl;
        memcpy(wmBits.data(), gen.data(), gen.size());
    }
}

NdslWmV2::NdslWmV2(int procWidth, int procHeight, int wmBitlen, const std::vector<uint8_t> &wmBits)
    : procWidth(procWidth), procHeight(procHeight), bitmask(8)
{
    this->wmBitlen = wmBitlen;
    this->wmBits = std::vector<uint8_t>(wmBitlen, 0);
    memcpy(this->wmBits.data(), wmBits.data(), wmBits.size());
}

void NdslWmV2::encode(const cv::Mat &img, cv::Mat &dstimg)
{
    dstimg = img.clone();

    int rows = dstimg.rows;
    int cols = dstimg.cols;

    int r = rows - 1;
    std::cout << "add marker code"<< std::endl;
    for (int col = 0; col < wmBits.size(); ++col)
    {
        if (wmBits[col] == 0)
        {
            dstimg.at<cv::Vec3b>(r, col) = cv::Vec3b(0, 0, 0);
            std::cout << "0";
        }
        else
        {
            dstimg.at<cv::Vec3b>(r, col) = cv::Vec3b(50, 50, 50);
            std::cout << "1";
        }
    }

    std::cout << std::endl;
    std::cout << "----------------------------------------------------" <<std::endl;

    int r2 = rows - 2;
    int add_mask = cols / bitmask;
    for (int col = 0; col < add_mask; ++col) {
        for (int i = 0; i < bitmask; ++i) {
            if (col%2 == 0) {
                dstimg.at<cv::Vec3b>(r2, col*bitmask+i) = cv::Vec3b(0, 0, 0);
                std::cout << "0";
            } else {
                dstimg.at<cv::Vec3b>(r2, col*bitmask+i) = cv::Vec3b(50, 50, 50);
                std::cout << "1";
            }
        }
    }

    std::cout << std::endl;


    // int dstcol = cols - cols%8;
    // for (int col = 0; col < dstcol; ++col)
    // {
    //   // uchar pixelValue = gray_mat.at<uchar>(r2, col);
    //   uchar pixelValue = img.at<cv::Vec3b>(r2, col)(0);
    //   std::cout << (int)pixelValue;
    // }

}

std::vector<uint8_t> NdslWmV2::decode(cv::Mat &img)
{
    /*int cols = dsting.cols;
  for (int col = 0; col < cols; ++col) {
    cv::Vec3b color = saved_image.at<cv::Vec3b>(rows - 1, col);
  }*/

    cv::Mat gray_mat;
    std::vector<uint8_t> code(wmBitlen, 0);
    cv::cvtColor(img, gray_mat, cv::COLOR_BGR2GRAY);
    int i = gray_mat.rows - 1;

    for (int j = 0; j < wmBitlen && j < gray_mat.cols; ++j)
    {
        uchar pixelValue = gray_mat.at<uchar>(i, j);
        if (pixelValue > 25)
        {
            std::cout << "1";
            code[j] = 1;
        }
        else
        {
            std::cout << "0";
            code[j] = 0;
        }
    }
    std::cout << std::endl;

    // auto extract_str = BinaryUtils::binArrayToUtf8Str(code);
    // std::cout << "-----------" << std::endl;
    // std::cout << ((char*)extract_str.data()) << std::endl;

    int dstcol = gray_mat.cols - gray_mat.cols%8;
    // std::cout << "r-3" << std::endl;
    // int r3 = gray_mat.rows - 3;
    // for (int col = 0; col < dstcol; ++col)
    // {
    //   uchar pixelValue = gray_mat.at<uchar>(r3, col);
    //     std::cout << "0";

    //   if (pixelValue > 128)
    //   {
    //     std::cout << "1";
    //     // code[j] = 1;
    //   }
    //   else
    //   {
    //     std::cout << "0";
    //     // code[j] = 0;
    //   }
    // }
    // std::cout << std::endl;

    std::cout << "+++++++++++++++++++++++++++++" << std::endl;

    std::cout << "r-2" << std::endl;
    int r2 = gray_mat.rows - 2;
    for (int col = 0; col < dstcol; ++col)
    {
        uchar pixelValue = gray_mat.at<uchar>(r2, col);
        std::cout << (int)pixelValue << " ";
        // if (pixelValue > 128)
        // {
        //   std::cout << "1";
        //   // code[j] = 1;
        // }
        // else
        // {
        //   std::cout << "0";
        //   // code[j] = 0;
        // }
    }
    std::cout << std::endl;

    std::cout << "+++++++++++++++++++++++++++++" << std::endl;

    std::cout << "r-1" << std::endl;
    int r1 = gray_mat.rows - 1;
    for (int col = 0; col < dstcol; ++col)
    {
        uchar pixelValue = gray_mat.at<uchar>(r1, col);
        std::cout << (int)pixelValue << " ";
        // if (pixelValue > 128)
        // {
        //   std::cout << "1";
        //   // code[j] = 1;
        // }
        // else
        // {
        //   std::cout << "0";
        //   // code[j] = 0;
        // }
    }
    std::cout << std::endl;

    std::cout << "+++++++++++++++++++++++++++++" << std::endl;

    return code;
}

std::vector<uint8_t> NdslWmV2::generateWmBits(const char *secret)
{
    // return BinaryUtils::utf8StrToBinArray(secret);
    return std::vector<uint8_t>(0, 100);
}
