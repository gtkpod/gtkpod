/* -*- coding: utf-8; -*-
|  Time-stamp: <2005-04-29 12:15:40 jcs>
|
|  Copyright (C) 2002-2004 Jorg Schuler <jcsjcs at users.sourceforge.net>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "misc.h"
#include "prefs.h"
#include "misc_track.h"


#define DEBUG_MISC 0

/*------------------------------------------------------------------*\
 *                                                                  *
 *             About Window                                         *
 *                                                                  *
\*------------------------------------------------------------------*/

static GtkWidget *about_window = NULL;

void open_about_window ()
{
  GtkLabel *about_label;
  gchar *label_text;
  GtkTextView *textview;
  GtkTextIter ti;
  GtkTextBuffer *tb;
  gchar *xml_file;
  GladeXML *about_xml;

  if (about_window != NULL) return;
  /* about_window = create_gtkpod_about_window (); */

  xml_file = g_build_filename (PACKAGE_DATA_DIR, G_DIR_SEPARATOR_S, PACKAGE, "gtkpod.glade", NULL);
  about_xml = glade_xml_new (xml_file, "gtkpod_about_window", NULL);
  glade_xml_signal_autoconnect (about_xml);
  about_window = glade_xml_get_widget (about_xml, "gtkpod_about_window");
  
  
  about_label = GTK_LABEL (glade_xml_get_widget (about_xml, "about_label"));
  label_text = g_strdup_printf (_("gtkpod Version %s: Cross-Platform Multi-Lingual Interface to Apple's iPod(tm)."), VERSION);
  gtk_label_set_text (about_label, label_text);
  g_free (label_text);
  {
      gchar *text[] = {_("\
(C) 2002 - 2003\n\
Jorg Schuler (jcsjcs at users dot sourceforge dot net)\n\
Corey Donohoe (atmos at atmos dot org)\n\
\n\
\n"),
		       _("\
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.\n\
\n\
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.\n\
\n\
\n"),
		       _("\
Patches were supplied by the following people (list may be incomplete -- please contact me)\n\n"),
		       _("\
Ramesh Dharan: Multi-Edit (edit tags of several tracks in one run)\n"),
		       _("\
Hiroshi Kawashima: Japanese charset autodetecion feature\n"),
		       _("\
Adrian Ulrich: porting of playlist code from mktunes.pl to itunesdb.c\n"),
		       _("\
Walter Bell: correct handling of DND URIs with escaped characters and/or cr/newlines at the end\n"),
		       _("\
Sam Clegg: user defined filenames when exporting tracks from the iPod\n"),
		       _("\
Chris Cutler: automatic creation of various playlist types\n"),
		       _("\
Graeme Wilford: reading and writing of the 'Composer' ID3 tags, progress dialogue during sync\n"),
		       _("\
Edward Matteucci: debugging, special playlist creation, most of the volume normalizing code\n"),
		       _("\
Jens Lautenbach: some optical improvements\n"),
		       _("\
Alex Tribble: iPod eject patch\n"),
		       _("\
Yaroslav Halchenko: Orphaned and dangling tracks handling\n"),
		       _("\
Andrew Huntwork: Filename case sensitivity fix and various other bugfixes\n"),
		       _("\
Ero Carrera: Filename validation and quick sync when copying tracks from the iPod\n"),
		       _("\
Jens Taprogge: Support for LAME's replay gain tag to normalize volume\n"),
		       _("\
Armando Atienza: Support with external playcounts\n"),
		       _("\
D.L. Sharp: Support for m4b files (bookmarkable AAC files)\n"),
		       _("\
Jim Hall: Decent INSTALL file\n"),
		       _("\
Juergen Helmers, Markus Gaugusch: Conversion scripts to sync calendar/contacts to the iPod\n"),    /* J"urgen! */
		       _("\
Flavio Stanchina: bugfixes\n"),
		       _("\
Chris Micacchi: when sorting ignore 'the' and similar at the beginning of the title\n"),
		       _("\
Steve Jay: use statvfs() instead of df (better portability, faster)\n"),
		       "\n",
		       _("\
Icons of buttons were made by Nicolas Chariot.\n\
\n\
\n"),
		       _("\
This program borrows code from the following projects:\n"),
		       _("\
    gnutools: (mktunes.pl, ported to C) reading and writing of iTunesDB (http://www.gnu.org/software/gnupod/)\n"),
		       _("\
    iPod.cpp, iPod.h by Samuel Wood (sam dot wood at gmail dot com): some code for smart playlists is based on his C++-classes.\n"),
		       _("\
    mp3info:  mp3 playlength detection (http://ibiblio.org/mp3info/)\n"),
		       _("\
    xmms:     dirbrowser, mp3 playlength detection (http://www.xmms.org)\n"),
		       "\n",
		       _("\
The GUI was created with the help of glade-2 (http://glade.gnome.org/).\n"),
		       NULL };
      gchar **strp = text;
      textview = GTK_TEXT_VIEW (glade_xml_get_widget (about_xml, "credits_textview"));
      tb = gtk_text_view_get_buffer (textview);
      while (*strp)
      {
	  gtk_text_buffer_get_end_iter (tb, &ti);
	  gtk_text_buffer_insert (tb, &ti, *strp, -1);
	  ++strp;
      }
  }

 {
     gchar  *text[] = { _("\
French:   David Le Brun (david at dyn-ns dot net)\n"),
				     _("\
German:   Jorg Schuler (jcsjcs at users dot sourceforge dot net)\n"),
				     _("\
Italian:  Edward Matteucci (edward_matteucc at users dot sourceforge dot net)\n"),
				     _("\
Japanese: Ayako Sano\n"),
				     _("\
Japanese: Kentaro Fukuchi (fukuchi at users dot sourceforge dot net)\n"),
				     _("\
Swedish: Stefan Asserhall (stefan asserhall at comhem dot se)\n"),
				     NULL };
      gchar **strp = text;
      textview = GTK_TEXT_VIEW (glade_xml_get_widget (about_xml, "translators_textview"));
      tb = gtk_text_view_get_buffer (textview);
      while (*strp)
      {
	  gtk_text_buffer_get_end_iter (tb, &ti);
	  gtk_text_buffer_insert (tb, &ti, *strp, -1);
	  ++strp;
      }
  }

  gtk_widget_show (about_window);
}

