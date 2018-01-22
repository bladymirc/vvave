import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "../view_models"
import "../utils/Icons.js" as MdiFont
import "../utils"
import "../db/Queries.js" as Q

BabeGrid
{
    id:artistsViewGrid
    visible: true
    property int hintSize : Math.sqrt(root.width*root.height)*0.25
    albumSize:
    {
        if(hintSize>200)
            200
        else if (hintSize<150)
            bae.isMobile() && hintSize < 120 ? 120 : 150
        else
            hintSize

    }

    signal rowClicked(var track)
    signal playAlbum(var tracks)
    signal playTrack(var track)
    signal queueTrack(var track)
    signal appendAlbum(var tracks)

    transform: Translate
    {
        y: (drawer.position * artistsViewGrid.height * 0.33)*-1
    }

    onBgClicked: if(drawer.visible) drawer.close()
    onFocusChanged:  drawer.close()

    Drawer
    {
        id: drawer
        height: parent.height * 0.4
        width: parent.width
        edge: Qt.BottomEdge
        interactive: false
        focus: true
        modal: bae.isMobile()
        dragMargin: 0
        clip: true

        background: Rectangle
        {
            anchors.fill: parent
            z: -999
            color: bae.altColor()
        }

        Column
        {
            anchors.fill: parent

            BabeTable
            {
                id: drawerList
                width: parent.width
                height: parent.height
                trackNumberVisible: true
                quickBtnsVisible: true
                headerBar: true
                onRowClicked:
                {
                    drawer.close()
                    artistsViewGrid.rowClicked(model.get(index))
                }

                onQuickPlayTrack:
                {
                    drawer.close()
                    artistsViewGrid.playTrack(model.get(index))
                }

                onQueueTrack:
                {
                    drawer.close()
                    artistsViewGrid.queueTrack(model.get(index))
                }

                onPlayAll:
                {
                    drawer.close()
                    var data = artistsViewGrid.gridModel.get(artistsViewGrid.grid.currentIndex)

                    var query = Q.Query.artistTracks_.arg(data.artist)
                    var tracks = bae.get(query)
                    artistsViewGrid.playAlbum(tracks)
                }

                onAppendAll:
                {
                    var data = artistsViewGrid.gridModel.get(artistsViewGrid.grid.currentIndex)
                    var query = Q.Query.artistTracks_.arg(data.artist)
                    var tracks = bae.get(query)
                    artistsViewGrid.appendAlbum(tracks)
                    drawer.close()
                }
            }
        }
    }

    onAlbumCoverClicked:
    {
        drawerList.headerTitle = artist
        drawer.open()
        drawerList.clearTable()
        var query = Q.Query.artistTracks_.arg(artist)
        var map = bae.get(query)

        for(var i in map)
            drawerList.model.append(map[i])

    }

    function populate()
    {
        var map = bae.get(Q.Query.allArtistsAsc)
        for(var i in map)
            gridModel.append(map[i])
    }

    Component.onCompleted: populate()

}
