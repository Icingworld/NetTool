#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("网络切换工具 by jjjzw");
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setWindowIcon(QIcon(":/Icon.png"));
    setFixedSize(this->width(), this->height());
    ReadLogFile("log1.txt", ui->choose1, list1);
    ReadLogFile("log2.txt", ui->choose2, list2);
    EthernetName = ReadSetFile();
    GetNetStatus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::ReadSetFile()
{
    QString filePath = ":/set.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Message("配置文件出错，请检查");
        close();
    }
    QTextStream in(&file);
    QString text = in.readAll().remove("\n");
    file.close();
    return text;
}

void MainWindow::GetNetStatus()
{
    ui->adapter->setText(EthernetName);
    QProcess process;
    process.start("ipconfig");
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    QString outputStr = QString::fromLocal8Bit(output);
    int ethernetIndex = outputStr.indexOf(EthernetName);
    int nextAdapterIndex = outputStr.indexOf("\r\n\r\n", ethernetIndex + 10);
    QString ethernetInfo = outputStr.mid(ethernetIndex, nextAdapterIndex - ethernetIndex);
    QStringList lines = ethernetInfo.split("\r\n");
    NetStatus net;
    for (const QString & line : lines) {
        if (line.contains("IPv4 地址")) {
            net.ip = line.split(":").value(1).trimmed();
        } else if (line.contains("子网掩码")) {
            net.mask = line.split(":").value(1).trimmed();
        } else if (line.contains("默认网关")) {
            net.gate = line.split(":").value(1).trimmed();
        }
    }
    ShowNetStatus(net);
}

bool MainWindow::isValid(const QString & string, const QString & pattern)
{
    QRegularExpression regex(pattern);
    QRegularExpressionMatch match = regex.match(string);
    return match.hasMatch();
}

void MainWindow::ShowNetStatus(const NetStatus & net)
{
    ui->ipnow->setText(net.ip);
    ui->masknow->setText(net.mask);
    ui->gatenow->setText(net.gate);
}

void MainWindow::Message(const QString & string)
{
    QMessageBox messageBox;
    messageBox.setWindowTitle("提示");
    messageBox.setText(string);
    messageBox.setIcon(QMessageBox::Information);
    messageBox.addButton("确定", QMessageBox::AcceptRole);
    messageBox.exec();
}

void MainWindow::ReadLogFile(const QString & path, QComboBox * box, QStringList & list)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        list << in.readLine().remove("\n");
    }
    box->addItems(list);
    file.close();
}

void MainWindow::WriteLogFile(const QString & path, const QString & string)
{
    QFile file(path);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
    }
    QTextStream out(&file);
    out << string << "\n";
    out.flush();
    file.close();
}

void MainWindow::on_choose1_currentTextChanged(const QString & arg1)
{
    ui->ipnew->setText(arg1);
}

void MainWindow::on_choose2_currentTextChanged(const QString & arg1)
{
    ui->tobepinged->setText(arg1);
}

void MainWindow::on_pushButton_clicked()
{
    QProcess process;
    if (ui->dhcp->isChecked())
    {
        process.start("netsh interface ip set address name=\"" + EthernetName + "\" dhcp");
    } else
    {
        if (!isValid(ui->ipnew->text(), "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"))
        {
            Message("请输入正确的IP");
        } else if (!isValid(ui->masknew->text(), "^((128|192)|2(24|4[08]|5[245]))(\\.(0|(128|192)|2((24)|(4[08])|(5[245])))){3}$"))
        {
            Message("请输入正确的子网掩码");
        } else if (ui->gatenew->text() != "" && !isValid(ui->gatenew->text(), "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"))
        {
            Message("请输入正确的网关");
        } else {
            if (!list1.contains(ui->ipnew->text()))
            {
                list1 << ui->ipnew->text();
                ui->choose1->addItem(ui->ipnew->text());
                WriteLogFile("log1.txt", ui->ipnew->text());
            }
            process.start("netsh interface ip set address name=\"" + EthernetName + "\" static " + ui->ipnew->text() + " " + ui->masknew->text() + " " + ui->gatenew->text());
        }
    }
    process.waitForFinished();
    //QByteArray output = process.readAllStandardOutput();
    //QString outputStr = QString::fromLocal8Bit(output);
    //qDebug() << outputStr;
    ui->ipnow->setText("重新检测中");
    ui->masknow->setText("重新检测中");
    ui->gatenow->setText("重新检测中");
    ui->ipnow->repaint();
    ui->masknow->repaint();
    ui->gatenow->repaint();
    QThread::msleep(5000);
    GetNetStatus();
}

void MainWindow::on_pushButton_2_clicked()
{
    if (!isValid(ui->tobepinged->text(), "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"))
    {
        Message("请输入正确的IP");
    } else {
        if (!list2.contains(ui->tobepinged->text()))
        {
            list2 << ui->tobepinged->text();
            ui->choose2->addItem(ui->tobepinged->text());
            WriteLogFile("log2.txt", ui->tobepinged->text());
        }
        ui->status->setText("检测中...");
        ui->status->repaint();
        QProcess process;
        QString command = ui->tobepinged->text();
        process.start("ping -n 1 " + command);
        process.waitForFinished();
        QByteArray output = process.readAllStandardOutput();
        QString outputStr = QString::fromLocal8Bit(output);
        if (outputStr.contains("回复"))
        {
            ui->status->setText("连通");
        } else if (outputStr.contains("超时"))
        {
            ui->status->setText("未连通");
        }
    }
}

void MainWindow::on_dhcp_stateChanged(int arg1)
{
    if (arg1 == 2)
    {
        ui->ipnew->setEnabled(false);
        ui->masknew->setEnabled(false);
        ui->gatenew->setEnabled(false);
        ui->choose1->setEnabled(false);
    } else {
        ui->ipnew->setEnabled(true);
        ui->masknew->setEnabled(true);
        ui->gatenew->setEnabled(true);
        ui->choose1->setEnabled(true);
    }
}


void MainWindow::on_check_clicked()
{
    EthernetName = ui->adapter->text();
    GetNetStatus();
}