void close_about_window (void)
{
  g_return_if_fail (about_window != NULL);
  gtk_widget_destroy (about_window);
  about_window = NULL;
}




/*------------------------------------------------------------------*\
 *                                                                  *
 *             Miscellaneous                                        *
 *                                                                  *
\*------------------------------------------------------------------*/


/* Calculate the time in ms passed since @old_time. @old_time is
   updated with the current time if @update is TRUE*/
float get_ms_since (GTimeVal *old_time, gboolean update)
{
    GTimeVal new_time;
    float result;

    g_get_current_time (&new_time);
    result = (new_time.tv_sec - old_time->tv_sec) * 1000 +
	(float)(new_time.tv_usec - old_time->tv_usec) / 1000;
    if (update)
    {
	old_time->tv_sec = new_time.tv_sec;
	old_time->tv_usec = new_time.tv_usec;
    }
    return result;
}

/* parse a bunch of track pointers delimited by \n
 * @s - address of the character string we're parsing (gets updated)
 * @track - pointer the track pointer parsed from the string
 * Returns FALSE when the string is empty, TRUE when the string can still be
 *	parsed
 */
gboolean
parse_tracks_from_string(gchar **s, Track **track)
{
    g_return_val_if_fail (track, FALSE);
    *track = NULL;
    g_return_val_if_fail (s, FALSE);

    if(*s)
    {
	gchar *str = *s;
	gchar *strp = strchr (str, '\n');
	int tokens;

	if (strp == NULL)
	{
	    *track = NULL;
	    *s = NULL;
	    return FALSE;
	}
	tokens = sscanf (str, "%p", track);
	++strp;
	if (*strp) *s = strp;
	else       *s = NULL;
	if (tokens == 1) 	return TRUE;
	else                    return FALSE;
    }
    return FALSE;
}



void cleanup_backup_and_extended_files (void)
{
  gchar *cfgdir;

  cfgdir = prefs_get_cfgdir ();
  /* in offline mode, there are no backup files! */
  if (cfgdir && !prefs_get_offline ())
    {
      gchar *cfe = g_build_filename (cfgdir, "iTunesDB.ext", NULL);
      if (!prefs_get_write_extended_info ())
	/* delete extended info file from computer */
	if (g_file_test (cfe, G_FILE_TEST_EXISTS))
	  if (remove (cfe) != 0)
	    gtkpod_warning (_("Could not delete backup file: \"%s\"\n"), cfe);
      g_free (cfe);
    }
  C_FREE (cfgdir);
}


