/* Time-stamp: <2003-11-29 12:37:45 jcs>
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

#ifndef __PREFS_H__
#define __PREFS_H__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include "display.h"

struct win_size {
    gint x;
    gint y;
};

struct cfg
{
  gchar    *ipod_mount;   /* mount point of iPod */
  gchar    *charset;      /* CHARSET to use with file operations */
  gboolean id3_write;     /* should changes to ID3 tags be written to file */
  gboolean id3_writeall;  /* should all ID3 tags be updated */
  gboolean md5tracks;	  /* don't allow track duplication on your ipod */
  gboolean update_existing;/* when adding track, update existing track */
  gboolean block_display; /* block display during change of selection? */
  gboolean autoimport;	  /* whether or not to automatically import files */
  struct
  {
    gboolean autoselect;      /* automatically select "All" in sort tab? */
    guint    category;        /* which category was selected last? */
    /* the following fields are for the "special" tab */
    gboolean sp_or;           /* logic operation: OR? (FALSE: AND) */
    gboolean sp_rating;       /* consider rating? */
    guint32  sp_rating_state; /* pass which rating? (1 star: bit 1...) */
    gboolean sp_playcount;    /* consider playcount? */
    guint sp_playcount_low;   /* lower limit for playcounts */
    guint sp_playcount_high;  /* higher limit for playcounts */
    gboolean sp_played;       /* consider last played? */
    gchar *sp_played_state;   /* current "played" string */
    gboolean sp_modified;     /* consider last modified? */
    gchar *sp_modified_state; /* current "modified" string */
    gboolean sp_autodisplay;  /* display automatically? */ 
  } st[SORT_TAB_MAX];
  struct sortcfg
  {         /* sort type: SORT_ASCENDING, SORT_DESCENDING, SORT_NONE */
    gint pm_sort;             /* sort type for playlists           */
    gint st_sort;             /* sort type for sort tabs           */
    gint tm_sort;             /* sort type for tracks              */
    TM_item tm_sortcol;       /* sort column for tracks            */
    gboolean pm_autostore;    /* save sort order automatically?    */
    gboolean tm_autostore;    /* save sort order automatically?    */
    gboolean case_sensitive;  /* Should sorting be case-sensitive? */
  } sortcfg;
  gboolean info_window;   /* is info window open (will then open on restart */
  gboolean mpl_autoselect;/* select mpl automatically? */
  gboolean offline;       /* are we working offline, i.e. without iPod? */
  gboolean keep_backups;  /* write backups of iTunesDB etc to ~/.gtkpod? */
  gboolean write_extended_info; /* write additional file with PC
				   filenames etc? */
  struct
  {
      gchar *browse, *export;
  } last_dir;	          /* last directories used by the fileselections */
  struct
  {
      gboolean track, ipod_file, syncing;
  } deletion;
  struct win_size size_gtkpod;  /* last size of gtkpod main window */
  struct win_size size_cal;     /* last size of calendar window */
  struct win_size size_conf_sw; /* last size of conf window (scrolled) */
  struct win_size size_conf;    /* last size of conf window */
  struct win_size size_dirbr;   /* last size of dirbrowser window */
  struct win_size size_prefs;   /* last size of prefs window */
  gint tm_col_width[TM_NUM_COLUMNS];    /* width colums in track model */
  gboolean col_visible[TM_NUM_COLUMNS]; /* displayed track model colums */
  TM_item col_order[TM_NUM_COLUMNS];    /* order of columns */
  gboolean tag_autoset[TM_NUM_TAGS_PREFS]; /* autoset empty tags to filename?*/
  gint paned_pos[PANED_NUM];    /* position of the GtkPaned elements */

  gboolean show_duplicates;     /* show duplicate notification ?*/
  gboolean show_updated;        /* show update notification ?*/
  gboolean show_non_updated;    /* show update notification ?*/
  gboolean show_sync_dirs;      /* show dirs to be synced ? */
  gboolean sync_remove;         /* delete tracks removed from synced dirs? */
  gboolean display_toolbar;     /* should toolbar be displayed */
  GtkToolbarStyle toolbar_style;/* style of toolbar */
  gboolean display_tooltips_main; /* should tooltips be displayed (main) */
  gboolean display_tooltips_prefs;/* should toolbar be displayed (prefs) */
  gboolean update_charset;      /* Update charset when updating track? */
  gboolean write_charset;       /* Use selected charset when writing track? */
  gboolean add_recursively;     /* Add directories recursively? */
  gint sort_tab_num;            /* number of sort tabs displayed */
  guint32 statusbar_timeout;    /* timeout for statusbar messages */
  gint last_prefs_page;         /* last page selected in prefs window */
  gchar *play_now_path;         /* path for 'Play Now' */
  gchar *play_enqueue_path;     /* path for 'Play', i.e. 'Enqueue' */
  gchar *mp3gain_path;          /* path for the mp3gain executable */
  gchar *time_format;           /* time format for strftime() */
  gchar *filename_format;       /* filename for files exported from ipod */
  gboolean automount;		/* whether we should mount/unmount the ipod */
  gboolean multi_edit;          /* multi edit enabled? */
  gboolean multi_edit_title;    /* multi edit also enabled for title field? */
  gboolean not_played_track;     /* not played track in Highest rated playlist?*/
  gboolean special_export_charset; /* use original charset or specified one? */
  gint misc_track_nr;            /* track's nr in the Highest rated, most played and most recently played pl*/ 
  gboolean write_gaintag;       /* should we append the mp3gain's tag to the mp3files?*/
  float version;                /* version of gtkpod writing the cfg file */
};


