/* Time-stamp: <2005-01-01 14:53:37 jcs>
|
|  Copyright (C) 2002-2003 Jorg Schuler <jcsjcs at users.sourceforge.net>
|  Part of the gtkpod project.
|
|  URL: http://gtkpod.sourceforge.net/
|
|  Most of the code in this file has been ported from the perl
|  script "mktunes.pl" (part of the gnupod-tools collection) written
|  by Adrian Ulrich <pab at blinkenlights.ch>.
|
|  gnupod-tools: http://www.blinkenlights.ch/cgi-bin/fm.pl?get=ipod
|
|  The code contained in this file is free software; you can redistribute
|  it and/or modify it under the terms of the GNU Lesser General Public
|  License as published by the Free Software Foundation; either version
|  2.1 of the License, or (at your option) any later version.
|
|  This file is distributed in the hope that it will be useful,
|  but WITHOUT ANY WARRANTY; without even the implied warranty of
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|  Lesser General Public License for more details.
|
|  You should have received a copy of the GNU Lesser General Public
|  License along with this code; if not, write to the Free Software
|  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
|
|  iTunes and iPod are trademarks of Apple
|
|  This product is not supported/written/published by Apple!
|
|  $Id$
*/




/* Some notes on how to use the functions in this file:


   *** Reading the iTunesDB ***

   gboolean itunesdb_parse (gchar *path); /+ path to mountpoint +/
   will read an iTunesDB and pass the data over to your program. Your
   programm is responsible to keep a representation of the data.

   The information given in the "Play Counts" file is also read if
   available and the playcounts, star rating and the time last played
   is updated.

   For each track itunesdb_parse() will pass a filled out Track structure
   to "it_add_track()", which has to be provided. The return value is
   TRUE on success and FALSE on error. At the time being, the return
   value is ignored, however.

   The minimal Track structure looks like this (feel free to have
   it_add_track() do with it as it pleases -- and yes, you are
   responsible to free the memory):

   typedef struct
   {
     gunichar2 *album_utf16;    /+ album (utf16)         +/
     gunichar2 *artist_utf16;   /+ artist (utf16)        +/
     gunichar2 *title_utf16;    /+ title (utf16)         +/
     gunichar2 *genre_utf16;    /+ genre (utf16)         +/
     gunichar2 *comment_utf16;  /+ comment (utf16)       +/
     gunichar2 *composer_utf16; /+ Composer (utf16)      +/
     gunichar2 *fdesc_utf16;    /+ Filetype descr (utf16)+/
     gunichar2 *ipod_path_utf16;/+ name of file on iPod: uses ":" instead of "/" +/
     guint32 ipod_id;           /+ unique ID of track    +/
     gint32  size;              /+ size of file in bytes +/
     gint32  tracklen;          /+ Length of track in ms +/
     gint32  cd_nr;             /+ CD number             +/
     gint32  cds;               /+ number of CDs         +/
     gint32  track_nr;          /+ track number          +/
     gint32  tracks;            /+ number of tracks      +/
     gint32  year;              /+ year                  +/
     gint32  bitrate;           /+ bitrate               +/
     gint32  volume;            /+ volume adjustment     +/
     guint32 soundcheck;        /+ volume adjustment "soundcheck"   +/
     guint32 time_created;      /+ time when added (Mac type)       +/
     guint32 time_played;       /+ time of last play (Mac type)     +/
     guint32 time_modified;     /+ time of last modific. (Mac type) +/
     guint32 rating;            /+ star rating (stars * 20)         +/
     guint32 playcount;         /+ number of times track was played +/
     guint32 recent_playcount;  /+ times track was played since last sync+/
     gboolean transferred;      /+ has file been transferred to iPod?    +/
   } Track;

   "transferred" will be set to TRUE because all tracks read from a
   iTunesDB are obviously (or hopefully) already transferred to the
   iPod.

   "recent_playcount" is for information only and will not be stored
   to the iPod.

   By #defining ITUNESDB_PROVIDE_UTF8, itunesdb_parse() will also
   provide utf8 versions of the above utf16 strings. You must then add
   members "gchar *album"... to the Track structure.

   For each new playlist in the iTunesDB, it_add_playlist() is
   called with a pointer to the following Playlist struct:

   typedef struct
   {
     gunichar2 *name_utf16;
     guint32 type;         /+ 1: master play list (PL_TYPE_MPL) +/
   } Playlist;

   Again, by #defining ITUNESDB_PROVIDE_UTF8, a member "gchar *name"
   will be initialized with a utf8 version of the playlist name.

   it_add_playlist() must return a pointer under which it wants the
   playlist to be referenced when it_add_track_to_playlist() is called.

   For each track in the playlist, it_add_trackid_to_playlist() is called
   with the above mentioned pointer to the playlist and the trackid to
   be added.

   gboolean it_add_track (Track *track);
   Playlist *it_add_playlist (Playlist *plitem);
   void it_add_trackid_to_playlist (Playlist *plitem, guint32 id);


   *** Writing the iTunesDB ***

   gboolean itunesdb_write (gchar *path), /+ path to mountpoint +/
   will write an updated version of the iTunesDB.

   The "Play Counts" file is renamed to "Play Counts.bak" if it exists
   to avoid it being read multiple times.

   It uses the following functions to retrieve the data necessary data
   from memory:

   guint it_get_nr_of_tracks (void);
   Track *it_get_track_by_nr (guint32 n);
   guint32 it_get_nr_of_playlists (void);
   Playlist *it_get_playlist_by_nr (guint32 n);

   The master playlist is expected to be "it_get_playlist_by_nr(0)". Only
   the utf16 strings in the Playlist and Track struct are being used.

   Please note that non-transferred tracks are not automatically
   transferred to the iPod. A function

   gboolean itunesdb_copy_track_to_ipod (gchar *path, Track *track, gchar *pcfile)

   is provided to help you do that, however.

   The following functions most likely will also come in handy:

   Track *itunesdb_new_track (void);
   Use itunesdb_new_track() to get an "initialized" track structure
   (the "unknowns" are initialized with reasonable values).

   gboolean itunesdb_cp (gchar *from_file, gchar *to_file);
   guint32 itunesdb_time_get_mac_time (void);
   time_t itunesdb_time_mac_to_host (guint32 mactime);
   guint32 itunesdb_time_host_to_mac (time_t time);
   void itunesdb_convert_filename_fs2ipod(gchar *ipod_file);
   void itunesdb_convert_filename_ipod2fs(gchar *ipod_file);

   void itunesdb_rename_files (const gchar *dirname);

   (Renames/removes some files on the iPod (Playcounts, OTG
   semaphore). Needs to be called if you write the iTunesDB not
   directly to the iPod but to some other location and then manually
   copy the file from there to the iPod. That's much faster in the
   case of using an iPod mounted in sync'ed mode.)

   Define "itunesdb_warning()" as you need (or simply use g_print and
   change the default g_print handler with g_set_print_handler() as is
   done in gtkpod).

   Jorg Schuler, 19.12.2002 */


/* call itunesdb_parse () to read the iTunesDB  */
/* call itunesdb_write () to write the iTunesDB */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "itunesdb.h"
#include "support.h"
#include "file.h"

#ifndef P_tmpdir
#define P_tmpdir	"/tmp"
#endif

#ifdef IS_GTKPOD
/* we're being linked with gtkpod */
#define itunesdb_warning(...) g_print(__VA_ARGS__)
#else
/* The following prints the error messages to the shell, converting
 * UTF8 to the current locale on the fly: */
#define itunesdb_warning(...) do { gchar *utf8=g_strdup_printf (__VA_ARGS__); gchar *loc=g_locale_from_utf8 (utf8, -1, NULL, NULL, NULL); fprintf (stderr, "%s", loc); g_free (loc); g_free (utf8);} while (FALSE)
#endif

#define ITUNESDB_DEBUG 0
#define ITUNESDB_MHIT_DEBUG 0

#define ITUNESDB_COPYBLK 262144      /* blocksize for cp () */

/* list with the contents of the Play Count file for use when
 * importing the iTunesDB */
static GList *playcounts = NULL;
/* needed to keep a local copy of the IDs in the master playlist */
static guint32 *mpl_ids = NULL;
static guint32 mpl_length = 0;

/* structure to hold the contents of one entry of the Play Count file */
struct playcount {
    guint32 playcount;
    guint32 time_played;
    gint32 rating;
};

#define NO_PLAYCOUNT (-1)

enum MHOD_ID {
  MHOD_ID_TITLE = 1,
  MHOD_ID_PATH = 2,
  MHOD_ID_ALBUM = 3,
  MHOD_ID_ARTIST = 4,
  MHOD_ID_GENRE = 5,
  MHOD_ID_FDESC = 6,
  MHOD_ID_COMMENT = 8,
  MHOD_ID_COMPOSER = 12,
  MHOD_ID_GROUPING = 13,
  MHOD_ID_SPLPREF = 50,  /* settings for smart playlist */
  MHOD_ID_SPLRULES = 51, /* rules for smart playlist     */
  MHOD_ID_MHYP = 52,     /* unknown                     */
  MHOD_ID_PLAYLIST = 100
};

static struct playcount *get_next_playcount (void);

static guint32 utf16_strlen(gunichar2 *utf16_string);

/* Compare the two data. TRUE if identical */
static gboolean cmp_n_bytes (gchar *data1, gchar *data2, gint n)
{
  gint i;

  for(i=0; i<n; ++i)
    {
      if (data1[i] != data2[i]) return FALSE;
    }
  return TRUE;
}


/* Seeks to position "seek", then reads "n" bytes. Returns -1 on error
   during seek, or the number of bytes actually read */
static gint seek_get_n_bytes (FILE *file, gchar *data, glong seek, gint n)
{
  gint i;
  gint read;

  if (fseek (file, seek, SEEK_SET) != 0) return -1;

  for(i=0; i<n; ++i)
    {
      read = fgetc (file);
      if (read == EOF) return i;
      *data++ = (gchar)read;
    }
  return i;
}


/* Get the 1-byte-number stored at position "seek" in "file"
   (or -1 when an error occured) */
static guint32 get8int(FILE *file, glong seek)
{
  guchar data;
  guint32 n;

  if (seek_get_n_bytes (file, &data, seek, 1) != 1) return -1;
  n = data;
  return n;
}

/* Get the 4-byte-number stored at position "seek" in "file"
   in little endian encoding (or -1 when an error occured) */
static guint32 get32lint(FILE *file, glong seek)
{
    guint32 n;
    if (seek_get_n_bytes (file, (gchar *)&n, seek, 4) != 4) return -1;
#   if (G_BYTE_ORDER == G_BIG_ENDIAN)
    n = GUINT32_SWAP_LE_BE (n);
#   endif
    return n;
}


/* Get the 4-byte-number stored at position "seek" in "file"
   in big endian encoding (or -1 when an error occured) */
static guint32 get32bint(FILE *file, glong seek)
{
    guint32 n;
    if (seek_get_n_bytes (file, (gchar *)&n, seek, 4) != 4) return -1;
#   if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
    n = GUINT32_SWAP_LE_BE (n);
#   endif
    return n;
}

/* Get the 8-byte-number stored at position "seek" in "file"
   in little endian encoding (or -1 when an error occured) */
static guint64 get64lint(FILE *file, glong seek)
{
    guint64 n;

    if (seek_get_n_bytes (file, (gchar *)&n, seek, 8) != 8) return -1;
#   if (G_BYTE_ORDER == G_BIG_ENDIAN)
    n = GUINT64_SWAP_LE_BE (n);
#   endif
    return n;
}

