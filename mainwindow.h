#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void onGo();
    void onSelectFile();
    void onSelectFolder();
    void validate();
    void somethingFailed(QString msg);
    void copyDone();
private:
    Ui::MainWindow *ui;



    bool m_validFile = false;
    bool m_validFolder = false;
};

#endif // MAINWINDOW_H
