#include <QLabel>
#include <QFile>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QTextStream>
#include <QWidget>
#include <QSocketNotifier>
#include <QToolBar>
#include <QToolButton>
#include <QMenuBar>   
#include <QMenu>
#include <alsa/asoundlib.h>
#include <cerrno>   // for errno
#include <csignal>  // for sigaction()
#include <cstring>  // for strerror()
#include <unistd.h> // for pipe()


#include "mapdata.h"
#include "logwidget.h"
#include "passwidget.h"
#include "mainwindow.h"

#include "pixmaps/qmidiroute_48.xpm"
#include "pixmaps/mapadd.xpm"
#include "pixmaps/mapremove.xpm"
#include "pixmaps/maprename.xpm"
#include "pixmaps/mapclone.xpm"
#include "pixmaps/fileopen.xpm"
#include "pixmaps/filenew.xpm"
#include "pixmaps/filesave.xpm"
#include "pixmaps/filesaveas.xpm"
#include "pixmaps/filequit.xpm"

static const char FILEEXT[] = ".qmr";


int MainWindow::sigpipe[2];

MainWindow::MainWindow(int p_portCount)
{
    filename = "";
    lastDir = QDir::homePath();

    mapData = new MapData(this);
    mapData->registerPorts(p_portCount);

    tabWidget = new QTabWidget(this);

    logWidget = new LogWidget(this);
    QObject::connect(mapData->seqDriver, SIGNAL(midiEvent(snd_seq_event_t *)), 
            logWidget, SLOT(appendEvent(snd_seq_event_t *)));

    QDockWidget *logWindow = new QDockWidget(tr("Event Log"), this);
    logWindow->setFeatures(QDockWidget::DockWidgetClosable
            | QDockWidget::DockWidgetMovable
            | QDockWidget::DockWidgetFloatable);
    logWindow->setWidget(logWidget);;
    logWindow->setVisible(true);
    addDockWidget(Qt::BottomDockWidgetArea, logWindow);

    passWidget = new PassWidget(p_portCount, this);
    tabWidget->insertTab(1, passWidget, tr("Unmatched"));

    QObject::connect(passWidget, SIGNAL(discardToggled(bool)), 
            mapData->seqDriver, SLOT(setDiscardUnmatched(bool)));
    QObject::connect(passWidget, SIGNAL(newPortUnmatched(int)), 
            mapData->seqDriver, SLOT(setPortUnmatched(int)));

    mapNewAction = new QAction(QIcon(mapadd_xpm), tr("&New..."), this);
    mapNewAction->setShortcut(QKeySequence(tr("Ctrl+N", "Rule|New")));
    mapNewAction->setToolTip(tr("Add new rule"));
    connect(mapNewAction, SIGNAL(triggered()), this, SLOT(mapNew()));

    mapCloneAction = new QAction(QIcon(mapclone_xpm), tr("&Clone"), this);
    mapCloneAction->setShortcut(QKeySequence(tr("Ctrl+C", "Rule|Clone")));
    mapCloneAction->setToolTip(tr("Duplicate selected rule"));
    connect(mapCloneAction, SIGNAL(triggered()), this, SLOT(mapClone()));

    mapRenameAction = new QAction(QIcon(maprename_xpm), tr("&Rename..."), this);
    mapRenameAction->setShortcut(QKeySequence(tr("Ctrl+R", "Rule|Rename")));
    mapRenameAction->setToolTip(tr("Rename selected rule"));
    connect(mapRenameAction, SIGNAL(triggered()), this, SLOT(mapRename()));

    mapDeleteAction = new QAction(QIcon(mapremove_xpm), tr("&Delete..."), this);
    mapDeleteAction->setShortcut(QKeySequence(tr("Ctrl+Del", "Rule|Delete")));
    mapDeleteAction->setToolTip(tr("Delete selected rule"));
    connect(mapDeleteAction, SIGNAL(triggered()), this, SLOT(mapDelete()));

    fileNewAction = new QAction(QIcon(filenew_xpm), tr("&New"), this);
    fileNewAction->setShortcut(QKeySequence(QKeySequence::New));    
    fileNewAction->setToolTip(tr("Create new MIDI route file"));
    connect(fileNewAction, SIGNAL(triggered()), this, SLOT(fileNew()));

    fileOpenAction = new QAction(QIcon(fileopen_xpm), tr("&Open..."), this);
    fileOpenAction->setShortcut(QKeySequence(QKeySequence::Open));    
    fileOpenAction->setToolTip(tr("Open MIDI route file"));
    connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(fileOpen()));

    fileSaveAction = new QAction(QIcon(filesave_xpm), tr("&Save"), this);
    fileSaveAction->setShortcut(QKeySequence(QKeySequence::Save));    
    fileSaveAction->setToolTip(tr("Save current MIDI route file"));
    connect(fileSaveAction, SIGNAL(triggered()), this, SLOT(fileSave()));

    fileSaveAsAction = new QAction(QIcon(filesaveas_xpm), tr("Save &as..."),
            this);
    fileSaveAsAction->setToolTip(tr("Save current MIDI route file with new name"));
    connect(fileSaveAsAction, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

    fileQuitAction = new QAction(QIcon(filequit_xpm), tr("&Quit"), this);
    fileQuitAction->setShortcut(QKeySequence(tr("Ctrl+Q", "File|Quit")));    
    fileQuitAction->setToolTip(tr("Quit application"));
    connect(fileQuitAction, SIGNAL(triggered()), this, SLOT(close()));

    mapDeleteAction->setDisabled(true);
    mapRenameAction->setDisabled(true);
    mapCloneAction->setDisabled(true);

    connect(tabWidget, SIGNAL(currentChanged(int)), this,
            SLOT(selectedTabChanged(int)));


    QAction* viewLogAction = logWindow->toggleViewAction();
    viewLogAction->setText(tr("&Event Log"));
    viewLogAction->setShortcut(QKeySequence(tr("Ctrl+L", "View|Event Log")));
    viewLogAction->setToolTip(tr("Show/hide MIDI event log window"));


    QMenuBar *menuBar = new QMenuBar;
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    QMenu *viewMenu = new QMenu(tr("&View"), this);
    QMenu *mapMenu = new QMenu(tr("&Rule"), this);
    QMenu *helpMenu = new QMenu(tr("&Help"), this);

    fileMenu->addAction(fileNewAction);
    fileMenu->addAction(fileOpenAction);
    fileMenu->addAction(fileSaveAction);
    fileMenu->addAction(fileSaveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(fileQuitAction);

    mapMenu->addAction(mapNewAction);
    mapMenu->addAction(mapCloneAction);
    mapMenu->addAction(mapRenameAction);
    mapMenu->addAction(mapDeleteAction);

    helpMenu->addAction(tr("&About %1...").arg(APP_NAME), this,
            SLOT(helpAbout())); 
    helpMenu->addAction(tr("&About Qt..."), this,
            SLOT(helpAboutQt())); 

    menuBar->addMenu(fileMenu);
    menuBar->addMenu(viewMenu);
    menuBar->addMenu(mapMenu);
    menuBar->addMenu(helpMenu);

    QToolBar *toolBar = new QToolBar(tr("&Toolbar"), this);
    toolBar->addAction(fileNewAction);    
    toolBar->addAction(fileOpenAction);    
    toolBar->addAction(fileSaveAction);    
    toolBar->addAction(fileSaveAsAction);
    toolBar->addSeparator();
    toolBar->addAction(mapNewAction);
    toolBar->addAction(mapCloneAction);
    toolBar->addAction(mapRenameAction);
    toolBar->addAction(mapDeleteAction);
    toolBar->setFloatable(false);

    QAction* viewToolbarAction = toolBar->toggleViewAction();
    viewToolbarAction->setShortcut(QKeySequence(tr("Ctrl+T", "View|Toolbar")));
    viewToolbarAction->setToolTip(tr("Show/hide toolbar"));
    viewMenu->addAction(viewToolbarAction);
    viewMenu->addAction(viewLogAction);

    setMenuBar(menuBar);
    addToolBar(toolBar);

    setCentralWidget(tabWidget);
    setWindowIcon(QPixmap(qmidiroute_48_xpm));
    updateWindowTitle();

    if (!installSignalHandlers())
        qWarning("%s", "Signal handlers not installed!");

    resize(520, 480);
    show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateWindowTitle()
{
    if (filename.isEmpty())
        setWindowTitle(QString("%1 (%2)")
                .arg(APP_NAME)
                .arg(mapData->getAlsaClientId()));
    else
        setWindowTitle(QString("%1 - %2 (%3)")
                .arg(filename)
                .arg(APP_NAME)
                .arg(mapData->getAlsaClientId()));
}

void MainWindow::helpAbout()
{
    QMessageBox::about(this, tr("About %1").arg(APP_NAME), ABOUTMSG);
}

void MainWindow::helpAboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::mapNew()
{
    QString name;
    bool ok;

    name = QInputDialog::getText(this, APP_NAME,
            tr("Add rule"), QLineEdit::Normal,
            tr("Rule %1").arg(mapData->midiMapCount() + 1), &ok);

    if (ok && !name.isEmpty()) {
        MapWidget *mapWidget = new MapWidget(mapData->createMidiMap(),
                mapData->getPortCount(), tabWidget);
        mapWidget->mapName = name;
        mapData->addMapWidget(mapWidget);
        tabWidget->addTab(mapWidget, name);
        tabWidget->setCurrentWidget(mapWidget);
    }
}

void MainWindow::addMap(const QString& qs)
{
    MapWidget *mapWidget = new MapWidget(mapData->createMidiMap(),
            mapData->getPortCount(), tabWidget);
    mapWidget->mapName = qs;
    mapData->addMapWidget(mapWidget);
    tabWidget->addTab(mapWidget, qs);
    tabWidget->setCurrentWidget(mapWidget);
}

void MainWindow::mapClone()
{
    QString qs, qs2;
    int l1;

    if (tabWidget->currentIndex() < 1) {
        return;
    }
    qs = tabWidget->tabText(tabWidget->currentIndex());

    MapWidget *mapWidget = (MapWidget *)tabWidget->currentWidget();
    MapWidget *cloneWidget = new MapWidget(mapData->createMidiMap(),
            mapData->getPortCount(), tabWidget);
    mapData->addMapWidget(cloneWidget);

    cloneWidget->mapName = qs;

    cloneWidget->setTypeIn((int)mapWidget->getMidiMap()->typeIn);  
    cloneWidget->setTypeOut((int)mapWidget->getMidiMap()->typeOut);  
    cloneWidget->setChOutMode((int)mapWidget->getMidiMap()->chOutMode);

    switch ((int)mapWidget->getMidiMap()->chOutMode)
    {
        case 2:
        cloneWidget->setChOut(mapWidget->getMidiMap()->chOut + 1);
        break;
        case 1:
        cloneWidget->setChOut(mapWidget->getMidiMap()->chOut + 2);
        break;
        case 0:
        cloneWidget->setChOut(mapWidget->getMidiMap()->chOut);
        break;
    }
    
    cloneWidget->setIndexOutMode((int)mapWidget->getMidiMap()->indexOutMode);
    cloneWidget->setIndexOut(mapWidget->getMidiMap()->indexOut);
    cloneWidget->setPortOut(mapWidget->getMidiMap()->portOut + 1);
    for (l1 = 0; l1 < 2; l1++) {
        cloneWidget->setChIn(l1, mapWidget->getMidiMap()->chIn[l1] + 1);
        cloneWidget->setIndexIn(l1, mapWidget->getMidiMap()->indexIn[l1]);
        cloneWidget->setRangeIn(l1, mapWidget->getMidiMap()->rangeIn[l1]);
        cloneWidget->setRangeOut(l1, mapWidget->getMidiMap()->rangeOut[l1]);
    }
    tabWidget->addTab(cloneWidget, qs);
    tabWidget->setCurrentIndex(tabWidget->count());
    tabWidget->setCurrentWidget(cloneWidget);
    cloneWidget->updateTypeIn((int)mapWidget->getMidiMap()->typeIn);
    cloneWidget->updateTypeOut((int)mapWidget->getMidiMap()->typeOut);
    cloneWidget->updateChOutMode((int)mapWidget->getMidiMap()->chOutMode);
    cloneWidget->updateIndexOutMode((int)mapWidget->getMidiMap()->indexOutMode);
}     

void MainWindow::mapRename()
{
    QString newname, oldname;
    bool ok;

    if (tabWidget->currentIndex() < 1) {
        return;
    }
    oldname = tabWidget->tabText(tabWidget->currentIndex());
    newname = QInputDialog::getText(this, APP_NAME,
            tr("New Name"), QLineEdit::Normal, oldname, &ok);

    if (ok && !newname.isEmpty()){
        tabWidget->setTabText(tabWidget->currentIndex(), newname);                                
        MapWidget *mapWidget = (MapWidget *)tabWidget->currentWidget();
        mapWidget->mapName = newname;
    }
}

void MainWindow::mapDelete()
{
    QString qs;

    if (tabWidget->currentIndex() < 1) {
        return;
    }
    qs = tr("Delete \"%1\"?")
        .arg(tabWidget->tabText(tabWidget->currentIndex()));

    if (QMessageBox::question(this, APP_NAME, qs, QMessageBox::Yes,
                QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                Qt::NoButton) == QMessageBox::No) {
        return;
    }
    
    MapWidget *mapWidget = (MapWidget *)tabWidget->currentWidget();
    mapData->removeMapWidget(mapWidget);
    tabWidget->removeTab(tabWidget->currentIndex());
    delete mapWidget;
}

void MainWindow::removeMap(int index)
{
    MapWidget *mapWidget = mapData->mapWidget(index);
    mapData->removeMapWidget(mapWidget);
    tabWidget->removeTab(index + 1);
    delete mapWidget;
}

void MainWindow::selectedTabChanged(int index)
{
    if (index > 0) {
        mapDeleteAction->setDisabled(false);
        mapCloneAction->setDisabled(false);
        mapRenameAction->setDisabled(false);
    }
    else {
        mapDeleteAction->setDisabled(true);
        mapCloneAction->setDisabled(true);
        mapRenameAction->setDisabled(true);
    }
}

void MainWindow::clear()
{
    while (mapData->midiMapCount())
        removeMap(mapData->midiMapCount() - 1);
}

void MainWindow::fileNew()
{
    if (isSave()) {
        clear();
        filename = "";
        updateWindowTitle();
        mapData->setModified(false);
    }
}

void MainWindow::fileOpen()
{
    if (isSave())
        chooseFile();
}

void MainWindow::chooseFile()
{
    QString fn = QFileDialog::getOpenFileName(this,
            tr("Open MIDI route map file"), lastDir,
            tr("QMidiRoute files") + " (*" + FILEEXT + ")");

    if (fn.isEmpty())
        return;

    openFile(fn);
}

void MainWindow::openFile(const QString& fn) 
{
    QString line, qs;

    lastDir = fn.left(fn.lastIndexOf('/'));

    QFile f(fn);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, APP_NAME,
                tr("Could not read from file '%1'.").arg(fn));
        return;
    }

    clear();
    filename = fn;
    updateWindowTitle();

    QTextStream loadText(&f);
    line = loadText.readLine();
    qs = line.section(' ', 0, 0);
    passWidget->setDiscard(qs.toInt());
    qs = line.section(' ', 1, 1);
    passWidget->setPortUnmatched(qs.toInt() + 1);

    while (!loadText.atEnd()){
        line = loadText.readLine();
        addMap(line);
        mapData->readFileText(loadText);
    } 
    mapData->setModified(false);
}