/* used by glist_duplicate */
static void gl_dup_fe (gpointer data, GList **dup)
{
    *dup = g_list_append (*dup, data);
}


/* Duplicate a GList */
GList *glist_duplicate (GList *list)
{
    GList *dup = NULL;
    g_list_foreach (list, (GFunc)gl_dup_fe, &dup);
    return dup;
}




/* ------------------------------------------------------------
 *
 * Code to "eject" the iPod.
 *
 * Large parts of this code are Copyright (C) 1994-2001 Jeff Tranter
 * (tranter at pobox.com).  Repackaged to only include scsi eject code
 * by Alex Tribble (prat at rice.edu). Integrated into gtkpod by Jorg
 * Schuler (jcsjcs at users.sourceforge.net)
 *
 * ------------------------------------------------------------ */

/* This code does not work with FreeBSD -- let me know if you know a
 * way around it. */

#if (HAVE_LINUX_CDROM_H && HAVE_SCSI_SCSI_H && HAVE_SCSI_SCSI_IOCTL_H)
#include <linux/cdrom.h>
#include <scsi/scsi.h>
#include <scsi/scsi_ioctl.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

static gchar *getdevicename(const gchar *mount)
{
    if(mount) {
	gchar device[256], point[256];
	FILE *fp = fopen("/proc/mounts","r");
	if (!fp)
	    return NULL;
	do {
	    fscanf(fp, "%255s %255s %*s %*s %*s %*s", device, point);
	    if( strcmp(mount,point) == 0 ) {
		fclose(fp);
		return strdup(device);
	    }
	} while(!feof(fp));
	fclose(fp);
    }
    return NULL;
}

/*
 * Eject using CDROMEJECT ioctl. Return TRUE if successful, FALSE otherwise.
 */
static gboolean EjectCdrom(int fd)
{
	int status;

	status = ioctl(fd, CDROMEJECT);
	return (status == 0);
}

/*
 * Eject using SCSI commands. Return TRUE if successful, FALSE otherwise.
 */
static gboolean EjectScsi(int fd)
{
    int status;
    struct sdata {
	int inlen;
	int outlen;
	char cmd[256];
    } scsi_cmd;

    scsi_cmd.inlen = 0;
    scsi_cmd.outlen = 0;
    scsi_cmd.cmd[0] = ALLOW_MEDIUM_REMOVAL;
    scsi_cmd.cmd[1] = 0;
    scsi_cmd.cmd[2] = 0;
    scsi_cmd.cmd[3] = 0;
    scsi_cmd.cmd[4] = 0;
    scsi_cmd.cmd[5] = 0;
    status = ioctl(fd, SCSI_IOCTL_SEND_COMMAND, (void *)&scsi_cmd);
    if (status != 0)	return FALSE;

    scsi_cmd.inlen = 0;
    scsi_cmd.outlen = 0;
    scsi_cmd.cmd[0] = START_STOP;
    scsi_cmd.cmd[1] = 0;
    scsi_cmd.cmd[2] = 0;
    scsi_cmd.cmd[3] = 0;
    scsi_cmd.cmd[4] = 1;
    scsi_cmd.cmd[5] = 0;
    status = ioctl(fd, SCSI_IOCTL_SEND_COMMAND, (void *)&scsi_cmd);
    if (status != 0)	return FALSE;

    scsi_cmd.inlen = 0;
    scsi_cmd.outlen = 0;
    scsi_cmd.cmd[0] = START_STOP;
    scsi_cmd.cmd[1] = 0;
    scsi_cmd.cmd[2] = 0;
    scsi_cmd.cmd[3] = 0;
    scsi_cmd.cmd[4] = 2;
    scsi_cmd.cmd[5] = 0;
    status = ioctl(fd, SCSI_IOCTL_SEND_COMMAND, (void *)&scsi_cmd);
    if (status != 0)	return FALSE;

    status = ioctl(fd, BLKRRPART);
    return (status == 0);
}

static void eject_ipod(gchar *ipod_device)
{
     if(ipod_device)
     {
	  int fd = open (ipod_device, O_RDONLY|O_NONBLOCK);
	  if (fd == -1)
	  {
/*	      fprintf(stderr, "Unable to open '%s' to eject.\n",ipod_device); */
	      /* just ignore the error, i.e. if we don't have write access */
	  }
	  else
	  {
	      /* 2G seems to need EjectCdrom(), 3G EjectScsi()? */
	      if (!EjectCdrom (fd))
		  EjectScsi (fd);
	      close (fd);
	  }
     }
}
#else
static gchar *getdevicename(const gchar *mount)
{
    return NULL;
}
static void eject_ipod(gchar *ipod_device)
{
}
#endif

