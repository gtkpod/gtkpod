/* Time-stamp: <2003-11-29 13:00:17 jcs>
|
|  Copyright (C) 2002-2003 Jorg Schuler <jcsjcs at users.sourceforge.net>
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


#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "info.h"
#include "misc.h"
#include "normalize.h"
#include "prefs.h"
#include "support.h"
#include <stdlib.h>

#ifdef G_THREADS_ENABLED
static  GMutex *mutex = NULL;
static GCond  *cond = NULL;
static gboolean mutex_data = FALSE;
#endif


/* I'm not sure how exactly to calculate between iPod's volume tag and
 * mp3gain's gain. The following worked fine with 2 (two) tracks...
 * Change here if you know better. */
gint nm_gain_to_volume (gint gain)
{
    gint vol;

    vol = 10*gain;

    /* volume level is -100...+100 */
    if (vol > 100)  vol = 100;
    if (vol < -100) vol = -100;

    return vol;
}

gint nm_volume_to_gain (gint volume)
{
    return volume/10;
}


/* parse the mp3gain stdout to search the mp3gain output:
 * 
 * mp3gain stdout for a single file is something like this:
 * FIRST LINE (header)
 * FILENAME\tMP3GAIN\tOTHER NOT INTERESTING OUTPUT\n
 * ALBUM\tALBUMGAIN\tOTHER OUTPUT\n
 * 
 * we want to extract only the right file's MP3GAIN
 *
 * BEWARE: mp3gain doesn't separate stdout/stderror */
static gint parse_mp3gain_stdout(gchar *mp3gain_stdout, gchar *tracksfile)
{
   gint found=FALSE;
   gint gain=TRACKGAINERROR;
   /*they are just pointers, don't need to be freed*/
   gchar *filename=NULL;
   gchar *num=NULL;
  
   filename=strtok((gchar *)mp3gain_stdout,"\t\n\0");
   while(!found&&filename!=NULL&&((gchar *)mp3gain_stdout)!=NULL)
   {
#ifdef MP3GAIN_PARSE_DEBUG
      printf("mp3gain_stdout %s",mp3gain_stdout);
      printf("filename %s",filename);
#endif
      if(strcmp((gchar *)tracksfile,filename)==0)
      {
         num=strtok(NULL,"\t");
         strcat(num,"\0");
         gain=atoi(num);
         found=TRUE;
      }
      else{
         strtok(NULL,"\n\t\0");
         filename=strtok(NULL,"\t\n\0");
      }
#ifdef MP3GAIN_PARSE_DEBUG
      printf("mp3gain_stdout %s",mp3gain_stdout);
      printf("filename %s",filename);
#endif
   }
   return gain;
}

