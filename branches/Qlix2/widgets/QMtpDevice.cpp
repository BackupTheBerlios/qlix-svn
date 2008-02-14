#include "widgets/QMtpDevice.h"
//TODO retrieving files with NULL filenames
//TODO memory leaks
//TODO update model correctly

QMtpDevice::QMtpDevice(MtpDevice* in_device, MtpWatchDog* in_watchDog, 
                       QObject* parent):
                       _device(in_device),
                       _watchDog(in_watchDog),
                       _icon(QPixmap(":/pixmaps/miscDev.png"))
{ 
//  int* function (uint64_t, uint64_t const void* const) = ;
  _device->SetProgressFunction(progressWrapper, this);
  start(); 
}

QString QMtpDevice::Name() { return  _name; }
QString QMtpDevice::Serial() { return  _serial; }
QIcon QMtpDevice::Icon() { return _icon; }

void QMtpDevice::IssueCommand(GenericCommand* in_cmd)
{
  QMutexLocker locker(&_jobLock);
  _jobs.push_back(in_cmd);
  _noJobsCondition.wakeOne();
  qDebug() << "Issued command";
}

void QMtpDevice::run()
{
  lockusb();
  initializeDeviceStructures();
  findAndRetrieveDeviceIcon();
  unlockusb();
  emit Initialized(this);

  while (true)
  {
    bool ret = false;
    _jobLock.lock();
    while (_jobs.empty() )
      _noJobsCondition.wait(&_jobLock); 

    GenericCommand* currentJob = _jobs.dequeue();
    _jobLock.unlock();

    CommandCode type = currentJob->GetCommand();
    switch (type)
    {
      case Initialize:
      {
        delete currentJob;  
        break;
      }
      case SendFile:
      {
        SendFileCmd* sendCmd = (SendFileCmd*)currentJob;
        QString fullpath = sendCmd->Path;

        TagLib::FileRef tagFile(fullpath.toUtf8().data(), true,
                          TagLib::AudioProperties::Accurate);
        if (tagFile.isNull()) 
        {
          emit NotATrack(sendCmd);
          break;
        }
        qDebug() << "Syncing track with path: " << fullpath;
        syncTrack(tagFile, sendCmd->ParentID);
/*
 * TODO / FIXME this is funky logic, what is isValid()? 
        bool isTrack = (tagFile.file()->isValid());
        if (isTrack)
        {
          syncTrack(tagFile, sendCmd->ParentID);
        }
        else
        {
          //transfer as file?

        }
*/
        if (!ret)
        {
          delete sendCmd;
          break; 
        }
        else
        {


        }
      }
      case GetObj:
      {
        GetObjCmd* getObj = (GetObjCmd*) currentJob;
        ret = _device->Fetch(getObj->ID,  getObj->Path.toUtf8().data());
        //TODO update the model and delete memory
        break;
      }
      case CreateFSFolder:
      {
        CreateFSFolderCmd* createFolder = (CreateFSFolderCmd*) currentJob;
        qDebug() << "Got a create fs folder command path:" << createFolder->Path
        << " name: " << createFolder->Name;
        QDir temp(createFolder->Path);
        temp.mkdir(createFolder->Name);
        QFileInfo check (temp, createFolder->Name);
        ret = (check.exists() && check.isDir());
        if (ret)
          qDebug() << "Succes creating folder";
        else
          qDebug() << "Failed creating folder";
        //Todo update the model and delete memory
        break;
      }
    }
  }
}

void QMtpDevice::Progress(uint64_t const sent, uint64_t total)
{
  float percent = (float)sent/(float)total;
  count_t per = (count_t) (percent*100);
  QString blank = "";
  emit UpdateProgress(blank, per);
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
 * Initializes the base device which retrieves the device friendly name etc
 */
void QMtpDevice::initializeDeviceStructures()
{
  if (!_device)
    return;
  _device->Initialize();
  _name = QString::fromUtf8(_device->Name()); 
  _serial = QString::fromUtf8(_device->SerialNumber());
#ifdef QLIX_DEBUG
  qDebug() << "Discovered name to be: " << _name;
#endif
  _albumModel = new AlbumModel(_device);
  _dirModel = new DirModel(_device);
  _plModel = new PlaylistModel(_device);

  _sortedAlbums = new QSortFilterProxyModel();
  _sortedPlaylists = new QSortFilterProxyModel();
  _sortedFiles = new MtpDirSorter();
  _sortedAlbums->setDynamicSortFilter(true);
  _sortedFiles->setDynamicSortFilter(true);
  _sortedPlaylists->setDynamicSortFilter(true);

  _sortedAlbums->setSourceModel(_albumModel);
  _sortedPlaylists->setSourceModel(_plModel);
  _sortedFiles->setSourceModel(_dirModel);

} 

/*
 * Iterates over all the devices files and tries to find devIcon.fil
 */
void QMtpDevice::findAndRetrieveDeviceIcon()
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
    _device->Fetch(curFile->ID(), iconPath.toLatin1());
    QFile img_file(iconPath);
    if (img_file.exists())
    {
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
    else
      _icon = QIcon(QPixmap(":/pixmaps/miscDev.png"));
  }
}