/* ------------------------------------------------------------
 *                    end of eject code
 * ------------------------------------------------------------ */


/***************************************************************************
 * Mount Calls
 *
 **************************************************************************/
/**
 * mount_ipod - attempt to mount the ipod to prefs_get_ipod_mount() This
 * does not check prefs to see if the current prefs want gtkpod itself to
 * mount the ipod drive, that should be checked before making this call.
 */
void
mount_ipod(void)
{
    const gchar *str = prefs_get_ipod_mount ();
    if(str)
    {
	pid_t pid, tpid;
	int status;

	pid = fork ();
	switch (pid)
	{
	    case 0: /* child */
		execl(MOUNT_BIN, "mount", str, NULL);
		exit(0);
		break;
	    case -1: /* parent and error */
		break;
	    default: /* parent -- let's wait for the child to terminate */
		tpid = waitpid (pid, &status, 0);
		/* we could evaluate tpid and status now */
		break;
	}
    }
}

/**
 * umount_ipod - attempt to unmount the ipod from prefs_get_ipod_mount()
 * This does not check prefs to see if the current prefs want gtkpod itself
 * to unmount the ipod drive, that should be checked before making this
 * call.
 * After unmounting, an attempt to send the "eject" signal is
 * made. Errors are ignored.
 */
void
unmount_ipod(void)
{
    const gchar *str = prefs_get_ipod_mount ();
    gchar *ipod_device = getdevicename (prefs_get_ipod_mount());
    if(str)
    {
	pid_t pid, tpid;
	int status;

	pid = fork ();
	switch (pid)
	{
	    case 0: /* child */
		execl(UMOUNT_BIN, "umount", str, NULL);
		exit(0);
		break;
	    case -1: /* parent and error */
		break;
	    default: /* parent -- let's wait for the child to terminate */
		tpid = waitpid (pid, &status, 0);
		eject_ipod (ipod_device);
		/* we could evaluate tpid and status now */
		break;
	}
    }
    g_free (ipod_device);
}


/***************************************************************************
 * gtkpod.in,out calls
 *
 **************************************************************************/

/* tries to call "/bin/sh @script" */
static void do_script (gchar *script)
{
    if (script)
    {
	pid_t pid, tpid;
	int status;

	pid = fork ();
	switch (pid)
	{
	case 0: /* child */
	    execl("/bin/sh", "sh", script, NULL);
	    exit(0);
	break;
	case -1: /* parent and error */
	break;
	default: /* parent -- let's wait for the child to terminate */
	    tpid = waitpid (pid, &status, 0);
	    /* we could evaluate tpid and status now */
	    break;
	}
    }
}


/* tries to execute "/bin/sh ~/.gtkpod/@script" or
 * "/bin/sh /etc/gtkpod/@script" if the former does not exist */
void call_script (gchar *script)
{
    gchar *cfgdir;
    gchar *file;

    if (!script) return;

    cfgdir =  prefs_get_cfgdir ();
    file = g_build_filename (cfgdir, script, NULL);
    if (g_file_test (file, G_FILE_TEST_EXISTS))
    {
	do_script (file);
    }
    else
    {
	C_FREE (file);
	file = g_build_filename ("/etc/gtkpod/", script, NULL);
	if (g_file_test (file, G_FILE_TEST_EXISTS))
	{
	    do_script (file);
	}
    }
    C_FREE (file);
    C_FREE (cfgdir);
}



/* compare @str1 and @str2 case-sensitively or case-insensitively
 * depending on prefs settings */
gint compare_string (gchar *str1, gchar *str2)
{
    if (prefs_get_case_sensitive ())
	return strcmp (str1, str2);
    else
	return compare_string_case_insensitive (str1, str2);
}

#define NUM_ARTICLES 	7
static char* articles[NUM_ARTICLES] = {
	"the ",
	"a ",
	"le ", 
	"la ", 
	"les ", 
	"lo ", 
	"los ", 
};

/* compare @str1 and @str2 case-sensitively or case-insensitively
 * depending on prefs settings, and ignoring certain initial articles 
 * ("the", "le"/"la", etc) */
