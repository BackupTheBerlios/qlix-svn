#include "mtp/MtpObject.h"
using namespace MTP;
namespace MTP
{

/** Creates a new GenericObject, a generic base class for MTP objects
 * @param in_type the MTP type of the new GenericObject to be created
 * @return a new GenericObject
 */
GenericObject::GenericObject(MtpObjectType in_type) : _type(in_type) { }

/** Simple function to get the type of the current MTP object
 * @return returns the type of the GenericObject
 */
MtpObjectType GenericObject::GetType() { return _type; }


/** Creates a new Track object
 * @param in_track A pointer to the LIBMTP_track_t to wrap over
 * @return a new Track object
 */
Track::Track(LIBMTP_track_t* in_track): GenericObject(MtpTrack)
{
  assert(in_track);
  _rawTrack = in_track;
}

/**
 * @return the Mtp ID of the File
 */
count_t Track::GetID()
{
  assert(_rawTrack);
  return _rawTrack->item_id;
}


/** Creates a new File object and and stores its representative data
 * @param in_file A pointer to the LIBMTP_file_to wrap over
 * @param in_sample A pointer to the LIBMTP_filesampledata_t
 * @return a new File object
 */
File::File(LIBMTP_file_t* in_file, LIBMTP_filesampledata_t* in_sample) : 
           GenericObject (MtpFile)
{
  assert(in_file);
  _rawFile = in_file;
  _sampleData = in_sample;
}
/**
 * Deletes the sample data if its exists
 */
void File::DeleteSampleData()
{
  if (_sampleData)
  {
    LIBMTP_destroy_filesampledata_t(_sampleData);
    _sampleData = NULL;
  }
}

/**
 * @return the Mtp ID of the File
 */
count_t File::GetID()
{
  assert(_rawFile);
  return _rawFile->item_id;
}

/** Creates a new Folder object
 * @param in_folder A pointer to the LIBMTP_folder_t wrap over
 * @return a new Folder object
 */
Folder::Folder(LIBMTP_folder_t* in_folder) : GenericObject (MtpFolder)
{
  assert(in_folder);
  _rawFolder = in_folder;
}
/**
 * @return the number of files under this folder
 */
count_t Folder::FileCount() const
{
  return _childFiles.size();
}

/**
 * @return the number of folders under this folder
 */
count_t Folder::FolderCount() const
{
  return _childFolders.size();
}

/**
 * @return the Mtp ID of the Folder 
 */
count_t Folder::GetID()
{
  assert(_rawFolder);
  return _rawFolder->folder_id;
}

/** Adds the passed folder as a subdirectory to this folder
 * @param in_folder the folder to add as a subfolder of this folder
 */
void Folder::AddChildFolder(Folder* in_folder)
{
  _childFolders.push_back(in_folder);
}

/** Adds the passed file as a leaf file of this
 * @param in_file the file to add as a leaf of this folder
 */
void Folder::AddChildFile(File* in_file)
{
  _childFiles.push_back(in_file);
}

/** Creates a new Album object
 * @param in_album A pointer to the LIBMTP_album_t wrap over
 * @return a new Album object
 */
Album::Album(LIBMTP_album_t* in_album) : GenericObject (MtpAlbum)
{
  assert(in_album);
  _rawAlbum = in_album;
}

/**
 * @return the Mtp ID of the Album 
 */
count_t Album::GetID()
{
  assert(_rawAlbum);
  return _rawAlbum->album_id;
}

}
/** Creates a new Playlist object
 * @param in_pl t pointer to the LIBMTP_playlist_t wrap over
 * @return a new Playlist object
 */
Playlist::Playlist(LIBMTP_playlist_t* in_pl) : GenericObject(MtpPlaylist)
{
  _rawPlaylist =  in_pl;
}

/** 
 * @return the Mtp id of the Playlist
 */
count_t Playlist::GetID()
{
  assert(_rawPlaylist);
  return _rawPlaylist->playlist_id;
}


