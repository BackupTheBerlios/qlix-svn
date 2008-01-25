#include "widgets/QMtpDevice.h"

QMtpDevice::QMtpDevice(MtpDevice* in_device, MtpWatchDog* in_watchDog, 
                       QObject* parent):
                       _device(in_device),
                       _watchDog(in_watchDog),
                       _icon(QPixmap(":/pixmaps/miscDev.png"))
{ 
  start(); 
}

QString QMtpDevice::Name()
{ return  _name; }

QIcon QMtpDevice::Icon()
{ return _icon; }

void QMtpDevice::IssueCommand(MtpCommand* in_cmd)
{
  QMutexLocker locker(&_jobLock);
  _jobs.push_back(in_cmd);
}

void QMtpDevice::run()
{
  lockusb();
  initializeDeviceStructures();
  findAndRetreiveDeviceIcon();
  unlockusb();
  emit Initialized(this);

  while (true)
  {
    _jobLock.lock();
    while (_jobs.empty() )
      _noJobsCondition.wait(&_jobLock); 

    MtpCommand* currentJob = _jobs.dequeue();
    _jobLock.unlock();

    MtpCommandCode type = currentJob->GetCommand();
    switch (type)
    {
      case Initialize:
         break;
    }
  }
}

void QMtpDevice::lockusb()
{
  assert(_watchDog);
  _watchDog->Lock();
}

void QMtpDevice::unlockusb()
{
  assert(_watchDog);
  _watchDog->Unlock();
}

/*
 * Initializes the base device which retreives the device friendly name etc
 */
void QMtpDevice::initializeDeviceStructures()
{
  if (!_device)
    return;
  _device->Initialize();
  _name = QString::fromUtf8(_device->Name()); 
#ifdef QLIX_DEBUG
  qDebug() << "Discovered name to be: " << _name;
#endif
  _albumModel = new AlbumModel(_device);
  _dirModel = new DirModel(_device);
  _plModel = new PlaylistModel(_device);

  _sortedAlbums = new QSortFilterProxyModel();
  _sortedPlaylists = new QSortFilterProxyModel();
  _sortedFiles = new MtpDirSorter();

  _sortedAlbums->setSourceModel(_albumModel);
  _sortedPlaylists->setSourceModel(_plModel);
  _sortedFiles->setSourceModel(_dirModel);

} 

/*
 * Iterates over all the devices files and tries to find devIcon.fil
 */
void QMtpDevice::findAndRetreiveDeviceIcon()
{
  count_t fileCount = _device->FileCount();
  count_t thread_id = (int)this;
  QString iconPath = QString("/tmp/%1Icon").arg(thread_id); 
  MTP::File* curFile = NULL;
  for (count_t i = 0; i < fileCount; i++)
  {
    curFile = _device->File(i);
    QString name = QString::fromUtf8(curFile->Name());
    name = name.toLower();
    if (name == "devicon.fil")
      break;
  }
  if (curFile)
  {
    QPixmap image;
    _device->Retreive(curFile->ID(), iconPath.toLatin1());
    QFile img_file(iconPath);
    img_file.open(QFile::ReadOnly);
    QByteArray buffer = img_file.readAll();
    DeviceIcon devIcon(buffer);
    if (devIcon.IsValid())
    {
        size_t temp = devIcon.GetBestImageSize();
        char buf[temp];
        devIcon.Extract(buf);
        Dimensions dim = devIcon.GetDimensions();
        QImage tempImage( (uchar*)buf, dim.Width, dim.Height, QImage::Format_ARGB32);
        image = (QPixmap::fromImage(tempImage));
    }
    _icon = QIcon(QPixmap(image));
  }
}

/*
 * Returns the sorted AlbumModel
 */
QSortFilterProxyModel* QMtpDevice::GetAlbumModel() const
{
  return _sortedAlbums;
}
/*
 * Returns the sorted DirModel 
 */
QSortFilterProxyModel* QMtpDevice::GetDirModel() const
{
  return _sortedFiles;
}

/*
 * Returns the sorted PlaylistModel 
 */
QSortFilterProxyModel* QMtpDevice::GetPlaylistModel() const
{
  return _sortedPlaylists;
}