gint compare_string_fuzzy (gchar *str1, gchar *str2)
{
    static gint   articleKeysGenerated = FALSE;
    static gint   articleLengths[NUM_ARTICLES];
    static gchar *articleKeys[NUM_ARTICLES];
    gchar *tempStr;
    gint   result;
    gint   i;
    
    gchar *cleanStr1 = g_utf8_casefold (str1, -1);
    gchar *cleanStr2 = g_utf8_casefold (str2, -1);

    gchar *pstr1 = str1;
    gchar *pstr2 = str2;
    gchar *pcleanStr1 = cleanStr1;
    gchar *pcleanStr2 = cleanStr2;
    
    /* If the article collations keys have not been generated, 
     * do that first
     */
    if ( ! articleKeysGenerated ) {
	for ( i = 0; i < NUM_ARTICLES; i += 1 ) {
	    tempStr = g_utf8_casefold( articles[i], -1 );
	    articleLengths[i] = g_utf8_strlen( tempStr, -1 );
	    articleKeys[i] = g_utf8_collate_key( tempStr, -1 );
	    g_free( tempStr );
	}
	articleKeysGenerated = TRUE;
    } 
    
    /* Check the beginnings of both strings for any of the 
     * articles we should ignore
     */
    for ( i = 0; i < NUM_ARTICLES; i += 1 ) {
    	tempStr = g_utf8_collate_key( cleanStr1, articleLengths[i] );
	if ( strcmp( tempStr, articleKeys[i] ) == 0 ) {
	    /* Found article, bump pointers ahead appropriate distance
	     */
	    pstr1 += articleLengths[i];
	    pcleanStr1 = g_utf8_offset_to_pointer( cleanStr1, articleLengths[i] );
	    g_free( tempStr );
	    break;
	}
	g_free( tempStr );
    }
    for ( i = 0; i < NUM_ARTICLES; i += 1 ) {
    	tempStr = g_utf8_collate_key( cleanStr2, articleLengths[i] );
	if ( strcmp( tempStr, articleKeys[i] ) == 0 ) {
	    /* Found article, bump pointers ahead apropriate distance
	     */
	    pstr2 += articleLengths[i];
	    pcleanStr2 = g_utf8_offset_to_pointer( cleanStr2, articleLengths[i] );
	    g_free( tempStr );
	    break;
	}
	g_free( tempStr );
    }

    if (prefs_get_case_sensitive ())
	result = strcmp(pstr1, pstr2);
    else
    	result = g_utf8_collate(pcleanStr1, pcleanStr2);

    g_free (cleanStr1);
    g_free (cleanStr2);
    return result;
}

/* compare @str1 and @str2 case-sensitively or case-insensitively
 * depending on prefs settings */
gint compare_string_case_insensitive (gchar *str1, gchar *str2)
{
    gchar *string1 = g_utf8_casefold (str1, -1);
    gchar *string2 = g_utf8_casefold (str2, -1);
    gint result = g_utf8_collate (string1, string2);
    g_free (string1);
    g_free (string2);
    return result;
}


/* ------------------------------------------------------------
------------------------------------------------------------------
--------                                                 ---------
--------                 UTF16 section                   ---------
--------                                                 ---------
------------------------------------------------------------------
   ------------------------------------------------------------ */

/* Get length of utf16 string in number of characters (words) */
guint32 utf16_strlen (gunichar2 *utf16)
{
  guint32 i=0;
  if (utf16)
      while (utf16[i] != 0) ++i;
  return i;
}

/* duplicate a utf16 string */
gunichar2 *utf16_strdup (gunichar2 *utf16)
{
    guint32 len;
    gunichar2 *new = NULL;

    if (utf16)
    {
	len = utf16_strlen (utf16);
	new = g_malloc (sizeof (gunichar2) * (len+1));
	if (new) memcpy (new, utf16, sizeof (gunichar2) * (len+1));
    }
    return new;
}




/*------------------------------------------------------------------*\
 *                                                                  *
 *  Generic functions to handle options in pop-up requesters        *
 *                                                                  *
\*------------------------------------------------------------------*/




/* Set the toggle button to active that is specified by @prefs_string
   (integer value). If no parameter is set in the prefs, use
   @dflt. The corresponding widget names are stored in an array
   @widgets and are member of @win */