/* Get the 8-byte-number stored at position "seek" in "file"
   in big endian encoding (or -1 when an error occured) */
static guint64 get64bint(FILE *file, glong seek)
{
    guint64 n;

    if (seek_get_n_bytes (file, (gchar *)&n, seek, 8) != 8) return -1;
#   if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
    n = GUINT64_SWAP_LE_BE (n);
#   endif
    return n;
}




/* Fix little endian UTF16 String to correct byteorder if necessary
 * (all strings in the iTunesDB are little endian except for the ones
 * in smart playlists). */
static gunichar2 *fixup_little_utf16(gunichar2 *utf16_string) {
#if (G_BYTE_ORDER == G_BIG_ENDIAN)
gint32 i;
 if (utf16_string)
 {
     for(i=0; i<utf16_strlen(utf16_string); i++)
     {
	 utf16_string[i] = GUINT16_SWAP_LE_BE (utf16_string[i]);
     }
 }
#endif
return utf16_string;
}

/* Fix big endian UTF16 String to correct byteorder if necessary (only
 * strings in smart playlists are big endian) */
static gunichar2 *fixup_big_utf16(gunichar2 *utf16_string) {
#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
gint32 i;
 if (utf16_string)
 {
     for(i=0; i<utf16_strlen(utf16_string); i++)
     {
	 utf16_string[i] = GUINT16_SWAP_LE_BE (utf16_string[i]);
     }
 }
#endif
return utf16_string;
}


/* spl_action_known(), itb_splr_get_field_type(),
 * itb_splr_get_action_type() are adapted from source provided by
 * Samuel "Otto" Wood (sam dot wood at gmail dot com). These part can
 * also be used under a FreeBSD license. You may also contact Samuel
 * for a complete copy of his original C++-classes.
 * */

/* return TRUE if the smart playlist action @action is
   known. Otherwise a warning is displayed and FALSE is returned. */
static gboolean spl_action_known (SPLAction action)
{
    gboolean result = FALSE;

    switch (action)
    {
    case SPLACTION_IS_INT:
    case SPLACTION_IS_GREATER_THAN:
    case SPLACTION_IS_NOT_GREATER_THAN:
    case SPLACTION_IS_LESS_THAN:
    case SPLACTION_IS_NOT_LESS_THAN:
    case SPLACTION_IS_IN_THE_RANGE:
    case SPLACTION_IS_NOT_IN_THE_RANGE:
    case SPLACTION_IS_IN_THE_LAST:
    case SPLACTION_IS_STRING:
    case SPLACTION_CONTAINS:
    case SPLACTION_STARTS_WITH:
    case SPLACTION_DOES_NOT_START_WITH:
    case SPLACTION_ENDS_WITH:
    case SPLACTION_DOES_NOT_END_WITH:
    case SPLACTION_IS_NOT_INT:
    case SPLACTION_IS_NOT_IN_THE_LAST:
    case SPLACTION_IS_NOT:
    case SPLACTION_DOES_NOT_CONTAIN:
	result = TRUE;
    }
    if (result == FALSE)
    {	/* New action! */
	itunesdb_warning (_("Unknown action (%d) in smart playlist will be ignored.\n"), action);
    }
    return result;
}

/* return the logic type (string, int, date...) of the action field */
SPLFieldType itb_splr_get_field_type (const SPLRule *splr)
{
    g_return_val_if_fail (splr != NULL, splft_unknown);

    switch(splr->field)
    {
    case SPLFIELD_SONG_NAME:
    case SPLFIELD_ALBUM:
    case SPLFIELD_ARTIST:
    case SPLFIELD_GENRE:
    case SPLFIELD_KIND:
    case SPLFIELD_COMMENT:
    case SPLFIELD_COMPOSER:
    case SPLFIELD_GROUPING:
	return(splft_string);
    case SPLFIELD_BITRATE:
    case SPLFIELD_SAMPLE_RATE:
    case SPLFIELD_YEAR:
    case SPLFIELD_TRACKNUMBER:
    case SPLFIELD_SIZE:
    case SPLFIELD_PLAYCOUNT:
    case SPLFIELD_DISC_NUMBER:
    case SPLFIELD_BPM:
    case SPLFIELD_RATING:
    case SPLFIELD_TIME: /* time is the length of the track in
			   milliseconds */
	return(splft_int);
    case SPLFIELD_COMPILATION:
	return(splft_boolean);
    case SPLFIELD_DATE_MODIFIED:
    case SPLFIELD_DATE_ADDED:
    case SPLFIELD_LAST_PLAYED:
	return(splft_date);
    case SPLFIELD_PLAYLIST:
	return(splft_playlist);
    }
    return(splft_unknown);
}


/* return the type (range, date, string...) of the action field */
SPLActionType itb_splr_get_action_type (const SPLRule *splr)
{
    SPLFieldType fieldType;

    g_return_val_if_fail (splr != NULL, splft_unknown);

    fieldType = itb_splr_get_field_type (splr);

    switch(fieldType)
    {
    case splft_string:
	switch (splr->action)
	{
	case SPLACTION_IS_STRING:
	case SPLACTION_IS_NOT:
	case SPLACTION_CONTAINS:
	case SPLACTION_DOES_NOT_CONTAIN:
	case SPLACTION_STARTS_WITH:
	case SPLACTION_DOES_NOT_START_WITH:
	case SPLACTION_ENDS_WITH:
	case SPLACTION_DOES_NOT_END_WITH:
	    return splat_string;
	case SPLACTION_IS_NOT_IN_THE_RANGE:
	case SPLACTION_IS_INT:
	case SPLACTION_IS_NOT_INT:
	case SPLACTION_IS_GREATER_THAN:
	case SPLACTION_IS_NOT_GREATER_THAN:
	case SPLACTION_IS_LESS_THAN:
	case SPLACTION_IS_NOT_LESS_THAN:
	case SPLACTION_IS_IN_THE_RANGE:
	case SPLACTION_IS_IN_THE_LAST:
	case SPLACTION_IS_NOT_IN_THE_LAST:
	    return splat_invalid;
	default:
	    /* Unknown action type */
	    itunesdb_warning ("Unknown action type %d\n\n", splr->action);
	    return splat_unknown;
	}
	break;

    case splft_int:
	switch (splr->action)
	{
	case SPLACTION_IS_INT:
	case SPLACTION_IS_NOT_INT:
	case SPLACTION_IS_GREATER_THAN:
	case SPLACTION_IS_NOT_GREATER_THAN:
	case SPLACTION_IS_LESS_THAN:
	case SPLACTION_IS_NOT_LESS_THAN:
	    return splat_int;
	case SPLACTION_IS_NOT_IN_THE_RANGE:
	case SPLACTION_IS_IN_THE_RANGE:
	    return splat_range_int;
	case SPLACTION_IS_STRING:
	case SPLACTION_CONTAINS:
	case SPLACTION_STARTS_WITH:
	case SPLACTION_DOES_NOT_START_WITH:
	case SPLACTION_ENDS_WITH:
	case SPLACTION_DOES_NOT_END_WITH:
	case SPLACTION_IS_IN_THE_LAST:
	case SPLACTION_IS_NOT_IN_THE_LAST:
	case SPLACTION_IS_NOT:
	case SPLACTION_DOES_NOT_CONTAIN:
	    return splat_invalid;
	default:
	    /* Unknown action type */
	    itunesdb_warning ("Unknown action type %d\n\n", splr->action);
	    return splat_unknown;
	}
	break;

    case splft_boolean:
	return splat_none;

    case splft_date:
	switch (splr->action)
	{
	case SPLACTION_IS_INT:
	case SPLACTION_IS_NOT_INT:
	case SPLACTION_IS_GREATER_THAN:
	case SPLACTION_IS_NOT_GREATER_THAN:
	case SPLACTION_IS_LESS_THAN:
	case SPLACTION_IS_NOT_LESS_THAN:
	    return splat_date;
	case SPLACTION_IS_IN_THE_LAST:
	case SPLACTION_IS_NOT_IN_THE_LAST:
	    return splat_inthelast;
	case SPLACTION_IS_IN_THE_RANGE:
	case SPLACTION_IS_NOT_IN_THE_RANGE:
	    return splat_range_date;
	case SPLACTION_IS_STRING:
	case SPLACTION_CONTAINS:
	case SPLACTION_STARTS_WITH:
	case SPLACTION_DOES_NOT_START_WITH:
	case SPLACTION_ENDS_WITH:
	case SPLACTION_DOES_NOT_END_WITH:
	case SPLACTION_IS_NOT:
	case SPLACTION_DOES_NOT_CONTAIN:
	    return splat_invalid;
	default:
	    /* Unknown action type */
	    itunesdb_warning ("Unknown action type %d\n\n", splr->action);
	    return splat_unknown;
	}
	break;

    case splft_playlist:
	switch (splr->action)
	{
	case SPLACTION_IS_INT:
	case SPLACTION_IS_NOT_INT:
	    return splat_playlist;
	case SPLACTION_IS_GREATER_THAN:
	case SPLACTION_IS_NOT_GREATER_THAN:
	case SPLACTION_IS_LESS_THAN:
	case SPLACTION_IS_NOT_LESS_THAN:
	case SPLACTION_IS_IN_THE_LAST:
	case SPLACTION_IS_NOT_IN_THE_LAST:
	case SPLACTION_IS_IN_THE_RANGE:
	case SPLACTION_IS_NOT_IN_THE_RANGE:
	case SPLACTION_IS_STRING:
	case SPLACTION_CONTAINS:
	case SPLACTION_STARTS_WITH:
	case SPLACTION_DOES_NOT_START_WITH:
	case SPLACTION_ENDS_WITH:
	case SPLACTION_DOES_NOT_END_WITH:
	case SPLACTION_IS_NOT:
	case SPLACTION_DOES_NOT_CONTAIN:
	    return splat_invalid;
	default:
	    /* Unknown action type */
	    itunesdb_warning ("Unknown action type %d\n\n", splr->action);
	    return splat_unknown;
	}

    case splft_unknown:
	    /* Unknown action type */
	    itunesdb_warning ("Unknown action type %d\n\n", splr->action);
	    return splat_unknown;
    }
    return splat_unknown;
}


/* Validate a rule */
void itb_splr_validate (SPLRule *splr)
{
    SPLActionType at;

    g_return_if_fail (splr != NULL);

    at = itb_splr_get_action_type (splr);

    g_return_if_fail (at != splat_unknown);

    switch (at)
    {
    case splat_int:
    case splat_playlist:
    case splat_date:
	splr->fromdate = 0;
	splr->fromunits = 1;
	splr->tovalue = splr->fromvalue;
	splr->todate = 0;
	splr->tounits = 1;
	break;
    case splat_range_int:
    case splat_range_date:
	splr->fromdate = 0;
	splr->fromunits = 1;
	splr->todate = 0;
	splr->tounits = 1;
	break;
    case splat_inthelast:
	splr->fromvalue = SPLDATE_IDENTIFIER;
	splr->tovalue = SPLDATE_IDENTIFIER;
	break;
    case splat_none:
    case splat_string:
	splr->fromvalue = 0;
	splr->fromdate = 0;
	splr->fromunits = 0;
	splr->tovalue = 0;
	splr->todate = 0;
	splr->tounits = 0;
	break;
    case splat_invalid:
    case splat_unknown:
	g_return_if_fail (FALSE);
	break;
    }

}