/* this function return the @track gain in dB */
/* mp3gain version 1.4.2 */
gint nm_get_gain(Track *track)
{
   /*pipe's definition*/
    enum {
	READ = 0,
	WRITE = 1
    };
    enum {
	BUFLEN = 1000,
    };
    gint k,n;  /*for's counter*/
    gint len = 0;
    gint gain = TRACKGAINERROR;
    gint fdpipe[2];  /*a pipe*/
    gchar *filename=NULL; /*track's filename*/
    gchar *mp3gain_path;
    gchar *mp3gain_exec;
    const gchar *mp3gain_set;
    gchar *buf;
    GString* gain_output;
    pid_t pid,tpid;

    k=0;
    n=0;

    /* see if full path to mp3gain was set using the prefs dialogue */
    mp3gain_set = prefs_get_mp3gain_path ();
    /* use default if not */
    if (!mp3gain_set || !(*mp3gain_set)) mp3gain_set = "mp3gain";
    /* find full path */
    mp3gain_path = which (mp3gain_set);
    /* show error message if mp3gain cannot be found */
    if (!mp3gain_path)
    {
	gtkpod_warning (_("Could not find mp3gain. I tried to use the following executable: '%s'.\n\nIf the mp3gain executable is not in your path or named differently, you can set the full path in the 'Tools' section of the preferences dialog.\n\nIf you do not have mp3gain installed, you can download it from http://www.sourceforge.net/projects/mp3gain."), mp3gain_set);
	return gain;
    }

    mp3gain_exec = g_path_get_basename (mp3gain_path);

    buf = g_malloc (BUFLEN);
    gain_output = g_string_sized_new (BUFLEN);

    if (!mp3gain_path)
    {
	gtkpod_warning (_("Could not find mp3gain executable."));
	return TRACKGAINERROR;
    }

    filename=get_track_name_on_disk_verified (track);
   
    /*create the pipe*/
    pipe(fdpipe);
    /*than fork*/
    pid=fork();
	
    /*and cast mp3gain*/
    switch (pid)
    {
    case -1: /* parent and error, now what?*/
	break;
    case 0: /*child*/
	close(1); /*close stdout*/
	dup2(fdpipe[WRITE],fileno(stdout));
	close(fdpipe[READ]);
	close(fdpipe[WRITE]);
	if(prefs_get_write_gaintag())
	{
	    /* this call may add a tag to the mp3file!! */
	    execl(mp3gain_path, mp3gain_exec, "-o", filename, NULL);
	}
	else
	{
	    execl(mp3gain_path, mp3gain_exec, "-s", "s", "-o", filename, NULL);
	}
	break; 
    default: /*parent*/
	close(fdpipe[WRITE]);
	tpid = waitpid (pid, NULL, 0); /*wait mp3gain termination */
	do
	{
	    len = read (fdpipe[READ], buf, BUFLEN);
	    if (len > 0) g_string_append_len (gain_output, buf, len);
	} while (len > 0);
	close(fdpipe[READ]);
	/* now output->str contains the mp3gain stdout */
	gain = parse_mp3gain_stdout(gain_output->str, filename);
	break;
    }/*end switch*/
    
    /*free everything left*/
    g_free (filename);
    g_free (mp3gain_path);
    g_free (mp3gain_exec);
    g_free (buf);
    g_string_free (gain_output, TRUE);

    /*and happily return the right value*/
    return gain;
}

#ifdef G_THREADS_ENABLED
/* Threaded getTrackGain*/
static gpointer th_nm_get_gain (gpointer track)
{
   gint gain=nm_get_gain((Track *)track);
   g_mutex_lock (mutex);
   mutex_data = TRUE; /* signal that thread will end */
   g_cond_signal (cond);
   g_mutex_unlock (mutex);
   return (gpointer)gain;
}
#endif 

/* normalize the newly inserted tracks (i.e. non-transferred tracks) */
void nm_new_tracks (void)
{
    GList *tracks=NULL;
    gint i=0;
    Track *track;

    while ((track=get_next_track(i)))
    {
	i=1; /* for get_next_track() */
	if(!track->transferred)
	{
	    tracks = g_list_append(tracks, track);
	}
    }
    nm_tracks_list(tracks);
    g_list_free (tracks);
}

static void normalization_abort(gboolean *abort)
{
   *abort=TRUE;
}

