#ifndef __MTPOBJECT__
#define __MTPOBJECT__

#include <libmtp.h>
#include <vector>
#include <assert.h>
#include "types.h"
namespace MTP
{
/** 
 * @class Generic base class for other MTP object types
*/
class GenericObject
{
public:
  GenericObject(MtpObjectType, uint32_t);
  count_t ID();
  MtpObjectType Type();
private:
  MtpObjectType _type;
  count_t _id;
};


/** 
 * @class Track is a class that wraps around LIBMTP_track_t
*/
class Track : public GenericObject
{
public:
  Track(LIBMTP_track_t*);
  count_t ParentID();
private:
  LIBMTP_track_t* _rawTrack;
};

/** 
 * @class File is a class that wraps around LIBMTP_file_t
*/
class File : public GenericObject 
{
public:
  File(LIBMTP_file_t*, const LIBMTP_filesampledata_t&);
  const LIBMTP_filesampledata_t& SampleData() const;
  count_t ParentID() const;
private:
  LIBMTP_file_t* _rawFile;
  LIBMTP_filesampledata_t _sampleData;
};

/** 
 * @class Folder is a class that wraps around LIBMTP_folder_t
*/
class Folder : public GenericObject
{
public:
  Folder(LIBMTP_folder_t*, Folder*);
  count_t FileCount() const;
  count_t FolderCount() const;
  Folder* Parent() const;

  char* Name() const;
  Folder* SubFolder(count_t ) const;

  LIBMTP_folder_t* RawFolder() const;
  void AddChildFolder(Folder*);
  void AddChildFile(File*);
private:
  LIBMTP_folder_t* _rawFolder;
  Folder* _parent;
  std::vector<Folder*> _childFolders;
  std::vector<File*> _childFiles;
};

/** 
 * @class Album is a class that wraps around LIBMTP_album_t
*/
class Album: public GenericObject
{
public:
  Album(LIBMTP_album_t*);
  count_t TrackCount() const;
  void AddChildTrack(Track*);
private:
  count_t _trackCount;
  LIBMTP_album_t* _rawAlbum;
  std::vector <Track*> _childTracks;
};

/**
 * @class Playlist is a class that wraps around LIBMTP_playlist_t
*/
class Playlist: public GenericObject
{
public:
  Playlist(LIBMTP_playlist_t*);
  count_t TrackCount() const;

private:
  count_t _trackCount;
  LIBMTP_playlist_t* _rawPlaylist;
};

}

#endif