/* Returns the type of the mhod and the length *ml. *ml is set to -1
 * on error (e.g. because there's no mhod at @seek */
static gint32 get_mhod_type (FILE *file, glong seek, gint32 *ml)
{
    gchar data[4];
    gint32 type = -1;

#if ITUNESDB_DEBUG
    fprintf(stderr, "get_mhod_type seek: %x\n", (int)seek);
#endif

    *ml = -1;

    if ((seek_get_n_bytes (file, data, seek, 4) == 4) &&
	(cmp_n_bytes (data, "mhod", 4) == TRUE))
    {
	*ml = get32lint (file, seek+8);    /* total length   */
	type = get32lint (file, seek+12);  /* mhod_id number */
    }
    return type;
}


/* Returns a pointer to the data contained in the mhod at position
   @seek. This can be a simple string or something more complicated as
   in the case for SPLPREF or SPLRULES. *ml is set to the total length
   of the mhod (-1 in case of an error), *mty is set to the type of
   the mhod */
static void *get_mhod (FILE *file, glong seek, gint32 *ml, gint32 *mty)
{
  gchar data[4];
  gunichar2 *entry_utf16 = NULL;
  SPLPref *splp = NULL;
  guint8 limitsort_opposite;
  void *result = NULL;
  gint32 xl;
  gint32 header_length;

#if ITUNESDB_DEBUG
  fprintf(stderr, "get_mhod seek: %x\n", (int)seek);
#endif

  if (seek_get_n_bytes (file, data, seek, 4) != 4)
    {
      *ml = -1;
      return NULL;
    }
  if (cmp_n_bytes (data, "mhod", 4) == FALSE )
    {
      *ml = -1;
      return NULL;
    }
  header_length = get32lint (file, seek+4); /* header length  */
  *ml = get32lint (file, seek+8);           /* total length   */
  *mty = get32lint (file, seek+12);         /* mhod_id number */

  seek += header_length;

#if ITUNESDB_DEBUG
  fprintf(stderr, "ml: %x mty: %x, xl: %x\n", *ml, *mty, xl);
#endif

  switch ((enum MHOD_ID)*mty)
  {
  case MHOD_ID_PLAYLIST:
  case MHOD_ID_MHYP:
      /* these are not yet supported */
      break;
  case MHOD_ID_TITLE:
  case MHOD_ID_PATH:
  case MHOD_ID_ALBUM:
  case MHOD_ID_ARTIST:
  case MHOD_ID_GENRE:
  case MHOD_ID_FDESC:
  case MHOD_ID_COMMENT:
  case MHOD_ID_COMPOSER:
  case MHOD_ID_GROUPING:
      xl = get32lint (file, seek+4);   /* entry length   */
      entry_utf16 = g_malloc (xl+2);
      if (seek_get_n_bytes (file, (gchar *)entry_utf16, seek+16, xl) != xl) {
	  g_free (entry_utf16);
	  entry_utf16 = NULL;
	  *ml = -1;
      }
      else
      {
	  entry_utf16[xl/2] = 0; /* add trailing 0 */
      }
      fixup_little_utf16 (entry_utf16);
      result = entry_utf16;
      break;
  case MHOD_ID_SPLPREF:  /* Settings for smart playlist */
      splp = g_malloc0 (sizeof (SPLPref));
      splp->liveupdate = get8int (file, seek);
      splp->checkrules = get8int (file, seek+1);
      splp->checklimits = get8int (file, seek+2);
      splp->limittype = get8int (file, seek+3);
      splp->limitsort = get8int (file, seek+4);
      splp->limitvalue = get32lint (file, seek+8);
      splp->matchcheckedonly = get8int (file, seek+12);
      limitsort_opposite = get8int (file, seek+13);
      /* if the opposite flag is on, set limitsort's high bit -- see
	 note in itunesdb.h for more info */
      if (limitsort_opposite)
	  splp->limitsort |= 0x80000000;
      result = splp;
      break;
  case MHOD_ID_SPLRULES:  /* Rules for smart playlist */
      if ((seek_get_n_bytes (file, data, seek, 4) == 4) &&
	  (cmp_n_bytes (data, "SLst", 4) == TRUE))
      {
	  /* !!! for some reason the SLst part is the only part of the
	     iTunesDB with big-endian encoding, including UTF16
	     strings */
	  gint i;
	  guint32 numrules;
	  SPLRules *splrs = g_malloc0 (sizeof (SPLRules));
	  splrs->unk004 = get32bint (file, seek+4);
	  numrules = get32bint (file, seek+8);
	  splrs->match_operator = get32bint (file, seek+12);
	  seek += 136;  /* I can't find this value stored in the
			   iTunesDB :-( */
	  for (i=0; i<numrules; ++i)
	  {
	      SPLRule *splr = g_malloc0 (sizeof (SPLRule));
	      guint32 length;
	      splr->field = get32bint (file, seek);
	      splr->action = get32bint (file, seek+4);
	      seek += 52;
	      length = get32bint (file, seek);
	      if (spl_action_known (splr->action))
	      {
		  gint ft = itb_splr_get_field_type (splr);
		  if (ft == splft_string)
		  {
		      splr->string_utf16 = g_malloc0 (length+2);
		      if (seek_get_n_bytes (file, (gchar *)splr->string_utf16, seek+4, length) != length) 
		      {
			  g_free (splr->string_utf16);
			  g_free (splr);
			  splr = NULL;
			  *ml = -1;
			  break;  /* exits the "for (i...)" loop */
		      }
		      fixup_big_utf16 (splr->string_utf16);
#ifdef ITUNESDB_PROVIDE_UTF8
		      splr->string = g_utf16_to_utf8 (
			  splr->string_utf16, -1, NULL, NULL, NULL);
/*		      puts(splr->string);*/
#endif
		  }
		  else
		  {
		      if (length != 0x44)
		      {
			  itunesdb_warning (_("Length of smart playlist rule field (%d) not as expected. Trying to continue anyhow.\n"), length);
		      }
		      splr->fromvalue = get64bint (file, seek+4);
		      splr->fromdate = get64bint (file, seek+12);
		      splr->fromunits = get64bint (file, seek+20);
		      splr->tovalue = get64bint (file, seek+28);
		      splr->todate = get64bint (file, seek+36);
		      splr->tounits = get64bint (file, seek+44);
		      /* SPLFIELD_PLAYLIST seem to use these unknowns*/
		      splr->unk052 = get32bint (file, seek+52);
		      splr->unk056 = get32bint (file, seek+56);
		      splr->unk060 = get32bint (file, seek+60);
		      splr->unk064 = get32bint (file, seek+64);
		      splr->unk068 = get32bint (file, seek+68);
		  }  
		  seek += length+4;
	      }
	      else
	      {
		  g_free (splr);
		  splr = NULL;
	      }
	      if (splr)
	      {
		  splrs->rules = g_list_append (splrs->rules, splr);
	      } 
	  }
	  result = splrs;
      }
      else
      {
	  *ml = -1;
      }
      break;
  default:
      itunesdb_warning (_("Encountered unknown MHOD type (%d) while parsing the iTunesDB. Ignoring.\n\n"), *mty);
      break;
  }
  return result;
}

/* Returns the value of a string type mhod. return the length of the
   mhod *ml, the mhod type *mty, and a string with the entry (in
   UTF16). After use you must free the string with g_free(). Returns
   NULL if no string is avaible. *ml is set to -1 in case of error. */
static gunichar2 *get_mhod_string (FILE *file, glong seek, gint32 *ml, gint32 *mty)
{
    gunichar2 *result = NULL;

    *mty = get_mhod_type (file, seek, ml);

    if (*ml != -1) switch ((enum MHOD_ID)*mty)
    {
    case MHOD_ID_TITLE:
    case MHOD_ID_PATH:
    case MHOD_ID_ALBUM:
    case MHOD_ID_ARTIST:
    case MHOD_ID_GENRE:
    case MHOD_ID_FDESC:
    case MHOD_ID_COMMENT:
    case MHOD_ID_COMPOSER:
    case MHOD_ID_GROUPING:
	result = get_mhod (file, seek, ml, mty);
	break;
    case MHOD_ID_SPLPREF:
    case MHOD_ID_SPLRULES:
    case MHOD_ID_MHYP:
    case MHOD_ID_PLAYLIST:
	/* these do not have a string entry */
	break;
    }
    return result;
}

/* get a PL, return pos where next PL should be, name and content */
static glong get_pl(FILE *file, glong seek)
{
  guint32 type, tracknum, n;
  glong nextseek;
  gint32 zip;
  Playlist *plitem = NULL;
  guint32 ref;
  gchar data[4];


#if ITUNESDB_DEBUG
  fprintf(stderr, "mhyp seek: %x\n", (int)seek);
#endif

  if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
  if (cmp_n_bytes (data, "mhyp", 4) == FALSE)      return -1; /* not pl */
  zip = get32lint (file, seek+4); /* length of header */
  if (zip == 0) return -1;      /* error! */
  nextseek = seek + get32lint (file, seek+8); /* possible begin of next PL */
  tracknum = get32lint (file, seek+16); /* number of tracks in playlist */
  plitem = g_malloc0 (sizeof (Playlist));
  /* Some Playlists have added 256 to their type -- I don't know what
     it's for, so we just ignore it for now -> & 0xff */
  plitem->type = get32lint (file, seek+20) & 0xff;
  plitem->id = get64lint (file, seek+28);
  do
  {
      gunichar2 *plname_utf16_maybe;
      SPLPref *splpref = NULL;
      SPLRules *splrules = NULL;

      seek += zip;
      if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
      type = get_mhod_type (file, seek, &zip);
      if (zip != -1) switch ((enum MHOD_ID)type)
      {
      case MHOD_ID_PLAYLIST:
	  /* here we could do something about the playlist settings */
	  break;
      case MHOD_ID_TITLE:
	  plname_utf16_maybe = get_mhod (file, seek, &zip, &type);
	  if (plname_utf16_maybe)
	  {
	      /* sometimes there seem to be two mhod TITLE headers */
	      g_free (plitem->name_utf16);
	      plitem->name_utf16  = plname_utf16_maybe;

	  }
	  break;
      case MHOD_ID_SPLPREF:
	  splpref = get_mhod (file, seek, &zip, &type);
	  if (splpref)
	  {
	      plitem->is_spl = TRUE;
	      memcpy (&plitem->splpref, splpref, sizeof (SPLPref));
	      g_free (splpref);
	      splpref = NULL;
	  }
	  break;
      case MHOD_ID_SPLRULES:
	  splrules = get_mhod (file, seek, &zip, &type);
	  if (splrules)
	  {
	      plitem->is_spl = TRUE;
	      memcpy (&plitem->splrules, splrules, sizeof (SPLRules));
	      g_free (splrules);
	      splrules = NULL;
	  }
	  break;
      case MHOD_ID_PATH:
      case MHOD_ID_ALBUM:
      case MHOD_ID_ARTIST:
      case MHOD_ID_GENRE:
      case MHOD_ID_FDESC:
      case MHOD_ID_COMMENT:
      case MHOD_ID_COMPOSER:
      case MHOD_ID_GROUPING:
	  /* these are not expected here */
	  break;
      case MHOD_ID_MHYP:
	  /* this I don't know how to handle */
	  break;
      }
  } while (zip != -1); /* read all MHODs */

  if (!plitem->name_utf16)
  {   /* we did not read a valid mhod TITLE header -> */
      /* we simply make up our own name */
	if (plitem->type == PL_TYPE_MPL)
	    plitem->name_utf16 = g_utf8_to_utf16 (_("Master-PL"), -1,
						  NULL, NULL, NULL);
	else plitem->name_utf16 = g_utf8_to_utf16 (_("Playlist"), -1,
						   NULL, NULL, NULL);
  }
#ifdef ITUNESDB_PROVIDE_UTF8
  plitem->name = g_utf16_to_utf8 (plitem->name_utf16, -1,
				  NULL, NULL, NULL);
#endif

#if ITUNESDB_DEBUG
  fprintf(stderr, "pln: %s(%d Tracks) \n", plname_utf8, (int)tracknum);
#endif

  /* add new playlist */
  plitem = it_add_playlist(plitem);

#if ITUNESDB_DEBUG
  fprintf(stderr, "added pl: %s", plitem->plname);
#endif

  if ((plitem->type == PL_TYPE_MPL) &&
      (tracknum > 0) &&
      (mpl_ids == NULL))
  {   /* we need space to store the mpl IDs locally */
      mpl_ids = g_malloc0 (tracknum * sizeof (*mpl_ids));
      mpl_length = tracknum;
  }
  n = 0;  /* number of tracks read */
  while (n < tracknum)
    {
      /* We read the mhip headers and skip everything else. If we
	 find a mhyp header before n==tracknum, something is wrong */
      if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
      if (cmp_n_bytes (data, "mhyp", 4) == TRUE) return -1; /* Wrong!!! */
      if (cmp_n_bytes (data, "mhip", 4) == TRUE)
	{
	  ref = get32lint(file, seek+24);
	  it_add_trackid_to_playlist(plitem, ref);
	  if (plitem->type == PL_TYPE_MPL) mpl_ids[n] = ref;
	  ++n;
	}
      seek += get32lint (file, seek+8);
    }
  return nextseek;
}