void option_set_radio_button (GladeXML *win_xml,
			      const gchar *prefs_string,
			      const gchar **widgets,
			      gint dflt)
{
    gint wnum, num=0;
    GtkWidget *w;

    g_return_if_fail (win_xml && prefs_string && widgets);

    /* number of available widgets */
    num=0;
    while (widgets[num]) ++num;

    if (!prefs_get_int_value (prefs_string, &wnum))
	wnum = dflt;

    if ((wnum >= num) || (wnum < 0))
    {
	fprintf (stderr, "Programming error: wnum > num (%d,%d,%s)\n",
		 wnum, num, prefs_string);
	/* set to reasonable default value */
	prefs_set_int_value (prefs_string, 0);
	wnum = 0;
    }
    w = glade_xml_get_widget (win_xml, widgets[wnum]);
    if (w)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), TRUE);
}


/* Retrieve which toggle button was activated and store the state in
 * the prefs */
gint option_get_radio_button (GladeXML *win_xml,
			      const gchar *prefs_string,
			      const gchar **widgets)
{
    gint i;

    g_return_val_if_fail (win_xml && prefs_string && widgets, 0);

    for (i=0; widgets[i]; ++i)
    {
	GtkWidget *w = glade_xml_get_widget (win_xml, widgets[i]);
	if (w)
	{
	    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)))
		break;
	}
    }
    if (!widgets[i])
    {
	fprintf (stderr, "Programming error: no active toggle button (%s)", prefs_string);
	/* set reasonable default */
	i=0;
    }
    prefs_set_int_value (prefs_string, i);
    return i;
}


/* Set the current folder to what is stored in the prefs */
void option_set_folder (GtkFileChooser *fc, const gchar *prefs_string)
{
    gchar *folder;

    g_return_if_fail (fc && prefs_string);

    prefs_get_string_value (prefs_string, &folder);
    if (!folder)
	folder = g_strdup (g_get_home_dir ());
    gtk_file_chooser_set_current_folder (fc, folder);
    g_free (folder);
}


/* Retrieve the current folder and write it to the prefs */
/* If @value is != NULL, a copy of the folder is placed into
   @value. It has to be g_free()d after use */
void option_get_folder (GtkFileChooser *fc,
			const gchar *prefs_string,
			gchar **value)
{
    gchar *folder;

    g_return_if_fail (fc && prefs_string);

    folder = gtk_file_chooser_get_current_folder (fc);
    prefs_set_string_value (prefs_string, folder);

    if (value) *value = folder;
    else       g_free (folder);
}


/* Set the current filename to what is stored in the prefs */
void option_set_filename (GtkFileChooser *fc, const gchar *prefs_string)
{
    gchar *filename;

    g_return_if_fail (fc && prefs_string);

    prefs_get_string_value (prefs_string, &filename);
    if (!filename)
	filename = g_strdup (g_get_home_dir ());
    gtk_file_chooser_set_current_name (fc, filename);
    g_free (filename);
}


/* Retrieve the current filename and write it to the prefs */
/* If @value is != NULL, a copy of the filename is placed into
   @value. It has to be g_free()d after use */
void option_get_filename (GtkFileChooser *fc,
			  const gchar *prefs_string,
			  gchar **value)
{
    gchar *filename;

    g_return_if_fail (fc && prefs_string);

    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(fc));
    prefs_set_string_value (prefs_string, filename);

    if (value) *value = filename;
    else       g_free (filename);
}


/* Set the string entry @name to the prefs value stored in @name or
   to @default if @name is not yet defined. */
void option_set_string (GladeXML *win_xml,
			const gchar *name,
			const gchar *dflt)
{
    gchar *string;
    GtkWidget *entry;

    g_return_if_fail (win_xml && name && dflt);

    prefs_get_string_value (name, &string);

    if (!string)
	string = g_strdup (dflt);

    entry = glade_xml_get_widget (win_xml, name);

    if (entry)
	gtk_entry_set_text(GTK_ENTRY(entry), string);

    g_free (string);
}

/* Retrieve the current content of the string entry @name and write it
 * to the prefs (@name) */
/* If @value is != NULL, a copy of the string is placed into
   @value. It has to be g_free()d after use */
void option_get_string (GladeXML *win_xml,
			const gchar *name,
			gchar **value)
{
    GtkWidget *entry;

    g_return_if_fail (win_xml && name);

    entry = glade_xml_get_widget (win_xml, name);

    if (entry)
    {
	const gchar *str = gtk_entry_get_text (GTK_ENTRY (entry));
	prefs_set_string_value (name, str);
	if (value) *value = g_strdup (str);
    }
}


/* Set the state of toggle button @name to the prefs value stored in
   @name or to @default if @name is not yet defined. */
