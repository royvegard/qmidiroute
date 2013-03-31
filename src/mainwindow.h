#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QTabWidget>

#include "mapwidget.h"
#include "logwidget.h"
#include "mapdata.h"
#include "passwidget.h"
#include "config.h"


static const char ABOUTMSG[] = APP_NAME " " PACKAGE_VERSION "\n"
                          "(C) 2002-2003 Matthias Nagorni (SuSE AG Nuremberg)\n"
			  "(C) 2009 Frank Kober\n"
			  "(C) 2009 Guido Scholz\n\n"
                          APP_NAME " is licensed under the GPL.\n";

class MainWindow : public QMainWindow
{
    Q_OBJECT

    static int sigpipe[2];

    QTabWidget *tabWidget;
    MapData *mapData;
    LogWidget *logWidget;
    PassWidget *passWidget;
    QString lastDir, filename;

    void chooseFile();
    void clear();
    bool isSave();
    void updateWindowTitle();
    bool saveFile();
    bool saveFileAs();
    void removeMap(int);
    void addMap(const QString&);
    bool isModified();
    bool installSignalHandlers();
    static void handleSignal(int);

  protected:
    void closeEvent(QCloseEvent*);

  public:
    MainWindow(int p_portCount);
    ~MainWindow();

    QAction *mapCloneAction, *mapNewAction, *mapDeleteAction, *mapRenameAction;
    QAction *fileNewAction, *fileOpenAction, *fileSaveAction, *fileSaveAsAction;
    QAction *fileQuitAction;
    void openFile(const QString&);
  
  public slots: 
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void mapNew();
    void mapClone();
    void mapRename();
    void mapDelete();
    void helpAbout();
    void helpAboutQt();
    void selectedTabChanged(int);

  private slots:
    void signalAction(int);

};
  
#endif
