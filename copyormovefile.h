#ifndef COPYORMOVEFILE_H
#define COPYORMOVEFILE_H

#include <QFile>
#include <QThread>
#include <QRunnable>
#include <QStorageInfo>

class copyOrMoveFile : public QThread, public QRunnable
{
    Q_OBJECT
public:
    enum op { move=0, copy=1 };

    explicit copyOrMoveFile(QString fileName, QString newName, op operation) ;
    virtual ~copyOrMoveFile();
    virtual void run() override;

    void setChunkSize(int kiloBytes) { m_chunkSize = kiloBytes * 1024; }
signals:
    void progress(int value);
    void failed(QString msg);
public slots:

private:
    void finish();
    void copyFile();
    void moveFile();
    void reportProgress();

    bool enoughSpace();
    bool onSameVolume();
    bool valid();
    bool doCopy();

    int m_progress;
    int m_lastReport;

    bool m_sameVolume;
    bool m_valid;

    long long m_bytesToCopy;
    long long m_bytesCopied;
    long long m_chunkSize=2097152; // Default to 2MB

    QString m_fileName;
    QString m_newName;

    QFile * m_qFileSource;
    QFile * m_qFileDestination;

    QStorageInfo * m_srcStorageInfo;
    QStorageInfo * m_dstStorageInfo;

    op m_operation;


};

#endif // COPYORMOVEFILE_H