static glong get_mhit(FILE *file, glong seek)
{
  Track *track;
  gchar data[4];
#ifdef ITUNESDB_PROVIDE_UTF8
  gchar *entry_utf8;
#endif
  gunichar2 *entry_utf16;
  gint type;
  gint zip = 0;
  struct playcount *playcount;
  guint32 temp;

#if ITUNESDB_DEBUG
  fprintf(stderr, "get_mhit seek: %x\n", (int)seek);
#endif

  if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
  if (cmp_n_bytes (data, "mhit", 4) == FALSE ) return -1; /* we are lost! */

  track = itunesdb_new_track ();

  track->ipod_id = get32lint(file, seek+16);       /* iPod ID          */
  track->unk020 = get32lint (file, seek+20);
  track->unk024 = get32lint (file, seek+24);
  track->rating = get32lint (file, seek+28)>>24;   /* rating          */
  temp = get32lint (file, seek+32);
  track->compilation = (temp & 0x00ff0000) >> 16;
  track->type = temp & 0x0000ffff;
  track->time_created = get32lint(file, seek+32);  /* creation time    */
  track->size = get32lint(file, seek+36);          /* file size        */
  track->tracklen = get32lint(file, seek+40);      /* time             */
  track->track_nr = get32lint(file, seek+44);      /* track number     */
  track->tracks = get32lint(file, seek+48);        /* nr of tracks     */
  track->year = get32lint(file, seek+52);          /* year             */
  track->bitrate = get32lint(file, seek+56);       /* bitrate          */
  track->samplerate = get32lint(file,seek+60)>>16; /* sample rate      */
  track->volume = get32lint(file, seek+64);        /* volume adjust    */
  track->starttime = get32lint (file, seek+68);
  track->stoptime = get32lint (file, seek+72);
  track->soundcheck = get32lint (file, seek+76);   /* soundcheck       */
  track->playcount = get32lint (file, seek+80);    /* playcount        */
  track->unk084 = get32lint (file, seek+84);
  track->time_played = get32lint(file, seek+88);   /* last time played */
  track->cd_nr = get32lint(file, seek+92);         /* CD nr            */
  track->cds = get32lint(file, seek+96);           /* CD nr of..       */
  track->unk100 = get32lint (file, seek+100);
  track->time_modified = get32lint(file, seek+104);/* last mod. time   */
  track->unk108 = get32lint (file, seek+108);
  track->unk112 = get32lint (file, seek+112);
  track->unk116 = get32lint (file, seek+116);
  temp = get32lint (file, seek+120);
  track->BPM = temp >> 16;
  track->app_rating = (temp & 0xff00)>> 8;/* The rating set by * the
					     application, as opposed to
					     the rating set on the iPod
					     itself */
  track->checked = temp & 0xff;           /* Checked/Unchecked: 0/1 */
  track->unk124 = get32lint (file, seek+124);
  track->unk128 = get32lint (file, seek+128);
  track->unk132 = get32lint (file, seek+132);
  track->unk136 = get32lint (file, seek+136);
  track->unk140 = get32lint (file, seek+140);
  track->unk144 = get32lint (file, seek+144);
  track->unk148 = get32lint (file, seek+148);
  track->unk152 = get32lint (file, seek+152);

  track->transferred = TRUE;                   /* track is on iPod! */

#if ITUNESDB_MHIT_DEBUG
time_t time_mac_to_host (guint32 mactime);
gchar *time_time_to_string (time_t time);
#define printf_mhit(sk, str)  printf ("%3d: %d (%s)\n", sk, get32lint (file, seek+sk), str);
#define printf_mhit_time(sk, str) { gchar *buf = time_time_to_string (itunesdb_time_mac_to_host (get32lint (file, seek+sk))); printf ("%3d: %s (%s)\n", sk, buf, str); g_free (buf); }
  {
      printf ("\nmhit: seek=%lu\n", seek);
      printf_mhit (  4, "header size");
      printf_mhit (  8, "mhit size");
      printf_mhit ( 12, "nr of mhods");
      printf_mhit ( 16, "iPod ID");
      printf_mhit ( 20, "?");
      printf_mhit ( 24, "?");
      printf (" 28: %u (type)\n", get32lint (file, seek+28) & 0xffffff);
      printf (" 28: %u (rating)\n", get32lint (file, seek+28) >> 24);
      printf_mhit ( 32, "timestamp file");
      printf_mhit_time ( 32, "timestamp file");
      printf_mhit ( 36, "size");
      printf_mhit ( 40, "tracklen (ms)");
      printf_mhit ( 44, "track_nr");
      printf_mhit ( 48, "total tracks");
      printf_mhit ( 52, "year");
      printf_mhit ( 56, "bitrate");
      printf_mhit ( 60, "sample rate");
      printf (" 60: %u (sample rate LSB)\n", get32lint (file, seek+60) & 0xffff);
      printf (" 60: %u (sample rate HSB)\n", (get32lint (file, seek+60) >> 16));
      printf_mhit ( 64, "?");
      printf_mhit ( 68, "?");
      printf_mhit ( 72, "?");
      printf_mhit ( 76, "?");
      printf_mhit ( 80, "playcount");
      printf_mhit ( 84, "?");
      printf_mhit ( 88, "last played");
      printf_mhit_time ( 88, "last played");
      printf_mhit ( 92, "CD");
      printf_mhit ( 96, "total CDs");
      printf_mhit (100, "?");
      printf_mhit (104, "?");
      printf_mhit_time (104, "?");
      printf_mhit (108, "?");
      printf_mhit (112, "?");
      printf_mhit (116, "?");
      printf_mhit (120, "?");
      printf_mhit (124, "?");
      printf_mhit (128, "?");
      printf_mhit (132, "?");
      printf_mhit (136, "?");
      printf_mhit (140, "?");
      printf_mhit (144, "?");
      printf_mhit (148, "?");
      printf_mhit (152, "?");
  }
#undef printf_mhit_time
#undef printf_mhit
#endif

  seek += get32lint (file, seek+4);             /* 1st mhod starts here! */
  while(zip != -1)
    {
     seek += zip;
     entry_utf16 = get_mhod_string (file, seek, &zip, &type);
     if (entry_utf16 != NULL) {
#ifdef ITUNESDB_PROVIDE_UTF8
       entry_utf8 = g_utf16_to_utf8 (entry_utf16, -1, NULL, NULL, NULL);
#endif
       switch ((enum MHOD_ID)type)
	 {
	 case MHOD_ID_ALBUM:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->album = entry_utf8;
#endif
	   track->album_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_ARTIST:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->artist = entry_utf8;
#endif
	   track->artist_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_TITLE:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->title = entry_utf8;
#endif
	   track->title_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_GENRE:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->genre = entry_utf8;
#endif
	   track->genre_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_PATH:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->ipod_path = entry_utf8;
#endif
	   track->ipod_path_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_FDESC:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->fdesc = entry_utf8;
#endif
	   track->fdesc_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_COMMENT:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->comment = entry_utf8;
#endif
	   track->comment_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_COMPOSER:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->composer = entry_utf8;
#endif
	   track->composer_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_GROUPING:
#ifdef ITUNESDB_PROVIDE_UTF8
	   track->grouping = entry_utf8;
#endif
	   track->grouping_utf16 = entry_utf16;
	   break;
	 default: /* unknown entry -- discard */
#ifdef ITUNESDB_PROVIDE_UTF8
	   g_free (entry_utf8);
#endif
	   g_free (entry_utf16);
	   break;
	 }
     }
    }

  playcount = get_next_playcount ();
  if (playcount)
  {
      if (playcount->rating != NO_PLAYCOUNT)  track->rating = playcount->rating;
      if (playcount->time_played) track->time_played = playcount->time_played;
      track->playcount += playcount->playcount;
      track->recent_playcount = playcount->playcount;
      g_free (playcount);
  }
  it_add_track (track);
  return seek;
}

/* get next playcount, that is the first entry of GList
 * playcounts. This entry is removed from the list. You must free the
 * return value after use */
static struct playcount *get_next_playcount (void)
{
    struct playcount *playcount = g_list_nth_data (playcounts, 0);

    if (playcount)  playcounts = g_list_remove (playcounts, playcount);
    return playcount;
}

/* delete all entries of GList *playcounts */
static void reset_playcounts (void)
{
    struct playcount *playcount;
    while ((playcount=get_next_playcount())) g_free (playcount);
}

/* Read the Play Count file (formed by adding "Play Counts" to the
 * directory contained in @dirname) and set up the GList *playcounts
 * */
