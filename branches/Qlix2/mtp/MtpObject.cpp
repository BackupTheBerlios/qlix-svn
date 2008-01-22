//TODO error checkking when returning char* ?
#include "mtp/MtpObject.h"
using namespace MTP;
namespace MTP
{

/** Creates a new GenericObject, a generic base class for MTP objects
 * @param in_type the MTP type of the new GenericObject to be created
 * @return a new GenericObject
 */
GenericObject::GenericObject(MtpObjectType in_type, uint32_t id) :
                             _type(in_type),
                             _id(id)
{ }

/** 
 * Returns the ID of this object
 */
count_t GenericObject::ID() { return _id; }

/** Simple function to get the type of the current MTP object
 * @return returns the type of the GenericObject
 */
MtpObjectType GenericObject::Type() { return _type; }


/** Creates a new Track object
 * @param in_track A pointer to the LIBMTP_track_t to wrap over
 * @return a new Track object
 */
Track::Track(LIBMTP_track_t* in_track): 
            GenericObject(MtpTrack, in_track->item_id)
{
  assert(in_track);
  _rawTrack = in_track;
}

count_t Track::ParentID() { return _rawTrack->parent_id; }


/** Creates a new File object and and stores its representative data
 * @param in_file A pointer to the LIBMTP_file_to wrap over
 * @param in_sample A pointer to the LIBMTP_filesampledata_t
 * @return a new File object
 */
File::File(LIBMTP_file_t* in_file, const LIBMTP_filesampledata_t& in_sample) : 
           GenericObject (MtpFile, in_file->item_id)
{
  _rawFile = in_file;
  _sampleData = in_sample;
}


/**
 * Retreives the file's parent ID
 * @return the file's parent ID it is zero if its in the root folder 
 */
count_t File::ParentID() const { return _rawFile->parent_id; }

/**
 * Retreives the files sample data
 * @return the file's sample data
 */
const LIBMTP_filesampledata_t& File::SampleData() const
{
  return _sampleData;
}

char const * File::Name() const
{
  //TODO error checking here?
  return _rawFile->filename;
}

/** Creates a new Folder object
 * @param in_folder A pointer to the LIBMTP_folder_t wrap over
 * @param in_parent A pointer to this folder's parent
 * @return a new Folder object
 */
Folder::Folder(LIBMTP_folder_t* in_folder, Folder* in_parent) :
               GenericObject (MtpFolder, in_folder->folder_id)
{
  assert(in_folder);
  _rawFolder = in_folder;
  _parent = in_parent;
}
/**
 * Return's this folder's parent
 * @return the folder's parent
 */
Folder* Folder::Parent() const
{
  return _parent;
}

/**
 * Get the raw LIBMTP folder
 * @return the raw LIBMTP folder for this Folder object
 */
LIBMTP_folder_t* Folder::RawFolder() const
{
  return _rawFolder;
}

/**
 * Returns the subfolder at the given index if it exists.
 * @return the subfolder at the given index or NULL if it doesn't exist
 */
Folder* Folder::SubFolder(count_t idx) const
{
  if (idx >= _childFolders.size())
    return NULL;
  else
    return _childFolders[idx];
}

/**
 * Returns the number of files under this folder
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

/** Retreives the name of the wrapped folder or "" if it doesn't exist
 * @return the folder's UTF8 name or a blank string if it doesn't exist
 */
char* Folder::Name() const
{
  if (!_rawFolder)
    return "";
  else
    return _rawFolder->name;
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
Album::Album(LIBMTP_album_t* in_album) : 
             GenericObject (MtpAlbum, in_album->album_id)
{
  assert(in_album);
  _rawAlbum = in_album;
}

/** Adds the passed track as s subtrack to this album
 * @param in_track the track to ass as a subtrack to this folder
 */
void Album::AddChildTrack(Track* in_track) 
{
   _childTracks.push_back(in_track); 
}


}
/** Creates a new Playlist object
 * @param in_pl t pointer to the LIBMTP_playlist_t wrap over
 * @return a new Playlist object
 */
Playlist::Playlist(LIBMTP_playlist_t* in_pl) : 
                  GenericObject(MtpPlaylist, in_pl->playlist_id)
{
  _rawPlaylist =  in_pl;
}
