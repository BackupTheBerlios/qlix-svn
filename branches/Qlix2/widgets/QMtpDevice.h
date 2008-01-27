#ifndef __QMTPDEVICE__
#define __QMTPDEVICE__
#include <QThread>
#include <QIcon>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QQueue>
#include <QFile>
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

  void IssueCommand (MtpCommand* in_command);
  QSortFilterProxyModel* GetAlbumModel() const;
  QSortFilterProxyModel* GetPlaylistModel() const;
  QSortFilterProxyModel* GetDirModel() const;

signals:
  void Initialized(QMtpDevice*);
  void TrackTransferComplete();

protected:
  void run();

private:

  void QMtpDevice::findAndRetreiveDeviceIcon();
  void QMtpDevice::initializeDeviceStructures();

  void lockusb();
  void unlockusb();
  MtpDevice* _device;

  MtpWatchDog* _watchDog;
  QIcon _icon;
  QString _name;
  QString _serial;

  QQueue <MtpCommand*> _jobs;
  QMutex _jobLock;
  QWaitCondition _noJobsCondition;

  AlbumModel* _albumModel;
  DirModel* _dirModel;
  PlaylistModel* _plModel;
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