void MainWindow::fileSaveAs()
{
    saveFileAs();
}

bool MainWindow::saveFileAs()
{
    bool result = false;

    QString fn =  QFileDialog::getSaveFileName(this,
            tr("Save MIDI route map file"), lastDir,
            tr("QMidiRoute files") + " (*" + FILEEXT + ")");

    if (!fn.isEmpty()) {
        if (!fn.endsWith(FILEEXT))
            fn.append(FILEEXT);
        lastDir = fn.left(fn.lastIndexOf('/'));

        filename = fn;
        updateWindowTitle();
        result = saveFile();
    }
    return result;
}

void MainWindow::fileSave()
{
    if (filename.isEmpty())
        saveFileAs();
    else
        saveFile();
}

bool MainWindow::saveFile()
{
    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, APP_NAME,
                tr("Could not write to file '%1'.").arg(filename));
        return false;
    }

    QTextStream ts(&f);
    mapData->saveFileText(ts);

    return true;
}

bool MainWindow::isSave()
{
    bool result = false;
    QString queryStr;

    if (isModified()) {
        if (filename.isEmpty())
            queryStr = tr("Unnamed file was changed.\nSave changes?");
        else
            queryStr = tr("File '%1' was changed.\n"
                    "Save changes?").arg(filename);

        QMessageBox::StandardButton choice = QMessageBox::warning(this,
                tr("Save changes"), queryStr,
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                QMessageBox::Yes);

        switch (choice) {
            case QMessageBox::Yes:
                if (filename.isEmpty())
                    result = saveFileAs();
                else
                    result = saveFile();
                break;
            case QMessageBox::No:
                result = true;
                break;
            case QMessageBox::Cancel:
            default:
                break;
        }
    }
    else
        result = true;

    return result;
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    if (isSave())
        e->accept();
    else 
        e->ignore();
}

