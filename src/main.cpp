#include <getopt.h>  
#include <QApplication>
#include <QFileInfo>
#include <QString>
#include <QTextStream>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include "mainwindow.h"
#include "main.h"


static struct option options[] = {
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'v'},
    {"portCount", 1, 0, 'p'},
    {0, 0, 0, 0}
};


int main(int argc, char *argv[])  
{
    int getopt_return;
    int option_index; 
    int portCount = 2;
    QTextStream out(stdout);

    while ((getopt_return = getopt_long(argc, argv, "vhp:", options,
                    &option_index)) >= 0) {
        switch(getopt_return) {
            case 'p':
                portCount = atoi(optarg);
                if (portCount > MAX_PORTS)
                    portCount = MAX_PORTS;
                else if (portCount < 1)
                    portCount = 2;
                break;
            case 'v':
                out << ABOUTMSG;
                out.flush();
                exit(EXIT_SUCCESS);
            case 'h':
                out << "Usage: " PACKAGE " [OPTION] [FILENAME]" << endl;
                out << endl;
                out << "Options:" << endl;
                out << "  -v, --version             "
                        "Print application version" << endl;
                out << "  -h, --help                Print this message" << endl;
                out << QString("  -p, --portCount <num>     "
                        "Set number of output ports [%1]\n")
                    .arg(portCount) << endl;
                out.flush();
                exit(EXIT_SUCCESS);
        }
    }

    QApplication app(argc, argv);
    QLocale loc = QLocale::system();

    // translator for Qt library messages
    QTranslator qtTr;

    if (qtTr.load(QString("qt_") + loc.name(),
                QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTr);

    // translator for qmidroute messages       
    QTranslator qmidirouteTr;

    if (qmidirouteTr.load(QString(PACKAGE "_") + loc.name(), TRANSLATIONSDIR))
        app.installTranslator(&qmidirouteTr);

    MainWindow* qmidiroute = new MainWindow(portCount);
    if (optind < argc) {
        QFileInfo fi(argv[optind]);
        if (fi.exists())
            qmidiroute->openFile(fi.absoluteFilePath());
        else
            qWarning("File not found: %s", argv[optind]);
    }

    int result = app.exec();
    delete qmidiroute;
    return result;
}