void option_set_toggle_button (GladeXML *win_xml,
			       const gchar *name,
			       gboolean dflt)
{
    gboolean active;
    GtkWidget *button;

    g_return_if_fail (win_xml && name);

    if (!prefs_get_int_value (name, &active))
	active = dflt;

    button = glade_xml_get_widget (win_xml, name);

    if (button)
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(button),
				      active);
}

/* Retrieve the current state of the toggle button @name and write it
 * to the prefs (@name) */
/* Return value: the current state */
gboolean option_get_toggle_button (GladeXML *win_xml,
				   const gchar *name)
{
    gboolean active = FALSE;
    GtkWidget *button;

    g_return_val_if_fail (win_xml && name, active);

    button = glade_xml_get_widget (win_xml, name);

    if (button)
    {
	active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(button));
	prefs_set_int_value (name, active);
    }
    return active;
}



/*------------------------------------------------------------------*\
 *                                                                  *
 *  Functions to create string/filename from a template             *
 *                                                                  *
\*------------------------------------------------------------------*/




/*
|  Copyright (C) 2004 Ero Carrera <ero at dkbza.org>
|
|  Placed under GPL in agreement with Ero Carrera. (JCS -- 12 March 2004)
*/

/**
 * Check if supported char and return substitute.
 */
static gchar check_char(gchar c)
{
    gint i;
    static const gchar 
	invalid[]={'"', '*', ':', '<', '>', '?', '\\', '|', '/', 0};
    static const gchar
	replace[]={'_', '_', '-', '_', '_', '_', '-',  '-', '-', 0};
    for(i=0; invalid[i]!=0; i++)
	if(c==invalid[i])  return replace[i];
    return c;
}

/**
 * Process a path. It will substitute all the invalid characters.
 * The changes are made within the original string. A pointer to the
 * original string is returned.
 */
static gchar *fix_path(gchar *orig)
{
        if(orig)
	{
	    gchar *op = orig;
	    while (*op)
	    {
		*op = check_char(*op);
		++op;
	    }
	}
	return orig;
}

/* End of code originally supplied by Ero Carrera */


/* Match a list of templates @p separated by ';' with the type of the
   filename. E.g. '%s.mp3;%t.wav' will return '%s.mp3' if @track is an
   mp3 file, or '%t.wav' if @track is a wav file. If no template can
   be matched, an empty string is returned. */
static gchar *select_template (Track *track, const gchar *p)
{
    gchar **templates, **tplp;
    gchar *tname, *ext = NULL;
    gchar *result;
    ExtraTrackData *etr;

    g_return_val_if_fail (track, strdup (""));
    etr = track->userdata;
    g_return_val_if_fail (etr, strdup (""));
    tname = get_track_name_on_disk (track);
    if (!tname) return (NULL);         /* this should not happen... */
    ext = strrchr (tname, '.');        /* pointer to filename extension */

    templates = g_strsplit (p, ";", 0);
    tplp = templates;
    while (*tplp)
    {
	if (strcmp (*tplp, "%o") == 0)
	{   /* this is only a valid extension if the original filename
	       is present */
	    if (etr->pc_path_locale && strlen(etr->pc_path_locale))  break;
	}
	else if (strrchr (*tplp, '.') == NULL)
	{   /* this templlate does not have an extension and therefore
	     * matches */
	    if (ext)
	    {   /* if we have an extension, add it */
		gchar *str = g_strdup_printf ("%s%s", *tplp, ext);
		g_free (*tplp);
		*tplp = str;
	    }
	    break;
	}
	else if (ext && (strlen (*tplp) >= strlen (ext)))
	{  /* this template is valid if the extensions match */
	    if (strcmp (&((*tplp)[strlen (*tplp) - strlen (ext)]), ext) == 0)
		break;
	}
	++tplp;
    }
    result = g_strdup (*tplp);
    g_strfreev (templates);
    return result;
}


/* Return a string for @track built according to @full_template.
   @full_template can contain several templates separated by ';',
   e.g. '%s.mp3;%t.wav'. The correct one is selected using
   select_template() defined above.
   If @is_filename is TRUE, potentially harmful characters are
   replaced in an attempt to create a valid filename.
   If @is_filename is FALSE, the extension (e.g. '.mp3' will be
   removed). */
