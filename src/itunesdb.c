/* Time-stamp: <2003-06-19 23:57:26 jcs>
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
*/




/* Some notes on how to use the functions in this file:


   *** Reading the iTunesDB ***

   gboolean itunesdb_parse (gchar *path); /+ path to mountpoint +/
   will read an iTunesDB and pass the data over to your program. Your
   programm is responsible to keep a representation of the data.

   The information given in the "Play Counts" file is also read if
   available and the playcounts, star rating and the time last played
   is updated.

   For each song itunesdb_parse() will pass a filled out Song structure
   to "it_add_song()", which has to be provided. The return value is
   TRUE on success and FALSE on error. At the time being, the return
   value is ignored, however.

   The minimal Song structure looks like this (feel free to have
   it_add_song() do with it as it pleases -- and yes, you are
   responsible to free the memory):

   typedef struct
   {
     gunichar2 *album_utf16;    /+ album (utf16)         +/
     gunichar2 *artist_utf16;   /+ artist (utf16)        +/
     gunichar2 *title_utf16;    /+ title (utf16)         +/
     gunichar2 *genre_utf16;    /+ genre (utf16)         +/
     gunichar2 *comment_utf16;  /+ comment (utf16)       +/
     gunichar2 *composer_utf16; /+ Composer (utf16)      +/
     gunichar2 *fdesc_utf16;    /+ ? (utf16)             +/
     gunichar2 *ipod_path_utf16;/+ name of file on iPod: uses ":" instead of "/" +/
     guint32 ipod_id;           /+ unique ID of song     +/
     gint32  size;              /+ size of file in bytes +/
     gint32  songlen;           /+ Length of song in ms  +/
     gint32  cd_nr;             /+ CD number             +/
     gint32  cds;               /+ number of CDs         +/
     gint32  track_nr;          /+ track number          +/
     gint32  tracks;            /+ number of tracks      +/
     gint32  year;              /+ year                  +/
     gint32  bitrate;           /+ bitrate               +/
     guint32 time_played;       /+ time of last play  (Mac type)         +/
     guint32 time_modified;     /+ time of last modification  (Mac type) +/
     guint32 rating;            /+ star rating (stars * 20)              +/
     guint32 playcount;         /+ number of times song was played       +/
     gboolean transferred;      /+ has file been transferred to iPod?    +/
   } Song;

   "transferred" will be set to TRUE because all songs read from a
   iTunesDB are obviously (or hopefully) already transferred to the
   iPod.

   By #defining ITUNESDB_PROVIDE_UTF8, itunesdb_parse() will also
   provide utf8 versions of the above utf16 strings. You must then add
   members "gchar *album"... to the Song structure.

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
   playlist to be referenced when it_add_song_to_playlist() is called.

   For each song in the playlist, it_add_songid_to_playlist() is called
   with the above mentioned pointer to the playlist and the songid to
   be added.

   gboolean it_add_song (Song *song);
   Playlist *it_add_playlist (Playlist *plitem);
   void it_add_songid_to_playlist (Playlist *plitem, guint32 id);


   *** Writing the iTunesDB ***

   gboolean itunesdb_write (gchar *path), /+ path to mountpoint +/
   will write an updated version of the iTunesDB.

   The "Play Counts" file is renamed to "Play Counts.bak" if it exists
   to avoid it being read multiple times.

   It uses the following functions to retrieve the data necessary data
   from memory:

   guint it_get_nr_of_songs (void);
   Song *it_get_song_by_nr (guint32 n);
   guint32 it_get_nr_of_playlists (void);
   Playlist *it_get_playlist_by_nr (guint32 n);
   guint32 it_get_nr_of_songs_in_playlist (Playlist *plitem);
   Song *it_get_song_in_playlist_by_nr (Playlist *plitem, guint32 n);

   The master playlist is expected to be "it_get_playlist_by_nr(0)". Only
   the utf16 strings in the Playlist and Song struct are being used.

   Please note that non-transferred songs are not automatically
   transferred to the iPod. A function

   gboolean itunesdb_copy_song_to_ipod (gchar *path, Song *song, gchar *pcfile)

   is provided to help you do that, however.

   The following functions most likely will also come in handy:

   gchar *itunesdb_concat_dir (G_CONST_RETURN gchar *dir,
                               G_CONST_RETURN gchar *file);
   gboolean itunesdb_cp (gchar *from_file, gchar *to_file);
   guint32 itunesdb_time_get_mac_time (void);
   time_t itunesdb_time_mac_to_host (guint32 mactime);
   guint32 itunesdb_time_host_to_mac (time_t time);

   Define "itunesdb_warning()" as you need (or simply use g_print and
   change the default g_print handler with g_set_print_handler() as is
   done in gtkpod).

   Jorg Schuler, 19.12.2002 */



#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "itunesdb.h"
#include "support.h"

#ifdef IS_GTKPOD
/* we're being linked with gtkpod */
#define itunesdb_warning(...) g_print(__VA_ARGS__)
#else
/* The following prints the error messages to the shell, converting
 * UTF8 to the current locale on the fly: */
#define itunesdb_warning(...) do { gchar *utf8=g_strdup_printf (__VA_ARGS__); gchar *loc=g_locale_from_utf8 (utf8, -1, NULL, NULL, NULL); fprintf (stderr, "%s", loc); g_free (loc); g_free (utf8);} while (FALSE)
#endif

/* We instruct itunesdb_parse to provide utf8 versions of the strings */
#define ITUNESDB_PROVIDE_UTF8

#define ITUNESDB_DEBUG 0
#define ITUNESDB_MHIT_DEBUG 0
/* call itunesdb_parse () to read the iTunesDB  */
/* call itunesdb_write () to write the iTunesDB */

/* list with the contents of the Play Count file for use when
 * importing the iTunesDB */
GList *playcounts = NULL;

/* structure to hold the contents of one entry of the Play Count file */
struct playcount {
    guint32 playcount;
    guint32 time_played;
    guint32 rating;
};

static struct playcount *get_next_playcount (void);

static guint32 utf16_strlen(gunichar2 *utf16_string);

