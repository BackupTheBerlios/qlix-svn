#ifndef __DIRMODEL__
#define __DIRMODEL__
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QPixmap>
#include <mtp/MtpDevice.h>
#include "types.h"
#include "QtDebug"
#include "QIcon"
#include "QFont"
#include <libmtp.h>
class DirModel : public QAbstractItemModel
{
Q_OBJECT
public:
  DirModel(MtpDevice*, QObject* parent = NULL);
  QModelIndex index(int, int, const QModelIndex& parent= QModelIndex()) const;
  QModelIndex parent (const QModelIndex& index) const;
  int rowCount(const QModelIndex& parent= QModelIndex() ) const ;
  int columnCount(const QModelIndex& parent= QModelIndex() ) const;
  QVariant data(const QModelIndex& , int role = Qt::DisplayRole ) const;
private:
  MtpDevice* _device;
};
#endif