gchar *get_string_from_template (Track *track,
				 const gchar *full_template,
				 gboolean is_filename)
{
    GString *result;
    gchar *p, *res_utf8;
    gchar *basename = NULL;
    gchar *template;
    ExtraTrackData *etr;

    g_return_val_if_fail (track && full_template, NULL);
    etr = track->userdata;
    g_return_val_if_fail (etr, NULL);

    template = select_template (track, full_template);

    if (!template)
    {
	gchar *fn = get_track_name_on_disk (track);
	gtkpod_warning (_("Template ('%s') does not match file type '%s'\n"), full_template, fn ? fn:"");
	g_free (fn);
	return NULL;
    }

    if (!is_filename)
    {   /* remove an extension, if present ('.???' or '.????'  at the
	   end) */
	gchar *pnt = strrchr (template, '.');
	if (pnt)
	{
	    if (pnt == template+strlen(template)-3)
		*pnt = 0;
	    if (pnt == template+strlen(template)-4)
		*pnt = 0;
	}
    }

    result = g_string_new ("");

    /* try to get the original filename */
    if (etr->pc_path_utf8)
	basename = g_path_get_basename (etr->pc_path_utf8);

    p=template;
    while (*p != '\0') {
	if (*p == '%') {
	    const gchar* tmp = NULL;
	    gchar dummy[100];
	    Playlist *pl;
	    p++;
	    switch (*p) {
	    case 'o':
		if (basename)
		{
		    tmp = basename;
		}
		break;
	    case 'p':
		pl = pm_get_selected_playlist ();
		if (pl)
		    tmp = pl->name;
		break;
	    case 'a':
		tmp = track_get_item (track, T_ARTIST);
		break;
	    case 'A':
		tmp = track_get_item (track, T_ALBUM);
		break;
	    case 't':
		tmp = track_get_item (track, T_TITLE);
		break;
	    case 'c':
		tmp = track_get_item (track, T_COMPOSER);
		break;
	    case 'g':
	    case 'G':
		tmp = track_get_item (track, T_GENRE);
		break;
	    case 'C':
		if (track->cds == 0)
		    sprintf (dummy, "%.2d", track->cd_nr);
		else if (track->cds < 10)
		    sprintf(dummy, "%.1d", track->cd_nr);
		else if (track->cds < 100)
		    sprintf (dummy, "%.2d", track->cd_nr);
		else if (track->cds < 1000)
		    sprintf (dummy, "%.3d", track->cd_nr);
		else
		    sprintf (dummy,"%.4d", track->cd_nr);
		tmp = dummy;
		break;
	    case 'T':
		if (track->tracks == 0)
		    sprintf (dummy, "%.2d", track->track_nr);
		else if (track->tracks < 10)
		    sprintf(dummy, "%.1d", track->track_nr);
		else if (track->tracks < 100)
		    sprintf (dummy, "%.2d", track->track_nr);
		else if (track->tracks < 1000)
		    sprintf (dummy, "%.3d", track->track_nr);
		else
		    sprintf (dummy,"%.4d", track->track_nr);
		tmp = dummy;
		break;
	    case 'Y':
		sprintf (dummy, "%4d", track->year);
		tmp = dummy;
		break;
	    case '%':
		tmp = "%";
		break;
	    default:
		gtkpod_warning (_("Unknown token '%%%c' in template '%s'"),
				*p, template);
		break;
	    }
	    if (tmp)
	    {
		gchar *tmpcp = g_strdup (tmp);
		if (is_filename)
		{
		    /* remove potentially illegal/harmful characters */
		    fix_path (tmpcp);
		    /* strip spaces to avoid problems with vfat */
		    g_strstrip (tmpcp);
		    /* append to current string */
		}
		result = g_string_append (result, tmpcp);
		tmp = NULL;
		g_free (tmpcp);
	    }
	}
	else 
	    result = g_string_append_c (result, *p);
	p++;
    }
    /* get the utf8 version of the filename */
    res_utf8 = g_string_free (result, FALSE);

    if (is_filename)
    {   /* remove white space before the filename extension
	   (last '.') */
	gchar *ext = strrchr (res_utf8, '.');
	gchar *extst = NULL;
	if (ext)
	{
	    extst = g_strdup (ext);
	    *ext = '\0';
	}
	g_strstrip (res_utf8);
	if (extst)
	{
	    /* The following strcat() is safe because g_strstrip()
	       does not increase the original string size. Therefore
	       the result of the strcat() call will not be longer than
	       the original string. */
	    strcat (res_utf8, extst);
	    g_free (extst);
	}
    }

    return res_utf8;
}