/* Concats "dir" and "file" into full filename, taking
   into account that "dir" may or may not end with "/".
   You must free the return string after use
   This code tries to take into account some stupid constellations
   when either "dir" or "file" is not set, or file starts with a "/"
   (ignore) etc.  */
gchar *itunesdb_concat_dir (G_CONST_RETURN gchar *dir, G_CONST_RETURN gchar *file)
{
    if (file && (*file == '/'))
    { 
	if (dir && (strlen (dir) > 0))
	{
	    return itunesdb_concat_dir (dir, file+1);
	}
    }
    if (dir && (strlen (dir) > 0))
    {	    
	if(dir[strlen(dir)-1] == '/')
	{ /* "dir" ends with "/" */
	    if (file)
		return g_strdup_printf ("%s%s", dir, file);
	    else
		return g_strdup (dir);
	}
	else
	{ /* "dir" does not end with "/" */
	    if (file)
		return g_strdup_printf ("%s/%s", dir, file);
	    else
		return g_strdup_printf ("%s/", dir);
	}
    }
    else
    { /* dir == NULL or 0-byte long */
	if (file)
	    return g_strdup (file);
	else
	    return g_strdup (""); /* how stupid can the caller be... */
    }
}


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


/* Get the 4-byte-number stored at position "seek" in "file"
   (or -1 when an error occured) */
static guint32 get4int(FILE *file, glong seek)
{
  guchar data[4];
  guint32 n;

  if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
  n =  ((guint32)data[3]) << 24;
  n += ((guint32)data[2]) << 16;
  n += ((guint32)data[1]) << 8;
  n += ((guint32)data[0]);
  return n;
}




/* Fix UTF16 String for BIGENDIAN machines (like PPC) */
static gunichar2 *fixup_utf16(gunichar2 *utf16_string) {
#if (G_BYTE_ORDER == G_BIG_ENDIAN)
gint32 i;
 if (utf16_string)
 {
     for(i=0; i<utf16_strlen(utf16_string); i++)
     {
	 utf16_string[i] = ((utf16_string[i]<<8) & 0xff00) | 
	     ((utf16_string[i]>>8) & 0xff);
     }
 }
#endif
return utf16_string;
}


/* return the length of the header *ml, the genre number *mty,
   and a string with the entry (in UTF16?). After use you must
   free the string with g_free (). Returns NULL in case of error. */
static gunichar2 *get_mhod (FILE *file, glong seek, gint32 *ml, gint32 *mty)
{
  gchar data[4];
  gunichar2 *entry_utf16 = NULL;
  gint32 xl;

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
  *ml = get4int (file, seek+8);       /* length         */
  *mty = get4int (file, seek+12);     /* mhod_id number */
  xl = get4int (file, seek+28);       /* entry length   */

#if ITUNESDB_DEBUG
  fprintf(stderr, "ml: %x mty: %x, xl: %x\n", *ml, *mty, xl);
#endif

  switch (*mty)
  {
  case MHOD_ID_PLAYLIST: /* do something with the "weird mhod" */
      break;
  default:
      entry_utf16 = g_malloc (xl+2);
      if (seek_get_n_bytes (file, (gchar *)entry_utf16, seek+40, xl) != xl) {
	  g_free (entry_utf16);
	  entry_utf16 = NULL;
	  *ml = -1;
      }
      else
      {
	  entry_utf16[xl/2] = 0; /* add trailing 0 */
      }
      break;
  }
  return fixup_utf16(entry_utf16);
}



/* get a PL, return pos where next PL should be, name and content */
static glong get_pl(FILE *file, glong seek) 
{
  gunichar2 *plname_utf16 = NULL, *plname_utf16_maybe;
#ifdef ITUNESDB_PROVIDE_UTF8
  gchar *plname_utf8;
#endif
  guint32 type, pltype, songnum, n;
  guint32 nextseek;
  gint32 zip;
  Playlist *plitem;
  guint32 ref;

  gchar data[4];


#if ITUNESDB_DEBUG
  fprintf(stderr, "mhyp seek: %x\n", (int)seek);
#endif

  if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
  if (cmp_n_bytes (data, "mhyp", 4) == FALSE)      return -1; /* not pl */
  /* Some Playlists have added 256 to their type -- I don't know what
     it's for, so we just ignore it for now -> & 0xff */
  pltype = get4int (file, seek+20) & 0xff;  /* Type of playlist (1= MPL) */
  songnum = get4int (file, seek+16); /* number of songs in playlist */
  nextseek = seek + get4int (file, seek+8); /* possible begin of next PL */
  zip = get4int (file, seek+4); /* length of header */
  if (zip == 0) return -1;      /* error! */
  do
  {
      seek += zip;
      if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
      plname_utf16_maybe = get_mhod(file, seek, &zip, &type); /* PL name */
      if (zip != -1) switch (type)
      {
      case MHOD_ID_PLAYLIST:
	  break; /* here we could do something about the "weird mhod" */
      case MHOD_ID_TITLE:
	  if (plname_utf16_maybe)
	  {
	      /* sometimes there seem to be two mhod TITLE headers */
	      if (plname_utf16) g_free (plname_utf16);
	      plname_utf16 = plname_utf16_maybe;
	  }
	  break;
      }
  } while (zip != -1); /* read all MHODs */
  if (!plname_utf16)
  {   /* we did not read a valid mhod TITLE header -> */
      /* we simply make up our own name */
	if (pltype == 1)
	    plname_utf16 = g_utf8_to_utf16 (_("Master-PL"),
					    -1, NULL, NULL, NULL);
	else plname_utf16 = g_utf8_to_utf16 (_("Playlist"),
					     -1, NULL, NULL, NULL);
  }
#ifdef ITUNESDB_PROVIDE_UTF8
  plname_utf8 = g_utf16_to_utf8 (plname_utf16, -1, NULL, NULL, NULL);
#endif 


#if ITUNESDB_DEBUG
  fprintf(stderr, "pln: %s(%d Tracks) \n", plname_utf8, (int)songnum);
#endif

  plitem = g_malloc0 (sizeof (Playlist));

#ifdef ITUNESDB_PROVIDE_UTF8
  plitem->name = plname_utf8;
#endif
  plitem->name_utf16 = plname_utf16;
  plitem->type = pltype;

  /* create new playlist */
  plitem = it_add_playlist(plitem);

#if ITUNESDB_DEBUG
  fprintf(stderr, "added pl: %s", plname_utf8);
#endif

  n = 0;  /* number of songs read */
  while (n < songnum)
    {
      /* We read the mhip headers and skip everything else. If we
	 find a mhyp header before n==songnum, something is wrong */
      if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
      if (cmp_n_bytes (data, "mhyp", 4) == TRUE) return -1; /* Wrong!!! */
      if (cmp_n_bytes (data, "mhip", 4) == TRUE)
	{
	  ref = get4int(file, seek+24);
	  it_add_songid_to_playlist(plitem, ref);
	  ++n;
	}
      seek += get4int (file, seek+8);
    }
  return nextseek;
}