/* types for sortcfg.xx_sort */
enum
{
    SORT_ASCENDING = GTK_SORT_ASCENDING,
    SORT_DESCENDING = GTK_SORT_DESCENDING,
    SORT_NONE = 10*(GTK_SORT_ASCENDING+GTK_SORT_DESCENDING),
};
/* SORT_RESET: only used for sort_window_set_tm_sort() */



/* FIXME: make the global struct obsolete! */
/*extern struct cfg *cfg;*/

gchar *prefs_get_cfgdir (void);
void prefs_print(void);
void cfg_free(struct cfg *c);
void sortcfg_free(struct sortcfg *c);
void write_prefs (void);
void discard_prefs (void);
struct cfg* clone_prefs(void);
struct sortcfg* clone_sortprefs(void);
void prefs_set_mount_point(const gchar *mp);
gboolean read_prefs (GtkWidget *gtkpod, int argc, char *argv[]);

void prefs_set_offline(gboolean active);
void prefs_set_pm_sort (gint type);
void prefs_set_tm_sort (gint type);
void prefs_set_st_sort (gint type);
void prefs_set_tm_sortcol (TM_item col);
void prefs_set_tm_autostore (gboolean active);
void prefs_set_pm_autostore (gboolean active);
void prefs_set_keep_backups(gboolean active);
void prefs_set_write_extended_info(gboolean active);
void prefs_set_auto_import(gboolean val);
void prefs_set_st_autoselect (guint32 inst, gboolean autoselect);
void prefs_set_mpl_autoselect (gboolean autoselect);
void prefs_set_st_category (guint32 inst, guint category);
void prefs_set_track_playlist_deletion(gboolean val);
void prefs_set_track_ipod_file_deletion(gboolean val);
void prefs_set_sync_remove_confirm(gboolean val);
void prefs_set_md5tracks(gboolean active);
void prefs_set_update_existing(gboolean active);
void prefs_set_block_display(gboolean active);
void prefs_set_id3_write(gboolean active);
void prefs_set_id3_writeall(gboolean active);
void prefs_set_last_dir_browse(const gchar * dir);
const gchar *prefs_get_last_dir_browse(void);
void prefs_set_last_dir_export(const gchar * dir);
const gchar *prefs_get_last_dir_export(void);
void prefs_set_charset (gchar *charset);
void prefs_cfg_set_charset (struct cfg *cfg, gchar *charset);
void prefs_set_size_gtkpod (gint x, gint y);
void prefs_set_size_cal (gint x, gint y);
void prefs_set_size_conf_sw (gint x, gint y);
void prefs_set_size_conf (gint x, gint y);
void prefs_set_size_dirbr (gint x, gint y);
void prefs_set_size_prefs (gint x, gint y);
void prefs_set_tm_col_width (gint col, gint width);
void prefs_set_tag_autoset (gint category, gboolean autoset);
void prefs_set_col_visible (TM_item tm_item, gboolean visible);
void prefs_set_col_order (gint pos, TM_item col);
void prefs_set_paned_pos (gint i, gint pos);
void prefs_set_statusbar_timeout (guint32 val);
void prefs_set_automount(gboolean val);
void prefs_set_info_window(gboolean val);

