#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(application);

    QApplication App(argc, argv);
    QCoreApplication::setOrganizationName("Cho-yeon");
    QCoreApplication::setApplicationName("Notepad");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCommandLineParser Parser;
    Parser.setApplicationDescription(QCoreApplication::applicationName());
    Parser.addHelpOption();
    Parser.addVersionOption();
    Parser.addPositionalArgument("file", "The file to open.");
    Parser.process(App);

    MainWindow MainWin;
    if (!Parser.positionalArguments().isEmpty())
        MainWin.LoadFile(Parser.positionalArguments().first());
    MainWin.show();
    return App.exec();
}
