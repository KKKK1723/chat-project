#include "mainwindow.h"

#include <QApplication>
#include<QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile qss(":/style/style.qss");
    if(qss.open(QFile::ReadOnly))//以只读的方式打开
    {
        qDebug()<<"open success";
        QString style=QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }
    else
    {
        qDebug()<<"open failed";
    }

    QString fileName="config.ini";
    QString app_path=QCoreApplication::applicationDirPath();
    QString config_path=QDir::toNativeSeparators((app_path + QDir::separator() + fileName));
    QSettings settings(config_path,QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();
    gate_url_prefix = "http://"+gate_host+":"+gate_port;


    qDebug() << "新的！！！！！！！！！！！！！！！！！！！！" ;


    MainWindow w;
    w.show();
    return a.exec();
}