gboolean prefs_get_offline(void);
gint prefs_get_pm_sort (void);
gint prefs_get_st_sort (void);
gint prefs_get_tm_sort (void);
TM_item prefs_get_tm_sortcol (void);
gboolean prefs_get_tm_autostore (void);
gboolean prefs_get_pm_autostore (void);
gboolean prefs_get_keep_backups(void);
gboolean prefs_get_write_extended_info(void);
gboolean prefs_get_auto_import(void);
gboolean prefs_get_st_autoselect (guint32 inst);
gboolean prefs_get_mpl_autoselect (void);
guint prefs_get_st_category (guint32 inst);
gboolean prefs_get_track_playlist_deletion(void);
gboolean prefs_get_track_ipod_file_deletion(void);
gboolean prefs_get_sync_remove_confirm(void);
gboolean prefs_get_id3_write(void);
gboolean prefs_get_id3_writeall(void);
const gchar *prefs_get_ipod_mount (void);
gchar * prefs_get_charset (void);
void prefs_get_size_gtkpod (gint *x, gint *y);
void prefs_get_size_cal (gint *x, gint *y);
void prefs_get_size_conf_sw (gint *x, gint *y);
void prefs_get_size_conf (gint *x, gint *y);
void prefs_get_size_dirbr (gint *x, gint *y);
void prefs_get_size_prefs (gint *x, gint *y);
gint prefs_get_tm_col_width (gint col);
gboolean prefs_get_tag_autoset (gint category);
gboolean prefs_get_col_visible (TM_item tm_item);
TM_item prefs_get_col_order (gint pos);
gboolean prefs_get_md5tracks(void);
gboolean prefs_get_update_existing(void);
gboolean prefs_get_block_display(void);
gint prefs_get_paned_pos (gint i);
guint32 prefs_get_statusbar_timeout (void);
gboolean prefs_get_show_duplicates (void);
void prefs_set_show_duplicates (gboolean val);
gboolean prefs_get_show_updated (void);
void prefs_set_show_updated (gboolean val);
gboolean prefs_get_show_non_updated (void);
void prefs_set_show_non_updated (gboolean val);
gboolean prefs_get_show_sync_dirs (void);
void prefs_set_show_sync_dirs (gboolean val);
gboolean prefs_get_sync_remove (void);
void prefs_set_sync_remove (gboolean val);
gboolean prefs_get_display_toolbar (void);
void prefs_set_display_toolbar (gboolean val);
gboolean prefs_get_update_charset (void);
void prefs_set_update_charset (gboolean val);
gboolean prefs_get_write_charset (void);
void prefs_set_write_charset (gboolean val);
gboolean prefs_get_add_recursively (void);
void prefs_set_add_recursively (gboolean val);
gboolean prefs_get_case_sensitive (void);
void prefs_set_case_sensitive (gboolean val);
gint prefs_get_sort_tab_num (void);
void prefs_set_sort_tab_num (gint i, gboolean update_display);
GtkToolbarStyle prefs_get_toolbar_style (void);
void prefs_set_toolbar_style (GtkToolbarStyle i);
gint prefs_get_last_prefs_page (void);
void prefs_set_last_prefs_page (gint i);
gchar *prefs_validate_play_path (const gchar *path);
void prefs_set_play_now_path (const gchar *path);
const gchar *prefs_get_play_now_path (void);
void prefs_set_play_enqueue_path (const gchar *path);
const gchar *prefs_get_play_enqueue_path (void);
void prefs_set_mp3gain_path (const gchar *path);
const gchar *prefs_get_mp3gain_path (void);
void prefs_set_time_format (const gchar *format);
gchar *prefs_get_time_format (void);
gboolean prefs_get_automount (void);
gboolean prefs_get_info_window (void);
void prefs_set_sp_or (guint32 inst, gboolean state);
gboolean prefs_get_sp_or (guint32 inst);
void prefs_set_sp_cond (guint32 inst, T_item t_item, gboolean state);
gboolean prefs_get_sp_cond (guint32 inst, T_item t_item);
void prefs_set_sp_rating_n (guint32 inst, gint n, gboolean state);
gboolean prefs_get_sp_rating_n (guint32 inst, gint n);
void prefs_set_sp_rating_state (guint32 inst, guint32 state);
guint32 prefs_get_sp_rating_state (guint32 inst);
void prefs_set_sp_entry (guint32 inst, T_item t_item, const gchar *str);
gchar *prefs_get_sp_entry (guint32 inst, T_item t_item);
void prefs_set_sp_autodisplay (guint32 inst, gboolean state);
gboolean prefs_get_sp_autodisplay (guint32 inst);
gint32 prefs_get_sp_playcount_low (guint32 inst);
gint32 prefs_get_sp_playcount_high (guint32 inst);
void prefs_set_sp_playcount_low (guint32 inst, gint32 limit);
void prefs_set_sp_playcount_high (guint32 inst, gint32 limit);
void prefs_set_display_tooltips_main (gboolean state);
gboolean prefs_get_display_tooltips_main (void);
void prefs_set_display_tooltips_prefs (gboolean state);
gboolean prefs_get_display_tooltips_prefs (void);
void prefs_set_multi_edit (gboolean state);
gboolean prefs_get_multi_edit (void);
void prefs_set_misc_track_nr (gint state);
gint prefs_get_misc_track_nr (void);
void prefs_set_not_played_track (gboolean state);
gboolean prefs_get_not_played_track (void);
void prefs_set_multi_edit_title (gboolean state);
gboolean prefs_get_multi_edit_title (void);
void prefs_set_filename_format (char* state);
char* prefs_get_filename_format (void);
void prefs_set_write_gaintag(gboolean val);
gboolean prefs_get_write_gaintag(void);
void prefs_set_special_export_charset(gboolean val);
gboolean prefs_get_special_export_charset(void);
#endif
