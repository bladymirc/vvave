#ifndef FILELOADER_H
#define FILELOADER_H

#include <QThread>
#include <QObject>
#include <QDirIterator>

#include "../services/local/taginfo.h"
#include "../db/collectionDB.h"

class FileLoader : public QObject
{
    Q_OBJECT

public:
    FileLoader() : QObject()
    {
        qRegisterMetaType<BAE::DB>("BAE::DB");
        qRegisterMetaType<BAE::TABLE>("BAE::TABLE");
        qRegisterMetaType<QMap<BAE::TABLE, bool>>("QMap<BAE::TABLE,bool>");

        this->con = new CollectionDB(this);
        this->moveToThread(&t);
        t.start();
    }

    ~FileLoader()
    {
        this->go = false;
        this->t.quit();
        this->t.wait();
    }

    void requestPaths(QStringList paths)
    {
        qDebug()<<"FROM file loader"<< &paths;

        if(!go)
        {
            this->go = true;
            QMetaObject::invokeMethod(this, "getTracks", Q_ARG(QStringList, paths));
        }

    }

    void nextTrack()
    {
        this->wait = !this->wait;
    }

public slots:

    void getTracks(QStringList paths)
    {
        qDebug()<<"GETTING TRACKS FROM SETTINGS";

        QStringList urls;

        for(auto path : paths)
        {
            if (QFileInfo(path).isDir())
            {
                this->con->addFolder(path);
                QDirIterator it(path, BAE::formats, QDir::Files, QDirIterator::Subdirectories);
                while (it.hasNext()) urls<<it.next();

            } else if (QFileInfo(path).isFile()) urls<<path;

        }

        qDebug()<<"URLS SIZEW FOR:"<<paths<< urls.size();
        int newTracks = 0;
        if(urls.size()>0)
        {

            for(auto url : urls)
            {
                if(go)
                {
                    if(!con->check_existance(BAE::TABLEMAP[BAE::TABLE::TRACKS],BAE::KEYMAP[BAE::KEY::URL],url))
                    {
                        info.feed(url);
                        auto album = BAE::fixString(info.getAlbum());
                        auto track= info.getTrack();
                        auto title = BAE::fixString(info.getTitle()); /* to fix*/
                        auto artist = BAE::fixString(info.getArtist());
                        auto genre = info.getGenre();
                        auto sourceUrl = QFileInfo(url).dir().path();
                        auto duration = info.getDuration();
                        auto year = info.getYear();

                        BAE::DB trackMap =
                        {
                            {BAE::KEY::URL,url},
                            {BAE::KEY::TRACK,QString::number(track)},
                            {BAE::KEY::TITLE,title},
                            {BAE::KEY::ARTIST,artist},
                            {BAE::KEY::ALBUM,album},
                            {BAE::KEY::DURATION,QString::number(duration)},
                            {BAE::KEY::GENRE,genre},
                            {BAE::KEY::SOURCES_URL,sourceUrl},
                            {BAE::KEY::BABE, url.startsWith(BAE::YoutubeCachePath)?"1":"0"},
                            {BAE::KEY::RELEASE_DATE,QString::number(year)}
                        };

                        this->con->addTrack(trackMap);
                        newTracks++;
                        //                        emit trackReady(trackMap);
                        //                            while(this->wait){t.msleep(100);}
                        //                            this->wait=!this->wait;
                    }
                }else break;
            }
        }

        this->t.msleep(100);
        emit this->finished(newTracks);
        this->go = false;
    }

signals:
    void trackReady(BAE::DB track);
    void finished(int size);
    void collectionSize(int size);

private:
    QThread t;
    TagInfo info;
    bool go = false;
    bool wait = true;
    QStringList queue;
    CollectionDB *con;
};


#endif // FILELOADER_H
