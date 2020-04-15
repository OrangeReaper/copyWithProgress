#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "copyormovefile.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->goButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onSelectFile(){
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec()){
        QString filePath = dialog.selectedFiles().first();
        m_validFile=true;
        ui->sourceFile->setText(filePath);
        validate();
    }
}
void MainWindow::onSelectFolder(){
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::Directory);
        if (dialog.exec()){
            QString path = dialog.selectedFiles().first();
            m_validFolder=true;
            ui->targetFolder->setText(path);
            validate();
        }
}
void MainWindow::onGo(){
    ui->goButton->setEnabled(false);
    QString newName;
    if (ui->changeName->isChecked()){
        newName=ui->newName->text();
    } else {
        QStringList list=ui->sourceFile->text().split("/");
        newName=list.at(list.size()-1);
    }
    newName=ui->targetFolder->text() + "/" + newName;

    copyOrMoveFile::op operation=copyOrMoveFile::op::copy;
    if (ui->move->isChecked()) operation=copyOrMoveFile::op::move;

    ui->progressBar->setVisible(true);

    QString src = ui->sourceFile->text();
    if (ui->invalidateSrc->isChecked()) src=src+"x";
    if (ui->invalidateDst->isChecked()) newName="x" + newName;

    copyOrMoveFile * thread = new copyOrMoveFile(src, newName, operation);
    connect (thread, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
    connect (thread, SIGNAL(failed(QString)), this, SLOT(somethingFailed(QString)));
    connect (thread, SIGNAL(finished()), this, SLOT(copyDone()));
    thread->setAutoDelete(true);
    thread->start();
}
void MainWindow::copyDone(){
    ui->goButton->setEnabled(true);
    ui->progressBar->setVisible(false);
}
void MainWindow::somethingFailed(QString msg){
    qDebug() << msg;
}
void MainWindow::validate(){
    bool result=true;
    if (ui->changeName->isChecked() && ui->newName->text().length()==0) result=false;

    result = result && m_validFile && m_validFolder;
    ui->goButton->setEnabled(result);
}