static void init_playcounts (const gchar *dirname)
{
//  gchar *plcname = g_build_filename (dirname, "Play Counts", NULL);
  const gchar *db[] = {"Play Counts", NULL};
  gchar *plcname = resolve_path (dirname, db);
  FILE *plycts = NULL;
  gboolean error = TRUE;

  reset_playcounts ();

  if (plcname)  plycts = fopen (plcname, "r");

  if (plycts) do
  {
      gchar data[4];
      guint32 header_length, entry_length, entry_num, i=0;
      time_t tt = time (NULL);

      localtime (&tt);  /* set the ext. variable 'timezone' (see below) */
      if (seek_get_n_bytes (plycts, data, 0, 4) != 4)  break;
      if (cmp_n_bytes (data, "mhdp", 4) == FALSE)      break;
      header_length = get32lint (plycts, 4);
      /* all the headers I know are 0x60 long -- if this one is longer
	 we can simply ignore the additional information */
      if (header_length < 0x60)                        break;
      entry_length = get32lint (plycts, 8);
      /* all the entries I know are 0x0c (firmware 1.3) or 0x10
       * (firmware 2.0) in length */
      if (entry_length < 0x0c)                         break;
      /* number of entries */
      entry_num = get32lint (plycts, 12);
      for (i=0; i<entry_num; ++i)
      {
	  struct playcount *playcount = g_malloc0 (sizeof (struct playcount));
	  glong seek = header_length + i*entry_length;

	  playcounts = g_list_append (playcounts, playcount);
	  /* check if entry exists by reading its last four bytes */
	  if (seek_get_n_bytes (plycts, data,
				seek+entry_length-4, 4) != 4) break;
	  playcount->playcount = get32lint (plycts, seek);
	  playcount->time_played = get32lint (plycts, seek+4);
          /* NOTE:
	   *
	   * The iPod (firmware 1.3) doesn't seem to use the timezone
	   * information correctly -- no matter what you set iPod's
	   * timezone to it will always record in UTC -- we need to
	   * subtract the difference between current timezone and UTC
	   * to get a correct display. 'timezone' (initialized above)
	   * contains the difference in seconds.
           */
	  if (playcount->time_played)
	      playcount->time_played += timezone;

	  /* rating only exists if the entry length is at least 0x10 */
	  if (entry_length >= 0x10)
	      playcount->rating = get32lint (plycts, seek+12);
	  else
	      playcount->rating = NO_PLAYCOUNT;
      }
      if (i == entry_num)  error = FALSE;
  } while (FALSE);
  if (plycts)  fclose (plycts);
  if (error)   reset_playcounts ();
  g_free (plcname);
}



/* Called by read_OTG_playlists(): reads and processes OTG playlist
 * file @filename by adding a new playlist (named @plname) with the
 * tracks specified in @filename. If @plname is NULL, a standard name
 * will be substituted */
/* Returns FALSE on error, TRUE on success */
static gboolean process_OTG_file (const gchar *filename,
				  const gchar *plname)
{
    FILE *otgf = NULL;
    gboolean result = FALSE;

    if (filename)  otgf = fopen (filename, "r");
    if (otgf)
    {
	gchar data[4];
	guint32 header_length, entry_length, entry_num, i=0;

	if (!plname) plname = _("OTG Playlist");

	if (seek_get_n_bytes (otgf, data, 0, 4) != 4)  goto end;
	if (cmp_n_bytes (data, "mhpo", 4) == FALSE)    goto end;
	header_length = get32lint (otgf, 4);
	/* all the headers I know are 0x14 long -- if this one is
	 longer we can simply ignore the additional information */
	if (header_length < 0x14)                      goto end;
	entry_length = get32lint (otgf, 8);
	/* all the entries I know are 0x04 long */
	if (entry_length < 0x04)                       goto end;
	/* number of entries */
	entry_num = get32lint (otgf, 12);
	if (entry_num > 0)
	{
	    /* Create new playlist */
	    Playlist *pl = g_malloc0 (sizeof (Playlist));
#ifdef ITUNESDB_PROVIDE_UTF8
	    pl->name = g_strdup (plname);
#endif
	    pl->name_utf16 = g_utf8_to_utf16 (plname, -1,
					      NULL, NULL, NULL);
	    pl->type = PL_TYPE_NORM;
	    pl = it_add_playlist (pl);

	    /* Add items */
	    for (i=0; i<entry_num; ++i)
	    {
		guint32 num = get32lint (otgf,
				      header_length + entry_length *i);
		if ((num < mpl_length) && mpl_ids)
		{
		    it_add_trackid_to_playlist (pl, mpl_ids[num]);
		}
	    }
	}
	result = TRUE;
    }
  end:
    if (otgf)   fclose (otgf);
    return result;
}


/* Add the On-The-Go Playlist(s) to the main playlist */
/* The OTG-Files are located in @dirname */
static void read_OTG_playlists (const gchar *dirname)
{
    gchar *filename;
    gint i=1;
    gchar *db[] = {"OTGPlaylistInfo", NULL};
    gchar *otgname = resolve_path (dirname, (const gchar **)db);

    /* only parse if "OTGPlaylistInfo" exists */
    if (otgname) do
    {
	db[0] = g_strdup_printf ("OTGPlaylistInfo_%d", i);
	filename = resolve_path (dirname, (const gchar **)db);
	g_free (db[0]);
	if (filename)
	{
	    gchar *plname = g_strdup_printf (_("OTG Playlist %d"), i);
	    process_OTG_file (filename, plname);
	    g_free (filename);
	    g_free (plname);
	    ++i;
	}
    } while (filename);
    g_free (otgname);
}


/* Parse the iTunesDB and store the tracks using it_addtrack () defined
   in track.c.
   Returns TRUE on success, FALSE on error.
   "path" should point to the mount point of the iPod,
   e.g. "/mnt/ipod" and be in local encoding */
gboolean itunesdb_parse (const gchar *path)
{
  gchar *filename;
  const gchar *db[] = {"iPod_Control","iTunes","iTunesDB",NULL};
  gboolean result;

  filename = resolve_path(path, db);
  result = itunesdb_parse_file (filename);
  g_free (filename);
  return result;
}

/* Same as itunesdb_parse(), but let's specify the filename directly */
gboolean itunesdb_parse_file (const gchar *filename)
{
  FILE *itunes = NULL;
  gboolean result = FALSE;
  gchar data[8];
  glong seek=0, pl_mhsd=0;
  guint32 zip, nr_tracks=0, nr_playlists=0;
  gboolean swapped_mhsd = FALSE;
  gchar *dirname=NULL;

#if ITUNESDB_DEBUG
  fprintf(stderr, "Parsing %s\nenter: %4d\n", filename, it_get_nr_of_tracks ());
#endif

  if (!filename) return FALSE;

  /* extract valid directory base from @filename */
  dirname = g_path_get_dirname (filename);

  itunes = fopen (filename, "r");
  do
  { /* dummy loop for easier error handling */
      if (itunes == NULL)
      {
	  itunesdb_warning (_("Could not open iTunesDB \"%s\" for reading.\n"),
			  filename);
	  break;
      }
      if (seek_get_n_bytes (itunes, data, seek, 4) != 4)
      {
	  itunesdb_warning (_("Error reading \"%s\".\n"), filename);
	  break;
      }
      /* for(i=0; i<8; ++i)  printf("%02x ", data[i]); printf("\n");*/
      if (cmp_n_bytes (data, "mhbd", 4) == FALSE)
      {
	  itunesdb_warning (_("\"%s\" is not a iTunesDB.\n"), filename);
	  break;
      }
      seek = get32lint (itunes, 4);
      /* all the headers I know are 0x68 long -- if this one is longer
	 we can simply ignore the additional information */
      /* we don't need any information of the mhbd header... */
      /*      if (seek < 0x68)
      {
	  itunesdb_warning (_("\"%s\" is not a iTunesDB.\n"), filename);
	  break;
	  }*/
      do
      {
	  if (seek_get_n_bytes (itunes, data, seek, 8) != 8)  break;
	  if (cmp_n_bytes (data, "mhsd", 4) == TRUE)
	  { /* mhsd header -> determine start of playlists */
	      if (get32lint (itunes, seek + 12) == 1)
	      { /* OK, tracklist, save start of playlists */
		  if (!swapped_mhsd)
		      pl_mhsd = seek + get32lint (itunes, seek+8);
	      }
	      else if (get32lint (itunes, seek + 12) == 2)
	      { /* bad: these are playlists... switch */
		  if (swapped_mhsd)
		  { /* already switched once -> forget it */
		      break;
		  }
		  else
		  {
		      pl_mhsd = seek;
		      seek += get32lint (itunes, seek+8);
		      swapped_mhsd = TRUE;
		  }
	      }
	      else
	      { /* neither playlist nor track MHSD --> skip it */
		  seek += get32lint (itunes, seek+8);
	      }
	  }
	  if (cmp_n_bytes (data, "mhlt", 4) == TRUE)
	  { /* mhlt header -> number of tracks */
	      nr_tracks = get32lint (itunes, seek+8);
	      if (nr_tracks == 0)
	      {   /* no tracks -- skip directly to next mhsd */
		  result = TRUE;
		  break;
	      }
	  }
	  if (cmp_n_bytes (data, "mhit", 4) == TRUE)
	  { /* mhit header -> start of tracks*/
	      result = TRUE;
	      break;
	  }
	  zip = get32lint (itunes, seek+4);
	  if (zip == 0)  break;
	  seek += zip;
      } while (result == FALSE);
      if (result == FALSE)  break; /* some error occured */
      result = FALSE;
      /* now we should be at the first MHIT */

      /* Read Play Count file if available */
      init_playcounts (dirname);

      /* get every file entry */
      if (nr_tracks)  while(seek != -1) {
	  /* get_mhit returns where it's guessing the next MHIT,
	     if it fails, it returns '-1' */
	  seek = get_mhit (itunes, seek);
      }

      /* next: playlists */
      seek = pl_mhsd;
      do
      {
	  if (seek_get_n_bytes (itunes, data, seek, 8) != 8)  break;
	  if (cmp_n_bytes (data, "mhsd", 4) == TRUE)
	  { /* mhsd header */
	      if (get32lint (itunes, seek + 12) != 2)
	      {  /* this is not a playlist MHSD -> skip it */
		  seek += get32lint (itunes, seek+8);
		  continue;
	      }
	  }
	  if (cmp_n_bytes (data, "mhlp", 4) == TRUE)
	  { /* mhlp header -> number of playlists */
	      nr_playlists = get32lint (itunes, seek+8);
	  }
	  if (cmp_n_bytes (data, "mhyp", 4) == TRUE)
	  { /* mhyp header -> start of playlists */
	      result = TRUE;
	      break;
	  }
	  zip = get32lint (itunes, seek+4);
	  if (zip == 0)  break;
	  seek += zip;
      } while (result == FALSE);
      if (result == FALSE)  break; /* some error occured */
      result = FALSE;

#if ITUNESDB_DEBUG
    fprintf(stderr, "iTunesDB part2 starts at: %x\n", (int)seek);
#endif

    while(seek != -1) {
	seek = get_pl(itunes, seek);
    }

    result = TRUE;
  } while (FALSE);

  if (itunes != NULL)     fclose (itunes);

  /* Read OTG playlists */
  read_OTG_playlists (dirname);

  g_free (dirname);
#if ITUNESDB_DEBUG
  fprintf(stderr, "exit:  %4d\n", it_get_nr_of_tracks ());
#endif

  /* clean up used memory */
  g_free (mpl_ids);
  mpl_ids = NULL;
  mpl_length = 0;

  return result;
}


/* up to here we had the functions for reading the iTunesDB               */
/* ---------------------------------------------------------------------- */
/* from here on we have the functions for writing the iTunesDB            */

/* Name of the device in utf16 */
gunichar2 ipod_name[] = { 'g', 't', 'k', 'p', 'o', 'd', 0 };


/* Get length of utf16 string in number of characters (words) */
static guint32 utf16_strlen (gunichar2 *utf16)
{
  guint32 i=0;
  if (utf16)
      while (utf16[i] != 0) ++i;
  return i;
}


/* Write "data", "n" bytes long to current position in file.
   Returns TRUE on success, FALSE otherwise */
static gboolean put_data (FILE *file, gchar *data, gint n)
{
  if (fwrite (data, 1, n, file) != n) return FALSE;
  return TRUE;
}


/* Write 1-byte integer "n" to "file".
   Returns TRUE on success, FALSE otherwise */
static gboolean put8int (FILE *file, guint8 n)
{
    return put_data (file, (gchar *)&n, 1);
}


