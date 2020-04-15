#include "copyormovefile.h"

#include <iostream>
#include <fstream>

#include <QApplication>
#include <QDir>

copyOrMoveFile::copyOrMoveFile(QString fileName, QString newName, op operation) : QThread() , QRunnable() {
    m_qFileSource = new QFile(fileName);
    m_qFileDestination = new QFile(newName);

    QFileInfo srcInfo(m_qFileSource->fileName());
    QString srcPath =srcInfo.absolutePath();
    m_srcStorageInfo = new QStorageInfo(srcPath);

    QFileInfo dstInfo(m_qFileDestination->fileName());
    QString dstPath =dstInfo.absolutePath();
    m_dstStorageInfo = new QStorageInfo(dstPath);


    m_fileName=fileName;
    m_newName=newName;

    m_operation=operation;
}
copyOrMoveFile::~copyOrMoveFile(){
    m_qFileSource->deleteLater();
    m_qFileDestination->deleteLater();
    delete m_srcStorageInfo;
    delete m_dstStorageInfo;
}
void copyOrMoveFile::run(){

    if (valid()) {
        switch (m_operation) {
            case copy: copyFile();  break;
            case move: moveFile();  break;
        default: break; //cannot get here
        }
    }
}
bool copyOrMoveFile::valid(){
    bool result=true;

    result=m_qFileSource->exists();
    if (!result) emit failed("Source file doesn't exist");

    m_valid=result;
    return result;
}
void copyOrMoveFile::copyFile(){
    doCopy();
}
void copyOrMoveFile::moveFile(){
    //if files are on the same volume then use QFile to move.
    if (onSameVolume()){
        m_qFileSource->rename(m_newName);
        finish();
    } else {                 //otherwise copy first, then delete sourcefile.
        if (doCopy()) {
            if (!m_qFileSource->remove()) emit failed("unable to delete sourcefile");
        }
    }
}
bool copyOrMoveFile::onSameVolume(){
    return ( m_srcStorageInfo->rootPath() == m_dstStorageInfo->rootPath() );
}
void copyOrMoveFile::finish(){
    emit progress(100);
}
bool copyOrMoveFile::doCopy(){
    bool result=false;

    m_bytesToCopy = m_qFileSource->size();

    if (m_bytesToCopy==0) {
        emit failed("Source file is empty");
    } else {

        if (enoughSpace()){
            m_bytesCopied=0;
            m_lastReport=-1;

            // Use binary mode
            std::ifstream in(m_fileName.toStdString(), std::ios_base::in | std::ios_base::binary);
            std::ofstream out(m_newName.toStdString(), std::ios_base::out | std::ios_base::binary);

            if (out.fail()){
                emit failed("Invalid target location");
            } else {

                char buf[m_chunkSize];

                do {
                    reportProgress();
                    in.read(&buf[0], m_chunkSize);    // Read at most n bytes into
                    out.write(&buf[0], in.gcount());  // buf, then write the buf to the output
                    m_bytesCopied = m_bytesCopied + m_chunkSize;

                } while (in.gcount() > 0);

            }
            in.close();
            out.close();

            result=!out.fail();
        }
    }
    return result;
}

void copyOrMoveFile::reportProgress(){
    if (m_bytesToCopy < 100) m_progress=0;
        else m_progress=m_bytesCopied / (m_bytesToCopy / 100);
    if (m_progress != m_lastReport){
        emit progress(m_progress);
        m_lastReport=m_progress;
        QApplication::processEvents();
    }

}
bool copyOrMoveFile::enoughSpace(){
    bool result = false;
    long long availableSpace = m_dstStorageInfo->bytesAvailable();

    result = (availableSpace > m_bytesToCopy);
    if (!result) emit failed("Insufficient space on target drive");

    return result;
}
