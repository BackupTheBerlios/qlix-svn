//TODO figure out why we have to subtract -1 from data
#include "widgets/AlbumModel.h"
/**
 * Construct a new AlbumModel
 */
AlbumModel::AlbumModel(MtpDevice* in_dev, QObject* parent) :
                       _device(in_dev)
{ }


/**
 * Return the Album or track at the given index and parent 
 * row and col are realtive to parent
 * The parent's internal pointer should only be of types MtpTrack and MtpAlbum
 * @param row the row coordinate of the item to display
 * @param col the column coordinate of the item to display
 * @param parent the parent of the object
 * @return an index constructed of the item to display or an invalid index if
 * the request coordinates are out of bounds
 */
QModelIndex AlbumModel::index(int row, int col, 
                        const QModelIndex& parent) const
{ 
  if(!parent.isValid() && (row >= (int)_device->AlbumCount()))
    return QModelIndex();

  else if (!parent.isValid() )
  {
    MTP::Album* album = _device->Album(row);
    return createIndex(row, col, (void*) album);
  }

  MTP::GenericObject* obj= (MTP::GenericObject*)parent.internalPointer();
  assert(obj->Type() == MtpAlbum);

  MTP::Album* album = (MTP::Album*) obj;

  if (row >= (int)album->TrackCount())
    return QModelIndex();

  MTP::Track* track = album->ChildTrack(row);
  assert (track->Type() == MtpTrack);

  return createIndex(row, col, track);
}

/** 
 * Returns the parent of the given index
 * @param idx the index of whose parent we must create
 * @return an index constructured of parent the passed paramameter idx or an
 * invalid index if the parent is the root level
 */
QModelIndex AlbumModel::parent(const QModelIndex& idx) const
{
  if (!idx.isValid())
    return QModelIndex();

  MTP::GenericObject* obj=(MTP::GenericObject*) idx.internalPointer();

  if(obj->Type() == MtpTrack)
  {
    MTP::Album* parent = ((MTP::Track*)obj)->ParentAlbum();
    QModelIndex ret = index((int)parent->rowid-1, 0, QModelIndex()); 

    return ret;
  }
  else if (obj->Type() == MtpAlbum)
    return QModelIndex();
  else
  {
    qDebug() << "object is of type: " << obj->Type();
    qDebug() << "Requesting row: "<< idx.row() << "column: " << idx.column() << "of object " << (void*)obj;
    assert(false);
  }
  return QModelIndex();
}

/**
 * Returns the row count of the parent this should be the number of tracks 
 * under an album or 0 if the parent happens to be a track
 * @param parent the parent item whos row counts we are trying to discover
 * @return the number of rows that occur under the given parameter: parent
 */ 
int AlbumModel::rowCount(const QModelIndex& parent) const 
{ 
  if (!parent.isValid() )
    return _device->AlbumCount();
  MTP::GenericObject* obj= (MTP::Album*)parent.internalPointer();
  if(obj->Type() == MtpTrack)
    return 0;
  else if (obj->Type() == MtpAlbum)
    return ((MTP::Album*)obj)->TrackCount();
  else
  {
    qDebug() << "invalid reference of type: " << obj->Type();
    qDebug() << "Requesting row: "<< parent.row() << "column: " << parent.column() << "of object " << (void*)obj;
    assert(false);
  }
}
/** Return the column count at the given parent index, 2 seemed reasonable 
 * at the current time
 * @param parent the index whos column count we are trying to discover
 * @return the number of colums that occur beside the given parent
 */
int AlbumModel::columnCount(const QModelIndex& parent ) const 
{ 
    return 2;
}
/**
 * Returns the data to display at the given index and the role
 * @param index the index of the item to display
 * @param role the role this data will be used for
 */
QVariant AlbumModel::data(const QModelIndex& index, int role ) const
{ 
  if (role == Qt::DisplayRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpAlbum && index.column() == 0)
    {
        MTP::Album* tempAlbum = (MTP::Album*)temp;
        QString first = QString::fromUtf8(tempAlbum->Name());
        QString second = QString('\n') + QString("    ") + QString::fromUtf8(tempAlbum->Artist());
#ifdef QLIX_DEBUG
        qDebug() << "Returning for artist + album combo : " << first + second;
#endif
        return (first);
    }
    else if (temp->Type() == MtpTrack && index.column() == 0)
    {
        MTP::Track* tempTrack = (MTP::Track*)temp;
        QString temp = QString::fromUtf8(tempTrack->Name());
        return temp;
    }
    return QVariant();
  }
  if (role == Qt::DecorationRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpAlbum && index.column() == 0)
    {
        MTP::Album* tempAlbum = (MTP::Album*)temp;
        LIBMTP_filesampledata_t sample = tempAlbum->SampleData();
        if (sample.filetype == LIBMTP_FILETYPE_UNKNOWN)
        {
          QPixmap ret;
          if (sample.size > 0 && sample.data)
          {
            ret.loadFromData( (const uchar*)sample.data, sample.size);
            return ret.scaledToWidth(24, Qt::SmoothTransformation);
          }
          ret.load(":/pixmaps/miscAlbumCover.png");

          return ret.scaledToWidth(24, Qt::SmoothTransformation);
         // return ret;
        }
        else
          qDebug() << "album decoration is not a jpeg:" << sample.filetype;
    }
    else if (temp->Type() == MtpTrack && index.column() == 0)
    {
        return QIcon(QPixmap (":/pixmaps/track.png"));
    }
    else
      return QVariant();
  }
  if (role == Qt::SizeHintRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpAlbum && index.column() == 0)
      return QSize(26, 26);
  }
  if (role == Qt::FontRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    //Its an album
    if (temp->Type() == MtpAlbum && index.column() == 0)
    {
      QFont temp;
      temp.setBold(true);
      temp.setPointSize(8);
      return temp;
    }
  }
  return QVariant();
}