/* Write 4-byte integer "n" to "file" in little endian order.
   Returns TRUE on success, FALSE otherwise */
static gboolean put32lint (FILE *file, guint32 n)
{
#   if (G_BYTE_ORDER == G_BIG_ENDIAN)
    n = GUINT32_SWAP_LE_BE (n);
#   endif
    return put_data (file, (gchar *)&n, 4);
}


/* Write 4-byte integer "n" to "file" at position "seek" in little
   endian order.
   After writing, the file position indicator is set
   to the end of the file.
   Returns TRUE on success, FALSE otherwise */
static gboolean put32lint_seek (FILE *file, guint32 n, glong seek)
{
  gboolean result;

  if (fseek (file, seek, SEEK_SET) != 0) return FALSE;
  result = put32lint (file, n);
  if (fseek (file, 0, SEEK_END) != 0) return FALSE;
  return result;
}


/* Write 8-byte integer "n" to "file" in big little order.
   Returns TRUE on success, FALSE otherwise */
static gboolean put64lint (FILE *file, guint64 n)
{
#   if (G_BYTE_ORDER == G_BIG_ENDIAN)
    n = GUINT64_SWAP_LE_BE (n);
#   endif
    return put_data (file, (gchar *)&n, 8);
}


/* Write 4-byte integer "n" to "file" in big endian order.
   Returns TRUE on success, FALSE otherwise */
static gboolean put32bint (FILE *file, guint32 n)
{
#   if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
    n = GUINT32_SWAP_LE_BE (n);
#   endif
    return put_data (file, (gchar *)&n, 4);
}


/* Write 8-byte integer "n" to "file" in big endian order.
   Returns TRUE on success, FALSE otherwise */
static gboolean put64bint (FILE *file, guint64 n)
{
#   if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
    n = GUINT64_SWAP_LE_BE (n);
#   endif
    return put_data (file, (gchar *)&n, 8);
}


#if 0
/* Write 4-byte integer "n" to "file" at position "seek" in big endian
   order.
   After writing, the file position indicator is set
   to the end of the file.
   Returns TRUE on success, FALSE otherwise */
static gboolean put32bint_seek (FILE *file, guint32 n, glong seek)
{
  gboolean result;

  if (fseek (file, seek, SEEK_SET) != 0) return FALSE;
  result = put32bint (file, n);
  if (fseek (file, 0, SEEK_END) != 0) return FALSE;
  return result;
}

/* Write 8-byte integer "n" to "file" at position "seek" in big endian
   order.
   After writing, the file position indicator is set
   to the end of the file.
   Returns TRUE on success, FALSE otherwise */
static gboolean put64bint_seek (FILE *file, guint64 n, glong seek)
{
  gboolean result;

  if (fseek (file, seek, SEEK_SET) != 0) return FALSE;
  result = put64bint (file, n);
  if (fseek (file, 0, SEEK_END) != 0) return FALSE;
  return result;
}
#endif


/* Write "n" times 4-byte-zero at current position
   Returns TRUE on success, FALSE otherwise */
static gboolean put_n0 (FILE*file, guint32 n)
{
  guint32 i;
  gboolean result = TRUE;

  for (i=0; i<n; ++i)  result &= put32lint (file, 0);
  return result;
}



/* Write out the mhbd header. Size will be written later */
static void mk_mhbd (FILE *file)
{
  put_data (file, "mhbd", 4);
  put32lint (file, 104); /* header size */
  put32lint (file, -1);  /* size of whole mhdb -- fill in later */
  put32lint (file, 1);   /* ? */
  put32lint (file, 1);   /*  - changed to 2 from itunes2 to 3 ..
				    version? We are iTunes version 1 ;) */
  put32lint (file, 2);   /* ? */
  put_n0 (file, 20);    /* dummy space */
}

/* Fill in the length of a standard header */
static void fix_header (FILE *file, glong header_seek, glong cur)
{
  put32lint_seek (file, cur-header_seek, header_seek+8);
}


/* Write out the mhsd header. Size will be written later */
static void mk_mhsd (FILE *file, guint32 type)
{
  put_data (file, "mhsd", 4);
  put32lint (file, 96);   /* Headersize */
  put32lint (file, -1);   /* size of whole mhsd -- fill in later */
  put32lint (file, type); /* type: 1 = track, 2 = playlist */
  put_n0 (file, 20);    /* dummy space */
}


/* Write out the mhlt header. */
static void mk_mhlt (FILE *file, guint32 track_num)
{
  put_data (file, "mhlt", 4);
  put32lint (file, 92);         /* Headersize */
  put32lint (file, track_num);   /* tracks in this itunesdb */
  put_n0 (file, 20);           /* dummy space */
}


/* Write out the mhit header. Size will be written later */
static void mk_mhit (FILE *file, Track *track)
{
  put_data (file, "mhit", 4);
  put32lint (file, 156);  /* header size */
  put32lint (file, -1);   /* size of whole mhit -- fill in later */
  put32lint (file, -1);   /* nr of mhods in this mhit -- later   */
  put32lint (file, track->ipod_id); /* track index number
					* */
  put32lint (file, track->unk020);
  put32lint (file, track->unk024);
  put32lint (file, (track->rating << 24) |
		(track->compilation << 16) |
		(track->type & 0x0000ffff));/* rating, compil., type */

  put32lint (file, track->time_created); /* timestamp             */
  put32lint (file, track->size);    /* filesize                  */
  put32lint (file, track->tracklen); /* length of track in ms     */
  put32lint (file, track->track_nr);/* track number               */
  put32lint (file, track->tracks);  /* number of tracks           */
  put32lint (file, track->year);    /* the year                   */
  put32lint (file, track->bitrate); /* bitrate                    */
  put32lint (file, track->samplerate << 16);
  put32lint (file, track->volume);  /* volume adjust              */
  put32lint (file, track->starttime);
  put32lint (file, track->stoptime);
  put32lint (file, track->soundcheck);
  put32lint (file, track->playcount);/* playcount                 */
  put32lint (file, track->unk084);
  put32lint (file, track->time_played); /* last time played       */
  put32lint (file, track->cd_nr);   /* CD number                  */
  put32lint (file, track->cds);     /* number of CDs              */
  put32lint (file, track->unk100);
  put32lint (file, track->time_modified); /* timestamp            */
  put32lint (file, track->unk108);
  put32lint (file, track->unk112);
  put32lint (file, track->unk116);
  put32lint (file, (track->BPM << 16) |
		((track->app_rating & 0xff) << 8) |
		(track->checked & 0xff));
  put32lint (file, track->unk124);
  put32lint (file, track->unk128);
  put32lint (file, track->unk132);
  put32lint (file, track->unk136);
  put32lint (file, track->unk140);
  put32lint (file, track->unk144);
  put32lint (file, track->unk148);
  put32lint (file, track->unk152);
}


/* Fill in the missing items of the mhit header:
   total size and number of mhods */
static void fix_mhit (FILE *file, glong mhit_seek, glong cur, gint mhod_num)
{
  put32lint_seek (file, cur-mhit_seek, mhit_seek+8); /* size of whole mhit */
  put32lint_seek (file, mhod_num, mhit_seek+12);     /* nr of mhods        */
}


/* Write out one mhod header.
     type: see enum of MHMOD_IDs;
     data: utf16 string to pack for text items
           track ID for MHOD_ID_PLAYLIST
           SPLPref for MHOD_ID_SPLPREF
	   SPLRules for MHOD_ID_SPLRULES */
static void mk_mhod (FILE *file, enum MHOD_ID type, void *data)
{
  switch (type)
  {
  case MHOD_ID_TITLE:
  case MHOD_ID_PATH:
  case MHOD_ID_ALBUM:
  case MHOD_ID_ARTIST:
  case MHOD_ID_GENRE:
  case MHOD_ID_FDESC:
  case MHOD_ID_COMMENT:
  case MHOD_ID_COMPOSER:
  case MHOD_ID_GROUPING:
      if (data)
      {
	  /* length of string in words  */
	  guint32 len = utf16_strlen ((gunichar2 *)data);
	  put_data (file, "mhod", 4); /* header                     */
	  put32lint (file, 24);        /* size of header             */
	  put32lint (file, 2*len+40);  /* size of header + body      */
	  put32lint (file, type);      /* type of the entry          */
	  put_n0 (file, 2);           /* dummy space                */
	  /* end of header, start of data */
	  put32lint (file, 1);         /* always 1 for these MHOD_IDs*/
	  put32lint (file, 2*len);     /* size of string             */
	  put_n0 (file, 2);           /* unknown                    */
	  if (len != 0)
	  {
	      /* FIXME: this assumes "string" is writable.
		 However, this might not be the case,
		 e.g. ipod_name might be in read-only mem. */
	      fixup_little_utf16(data);
	      put_data (file, data, 2*len); /* the string */
	      fixup_little_utf16(data);
	  }
      }
      break;
  case MHOD_ID_PLAYLIST:
      put_data (file, "mhod", 4); /* header                     */
      put32lint (file, 24);        /* size of header             */
      put32lint (file, 44);        /* size of header + body      */
      put32lint (file, type);      /* type of the entry          */
      put_n0 (file, 2);           /* unknown                    */
      /* end of header, start of data */
      put32lint (file, (guint32)data);/* track ID of playlist member*/
      put_n0 (file, 4);           /* unknown                    */
      break;
  case MHOD_ID_SPLPREF:
      if (data)
      {
	  SPLPref *splp = data;
	  put_data (file, "mhod", 4); /* header                 */
	  put32lint (file, 24);        /* size of header         */
	  put32lint (file, 96);        /* size of header + body  */
	  put32lint (file, type);      /* type of the entry      */
	  put_n0 (file, 2);           /* unknown                */
	  /* end of header, start of data */
	  put8int (file, splp->liveupdate);
	  put8int (file, splp->checkrules? 1:0);
	  put8int (file, splp->checklimits);
	  put8int (file, splp->limittype);
	  put8int (file, splp->limitsort & 0xff);
	  put8int (file, 0);          /* unknown                */
	  put8int (file, 0);          /* unknown                */
	  put8int (file, 0);          /* unknown                */
	  put32lint (file, splp->limitvalue);
	  put8int (file, splp->matchcheckedonly);
	  /* for the following see note at definitions of limitsort
	     types in itunesdb.h */
	  put8int (file, (splp->limitsort & 0x80000000) ? 1:0);
	  put8int (file, 0);          /* unknown                */
	  put8int (file, 0);          /* unknown                */
	  put_n0 (file, 14);          /* unknown                */
      }
      break;
  case MHOD_ID_SPLRULES:
      if (data)
      {
	  SPLRules *splrs = data;
	  glong header_seek = ftell (file); /* needed to fix length */
	  GList *gl;
	  gint numrules = g_list_length (splrs->rules);

	  put_data (file, "mhod", 4); /* header                   */
	  put32lint (file, 24);       /* size of header           */
	  put32lint (file, -1);       /* total length, fix later  */
	  put32lint (file, type);     /* type of the entry        */
	  put_n0 (file, 2);           /* unknown                  */
	  /* end of header, start of data */
	  /* For some reason this is the only part of the iTunesDB
	     that uses big endian */
	  put_data (file, "SLst", 4);      /* header               */
	  put32bint (file, splrs->unk004); /* unknown              */
	  put32bint (file, numrules);
	  put32bint (file, splrs->match_operator);
	  put_n0 (file, 30);              /* unknown              */
	  /* end of header, now follow the rules */
	  for (gl=splrs->rules; gl; gl=gl->next)
	  {
	      SPLRule *splr = gl->data;
	      gint ft = itb_splr_get_field_type (splr);
/*	      printf ("%p: field: %d ft: %d\n", splr, splr->field, ft);*/
	      itb_splr_validate (splr);
	      put32bint (file, splr->field);
	      put32bint (file, splr->action);
	      put_n0 (file, 11);          /* unknown              */
	      if (ft == splft_string)
	      {   /* write string-type rule */
		  gint len = utf16_strlen (splr->string_utf16);
		  put32bint (file, 2*len); /* length of string     */
		  /* FIXME: this assumes string_utf16 is writable */
		  fixup_big_utf16 (splr->string_utf16);
		  put_data (file, (gchar *)splr->string_utf16, 2*len);
		  fixup_big_utf16 (splr->string_utf16);
	      }
	      else
	      {   /* write non-string-type rule */
		  put32bint (file, 0x44); /* length of data        */
		  /* data */
		  put64bint (file, splr->fromvalue);
		  put64bint (file, splr->fromdate);
		  put64bint (file, splr->fromunits);
		  put64bint (file, splr->tovalue);
		  put64bint (file, splr->todate);
		  put64bint (file, splr->tounits);
		  put32bint (file, splr->unk052);
		  put32bint (file, splr->unk056);
		  put32bint (file, splr->unk060);
		  put32bint (file, splr->unk064);
		  put32bint (file, splr->unk068);
	      }
	  }
	  /* insert length of mhod junk */
	  fix_header (file, header_seek, ftell (file));
      }
      break;
  case MHOD_ID_MHYP:
      printf ("Cannot write mhod of type %d\n", type);
      break;
  }
}


