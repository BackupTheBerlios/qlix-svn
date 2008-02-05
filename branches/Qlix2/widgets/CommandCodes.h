//Copyright 2007 Ali Shah
//Qlix is distributed under the terms of the GNU General Public License

#ifndef __COMMANDCODES__
#define __COMMANDCODES__
#include "types.h"
#include <QString>
#include <QtDebug>
#include <QList>
/**
 * @file
 * This file will detail the thread level protocol between the GUI and the device
 */
namespace MTPCMD
{
enum CommandCode
{
    Initialize,
    SendFile,
    GetObj,
    CreateFSFolder
    /* Not implemented yet
    Connect,
    Disconnect,
    GetDeviceInfo,
    GetFile,
    Delete,
    GetDirMetaData,
    GetFileMetaData,
    GetSampleData,
    SendSampleData,
    CreateDirectory,
    TransferDeviceFolder,
    TransferSystemFolder
    */
}; 

struct GenericCommand 
{
  CommandCode ComCode;
  CommandCode GetCommand()  { return ComCode; }
};

struct SendFileCmd : GenericCommand
{
    QString Path;
    bool IsTrack;
    uint32_t ParentID;
    SendFileCmd (QString in_path, uint32_t in_parent, 
                        bool in_isTrack = false) 
    {
        Path = in_path;
        ParentID = in_parent;
        IsTrack = in_isTrack;
        ComCode = SendFile;
    }
};

struct GetObjCmd : GenericCommand
{
    QString Path;
    uint32_t ID;
    GetObjCmd (uint32_t file_id, const QString& in_path)
    {
        ID = file_id;
        Path = in_path;
        ComCode = GetObj;
    }
};

struct CreateFSFolderCmd: GenericCommand
{
    QString Path;
    QString Name;
    CreateFSFolderCmd(const QString& in_path, const QString& in_name)
    {
      Path = in_path;
      Name = in_name;
      ComCode = CreateFSFolder;
    }
};

/*
struct MtpDeviceInfo
{
    QString _serial;
    QString _modelName;
    QString _friendlyName;
    QString _syncPartner;
    ubyte _curBatteryLevel;
    ubyte _maxBatteryLevel;
};


struct MtpUpdate
{
    Code ComCode;
    bool Success;
    Code GetCommand()
    {
        return ComCode;
    }
bool isSuccess() {
        return Success;
    }
};

struct GetDeviceInfo : GenericCommand
{
    GetDeviceInfo ( void )
    {
        ComCode = GetDeviceInfo;
    }
};


struct MtpUpdateDeviceInfo
{
    //todo
    MtpDeviceInfo Info;
};

struct Connect : GenericCommand
{
    Connect (void )
    {
        ComCode = Connect;
    }
};

 MTPFS does not exist anymore
struct MtpUpdateConnect: MtpUpdate
{
    MtpFS* MtpFileSystem;
    MtpUpdateConnect(bool in_success, MtpFS* in_fs)
    {
        MtpFileSystem = in_fs;
        Success = in_success;
    }
};

struct Disconnect : GenericCommand
{
    Disconnect ( void )
    {
        ComCode= Disconnect;
    }
};



struct Delete : GenericCommand
{
    uint32_t FolderID;
    int FileID;
    Delete (uint32_t in_Folderid, int in_FileID)
    {
        FolderID = in_Folderid;
        FileID = in_FileID;
        ComCode = Delete;
    }
};


struct NewDirectory : GenericCommand
{
    uint32_t ParentID;
    QString Name;
    NewDirectory(const QString& in_name, uint32_t in_parent_id)
    {
        ComCode = CreateDirectory; 
        ParentID = in_parent_id;
        Name = in_name;
    }
};

 Not used: DirNode 
struct TransferSystemFolder : GenericCommand
{
    QFileInfoList Files;
    DirNode* Parent;
    QString DirName;
    TransferSystemFolder(QDir folder, DirNode* parent)
    {
        ComCode = TransferSystemFolder;
        DirName = folder.dirName();
        Parent = parent;
        Files = folder.entryInfoList();
    }
};

struct MtpUpdateDelete : MtpUpdate
{
    uint32_t FolderID;
    int Depth;
    MtpUpdateDelete (bool in_success, uint32_t in_Folderid, int in_depth) 
    {
        Success = in_success;
        FolderID = in_Folderid;
        Depth = in_depth;
    }
};


struct MtpUpdateTransfer : MtpUpdate
{
    uint32_t FolderID;
    MtpUpdateTransfer(bool in_success, uint32_t in_Folderid)
    {
        FolderID = in_Folderid;
        Success = in_success;
    }
};
struct MtpThreadData
{
    * Command;
};

*/
}
#endif
