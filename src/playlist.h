/*
|  Copyright (C) 2002 Adrian Ulrich <pab at blinkenlights.ch>
|                2002-2003 Jörg Schuler  <jcsjcs at users.sourceforge.net>
|
|  Part of the gtkpod project.
|
|  URL: http://gtkpod.sourceforge.net/
|
|  This program is free software; you can redistribute it and/or modify
|  it under the terms of the GNU General Public License as published by
|  the Free Software Foundation; either version 2 of the License, or
|  (at your option) any later version.
|
|  This program is distributed in the hope that it will be useful,
|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|  GNU General Public License for more details.
|
|  You should have received a copy of the GNU General Public License
|  along with this program; if not, write to the Free Software
|  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
|
|  iTunes and iPod are trademarks of Apple
|
|  This product is not supported/written/published by Apple!
|
|  $Id$
*/

#ifndef __PLAYLIST_H__
#define __PLAYLIST_H__


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "itunesdb.h"
#include "track.h"


void create_mpl (void);
Playlist *add_playlist (Playlist *plitem, gint position);
Playlist *add_new_playlist (gchar *plname, gint position, gboolean spl);
void pl_free(Playlist *playlist);
Playlist *it_add_playlist (Playlist *plitem);
void randomize_playlist (Playlist *pl);
void it_add_trackid_to_playlist (Playlist *plitem, guint32 id);
void add_trackid_to_playlist (Playlist *plitem, guint32 id, gboolean display);
void add_track_to_playlist (Playlist *plitem, Track *track, gboolean display);
gboolean remove_trackid_from_playlist (Playlist *plitem, guint32 id);
gboolean remove_track_from_playlist (Playlist *plitem, Track *track);
gboolean track_is_in_playlist (Playlist *plitem, Track *track);
guint32 track_is_in_playlists (Track *track);
void move_playlist (Playlist *playlist, gint pos);
void remove_playlist (Playlist *playlist);
void remove_all_playlists (void);
guint remove_playlist_by_name(gchar *pl_name);
gboolean playlist_exists (Playlist *pl);
#define it_get_nr_of_playlists get_nr_of_playlists
guint32 get_nr_of_playlists (void);
#define it_get_playlist_by_nr get_playlist_by_nr
Playlist *get_playlist_by_nr (guint32 n);
guint get_playlist_by_name(gchar *pl_name, guint startfrom);
Playlist* get_newplaylist_by_name (gchar *pl_name, gboolean spl);
Playlist *get_playlist_by_id (guint64 id);

Playlist *pl_new (gchar *plname, gboolean spl);
Playlist *pl_duplicate (Playlist *pl);
void pl_copy_spl_rules (Playlist *dest, Playlist *src);

void splr_remove (Playlist *pl, SPLRule *splr);
SPLRule *splr_new (void);
void splr_add (Playlist *pl, SPLRule *splr, gint pos);
SPLRule *splr_add_new (Playlist *pl, gint pos);
void spl_update (Playlist *spl, GList *tracks);
void spl_update_all (void);


guint32 get_nr_of_tracks_in_playlist (Playlist *plitem);
Track *get_track_in_playlist_by_nr (Playlist *plitem, guint32 n);
#endif