/* Write out the mhlp header. Size will be written later */
static void mk_mhlp (FILE *file, guint32 lists)
{
  put_data (file, "mhlp", 4);      /* header                   */
  put32lint (file, 92);             /* size of header           */
  put32lint (file, lists);          /* playlists on iPod (including main!) */
  put_n0 (file, 20);               /* dummy space              */
}


/* Fix the mhlp header (number of playlists) */
static void fix_mhlp (FILE *file, glong mhlp_seek, gint playlist_num)
{
  put32lint_seek (file, playlist_num, mhlp_seek+8); /* nr of playlists    */
}



/* Write out the long MHOD_ID_PLAYLIST mhod header.
   This seems to be an itunespref thing.. dunno know this
   but if we set everything to 0, itunes doesn't show any data
   even if you drag an mp3 to your ipod: nothing is shown, but itunes
   will copy the file!
   .. so we create a hardcoded-pref.. this will change in future
   Seems to be a Preferences mhod, every PL has such a thing
   FIXME !!! */
static void mk_long_mhod_id_playlist (FILE *file)
{
  put_data (file, "mhod", 4);      /* header                   */
  put32lint (file, 0x18);           /* size of header  ?        */
  put32lint (file, 0x0288);         /* size of header + body    */
  put32lint (file,MHOD_ID_PLAYLIST);/* type of the entry        */
  put_n0 (file, 6);
  put32lint (file, 0x010084);       /* ? */
  put32lint (file, 0x05);           /* ? */
  put32lint (file, 0x09);           /* ? */
  put32lint (file, 0x03);           /* ? */
  put32lint (file, 0x120001);       /* ? */
  put_n0 (file, 3);
  put32lint (file, 0xc80002);       /* ? */
  put_n0 (file, 3);
  put32lint (file, 0x3c000d);       /* ? */
  put_n0 (file, 3);
  put32lint (file, 0x7d0004);       /* ? */
  put_n0 (file, 3);
  put32lint (file, 0x7d0003);       /* ? */
  put_n0 (file, 3);
  put32lint (file, 0x640008);       /* ? */
  put_n0 (file, 3);
  put32lint (file, 0x640017);       /* ? */
  put32lint (file, 0x01);           /* bool? (visible? / colums?) */
  put_n0 (file, 2);
  put32lint (file, 0x500014);       /* ? */
  put32lint (file, 0x01);           /* bool? (visible?) */
  put_n0 (file, 2);
  put32lint (file, 0x7d0015);       /* ? */
  put32lint (file, 0x01);           /* bool? (visible?) */
  put_n0 (file, 114);
}



/* Header for new PL item */
static void mk_mhip (FILE *file, guint32 id)
{
  put_data (file, "mhip", 4);
  put32lint (file, 76);
  put32lint (file, 76);
  put32lint (file, 1);
  put32lint (file, 0);
  put32lint (file, id);  /* track id in playlist */
  put32lint (file, id);  /* ditto.. don't know the difference, but this
			   seems to work. Maybe a special ID used for
			   playlists? */
  put_n0 (file, 12);
}

static void
write_mhsd_one(FILE *file)
{
    Track *track;
    guint32 i, track_num, mhod_num;
    glong mhsd_seek, mhit_seek, mhlt_seek;

    track_num = it_get_nr_of_tracks();

    mhsd_seek = ftell (file);  /* get position of mhsd header */
    mk_mhsd (file, 1);         /* write header: type 1: track  */
    mhlt_seek = ftell (file);  /* get position of mhlt header */
    mk_mhlt (file, track_num);  /* write header with nr. of tracks */
    for (i=0; i<track_num; ++i)  /* Write each track */
    {
	if((track = it_get_track_by_nr (i)) == 0)
	{
	    g_warning ("Invalid track Index!\n");
	    break;
	}
	mhit_seek = ftell (file);
	mk_mhit (file, track);
	mhod_num = 0;
	if (utf16_strlen (track->title_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_TITLE, track->title_utf16);
	    ++mhod_num;
	}
	if (utf16_strlen (track->ipod_path_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_PATH, track->ipod_path_utf16);
	    ++mhod_num;
	}
	if (utf16_strlen (track->album_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_ALBUM, track->album_utf16);
	    ++mhod_num;
	}
	if (utf16_strlen (track->artist_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_ARTIST, track->artist_utf16);
	    ++mhod_num;
	}
	if (utf16_strlen (track->genre_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_GENRE, track->genre_utf16);
	    ++mhod_num;
	}
	if (utf16_strlen (track->fdesc_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_FDESC, track->fdesc_utf16);
	    ++mhod_num;
	}
	if (utf16_strlen (track->comment_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_COMMENT, track->comment_utf16);
	    ++mhod_num;
	}
	if (utf16_strlen (track->composer_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_COMPOSER, track->composer_utf16);
	    ++mhod_num;
	}
	if (utf16_strlen (track->grouping_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_GROUPING, track->grouping_utf16);
	    ++mhod_num;
	}
        /* Fill in the missing items of the mhit header */
	fix_mhit (file, mhit_seek, ftell (file), mhod_num);
    }
    fix_header (file, mhsd_seek, ftell (file));
}

/* corresponds to mk_mhyp */
static void
write_playlist(FILE *file, Playlist *pl)
{
    GList *gl;
    glong mhyp_seek = ftell(file);
    guint32 track_num = g_list_length (pl->members);

#if ITUNESDB_DEBUG
  fprintf(stderr, "Playlist: %s (%d tracks)\n", pl->name, track_num);
#endif

    put_data (file, "mhyp", 4);    /* header                    */
    put32lint (file, 108);          /* length		        */
    put32lint (file, -1);           /* size -> later             */
    if (pl->is_spl)
	put32lint (file, 4);        /* nr of mhods               */
    else
	put32lint (file, 2);        /* nr of mhods               */
    put32lint (file, track_num);    /* number of tracks in plist */
    put32lint (file, pl->type);     /* 1 = main, 0 = visible     */
    put32lint (file, 0);            /* ?                         */
    put64lint (file, pl->id);       /* 64 bit ID                 */
    put_n0 (file, 18);             /* ?                         */
    mk_mhod (file, MHOD_ID_TITLE, pl->name_utf16);
    mk_long_mhod_id_playlist (file);
    if (pl->is_spl)
    {  /* write the smart rules */
	mk_mhod (file, MHOD_ID_SPLPREF, &pl->splpref);
	mk_mhod (file, MHOD_ID_SPLRULES, &pl->splrules);
    }
    /* write hard-coded tracks */
    for (gl=pl->members; gl; gl=gl->next)
    {
	Track *track = gl->data;
	mk_mhip (file, track->ipod_id);
	mk_mhod (file, MHOD_ID_PLAYLIST, (void *)track->ipod_id);
    }
   fix_header (file, mhyp_seek, ftell(file));
}



/* Expects the master playlist to be (it_get_playlist_by_nr (0)) */
static void
write_mhsd_two(FILE *file)
{
    guint32 playlists, i;
    glong mhsd_seek, mhlp_seek;

    mhsd_seek = ftell (file);  /* get position of mhsd header */
    mk_mhsd (file, 2);         /* write header: type 2: playlists  */
    mhlp_seek = ftell (file);
    playlists = it_get_nr_of_playlists();
    mk_mhlp (file, playlists);
    for(i = 0; i < playlists; i++)
    {
	write_playlist(file, it_get_playlist_by_nr(i));
    }
    fix_mhlp (file, mhlp_seek, playlists);
    fix_header (file, mhsd_seek, ftell (file));
}


/* Do the actual writing to the iTunesDB */
gboolean
write_it (FILE *file)
{
    glong mhbd_seek;

    mhbd_seek = 0;
    mk_mhbd (file);
    write_mhsd_one(file);		/* write tracks mhsd */
    write_mhsd_two(file);		/* write playlists mhsd */
    fix_header (file, mhbd_seek, ftell (file));
    return TRUE;
}

/* Write out an iTunesDB.
   Note: only the _utf16 entries in the Track-struct are used

   An existing "Play Counts" file is renamed to "Play Counts.bak" if
   the export was successful.
   An existing "OTGPlaylistInfo" file is removed if the export was
   successful.

   Returns TRUE on success, FALSE on error.
   "path" should point to the mount point of the
   iPod, e.g. "/mnt/ipod" and be in local encoding */
gboolean itunesdb_write (const gchar *path)
{
    gchar *itunes_filename, *itunes_path;
/*    gchar *tmp_filename = g_build_filename (P_tmpdir, "itunesDB.XXXXXX", NULL);*/
/*     int fd; */
    const gchar *itunes[] = {"iPod_Control","iTunes",NULL};
    gboolean result = FALSE;

    itunes_path = resolve_path(path, itunes);
    
    if(!itunes_path)
      return FALSE;
    
    itunes_filename = g_build_filename (itunes_path, "iTunesDB", NULL);
    /* We write to /tmp/ first, then copy to the iPod. For me this
       means a speed increase from 38 seconds to 4 seconds (my iPod is
       mounted "sync" -- that means all changes are written to the
       iPod immediately. Because we are seeking around the file,
       writing is slow. */
/*     printf ("%s: %s\n", P_tmpdir, tmp_filename); */
/*     fd = mkstemp (tmp_filename); */
/*     printf ("%s: %d %d\n", tmp_filename, fd, errno); */
/*     if (fd != -1) */
/*     { */
/* 	close (fd); */
/* 	result = itunesdb_write_to_file (tmp_filename); */
/* 	if (result) */
/* 	{ */
/* 	    itunesdb_cp (tmp_filename, itunes_filename); */
/* 	    remove (tmp_filename); */
/* 	} */
/*     } */
/*     else */
/*     {   /\* tmp file could not be opened *\/ */
/* 	result = itunesdb_write_to_file (itunes_filename); */
/*     } */
/*  g_free(tmp_filename);*/
    result = itunesdb_write_to_file (itunes_filename);
    g_free(itunes_filename);
    g_free(itunes_path);
    return result;
}

