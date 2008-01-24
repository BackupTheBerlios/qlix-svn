//TODO figure out why we have to subtract -1 from data
#include "widgets/AlbumModel.h"

AlbumModel::AlbumModel(MtpDevice* in_dev, QObject* parent) :
                       _device(in_dev)
{ }

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

QModelIndex AlbumModel::parent(const QModelIndex& idx) const
{
  if (!idx.isValid())
    return QModelIndex();

  MTP::GenericObject* obj=(MTP::GenericObject*) idx.internalPointer();

  if(obj->Type() == MtpTrack)
  {
    MTP::Album* parent = ((MTP::Track*)obj)->Parent();
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

int AlbumModel::columnCount(const QModelIndex& parent ) const 
{ 
    return 2;
}

QVariant AlbumModel::data(const QModelIndex& index, int role ) const
{ 
  if (role == Qt::DisplayRole)
  {
    MTP::GenericObject* temp = (MTP::GenericObject*) index.internalPointer();
    if (temp->Type() == MtpAlbum && index.column() == 0)
    {
        MTP::Album* tempAlbum = (MTP::Album*)temp;
        return QString::fromUtf8(tempAlbum->Name());
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
            return ret.scaledToWidth(32, Qt::SmoothTransformation);
          }
          ret.load(":/pixmaps/miscAlbumCover.png");
          return ret;
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
      return QSize(40, 40);
  }
  return QVariant();
}
