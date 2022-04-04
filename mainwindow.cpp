#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    button = findChild<QPushButton*>("playButton");

    rootPath = Directory.GetCurrentDirectory();
    versionFile = Path.Combine(rootPath, "Version.txt");
    gameZip = Path.Combine(rootPath, "Build.zip");
    gameExe = Path.Combine(rootPath, "Temalabtest.exe");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::PlayButtonClick()
{
    return;
}


void MainWindow::SetStatus(LauncherStatus value)
{
    _status = value;
    switch (_status)
    {
        case ready:
            button->setText("PLAY");
            break;
        case failed:
            button->setText("Update failed - Retry");
            break;
        case downloadingGame:
            button->setText("Downloading Game");
            break;
        case downloadingUpdate:
            button->setText("Downloading Update");
            break;
        default:
            break;
    }
}



        void MainWindow::CheckForUpdates()
        {
            if (File.Exists(versionFile))
            {
                Version localVersion = new Version(File.ReadAllText(versionFile));
                VersionText.Text = localVersion.ToString();

                try
                {
                    WebClient webClient = new WebClient();
                    Version onlineVersion = new Version(webClient.DownloadString("https://storage.googleapis.com/covidshooterzip/Version.txt"));

                    if (onlineVersion.IsDifferentThan(localVersion))
                    {
                        InstallGameFiles(true, onlineVersion);
                    }
                    else
                    {
                        Status = LauncherStatus.ready;
                    }
                }
                catch (Exception ex)
                {
                    Status = LauncherStatus.failed;
                    MessageBox.Show($"Error checking for game updates: {ex}");
                }
            }
            else
            {
                InstallGameFiles(false, Version.zero);
            }
        }

        void MainWindow::InstallGameFiles(bool _isUpdate, Version _onlineVersion)
        {
            try
            {
                WebClient webClient = new WebClient();
                if (_isUpdate)
                {
                    Status = LauncherStatus.downloadingUpdate;
                }
                else
                {
                    Status = LauncherStatus.downloadingGame;
                    _onlineVersion = new Version(webClient.DownloadString("https://storage.googleapis.com/covidshooterzip/Version.txt"));
                }
                webClient.DownloadFileCompleted += new AsyncCompletedEventHandler(DownloadGameCompletedCallback);
                webClient.DownloadFileAsync(new Uri("https://storage.googleapis.com/covidshooterzip/Build.zip"), gameZip, _onlineVersion);
            }
            catch (Exception ex)
            {
                Status = LauncherStatus.failed;
                MessageBox.Show($"Error installing game files: {ex}");
            }
        }

        private void DownloadGameCompletedCallback(object sender, AsyncCompletedEventArgs e)
        {
            try
            {
                string onlineVersion = ((Version)e.UserState).ToString();
                ZipFile.ExtractToDirectory(gameZip, rootPath, true);
                File.Delete(gameZip);

                File.WriteAllText(versionFile, onlineVersion);
                VersionText.Text = onlineVersion;
                Status = LauncherStatus.ready;
            }
            catch (Exception ex)
            {
                Status = LauncherStatus.failed;
                MessageBox.Show($"Error installing: {ex}");
            }
        }

        private void Window_ContentRendered(object sender, EventArgs e)
        {
            CheckForUpdates();
        }

        private void PlayButton_Click(object sender, RoutedEventArgs e)
        {
            if(File.Exists(gameExe) && Status == LauncherStatus.ready)
            {
                ProcessStartInfo startInfo = new ProcessStartInfo(gameExe);
                startInfo.WorkingDirectory = rootPath;
                Process.Start(startInfo);
                Close();
            }
            else if(Status == LauncherStatus.failed)
            {
                CheckForUpdates();
            }
        }
    }

    struct Version
    {
        internal static Version zero = new Version(0, 0, 0);

        private int major;
        private int minor;
        private int subMinor;

        public Version(int _major, int _minor, int _subMinor)
        {
            major = _major;
            minor = _minor;
            subMinor = _subMinor;
        }

        public Version(string _version)
        {
            string[] _versionStrings = _version.Split('.');
            if(_versionStrings.Length != 3)
            {
                major = 0;
                minor = 0;
                subMinor = 0;
                return;
            }

            major = Int32.Parse(_versionStrings[0]);
            minor = Int32.Parse(_versionStrings[1]);
            subMinor = Int32.Parse(_versionStrings[2]);
        }

        public bool IsDifferentThan(Version _other)
        {
            if(major != _other.major)
            {
                return true;
            }
            else
            {
                if(minor != _other.minor)
                {
                    return true;
                }
                else
                {
                    if(subMinor != _other.subMinor)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        public override string ToString()
        {
            return $"{major}.{minor}.{subMinor}";
        }
