#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <bitset>
#include <QDebug>
#include <QFileInfo>

#include "videoprocesser.h"

using namespace cv;
using namespace std;

VideoProcesser::VideoProcesser(const QString& outputdir,
                               const QString& source,
                               const QString& fourcc)
    : m_outputdir(outputdir)
    , m_source(source)
    , m_fourcc(fourcc){
    m_pCore = CreateWmInstance("");
    // 创建 QFileInfo 对象
    QFileInfo fileInfo(source);
    // // 获取文件名（包括后缀）
    // QString fileNameWithSuffix = fileInfo.fileName();

    // 获取无后缀的文件名
    QString fileName = fileInfo.baseName();
    m_output = QString().append(outputdir).append("/").append(fileName).append(".avi");
}

void VideoProcesser::processWithOpenCV() {
    cv::setNumThreads(7);

    // 打开视频文件
    VideoCapture cap(m_source.toStdString());
    if (!cap.isOpened()) {
        cerr << "Error opening video file" << endl;
        return;
    }

    // 获取视频的帧率
    double fps = cap.get(CAP_PROP_FPS);

    // 获取视频的宽度和高度
    int width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
    int height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);

    // 输出视频信息
    qDebug() << "Video resolution: " << width << "x" << height;
    qDebug() << "FPS: " << fps;

    int fourcc = cv::VideoWriter::fourcc(m_fourcc.at(0).toLatin1(), m_fourcc.at(1).toLatin1(), m_fourcc.at(2).toLatin1(), m_fourcc.at(3).toLatin1());
    // ".mp4"
    //     fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    // ".avi"
    //     fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    //     fourcc = cv::VideoWriter::fourcc('F', 'F', 'V', '1');
    //     fourcc = cv::VideoWriter::fourcc('U', 'Y', 'V', 'Y');

    // 创建 VideoWriter 对象，用于保存处理后的视频
    cv::VideoWriter writer(m_output.toStdString(), fourcc, fps, cv::Size(width, height));
    if (!writer.isOpened()) {
        qDebug() << "Failed to open VideoWriter.";
        return;
    }

    int idx = 0;
    // 逐帧读取和处理视频
    Mat frame;
    while (cap.read(frame)) {

        Mat dstFrame;
        m_pCore->encode(frame, dstFrame);

        // 将处理后的帧写入视频文件
        writer.write(dstFrame);

        if (m_maxFrame > 0 && ++idx >= m_maxFrame) {
            break;
        }
    }

    // 关闭视频文件
    cap.release();
    writer.release();
}

void VideoProcesser::run() {
    processWithOpenCV();
}