static glong get_mhit(FILE *file, glong seek)
{
  Song *song;
  gchar data[4];
#ifdef ITUNESDB_PROVIDE_UTF8
  gchar *entry_utf8;
#endif 
  gunichar2 *entry_utf16;
  gint type;
  gint zip = 0;
  struct playcount *playcount;

#if ITUNESDB_DEBUG
  fprintf(stderr, "get_mhit seek: %x\n", (int)seek);
#endif

  if (seek_get_n_bytes (file, data, seek, 4) != 4) return -1;
  if (cmp_n_bytes (data, "mhit", 4) == FALSE ) return -1; /* we are lost! */

  song = g_malloc0 (sizeof (Song));

  song->ipod_id = get4int(file, seek+16);     /* iPod ID          */
  song->rating = get4int(file, seek+28) >> 24;/* rating           */
  song->time_modified = get4int(file, seek+32);/* modification time    */
  song->size = get4int(file, seek+36);        /* file size        */
  song->songlen = get4int(file, seek+40);     /* time             */
  song->track_nr = get4int(file, seek+44);    /* track number     */
  song->tracks = get4int(file, seek+48);      /* nr of tracks     */
  song->year = get4int(file, seek+52);        /* year             */
  song->bitrate = get4int(file, seek+56);     /* bitrate          */
  song->playcount = get4int(file, seek+80);   /* playcount        */
  song->time_played = get4int(file, seek+88); /* last time played */
  song->cd_nr = get4int(file, seek+92);       /* CD nr            */
  song->cds = get4int(file, seek+96);         /* CD nr of..       */
  song->transferred = TRUE;                   /* song is on iPod! */

#if ITUNESDB_MHIT_DEBUG
time_t time_mac_to_host (guint32 mactime);
gchar *time_time_to_string (time_t time);
#define printf_mhit(sk, str)  printf ("%3d: %d (%s)\n", sk, get4int (file, seek+sk), str);
#define printf_mhit_time(sk, str) { gchar *buf = time_time_to_string (itunesdb_time_mac_to_host (get4int (file, seek+sk))); printf ("%3d: %s (%s)\n", sk, buf, str); g_free (buf); }
  {
      printf ("\nmhit: seek=%lu\n", seek);
      printf_mhit (  4, "header size");
      printf_mhit (  8, "mhit size");
      printf_mhit ( 12, "nr of mhods");
      printf_mhit ( 16, "iPod ID");
      printf_mhit ( 20, "?");
      printf_mhit ( 24, "?");
      printf (" 28: %u (type)\n", get4int (file, seek+28) & 0xffffff);
      printf (" 28: %u (rating)\n", get4int (file, seek+28) >> 24);
      printf_mhit ( 32, "timestamp file");
      printf_mhit_time ( 32, "timestamp file");
      printf_mhit ( 36, "size");
      printf_mhit ( 40, "songlen (ms)");
      printf_mhit ( 44, "track_nr");
      printf_mhit ( 48, "total tracks");
      printf_mhit ( 52, "year");
      printf_mhit ( 56, "bitrate");
      printf_mhit ( 60, "sample rate");
      printf (" 60: %u (sample rate LSB)\n", get4int (file, seek+60) & 0xffff);
      printf (" 60: %u (sample rate HSB)\n", (get4int (file, seek+60) >> 16));
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

  seek += get4int (file, seek+4);             /* 1st mhod starts here! */
  while(zip != -1)
    {
     seek += zip;
     entry_utf16 = get_mhod (file, seek, &zip, &type);
     if (entry_utf16 != NULL) {
#ifdef ITUNESDB_PROVIDE_UTF8
       entry_utf8 = g_utf16_to_utf8 (entry_utf16, -1, NULL, NULL, NULL);
#endif 
       switch (type)
	 {
	 case MHOD_ID_ALBUM:
#ifdef ITUNESDB_PROVIDE_UTF8
	   song->album = entry_utf8;
#endif 
	   song->album_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_ARTIST:
#ifdef ITUNESDB_PROVIDE_UTF8
	   song->artist = entry_utf8;
#endif
	   song->artist_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_TITLE:
#ifdef ITUNESDB_PROVIDE_UTF8
	   song->title = entry_utf8;
#endif
	   song->title_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_GENRE:
#ifdef ITUNESDB_PROVIDE_UTF8
	   song->genre = entry_utf8;
#endif
	   song->genre_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_PATH:
#ifdef ITUNESDB_PROVIDE_UTF8
	   song->ipod_path = entry_utf8;
#endif
	   song->ipod_path_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_FDESC:
#ifdef ITUNESDB_PROVIDE_UTF8
	   song->fdesc = entry_utf8;
#endif
	   song->fdesc_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_COMMENT:
#ifdef ITUNESDB_PROVIDE_UTF8
	   song->comment = entry_utf8;
#endif
	   song->comment_utf16 = entry_utf16;
	   break;
	 case MHOD_ID_COMPOSER:
#ifdef ITUNESDB_PROVIDE_UTF8
	   song->composer = entry_utf8;
#endif
	   song->composer_utf16 = entry_utf16;
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
      if (playcount->rating)  song->rating = playcount->rating;
      if (playcount->time_played) song->time_played = playcount->time_played;
      song->playcount += playcount->playcount;
      g_free (playcount);
  }
  it_add_song (song);
  return seek;   /* no more black magic */
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
 * directory contained in @filename) and set up the GList *playcounts
 * */
static void init_playcounts (gchar *filename)
{
  gchar *dirname = g_path_get_dirname (filename);
  gchar *plcname = itunesdb_concat_dir (dirname, "Play Counts");
  FILE *plycts = fopen (plcname, "r");
  gboolean error = TRUE;

  reset_playcounts ();

  if (plycts) do
  {
      gchar data[4];
      guint32 header_length, entry_length, entry_num, i=0;
      time_t tt;

      localtime (&tt);  /* set the ext. variable 'timezone' (see below) */
      if (seek_get_n_bytes (plycts, data, 0, 4) != 4)  break;
      if (cmp_n_bytes (data, "mhdp", 4) == FALSE)      break;
      header_length = get4int (plycts, 4);
      /* all the headers I know are 0x60 long -- if this one is longer
	 we can simply ignore the additional information */
      if (header_length < 0x60)                        break;
      entry_length = get4int (plycts, 8);
      /* all the entries I know are 0x0c (firmware 1.3) or 0x10
       * (firmware 2.0) in length */
      if (entry_length < 0x0c)                         break;
      /* number of entries */
      entry_num = get4int (plycts, 12);
      for (i=0; i<entry_num; ++i)
      {
	  struct playcount *playcount = g_malloc0 (sizeof (struct playcount));
	  glong seek = header_length + i*entry_length;

	  playcounts = g_list_append (playcounts, playcount);
	  /* check if entry exists by reading its last four bytes */
	  if (seek_get_n_bytes (plycts, data,
				seek+entry_length-4, 4) != 4) break;
	  playcount->playcount = get4int (plycts, seek);
	  playcount->time_played = get4int (plycts, seek+4);
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
	      playcount->rating = get4int (plycts, seek+12);
      }
      if (i == entry_num)  error = FALSE;
  } while (FALSE);
  if (plycts)  fclose (plycts);
  if (error)   reset_playcounts ();
  g_free (dirname);
  g_free (plcname);
}


/* Parse the iTunesDB and store the songs using it_addsong () defined
   in song.c.
   Returns TRUE on success, FALSE on error.
   "path" should point to the mount point of the iPod,
   e.e. "/mnt/ipod" */
/* Support for playlists should be added later */
gboolean itunesdb_parse (gchar *path)
{
  gchar *filename = NULL;
  gboolean result;

  filename = itunesdb_concat_dir (path, "iPod_Control/iTunes/iTunesDB");
  result = itunesdb_parse_file (filename);
  if (filename)  g_free (filename);
  return result;
}

/* Same as itunesdb_parse(), but let's specify the filename directly */
gboolean itunesdb_parse_file (gchar *filename)
{
  FILE *itunes = NULL;
  gboolean result = FALSE;
  gchar data[8];
  glong seek=0, pl_mhsd=0;
  guint32 zip, nr_songs=0, nr_playlists=0;
  gboolean swapped_mhsd = FALSE;

#if ITUNESDB_DEBUG
  fprintf(stderr, "Parsing %s\nenter: %4d\n", filename, it_get_nr_of_songs ());
#endif

  if (!filename) return FALSE;

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
      seek = get4int (itunes, 4);
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
	      if (get4int (itunes, seek + 12) == 1)
	      { /* OK, songlist, save start of playlists */
		  if (!swapped_mhsd)
		      pl_mhsd = seek + get4int (itunes, seek+8);
	      }
	      else if (get4int (itunes, seek + 12) == 2)
	      { /* bad: these are playlists... switch */
		  if (swapped_mhsd)
		  { /* already switched once -> forget it */
		      break;
		  }
		  else
		  {
		      pl_mhsd = seek;
		      seek += get4int (itunes, seek+8);
		      swapped_mhsd = TRUE;
		  }
	      }
	      else
	      { /* neither playlist nor song MHSD --> skip it */
		  seek += get4int (itunes, seek+8);
	      }
	  }
	  if (cmp_n_bytes (data, "mhlt", 4) == TRUE)
	  { /* mhlt header -> number of songs */
	      nr_songs = get4int (itunes, seek+8);
	      if (nr_songs == 0)
	      {   /* no songs -- skip directly to next mhsd */
		  result = TRUE;
		  break;
	      }
	  }
	  if (cmp_n_bytes (data, "mhit", 4) == TRUE)
	  { /* mhit header -> start of songs*/
	      result = TRUE;
	      break;
	  }
	  zip = get4int (itunes, seek+4);
	  if (zip == 0)  break;
	  seek += zip;
      } while (result == FALSE);
      if (result == FALSE)  break; /* some error occured */
      result = FALSE;
      /* now we should be at the first MHIT */

      /* Read Play Count file if available */
      init_playcounts (filename);

      /* get every file entry */
      if (nr_songs)  while(seek != -1) {
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
	      if (get4int (itunes, seek + 12) != 2)
	      {  /* this is not a playlist MHSD -> skip it */
		  seek += get4int (itunes, seek+8);
	      }
	  }
	  if (cmp_n_bytes (data, "mhlp", 4) == TRUE)
	  { /* mhlp header -> number of playlists */
	      nr_playlists = get4int (itunes, seek+8);
	  }
	  if (cmp_n_bytes (data, "mhyp", 4) == TRUE)
	  { /* mhyp header -> start of playlists */
	      result = TRUE;
	      break;
	  }
	  zip = get4int (itunes, seek+4);
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
#if ITUNESDB_DEBUG
  fprintf(stderr, "exit:  %4d\n", it_get_nr_of_songs ());
#endif 
  return result;
}


/* up to here we had the routines for reading the iTunesDB                */
/* ---------------------------------------------------------------------- */
/* from here on we have the routines for writing the iTunesDB             */

/* Name of the device in utf16 */
gunichar2 ipod_name[] = { 'g', 't', 'k', 'p', 'o', 'd', 0 };


/* Get length of utf16 string in number of characters (words) */
static guint32 utf16_strlen (gunichar2 *utf16)
{
  guint32 i=0;
  while (utf16[i] != 0) ++i;
  return i;
}


/* Write 4-byte-integer "n" in correct order to "data".
   "data" must be sufficiently long ... */
static void store4int (guint32 n, guchar *data)
{
  data[3] = (n >> 24) & 0xff;
  data[2] = (n >> 16) & 0xff;
  data[1] = (n >>  8) & 0xff;
  data[0] =  n & 0xff;
}


/* Write "data", "n" bytes long to current position in file.
   Returns TRUE on success, FALSE otherwise */
static gboolean put_data_cur (FILE *file, gchar *data, gint n)
{
  if (fwrite (data, 1, n, file) != n) return FALSE;
  return TRUE;
}

/* Write 4-byte integer "n" to "file".
   Returns TRUE on success, FALSE otherwise */
static gboolean put_4int_cur (FILE *file, guint32 n)
{
  gchar data[4];

  store4int (n, data);
  return put_data_cur (file, data, 4);
}


/* Write 4-byte integer "n" to "file" at position "seek".
   After writing, the file position indicator is set
   to the end of the file.
   Returns TRUE on success, FALSE otherwise */
static gboolean put_4int_seek (FILE *file, guint32 n, gint seek)
{
  gboolean result;

  if (fseek (file, seek, SEEK_SET) != 0) return FALSE;
  result = put_4int_cur (file, n);
  if (fseek (file, 0, SEEK_END) != 0) return FALSE;
  return result;
}


/* Write "n" times 4-byte-zero at current position
   Returns TRUE on success, FALSE otherwise */
static gboolean put_n0_cur (FILE*file, guint32 n)
{
  guint32 i;
  gboolean result = TRUE;

  for (i=0; i<n; ++i)  result &= put_4int_cur (file, 0);
  return result;
}



/* Write out the mhbd header. Size will be written later */
static void mk_mhbd (FILE *file)
{
  put_data_cur (file, "mhbd", 4);
  put_4int_cur (file, 104); /* header size */
  put_4int_cur (file, -1);  /* size of whole mhdb -- fill in later */
  put_4int_cur (file, 1);   /* ? */
  put_4int_cur (file, 1);   /*  - changed to 2 from itunes2 to 3 .. 
				    version? We are iTunes version 1 ;) */
  put_4int_cur (file, 2);   /* ? */
  put_n0_cur (file, 20);    /* dummy space */
}

/* Fill in the missing items of the mhsd header:
   total size and number of mhods */
static void fix_mhbd (FILE *file, glong mhbd_seek, glong cur)
{
  put_4int_seek (file, cur-mhbd_seek, mhbd_seek+8); /* size of whole mhit */
}


/* Write out the mhsd header. Size will be written later */
static void mk_mhsd (FILE *file, guint32 type)
{
  put_data_cur (file, "mhsd", 4);
  put_4int_cur (file, 96);   /* Headersize */
  put_4int_cur (file, -1);   /* size of whole mhsd -- fill in later */
  put_4int_cur (file, type); /* type: 1 = song, 2 = playlist */
  put_n0_cur (file, 20);    /* dummy space */
}  


/* Fill in the missing items of the mhsd header:
   total size and number of mhods */
static void fix_mhsd (FILE *file, glong mhsd_seek, glong cur)
{
  put_4int_seek (file, cur-mhsd_seek, mhsd_seek+8); /* size of whole mhit */
}


/* Write out the mhlt header. */
static void mk_mhlt (FILE *file, guint32 song_num)
{
  put_data_cur (file, "mhlt", 4);
  put_4int_cur (file, 92);         /* Headersize */
  put_4int_cur (file, song_num);   /* songs in this itunesdb */
  put_n0_cur (file, 20);           /* dummy space */
}  


/* Write out the mhit header. Size will be written later */
static void mk_mhit (FILE *file, Song *song)
{
  put_data_cur (file, "mhit", 4);
  put_4int_cur (file, 156);  /* header size */
  put_4int_cur (file, -1);   /* size of whole mhit -- fill in later */
  put_4int_cur (file, -1);   /* nr of mhods in this mhit -- later   */
  put_4int_cur (file, song->ipod_id); /* song index number          */
  put_4int_cur (file, 1);
  put_4int_cur (file, 0);
  put_4int_cur (file, 257 | song->rating<<24);  /* type, rating     */
  put_4int_cur (file, song->time_modified); /* timestamp             */
  put_4int_cur (file, song->size);    /* filesize                   */
  put_4int_cur (file, song->songlen); /* length of song in ms       */
  put_4int_cur (file, song->track_nr);/* track number               */
  put_4int_cur (file, song->tracks);  /* number of tracks           */
  put_4int_cur (file, song->year);    /* the year                   */
  put_4int_cur (file, song->bitrate); /* bitrate                    */
  put_4int_cur (file, 0xac440000);    /* ?                          */
  put_n0_cur (file, 4);               /* dummy space                */
  put_4int_cur (file, song->playcount);/* playcount                  */
  put_4int_cur (file, 0);             /* dummy space                */
  put_4int_cur (file, song->time_played); /* last time played       */
  put_4int_cur (file, song->cd_nr);   /* CD number                  */
  put_4int_cur (file, song->cds);     /* number of CDs              */
  put_4int_cur (file, 0);             /* hardcoded space            */
  put_4int_cur (file, itunesdb_time_get_mac_time ()); /* current timestamp */
  put_n0_cur (file, 12);              /* dummy space                */
}  


/* Fill in the missing items of the mhit header:
   total size and number of mhods */
static void fix_mhit (FILE *file, glong mhit_seek, glong cur, gint mhod_num)
{
  put_4int_seek (file, cur-mhit_seek, mhit_seek+8); /* size of whole mhit */
  put_4int_seek (file, mhod_num, mhit_seek+12);     /* nr of mhods        */
}


/* Write out one mhod header.
     type: see enum of MHMOD_IDs;
     string: utf16 string to pack
     fqid: will be used for playlists -- use 1 for songs */
static void mk_mhod (FILE *file, guint32 type,
		     gunichar2 *string, guint32 fqid)
{
  guint32 mod;
  guint32 len;

  if (fqid == 1) mod = 40;   /* normal mhod */
  else           mod = 44;   /* playlist entry */

  len = utf16_strlen (string);         /* length of string in _words_     */

  put_data_cur (file, "mhod", 4);      /* header                          */
  put_4int_cur (file, 24);             /* size of header                  */
  put_4int_cur (file, 2*len+mod);      /* size of header + body           */
  put_4int_cur (file, type);           /* type of the entry               */
  put_n0_cur (file, 2);                /* dummy space                     */
  put_4int_cur (file, fqid);           /* refers to this ID if a PL item,
					  otherwise always 1              */
  put_4int_cur (file, 2*len);          /* size of string                  */
  if (type < 100)
    {                                     /* no PL mhod */
      put_n0_cur (file, 2);               /* trash      */
      /* FIXME: this assumes "string" is writable. 
	 However, this might not be the case,
	 e.g. ipod_name might be in read-only mem. */
      string = fixup_utf16(string); 
      put_data_cur (file, (gchar *)string, 2*len); /* the string */
      string = fixup_utf16(string);
    }
  else
    {                                     
      put_n0_cur (file, 3);     /* PL mhods are different ... */
    }
}


/* Write out the mhlp header. Size will be written later */
static void mk_mhlp (FILE *file, guint32 lists)
{
  put_data_cur (file, "mhlp", 4);      /* header                   */
  put_4int_cur (file, 92);             /* size of header           */
  put_4int_cur (file, lists);          /* playlists on iPod (including main!) */
  put_n0_cur (file, 20);               /* dummy space              */
}


/* Fix the mhlp header */
static void fix_mhlp (FILE *file, glong mhlp_seek, gint playlist_num)
{
  put_4int_seek (file, playlist_num, mhlp_seek+8); /* nr of playlists    */
}



/* Write out the "weird" header.
   This seems to be an itunespref thing.. dunno know this
   but if we set everything to 0, itunes doesn't show any data
   even if you drag an mp3 to your ipod: nothing is shown, but itunes
   will copy the file! 
   .. so we create a hardcoded-pref.. this will change in future
   Seems to be a Preferences mhod, every PL has such a thing 
   FIXME !!! */
static void mk_weired (FILE *file)
{
  put_data_cur (file, "mhod", 4);      /* header                   */
  put_4int_cur (file, 0x18);           /* size of header  ?        */
  put_4int_cur (file, 0x0288);         /* size of header + body    */
  put_4int_cur (file, 0x64);           /* type of the entry        */
  put_n0_cur (file, 6);
  put_4int_cur (file, 0x010084);       /* ? */
  put_4int_cur (file, 0x05);           /* ? */
  put_4int_cur (file, 0x09);           /* ? */
  put_4int_cur (file, 0x03);           /* ? */
  put_4int_cur (file, 0x120001);       /* ? */
  put_n0_cur (file, 3);
  put_4int_cur (file, 0xc80002);       /* ? */
  put_n0_cur (file, 3);
  put_4int_cur (file, 0x3c000d);       /* ? */
  put_n0_cur (file, 3);
  put_4int_cur (file, 0x7d0004);       /* ? */
  put_n0_cur (file, 3);
  put_4int_cur (file, 0x7d0003);       /* ? */
  put_n0_cur (file, 3);
  put_4int_cur (file, 0x640008);       /* ? */
  put_n0_cur (file, 3);
  put_4int_cur (file, 0x640017);       /* ? */
  put_4int_cur (file, 0x01);           /* bool? (visible? / colums?) */
  put_n0_cur (file, 2);
  put_4int_cur (file, 0x500014);       /* ? */
  put_4int_cur (file, 0x01);           /* bool? (visible?) */
  put_n0_cur (file, 2);
  put_4int_cur (file, 0x7d0015);       /* ? */
  put_4int_cur (file, 0x01);           /* bool? (visible?) */
  put_n0_cur (file, 114);
}


/* Write out the mhyp header. Size will be written later */
static void mk_mhyp (FILE *file, gunichar2 *listname,
		     guint32 type, guint32 song_num)
{
  put_data_cur (file, "mhyp", 4);      /* header                   */
  put_4int_cur (file, 108);            /* length		   */
  put_4int_cur (file, -1);             /* size -> later            */
  put_4int_cur (file, 2);              /* ?                        */
  put_4int_cur (file, song_num);       /* number of songs in plist */
  put_4int_cur (file, type);           /* 1 = main, 0 = visible    */
  put_4int_cur (file, 0);              /* ?                        */
  put_4int_cur (file, 0);              /* ?                        */
  put_4int_cur (file, 0);              /* ?                        */
  put_n0_cur (file, 18);               /* dummy space              */
  mk_weired (file);
  mk_mhod (file, MHOD_ID_TITLE, listname, 1);
}


/* Fix the mhyp header */
static void fix_mhyp (FILE *file, glong mhyp_seek, glong cur)
{
  put_4int_seek (file, cur-mhyp_seek, mhyp_seek+8);
    /* size */
}


/* Header for new PL item */
static void mk_mhip (FILE *file, guint32 id)
{
  put_data_cur (file, "mhip", 4);
  put_4int_cur (file, 76);
  put_4int_cur (file, 76);
  put_4int_cur (file, 1);
  put_4int_cur (file, 0);
  put_4int_cur (file, id);  /* song id in playlist */
  put_4int_cur (file, id);  /* ditto.. don't know the difference, but this
                               seems to work. Maybe a special ID used for
			       playlists? */
  put_n0_cur (file, 12); 
}

static void
write_mhsd_one(FILE *file)
{
    Song *song;
    guint32 i, song_num, mhod_num;
    glong mhsd_seek, mhit_seek, mhlt_seek; 

    song_num = it_get_nr_of_songs();

    mhsd_seek = ftell (file);  /* get position of mhsd header */
    mk_mhsd (file, 1);         /* write header: type 1: song  */
    mhlt_seek = ftell (file);  /* get position of mhlt header */
    mk_mhlt (file, song_num);  /* write header with nr. of songs */
    for (i=0; i<song_num; ++i)  /* Write each song */
    {
	if((song = it_get_song_by_nr (i)) == 0)
	{
	    g_warning ("Invalid song Index!\n");
	    break;
	}
	mhit_seek = ftell (file);
	mk_mhit (file, song);
	mhod_num = 0;
	if (utf16_strlen (song->title_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_TITLE, song->title_utf16, 1);
	    ++mhod_num;
	}
	if (utf16_strlen (song->ipod_path_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_PATH, song->ipod_path_utf16, 1);
	    ++mhod_num;
	}
	if (utf16_strlen (song->album_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_ALBUM, song->album_utf16, 1);
	    ++mhod_num;
	}
	if (utf16_strlen (song->artist_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_ARTIST, song->artist_utf16, 1);
	    ++mhod_num;
	}
	if (utf16_strlen (song->genre_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_GENRE, song->genre_utf16, 1);
	    ++mhod_num;
	}
	if (utf16_strlen (song->fdesc_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_FDESC, song->fdesc_utf16, 1);
	    ++mhod_num;
	}
	if (utf16_strlen (song->comment_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_COMMENT, song->comment_utf16, 1);
	    ++mhod_num;
	}
	if (utf16_strlen (song->composer_utf16) != 0)
	{
	    mk_mhod (file, MHOD_ID_COMPOSER, song->composer_utf16, 1);
	    ++mhod_num;
	}
        /* Fill in the missing items of the mhit header */
	fix_mhit (file, mhit_seek, ftell (file), mhod_num);
    }
    fix_mhsd (file, mhsd_seek, ftell (file));
}

static void
write_playlist(FILE *file, Playlist *pl)
{
    Song *s;
    guint32 i, n;
    glong mhyp_seek;
    gunichar2 empty = 0;
    
    mhyp_seek = ftell(file);
    n = it_get_nr_of_songs_in_playlist (pl);
#if ITUNESDB_DEBUG
  fprintf(stderr, "Playlist: %s (%d tracks)\n", pl->name, n);
#endif    
    mk_mhyp(file, pl->name_utf16, pl->type, n);  
    for (i=0; i<n; ++i)
    {
        if((s = it_get_song_in_playlist_by_nr (pl, i)))
	{
	    mk_mhip(file, s->ipod_id);
	    mk_mhod(file, MHOD_ID_PLAYLIST, &empty, s->ipod_id); 
	}
    }
   fix_mhyp (file, mhyp_seek, ftell(file));
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
    fix_mhsd (file, mhsd_seek, ftell (file));
}


/* Do the actual writing to the iTunesDB */
gboolean 
write_it (FILE *file)
{
    glong mhbd_seek;

    mhbd_seek = 0;             
    mk_mhbd (file);            
    write_mhsd_one(file);		/* write songs mhsd */
    write_mhsd_two(file);		/* write playlists mhsd */
    fix_mhbd (file, mhbd_seek, ftell (file));
    return TRUE;
}


/* Write out an iTunesDB.
   Note: only the _utf16 entries in the Song-struct are used
   An existing "Play Counts" file is renamed to "Play Counts.bak" if
   the export was successful.
   Returns TRUE on success, FALSE on error.
   "path" should point to the mount point of the
   iPod, e.e. "/mnt/ipod" */
gboolean itunesdb_write (gchar *path)
{
    gchar *filename = NULL;
    gboolean result = FALSE;

    filename = itunesdb_concat_dir (path, "iPod_Control/iTunes/iTunesDB");
    result = itunesdb_write_to_file (filename);
    if (filename != NULL) g_free (filename);
    return result;
}

/* Same as itnuesdb_write (), but you specify the filename directly */
gboolean itunesdb_write_to_file (gchar *filename)
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
    }
  if (result == TRUE)
  {   /* rename "Play Counts" to "Play Counts.bak" */
      gchar *dirname = g_path_get_dirname (filename);
      gchar *plcname_o = itunesdb_concat_dir (dirname, "Play Counts");
      gchar *plcname_n = itunesdb_concat_dir (dirname, "Play Counts.bak");
      if (g_file_test (plcname_o, G_FILE_TEST_EXISTS))
      {
	  if (rename (plcname_o, plcname_n) == -1)
	  {   /* an error occured */
	      itunesdb_warning (_("Error renaming '%s' to '%s' (%s).\n"),
				plcname_o, plcname_n, g_strerror (errno));
	  }
      }
      g_free (dirname);
      g_free (plcname_o);
      g_free (plcname_n);
  }
  return result;
}

