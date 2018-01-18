#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QMovie>
#include <QFileSystemWatcher>
#include <QTimer>

#include "../utils/bae.h"
class FileLoader;
class CollectionDB;
class YouTube;
class Socket;
class Brain;

using namespace BAE;

class settings : public QObject
{
    Q_OBJECT

public:
    explicit settings(QObject *parent = nullptr);
    ~settings();
    void checkCollection();
    void collectionWatcher();

private slots:
    void handleDirectoryChanged(const QString &dir);
    void on_remove_clicked();

public slots:
    void startBrainz(const uint &speed);
    void populateDB(const QStringList &paths);

private:
    FileLoader *fileLoader;
    CollectionDB *connection;
    Brain *brainDeamon;
    YouTube *ytFetch;
    Socket *babeSocket;
    
    bool brainzOn = false;


    QString pathToRemove;

    QStringList dirs;
    QFileSystemWatcher *watcher;

    void refreshCollectionPaths();
    void addToWatcher(QStringList paths);

signals:
    void collectionPathChanged(QStringList newPaths);
    void refreshTables(QVariantMap tables);
    void albumArtReady(const DB &album);
    void brainFinished();

};

#endif // SETTINGS_H
