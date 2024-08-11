#ifndef VIDEOPROCESSER_H
#define VIDEOPROCESSER_H

#include <QThread>

#include "wmfactory.h"

class VideoProcesser : public QThread {

    Q_OBJECT
public:
    VideoProcesser(const QString& outputdir,
                   const QString& source = "",
                   const QString& fourcc = "FFV1");
    void run() override;

signals:
    void errorOccured(int, QString);
    void progress(int);
    void finished();

private:
    void processWithOpenCV();
private:
    std::unique_ptr<WmProc> m_pCore;
    QString m_source;
    QString m_outputdir;
    QString m_fourcc;
    QString m_output;
    int m_maxFrame = -1;
};

#endif // VIDEOPROCESSER_H




