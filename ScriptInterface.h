#ifndef SCRIPTINTERFACE_H
#define SCRIPTINTERFACE_H

#include <QObject>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include <QTextStream>

class ScriptInterface : public QObject
{
    Q_OBJECT
public:
    explicit ScriptInterface(QObject *parent = nullptr);

    // Getter
    Q_INVOKABLE int GetCurRoomLayerWidth(int layerId);
    Q_INVOKABLE int GetCurRoomLayerHeight(int layerId);
    Q_INVOKABLE int GetCurRoomTile16(int layerID, int x, int y);
    Q_INVOKABLE int GetCurRoomTile8(int layerID, int x, int y);
    Q_INVOKABLE int GetRoomNum();
    Q_INVOKABLE int GetCurRoomId();

    // Test
    Q_INVOKABLE void _DecompressData(int mappingtype, int address);
    Q_INVOKABLE unsigned int _GetLayerDecomdataPointer(int layerId);
    Q_INVOKABLE void _PrintRoomHeader();
    Q_INVOKABLE void _ExportLayerData(QString filePath = QString(""), int layerid = -1);
    Q_INVOKABLE void _ImportLayerData(QString fileName = QString(""), int layerid = -1);
    Q_INVOKABLE void _ExportEntityListData(QString filePath = QString(""), int entitylistid = -1);
    Q_INVOKABLE void _ImportEntityListData(QString filePath = QString(""), int entitylistid = -1);

    // Setter
    Q_INVOKABLE void SetCurrentRoomId(int roomid);
    Q_INVOKABLE void SetCurRoomTile16(int layerID, int TileID, int x, int y);
    Q_INVOKABLE void SetRoomSize(int roomwidth, int roomheight, int layer0width, int layer0height);

    // Localize JS function
    Q_INVOKABLE void alert(QString message);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void log(QString message);
    Q_INVOKABLE QString prompt(QString message, QString defaultInput);

    // UI
    Q_INVOKABLE void UpdateRoomGFXFull();

    // File operations
    Q_INVOKABLE void WriteTxtFile(QString filepath, QString test);
    Q_INVOKABLE QString ReadTxtFile(QString filepath);

signals:

};

#endif // SCRIPTINTERFACE_H
