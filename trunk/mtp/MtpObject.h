#ifndef MTP_OBJECT
#define MTP_OBJECT
/*
 *   Copyright (C) 2008 Ali Shah <caffein@gmail.com>
 *
 *   This file is part of the Qlix project on http://berlios.de
 *
 *   This file may be used under the terms of the GNU General Public
 *   License version 2.0 as published by the Free Software Foundation
 *   and appearing in the file COPYING included in the packaging of
 *   this file.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License version 2.0 for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *   TODO error checking when returning char* ?
 *   TODO use multimap for track/file distinction
 *        RESOLVED by Association() parameter (I think)
 *   TODO Figure out if we need to actually implement removeFromRawPlaylist/album/folder etc
 *      RESOLVED: Yes we do- we call removeFromRawPlaylist() then update the device by reuploading
 *      the raw container
 *      However removeFromRawFolder does not need to be implemented because of lack of async
 *      method calls available in libmtp to update teh raw folder containers
 */

#include "GenericObjects.h"
#include "File.h"
#include "Folder.h"
#include "Track.h"
#include "ShadowTrack.h"
#include "Album.h"
#include "Playlist.h"
#endif
