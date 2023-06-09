#pragma execution_character_set("utf-8")
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QThread>
#include <QTime>
#include <QMetaObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct NetStatus
{
    QString ip;
    QString mask;
    QString gate;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void GetNetStatus();
    bool isValid(const QString& string, const QString & pattern);
    void ShowNetStatus(const NetStatus & net);
    void Message(const QString & string);
    QString ReadSetFile();
    void ReadLogFile(const QString & path, QComboBox * box, QStringList & list);
    void WriteLogFile(const QString & path, const QString & string);
    QString EthernetName;
    QStringList list1;
    QStringList list2;
    ~MainWindow();

public slots:
    void on_choose1_currentTextChanged(const QString & arg1);
    void on_choose2_currentTextChanged(const QString & arg1);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private slots:
    void on_dhcp_stateChanged(int arg1);

    void on_check_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
