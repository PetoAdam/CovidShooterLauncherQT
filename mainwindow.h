#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum LauncherStatus
    {
        ready,
        failed,
        downloadingGame,
        downloadingUpdate
    };

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void SetStatus(LauncherStatus value);

    ~MainWindow();
public slots:
    void PlayButtonClick();

private:
    Ui::MainWindow *ui;
    QPushButton* button;

    string rootPath;
    string versionFile;
    string gameZip;
    string gameExe;

    LauncherStatus _status;
};
#endif // MAINWINDOW_H