/* Same as itnuesdb_write (), but you specify the filename directly */
gboolean itunesdb_write_to_file (const gchar *filename)
{
  FILE *file = NULL;
  gboolean result = FALSE;

#if ITUNESDB_DEBUG
  fprintf(stderr, "Writing to %s\n", filename);
#endif

  if((file = fopen (filename, "w+")))
    {
      write_it (file);
      fclose(file);
      result = TRUE;
    }
  else
    {
      itunesdb_warning (_("Could not open iTunesDB \"%s\" for writing.\n"),
		      filename);
      result = FALSE;
    }
  if (result == TRUE)
  {
      gchar *dirname = g_path_get_dirname (filename);
      itunesdb_rename_files (dirname);
      g_free (dirname);
  }
  return result;
}


void itunesdb_rename_files (const gchar *dirname)
{
    const gchar *db_itd[] =  {"iPod_Control", "iTunes", NULL};
    const gchar *db_plc_o[] = {"Play Counts", NULL};
    const gchar *db_otg[] = {"OTGPlaylistInfo", NULL};
    gchar *itunesdir = resolve_path (dirname, db_itd);
    gchar *plcname_o = resolve_path (itunesdir, db_plc_o);
    gchar *plcname_n = g_build_filename (itunesdir, 
					 "Play Counts.bak", NULL);
    gchar *otgname = resolve_path (itunesdir, db_otg);
/*    printf ("%s\n%s\n%s\n%s\n\n", itunesdir, plcname_o, plcname_n, otgname);*/
    /* rename "Play Counts" to "Play Counts.bak" */
    if (plcname_o)
    {
	if (rename (plcname_o, plcname_n) == -1)
	{   /* an error occured */
	    itunesdb_warning (_("Error renaming '%s' to '%s' (%s).\n"),
			      plcname_o, plcname_n, g_strerror (errno));
	}
    }
    /* remove "OTGPlaylistInfo" (the iPod will remove the remaining
     * files */
    if (otgname)
    {
	if (unlink (otgname) == -1)
	{   /* an error occured */
	    itunesdb_warning (_("Error removing '%s'.\n"), otgname);
	}
    }
    g_free (plcname_o);
    g_free (plcname_n);
    g_free (otgname);
    g_free (itunesdir);
}

/* Convert string from casual PC file name to iPod iTunesDB format
 * using ':' instead of slashes
 */
void itunesdb_convert_filename_fs2ipod (gchar *ipod_file)
{
    g_strdelimit (ipod_file, G_DIR_SEPARATOR_S, ':');
}

/* Convert string from iPod iTunesDB format to casual PC file name
 * using slashes instead of ':'
 */
void itunesdb_convert_filename_ipod2fs (gchar *ipod_file)
{
    g_strdelimit (ipod_file, ":", G_DIR_SEPARATOR);
}

/* create a new track structure with the "unknowns" filled in correctly
 * */ 
Track *itunesdb_new_track (void)
{
    Track *track = g_malloc0 (sizeof (Track));
    track->unk020 = 1;
    return track;
}

/* Copy one track to the ipod. The PC-Filename is
   "pcfile" and is taken literally.
   "path" is assumed to be the mountpoint of the iPod (in local
   encoding).
   For storage, the directories "f00 ... f19" will be
   cycled through. The filename is constructed from
   "track->ipod_id": "gtkpod_id" and written to
   "track->ipod_path_utf8" and "track->ipod_path_utf16" */
gboolean itunesdb_copy_track_to_ipod (const gchar *path,
				      Track *track,
				      const gchar *pcfile)
{
  static gint dir_num = -1;
  gchar *track_db_path = NULL, *ipod_fullfile = NULL;
  gchar *dest_components[] = {"","iPod_Control","Music",NULL,NULL,NULL},
    *parent_dir_filename;
  gchar *original_suffix;
  gchar dir_num_str[5];
  gboolean success;
  gint32 oops = 0;
  gint pathlen = 0;

#if ITUNESDB_DEBUG
  fprintf(stderr, "Entered itunesdb_copy_track_to_ipod: '%s', %p, '%s'\n", path, track, pcfile);
#endif
  if (track == NULL)
    {
      g_warning ("Programming error: copy_track_to_ipod () called NULL-track\n");
      return FALSE;
    }
  if(track->transferred == TRUE) return TRUE; /* nothing to do */ 

  if (path) pathlen = strlen (path); /* length of path in bytes */
  
  if (dir_num == -1) dir_num = g_random_int_range (0, 20);
  else dir_num = (dir_num + 1) % 20;
  
  g_snprintf(dir_num_str,5,"F%02d",dir_num);
  dest_components[3] = dir_num_str;
  
  parent_dir_filename = resolve_path(path,(const gchar **)dest_components);
  if(parent_dir_filename == NULL) {
          /* Can't find the parent of the filenames we're going to generate to copy into */
          return FALSE;
  }

  /* we may need the original suffix of pcfile to construct a correct
     ipod filename */
  original_suffix = strrchr (pcfile, '.');
  /* If there is no ".mp3", ".m4a" etc, set original_suffix to empty
     string. Note: the iPod will most certainly ignore this file... */
  if (!original_suffix) original_suffix = "";

  /* If track->ipod_path exists, we use that one instead. */

  for (ipod_fullfile = itunesdb_get_track_name_on_ipod (path, track) ; !ipod_fullfile ; oops++)
  { /* we need to loop until we find an unused filename */
      dest_components[4] = 
        g_strdup_printf("gtkpod%05d%s",track->ipod_id + oops,original_suffix);
      ipod_fullfile = resolve_path(path,
                                   (const gchar **)dest_components);
      if(ipod_fullfile)
      {
              g_free(ipod_fullfile);
              ipod_fullfile = NULL;
      }
      else
      {
        ipod_fullfile = g_build_filename(parent_dir_filename,
					 dest_components[4], NULL);
        track_db_path = g_strjoinv(":", dest_components);
/* 	printf ("ff: %s\ndb: %s\n", ipod_fullfile, track_db_path); */
      }
      g_free(dest_components[4]);
  }
  
  if(!track_db_path)
    track_db_path = g_strdup(track->ipod_path);
  

#if ITUNESDB_DEBUG
  fprintf(stderr, "ipod_fullfile: '%s'\n", ipod_fullfile);
#endif

  success = itunesdb_cp (pcfile, ipod_fullfile);
  if (success)
  {
      track->transferred = TRUE;
#ifdef ITUNESDB_PROVIDE_UTF8
      if (track->ipod_path) g_free (track->ipod_path);
      track->ipod_path = g_strdup (track_db_path);
#endif
      if (track->ipod_path_utf16) g_free (track->ipod_path_utf16);
      track->ipod_path_utf16 = g_utf8_to_utf16 (track_db_path, -1, NULL, NULL, NULL);
  }
  
  g_free(parent_dir_filename);
  g_free (track_db_path);
  g_free (ipod_fullfile);
  return success;
}


/* Return the full iPod filename as stored in @track. Return value
   must be g_free()d after use.
   @path: mount point of the iPod file system (in local encoding)
   @track: track
   Return value: full filename to @track on the iPod or NULL if no
   filename is set in @track. NOTE: the file does not necessarily
   exist. NOTE: this code works around a problem on some systems (see
   below) and might return a filename with different case than the
   original filename. Don't copy it back to @track */
gchar *itunesdb_get_track_name_on_ipod (const gchar *path, Track *track)
{
  gchar *result,*buf,*good_path,**components,*resolved;

  if(!track || !track->ipod_path || !*track->ipod_path)
    return NULL;
        
  buf = g_strdup (track->ipod_path);
  itunesdb_convert_filename_ipod2fs (buf);
  result = g_build_filename (path, buf, NULL);
  g_free(buf);
  if (g_file_test (result, G_FILE_TEST_EXISTS))
    return result;
    
  g_free(result);
  
  good_path = g_filename_from_utf8(path,-1,NULL,NULL,NULL);
  components = g_strsplit(track->ipod_path,":",10);
  
  resolved = resolve_path(good_path,(const gchar **)components);
  
  g_free(good_path);
  g_strfreev(components);
  
  return resolved;
}


/* Copy file "from_file" to "to_file".
   Returns TRUE on success, FALSE otherwise */
gboolean itunesdb_cp (const gchar *from_file, const gchar *to_file)
{
  gchar *data=g_malloc (ITUNESDB_COPYBLK);
  glong bread, bwrite;
  gboolean success = TRUE;
  FILE *file_in = NULL;
  FILE *file_out = NULL;

#if ITUNESDB_DEBUG
  fprintf(stderr, "Entered itunesdb_cp: '%s', '%s'\n", from_file, to_file);
#endif

  do { /* dummy loop for easier error handling */
    file_in = fopen (from_file, "r");
    if (file_in == NULL)
      {
	itunesdb_warning (_("Could not open file \"%s\" for reading.\n"), from_file);
	success = FALSE;
	break;
      }
    file_out = fopen (to_file, "w");
    if (file_out == NULL)
      {
	itunesdb_warning (_("Could not open file \"%s\" for writing.\n"), to_file);
	success = FALSE;
	break;
      }
    do {
      bread = fread (data, 1, ITUNESDB_COPYBLK, file_in);
#if ITUNESDB_DEBUG
      fprintf(stderr, "itunesdb_cp: read %ld bytes\n", bread);
#endif
      if (bread == 0)
	{
	  if (feof (file_in) == 0)
	    { /* error -- not end of file! */
	      itunesdb_warning (_("Error reading file \"%s\".\n"), from_file);
	      success = FALSE;
	    }
	}
      else
	{
	  bwrite = fwrite (data, 1, bread, file_out);
#if ITUNESDB_DEBUG
      fprintf(stderr, "itunesdb_cp: wrote %ld bytes\n", bwrite);
#endif
	  if (bwrite != bread)
	    {
	      itunesdb_warning (_("Error writing PC file \"%s\".\n"),to_file);
	      success = FALSE;
	    }
	}
    } while (success && (bread != 0));
  } while (FALSE);
  if (file_in)  fclose (file_in);
  if (file_out)
    {
      fclose (file_out);
      if (!success)
      { /* error occured -> delete to_file */
#if ITUNESDB_DEBUG
	  fprintf(stderr, "itunesdb_cp: copy unsuccessful, removing '%s'\n", to_file);
#endif
	remove (to_file);
      }
    }
  g_free (data);
  return success;
}



/*------------------------------------------------------------------*\
 *                                                                  *
 *                       Timestamp stuff                            *
 *                                                                  *
\*------------------------------------------------------------------*/

guint64 itunesdb_time_get_mac_time (void)
{
    GTimeVal time;

    g_get_current_time (&time);
    return itunesdb_time_host_to_mac (time.tv_sec);
}


/* convert Macintosh timestamp to host system time stamp -- modify
 * this function if necessary to port to host systems with different
 * start of Epoch */
/* A "0" time will not be converted */
time_t itunesdb_time_mac_to_host (guint64 mactime)
{
    if (mactime != 0)  return (time_t)(mactime - 2082844800);
    else               return (time_t)mactime;
}


/* convert host system timestamp to Macintosh time stamp -- modify
 * this function if necessary to port to host systems with different
 * start of Epoch */
guint64 itunesdb_time_host_to_mac (time_t time)
{
    return (guint64)(((gint64)time) + 2082844800);
}