void nm_tracks_list(GList *list)
{
  gint count, n, nrs;
  gchar *buf;
  Track  *track;
  gint new_gain=0;
  static gboolean abort;
  GtkWidget *dialog, *progress_bar, *label, *image, *hbox;
  time_t diff, start, mins, secs;
  char *progtext = NULL;

#ifdef G_THREADS_ENABLED
  GThread *thread = NULL;
  GTimeVal gtime;
  if (!mutex) mutex = g_mutex_new ();
  if (!cond) cond = g_cond_new ();
#endif

  block_widgets ();

  /* create the dialog window */
  dialog = gtk_dialog_new_with_buttons (_("Information"),
                                         GTK_WINDOW (gtkpod_window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_NONE,
                                         NULL);

  
  /* emulate gtk_message_dialog_new */
  image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO,
				    GTK_ICON_SIZE_DIALOG);
  label = gtk_label_new (
      _("Press button to abort."));

  gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_label_set_selectable (GTK_LABEL (label), TRUE);

  /* hbox to put the image+label in */
  hbox = gtk_hbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

  /* Create the progress bar */
  progress_bar = gtk_progress_bar_new ();
  progtext = g_strdup (_("Normalizing..."));
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR (progress_bar), progtext);
  g_free (progtext);

  /* Indicate that user wants to abort */
  g_signal_connect_swapped (GTK_OBJECT (dialog), "response",
			    G_CALLBACK (normalization_abort),
			    &abort);

  /* Add the image/label + progress bar to dialog */
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
                      hbox, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
                      progress_bar, FALSE, FALSE, 0);
  gtk_widget_show_all (dialog);

  while (widgets_blocked && gtk_events_pending ())  gtk_main_iteration ();

  /* count number of tracks to be normalized */
  n = g_list_length(list);
  count = 0; /* tracks normalized */
  nrs = 0;
  abort = FALSE;
  start = time(NULL);

  if(n==0)
  {
     /* FIXME we should tell something*/

  }
  while (!abort &&  (list!=NULL)) /*FIXME:change it in a do-while cycle*/
  {
     track=list->data;
     if (track->transferred)
     {
#ifdef G_THREADS_ENABLED
        mutex_data = FALSE;
	thread = g_thread_create (th_nm_get_gain, track, TRUE, NULL);
	if (thread)
	{
           gboolean first_abort = TRUE;
           g_mutex_lock (mutex);
	   do
           {
              while (widgets_blocked && gtk_events_pending ())
                 gtk_main_iteration ();
              /* wait a maximum of 10 ms */

	      if (abort && first_abort)
	      {
		  first_abort = FALSE;
		  progtext = g_strdup (_("Aborting..."));
		  gtk_progress_bar_set_text(GTK_PROGRESS_BAR (progress_bar),
					    progtext);
		  g_free (progtext);
		  gtkpod_statusbar_message(_("Will abort after current mp3gain process ends."));
		  while (widgets_blocked && gtk_events_pending ())
		      gtk_main_iteration ();
	      }
              g_get_current_time (&gtime);
              g_time_val_add (&gtime, 20000);
	      g_cond_timed_wait (cond, mutex, &gtime);
           }
           while(!mutex_data);
           new_gain = (gint)g_thread_join (thread);
           g_mutex_unlock (mutex);
        }
        else
        {
           g_warning ("Thread creation failed, falling back to default.\n");
           new_gain=nm_get_gain(track);
        }
#else
        new_gain=nm_get_gain(track);
#endif 

/*normalization part*/
        if(new_gain == TRACKGAINERROR)
        {
           abort=TRUE;
        }
        else
        {
           if(nm_gain_to_volume (new_gain) != track->volume)
           {
              track->volume = nm_gain_to_volume (new_gain);
	      pm_track_changed (track);
	      data_changed ();
           }
        }
/*end normalization*/
        
        ++count;
        if (count == 1)  /* we need ***much*** longer timeout */
           prefs_set_statusbar_timeout (30*STATUSBAR_TIMEOUT);

        buf = g_strdup_printf (ngettext ("Normalized %d of %d new track.",
					       "Normalized %d of %d new tracks.", n),
                                         count, n);
        gtkpod_statusbar_message(buf);
        g_free (buf);

        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR (progress_bar),
                                        (gdouble) count/n);

        diff = time(NULL) - start;
        mins = ((diff*n/count)-diff)/60;
        secs = ((((diff*n/count)-diff) & 60) / 5) * 5;
      /* don't bounce up too quickly (>10% change only) */
/*	      left = ((mins < left) || (100*mins >= 110*left)) ? mins : left;*/
        progtext = g_strdup_printf (_("%d%% (%d:%02d) left"),
	 		  count*100/n, (int)mins, (int)secs);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR (progress_bar),
					progtext);
	g_free (progtext);
     } /*end if transferred*/

     if (abort && (count != n))
        gtkpod_statusbar_message (_("Some tracks were not normalized. Normalization aborted!"));
     while (widgets_blocked && gtk_events_pending ())  gtk_main_iteration ();
     list=g_list_next(list);
  } /*end while*/

  prefs_set_statusbar_timeout (0);

  gtk_widget_destroy (dialog);
  release_widgets();
}