/* Copy one song to the ipod. The PC-Filename is
   "pcfile" and is taken literally.
   "path" is assumed to be the mountpoint of the iPod.
   For storage, the directories "f00 ... f19" will be
   cycled through. The filename is constructed from
   "song->ipod_id": "gtkpod_id" and written to
   "song->ipod_path_utf8" and "song->ipod_path_utf16" */
gboolean itunesdb_copy_song_to_ipod (gchar *path, Song *song, gchar *pcfile)
{
  static gint dir_num = -1;
  gchar *ipod_file = NULL, *ipod_fullfile = NULL;
  gboolean success;
  gint32 oops = 0;
  gint pathlen = 0;

  if (path) pathlen = strlen (path); /* length of path in bytes */

#if ITUNESDB_DEBUG
  fprintf(stderr, "Entered itunesdb_copy_song_to_ipod: '%s', %p, '%s'\n", path, song, pcfile);
#endif
  if (dir_num == -1) dir_num = (gint) (19.0*rand()/(RAND_MAX));
  if(song->transferred == TRUE) return TRUE; /* nothing to do */
  if (song == NULL)
    {
      g_warning ("Programming error: copy_song_to_ipod () called NULL-song\n");
      return FALSE;
    }

  /* If song->ipod_path exists, we use that one instead. */
  ipod_fullfile = itunesdb_get_song_name_on_ipod (path, song);
  if (!ipod_fullfile) do
  { /* we need to loop until we find a unused filename */
      if (ipod_file)     g_free(ipod_file);
      if (ipod_fullfile) g_free(ipod_fullfile);
      /* The iPod seems to need the .mp3 ending to play the song.
	 Of course the following line should be changed once gtkpod
	 also supports other formats. */
      ipod_file = g_strdup_printf ("/iPod_Control/Music/F%02d/gtkpod%05d.mp3",
				   dir_num, song->ipod_id + oops);
      ipod_fullfile = itunesdb_concat_dir (path, ipod_file+1);
      /* There is a case-sensitivity problem on some systems (see note
       * at itunesdb_get_song_name_on_ipod (). The following code
       tries to work around it */
      if (!g_file_test (ipod_fullfile, G_FILE_TEST_EXISTS))
      { /* does not exist -- let's try to create it */
	  FILE *file = fopen (ipod_fullfile, "w+");
	  if (file)
	  { /* OK -- everything's fine -- let's clean up */
	      fclose (file);
	      remove (ipod_fullfile);
	  }
	  else
	  { /* let's try to change the ".../Music/F..." to
	     * ".../Music/f..." and try again */
	      gchar *bufp = strstr (ipod_fullfile+pathlen,
				    "/Music/F");
	      if (bufp)	  bufp[7] = 'f';
	      /* we don't have to check if it works because if it
		 doesn't most likely @path is wrong in the first
		 place, or the iPod isn't mounted etc. We'll catch
		 that curing copy anyhow. */
	  }
      }
      if (oops == 0)   oops += 90000;
      else             ++oops;
  } while (g_file_test (ipod_fullfile, G_FILE_TEST_EXISTS));

#if ITUNESDB_DEBUG
  fprintf(stderr, "ipod_fullfile: '%s'\n", ipod_fullfile);
#endif

  success = itunesdb_cp (pcfile, ipod_fullfile);
  if (success)
  {
      gint i, len;
      song->transferred = TRUE;
      ++dir_num;
      if (dir_num == 20) dir_num = 0;
      if (ipod_file)
      { /* need to store ipod_filename */
	  len = strlen (ipod_file);
	  for (i=0; i<len; ++i)     /* replace '/' by ':' */
	      if (ipod_file[i] == '/')  ipod_file[i] = ':';
#ifdef ITUNESDB_PROVIDE_UTF8
	  if (song->ipod_path) g_free (song->ipod_path);
	  song->ipod_path = g_strdup (ipod_file);
#endif
	  if (song->ipod_path_utf16) g_free (song->ipod_path_utf16);
	  song->ipod_path_utf16 = g_utf8_to_utf16 (ipod_file,
						   -1, NULL, NULL, NULL);
      }
  }
  if (ipod_file )    g_free (ipod_file);
  if (ipod_fullfile) g_free (ipod_fullfile);
  return success;
}