/*
 * Returns the sorted AlbumModel
 */
QAbstractItemModel* QMtpDevice::GetAlbumModel() const
{
  return _albumModel;
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

void QMtpDevice::TransferTrack(QString inpath) 
{
  QFileInfo file(inpath);
  if (file.isDir())
    return;

  SendFileCmd* cmd= new SendFileCmd(inpath, 0, true);
  IssueCommand(cmd);
  qDebug() << "Attempting to transfer file: " << inpath;
}




/**
 * Transfers an object from device the passed location
*/
void QMtpDevice::TransferFrom(MTP::GenericObject* obj, QString filePath)
{
  GetObjCmd* cmd;
  switch (obj->Type())
  {
    case MtpTrack:
    case MtpFile:
    {
      QString filename;
      if (obj->Type() == MtpTrack)
      {
        MTP::Track* currentTrack= (MTP::Track*) obj;
        filename= QString::fromUtf8(currentTrack->FileName());
      }
      else
      {
        MTP::File* currentFile= (MTP::File*) obj;
        filename= QString::fromUtf8(currentFile->Name());
      }
      qDebug() << "check here: " << filePath;
      qDebug() << "and here: " << filename;
      filePath += QDir::separator() + filename;
      qDebug() << "Transfer here: " << filePath;
      qDebug() << "Or here: " << QDir::toNativeSeparators(filePath);
      cmd = new GetObjCmd(obj->ID(), filePath);
      IssueCommand(cmd);
      break;
    }
    case MtpAlbum:
    { 
      MTP::Album* album = (MTP::Album*) obj;
      MTP::Track* currentTrack;
      for (count_t i = 0; i < album->TrackCount(); i++)
      {
        currentTrack = album->ChildTrack(i);
        QString trackName = QString::fromUtf8(currentTrack->FileName());
        QString actualPath = filePath + QDir::separator() + trackName;
        cmd = new GetObjCmd (currentTrack->ID(), actualPath);
        IssueCommand(cmd);
      }
      break;
    }
    case MtpPlaylist:
    {
      MTP::Playlist* pl = (MTP::Playlist*) obj;
      MTP::Track* currentTrack;
      for (count_t i = 0; i < pl->TrackCount(); i++)
      {
        currentTrack = pl->ChildTrack(i);
        QString trackName = QString::fromUtf8(currentTrack->FileName());
        QString actualPath = filePath + QDir::separator() + trackName;
        cmd = new GetObjCmd (currentTrack->ID(), actualPath);
        IssueCommand(cmd);
      }
      break;
    }
    case MtpFolder:
    {
      MTP::Folder* rootFolder = (MTP::Folder*)obj;
      MTP::File* currentFile;
      MTP::Folder* currentFolder;

      QString folderName = QString::fromUtf8(rootFolder->Name());
      CreateFSFolderCmd* newFolderCmd = new CreateFSFolderCmd
                                            (filePath, folderName); 
      IssueCommand(newFolderCmd);

      QString subFolderPath = filePath + QDir::separator() + folderName;
      for (count_t i =0; i < rootFolder->FileCount(); i++)
      {
        currentFile = rootFolder->SubFile(i);
        QString subFilePath = subFolderPath + QDir::separator() + 
                              QString::fromUtf8(currentFile->Name());
        cmd = new GetObjCmd (currentFile->ID(), subFilePath);
        IssueCommand(cmd);
      }
      //recurse on all subfolders
      for (count_t i =0; i < rootFolder->FolderCount(); i++)
      {
        currentFolder = rootFolder->SubFolder(i);
        TransferFrom(currentFolder, subFolderPath);
      }
      break;
    }
    default:
      assert(false);
      break;
  }
}

int QMtpDevice::progressWrapper(uint64_t const sent, uint64_t const total, const void* const data)
{
//  cout << "Progress: " << (float)sent/(float)total << endl;
  if (!data)
    return 1;
  QMtpDevice const * const tempDevice = static_cast<const QMtpDevice* const> (data);
  QMtpDevice* thisDevice = const_cast<QMtpDevice* const> (tempDevice);
  thisDevice->Progress(sent, total);
  return 0;
}

void QMtpDevice::FreeSpace(uint64_t* total , uint64_t* free) 
{
  _device->FreeSpace(total, free);
}

/**
 * This function will sync a track to the device by looking up the track's
 * metadata and calling the TransferTrack function on the device.
 * @param in_path the path to the track on the host computer's filesystem
 * @param parent the parent id of this track, this should be the id of a
 *               folder where this track will reside in
 */
bool QMtpDevice::syncTrack(TagLib::FileRef tagFile, uint32_t parent)
{
//  _albumModel->rowsAboutToBeInserted(); //call this before insertion
//  _fileModel->rowsAboutToBeInserted();
  QString filePath = tagFile.file()->name();
  QFileInfo file(filePath);
  QString suffixStr = file.suffix().toUpper();
  char* suffix = strdup(suffixStr.toUtf8().data());
  QString filename = file.fileName();
  uint64_t size = (uint64_t) file.size();
  LIBMTP_filetype_t type = MTP::StringToType(suffix);
  delete suffix;

  MTP::Track* newTrack;
  MTP::File* newFile;
  newTrack = SetupTrackTransfer(tagFile, filename, size,
                                         parent, type);
  _albumModel->AddTrack(filePath, newTrack);

  //it is unclear how to handle this.. need more info
  //we are doing this only to maintain a sane object mapping 
  //newFile  = SetupFileTransfer(filename, size, parent, type); 


//FIXME this is a memory leak until we get these files into the object tree 
//and map but we must update the model first

return true;
}

/**
 * This function will sync a file to the device
 * @param in_path the path to the file on the host computer's filesystem
 * @param parent the parent id of this file, this should be the id of a
 *               folder
 */
bool QMtpDevice::syncFile(const QString& in_path, uint32_t parent)
{
//  _albumModel->rowsAboutToBeInserted(); //call this before insertion
//  _fileModel->rowsAboutToBeInserted();
  QFileInfo file(in_path);
  QString suffixStr = file.suffix().toUpper();

  char* suffix = suffixStr.toUtf8().data();
  char* filename = file.completeBaseName().toLocal8Bit().data();
  uint64_t size = (uint64_t) file.size();
  LIBMTP_filetype_t type = MTP::StringToType(suffix);

  MTP::File* newFile = SetupFileTransfer(filename, size, parent,
                                                  type); 
  return _device->TransferFile((const char*) in_path.toUtf8().data(), 
                                newFile);
}

/**
 * This function creates a MTP::File object filled with sane values
 * @param in_filename the name of the file
 * @param in_si the size of the file
 * @param in_parentid the parent folder of this object, if its 0, its on the
 *        root level
 * @param in_type the LIBMTP_filetype_t of file
 */
MTP::File* QMtpDevice::SetupFileTransfer(const char* in_filename, 
                                         uint64_t in_sz, 
                                         count_t in_parentid, 
                                         LIBMTP_filetype_t in_type)
{
  LIBMTP_file_t* file = LIBMTP_new_file_t();
  file->filename = strdup(in_filename);
  file->filesize = in_sz;
  file->parent_id = in_parentid;
  file->filetype = in_type;
  return new MTP::File(file);
}


MTP::Track* QMtpDevice::SetupTrackTransfer(TagLib::FileRef tagFile,
                                           const QString& in_filename,
                                           uint64_t in_size,
                                           uint32_t in_parentID, 
                                           LIBMTP_filetype_t in_type)
{
    TagLib::String unknownString = "Unknown";
    //Copy the album
    TagLib::String albumTag = tagFile.tag()->album();
    char* album;
    if (albumTag.isEmpty() || albumTag.upper() == TagLib::String("UNKNOWN"))
        album = strdup(unknownString.toCString(true));
    else
        album = strdup(albumTag.toCString(true));
    cout << "Album sanity check: " << album << endl;

    //Copy the title
    char* title;
    TagLib::String titleTag = tagFile.tag()->title();
    if (titleTag.isEmpty() || titleTag.upper() == TagLib::String("UNKNOWN"))
        title = strdup(unknownString.toCString(true));
    else
        title = strdup(titleTag.toCString(true));
    cout << "Title sanity check: " << title << endl;

    //Copy the artist
    char* artist;
    TagLib::String artistTag = tagFile.tag()->artist();
    if (artistTag.isEmpty() || artistTag.upper() == TagLib::String("UNKNOWN"))
        artist = strdup(unknownString.toCString(true));
    else
        artist = strdup(artistTag.toCString(true));
    cout << "Artist sanity check: " <<  artist << endl;

    //Copy the genre
    TagLib::String genreTag = tagFile.tag()->genre();
    char* genre;
    if (genreTag.isEmpty() || genreTag.upper() == TagLib::String("UNKNOWN"))
        genre =  strdup(unknownString.toCString(true));
    else
        genre = strdup(genreTag.toCString(true));
    cout<< "Genre sanity check: " << genre << endl;

    //Copy the filename
    //TODO why doesn't this work?
    char* filename = strdup(in_filename.toLocal8Bit().data());
    cout << "Filename sanity check: " << filename << endl;
    LIBMTP_track_t* newtrack = LIBMTP_new_track_t();
    cout << "File type sanity check: " << MTP::TypeToString(in_type) << endl;

    newtrack->parent_id = in_parentID;
    newtrack->title = title;
    newtrack->artist = artist;
    newtrack->genre = genre;
    newtrack->album = album;
    newtrack->filename= filename;
    newtrack->tracknumber = tagFile.tag()->track();
    newtrack->duration  = tagFile.audioProperties()->length()*1000;
    newtrack->bitrate   = tagFile.audioProperties()->bitrate();
    newtrack->filesize  = in_size;
    newtrack->filetype = in_type;
    newtrack->next = NULL;
    return new MTP::Track(newtrack);
}
