#ifndef __ALBUMMODEL__
#define __ALBUMMODEL__
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QPixmap>
#include <mtp/MtpDevice.h>
#include "types.h"
#include "QtDebug"
#include "QIcon"
#include <libmtp.h>
#include <QFont>
/**
 * @class This class wraps over the MTP::Album structures and provides a
 * hierarchy that displays tracks underneath albums
 */
class AlbumModel : public QAbstractItemModel
{
Q_OBJECT
public:
  AlbumModel(MtpDevice*, QObject* parent = NULL);
  QModelIndex index(int, int, const QModelIndex& parent= QModelIndex()) const;
  QModelIndex parent (const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent= QModelIndex() ) const ;
  int columnCount(const QModelIndex& parent= QModelIndex() ) const;
  QVariant data(const QModelIndex& , int role = Qt::DisplayRole ) const;
private:
  MtpDevice* _device;
};
#endif