/* Return the full iPod filename as stored in @s.
   @s: song
   @path: mount point of the iPod file system
   Return value: full filename to @s on the iPod or NULL if no
   filename is set in @s. NOTE: the file does not necessarily
   exist. NOTE: this code works around a problem on some systems (see
   below) and might return a filename with different case than the
   original filename. Don't copy it back to @s */
gchar *itunesdb_get_song_name_on_ipod (gchar *path, Song *s)
{
    gchar *result = NULL;

    if(s && s->ipod_path && *s->ipod_path)
    {
	guint i = 0, size = 0;
	gchar *buf = g_strdup (s->ipod_path);
	size = strlen(buf);
	for(i = 0; i < size; i++)
	    if(buf[i] == ':') buf[i] = '/';
	result = itunesdb_concat_dir(path, buf);
	/* There seems to be a problem with some distributions
	   (kernel versions or whatever -- even identical version
	   numbers don't don't show identical behaviour...): even
	   though vfat is supposed to be case insensitive, a
	   difference is made between upper and lower case under
	   some special circumstances. As in
	   "/iPod_Control/Music/F00" and "/iPod_Control/Music/f00
	   "... If the former filename does not exist, we try to
	   access the latter. If that exists we return it,
	   otherwise we return the first version. */
	if (!g_file_test (result, G_FILE_TEST_EXISTS))
	{
	    gchar *bufp = strstr (buf, "/Music/F");
	    if (bufp)
	    {
		gchar *result2;
		bufp[7] = 'f'; /* change the 'F' to 'f' */
		result2 = itunesdb_concat_dir(path, buf);
		if (g_file_test (result, G_FILE_TEST_EXISTS))
		{
		    g_free (result);
		    result = result2;
		}
		else g_free (result2);
	    }
	}
	g_free (buf);
    }
    return result;
}