bool MainWindow::isModified()
{
    return mapData->isModified();
}

/* Handler for system signals (SIGUSR1, SIGINT...)
 * Write a message to the pipe and leave as soon as possible
 */
void MainWindow::handleSignal(int sig)
{
    if (write(sigpipe[1], &sig, sizeof(sig)) == -1) {
        qWarning("write() failed: %s", std::strerror(errno));
    }
}

/* Install signal handlers (may be more than one; called from the
 * constructor of your MainWindow class*/
bool MainWindow::installSignalHandlers()
{
    /*install pipe to forward received system signals*/
    if (pipe(sigpipe) < 0) {
        qWarning("pipe() failed: %s", std::strerror(errno));
        return false;
    }

    /*install notifier to handle pipe messages*/
    QSocketNotifier* signalNotifier = new QSocketNotifier(sigpipe[0],
            QSocketNotifier::Read, this);
    connect(signalNotifier, SIGNAL(activated(int)),
            this, SLOT(signalAction(int)));

    /*install signal handlers*/
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = handleSignal;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        qWarning("sigaction() failed: %s", std::strerror(errno));
        return false;
    }

    if (sigaction(SIGINT, &action, NULL) == -1) {
        qWarning("sigaction() failed: %s", std::strerror(errno));
        return false;
    }

    if (sigaction(SIGTERM, &action, NULL) == -1) {
        qWarning("sigaction() failed: %s", std::strerror(errno));
        return false;
    }

    return true;
}

/* Slot to give response to the incoming pipe message;
   e.g.: save current file */
void MainWindow::signalAction(int fd)
{
    int message;

    if (read(fd, &message, sizeof(message)) == -1) {
        qWarning("read() failed: %s", std::strerror(errno));
        return;
    }
    
    switch (message) {
        case SIGUSR1:
            fileSave();
            break;

        case SIGINT:
        case SIGTERM:
            close();
            break;

        default:
            qWarning("Unexpected signal received: %d", message);
            break;
    }
}

