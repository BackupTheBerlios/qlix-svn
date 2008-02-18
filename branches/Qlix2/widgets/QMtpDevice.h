#ifndef __QMTPDEVICE__
#define __QMTPDEVICE__
#include <QThread>
#include <QIcon>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QQueue>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfo>
#include <QModelIndex>
#include <QSortFilterProxyModel>

#include "mtp/MtpDevice.h"
#include "widgets/MtpWatchDog.h"
#include "widgets/CommandCodes.h"
#include "widgets/AlbumModel.h"
#include "widgets/PlaylistModel.h"
#include "widgets/DirModel.h"

#include "mtp/Icon.h"
#include "mtp/BmpStructs.h"
#include <tag.h>
#include <fileref.h>
#include <tfile.h>
using namespace MTPCMD;

class MtpWatchDog;
/*
 * A threaded version of MtpDevice representing device attributes using QT 
 * objects and models
 */
class QMtpDevice : public QThread
{
  Q_OBJECT
public:
  QMtpDevice(MtpDevice*, MtpWatchDog*, QObject* parent = NULL);
  QString Name();
  QString Serial();
  QIcon Icon();

  void TransferTrack(QString filepath);
  void TransferFrom(MTP::GenericObject*, QString );

  void IssueCommand (GenericCommand* in_command);
  QSortFilterProxyModel* GetAlbumModel() const;
  QSortFilterProxyModel* GetPlaylistModel() const;
  QSortFilterProxyModel* GetDirModel() const;
  void Progress(uint64_t const sent, uint64_t const total);
  void FreeSpace(uint64_t* , uint64_t*);

signals:
  void Initialized(QMtpDevice*);
  void TrackTransferComplete(bool success, GenericCommand*);
  void NotATrack(SendFileCmd*);
  void UpdateProgress(QString, count_t);

protected:
  void run();

private:

  void findAndRetrieveDeviceIcon();
  void initializeDeviceStructures();

  MTP::Track* SetupTrackTransfer(TagLib::FileRef tagFile, const QString&, 
                                 uint64_t, uint32_t,  LIBMTP_filetype_t);

  MTP::File* SetupFileTransfer(const char*,  uint64_t,  count_t, 
                               LIBMTP_filetype_t);

  bool syncTrack(TagLib::FileRef, uint32_t parent); 
  bool syncFile(const QString& path, uint32_t parent);
  bool syncFile();

  void lockusb();
  void unlockusb();
  MtpDevice* _device;

  MtpWatchDog* _watchDog;
  QIcon _icon;
  QString _name;
  QString _serial;

  QQueue <GenericCommand*> _jobs;
  QMutex _jobLock;
  QWaitCondition _noJobsCondition;

  AlbumModel* _albumModel;
  DirModel* _dirModel;
  PlaylistModel* _plModel;

  static int progressWrapper(uint64_t const sent, uint64_t const total, 
                             const void* const data);
  /**
   * A private class to manage sorting of the Directory model
   * it sorts directories before files.
   */
  class MtpDirSorter  : public QSortFilterProxyModel
  {
  public:
    MtpDirSorter(QObject* parent = NULL) : QSortFilterProxyModel(parent) { }
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const
    {
      MTP::GenericObject* leftobj = (MTP::GenericObject*) left.internalPointer();
      MTP::GenericObject* rightobj = (MTP::GenericObject*) right.internalPointer();
      if (leftobj->Type() == MtpFolder && rightobj->Type() == MtpFolder)
      {
        MTP::Folder* leftFolder = (MTP::Folder*) leftobj;
        MTP::Folder* rightFolder = (MTP::Folder*) rightobj;
        return ( QString::fromUtf8(leftFolder->Name()  ) < 
                 QString::fromUtf8(rightFolder->Name() ) );
      }
      else if (leftobj->Type() == MtpFolder && rightobj->Type() == MtpFile)
        return true;
      else if (leftobj->Type() == MtpFile && rightobj->Type() == MtpFolder)
        return false;
      else if (leftobj->Type() == MtpFile && rightobj->Type() == MtpFile)
      {
        MTP::File* leftFile = (MTP::File*) leftobj;
        MTP::File* rightFile = (MTP::File*) rightobj;
        return ( QString::fromUtf8(leftFile->Name()  ) < 
                 QString::fromUtf8(rightFile->Name() ) );
       }
       assert(false);
      }
    };
    MtpDirSorter* _sortedFiles;
    QSortFilterProxyModel* _sortedAlbums;
    QSortFilterProxyModel* _sortedPlaylists;

};
#endif