/* Copy file "from_file" to "to_file".
   Returns TRUE on success, FALSE otherwise */
gboolean itunesdb_cp (gchar *from_file, gchar *to_file)
{
  gchar data[ITUNESDB_COPYBLK];
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
	      itunesdb_warning (_("Error reading file \"%s\"."), from_file);
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
	      itunesdb_warning (_("Error writing PC file \"%s\"."),to_file);
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
  return success;
}

/*------------------------------------------------------------------*\
 *                                                                  *
 *                       Timestamp stuff                            *
 *                                                                  *
\*------------------------------------------------------------------*/

guint32 itunesdb_time_get_mac_time (void)
{
    GTimeVal time;

    g_get_current_time (&time);
    return itunesdb_time_host_to_mac (time.tv_sec);
}


/* convert Macintosh timestamp to host system time stamp -- modify
 * this function if necessary to port to host systems with different
 * start of Epoch */
/* A "0" time will not be converted */
time_t itunesdb_time_mac_to_host (guint32 mactime)
{
    if (mactime != 0)  return ((time_t)mactime) - 2082844800;
    else               return (time_t)mactime;
}


/* convert host system timestamp to Macintosh time stamp -- modify
 * this function if necessary to port to host systems with different
 * start of Epoch */
guint32 itunesdb_time_host_to_mac (time_t time)
{
    return (guint32)(time + 2082844800);
}
