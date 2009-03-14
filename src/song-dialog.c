/*
 *  Copyright (c) 2009 Mike Massonnet <mmassonnet@xfce.org>
 *  Copyright (c) 2009 Vincent Legout <vincent@xfce.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <libxfcegui4/libxfcegui4.h>
#include <libxfce4util/libxfce4util.h>

#include "song-dialog.h"
#include "mpdclient.h"

#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), XFMPC_TYPE_SONG_DIALOG, XfmpcSongDialogPrivate))



static void             xfmpc_song_dialog_class_init    (XfmpcSongDialogClass *klass);
static void             xfmpc_song_dialog_init          (XfmpcSongDialog *dialog);
static void             xfmpc_song_dialog_finalize      (GObject *object);

static void             xfmpc_song_dialog_response      (GtkDialog *dialog,
                                                         gint response);

static void             xfmpc_song_dialog_set_song_info (XfmpcSongDialog *dialog,
                                                         XfmpcSongInfo *song_info);



struct _XfmpcSongDialogClass
{
  XfceTitledDialogClass          parent_class;
};

struct _XfmpcSongDialog
{
  XfceTitledDialog               parent;
  /*<private>*/
  XfmpcSongDialogPrivate        *priv;
};

struct _XfmpcSongDialogPrivate
{
  GtkWidget                     *label_file;
  GtkWidget                     *label_artist;
  GtkWidget                     *label_title;
  GtkWidget                     *label_album;
  GtkWidget                     *label_year;
  GtkWidget                     *label_track;
  GtkWidget                     *label_genre;
};



static GObjectClass *parent_class = NULL;



GType
xfmpc_song_dialog_get_type (void)
{
  static GType xfmpc_song_dialog_type = G_TYPE_INVALID;

  if (G_UNLIKELY (xfmpc_song_dialog_type == G_TYPE_INVALID))
    {
      static const GTypeInfo xfmpc_song_dialog_info =
      {
        sizeof (XfmpcSongDialogClass),
        NULL,
        NULL,
        (GClassInitFunc) xfmpc_song_dialog_class_init,
        NULL,
        NULL,
        sizeof (XfmpcSongDialog),
        0,
        (GInstanceInitFunc) xfmpc_song_dialog_init,
        NULL,
      };

      xfmpc_song_dialog_type = g_type_register_static (XFCE_TYPE_TITLED_DIALOG, "XfmpcSongDialog", &xfmpc_song_dialog_info, 0);
    }

  return xfmpc_song_dialog_type;
}



static void
xfmpc_song_dialog_class_init (XfmpcSongDialogClass *klass)
{
  GtkDialogClass *gtkdialog_class;
  GObjectClass   *gobject_class;

  g_type_class_add_private (klass, sizeof (XfmpcSongDialogPrivate));

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = xfmpc_song_dialog_finalize;

  gtkdialog_class = GTK_DIALOG_CLASS (klass);
  gtkdialog_class->response = xfmpc_song_dialog_response;
}

static void
xfmpc_song_dialog_init (XfmpcSongDialog *dialog)
{
  XfmpcSongDialogPrivate *priv = dialog->priv = GET_PRIVATE (dialog);

  gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
  gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER_ON_PARENT);
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW (dialog), TRUE);
  gtk_window_set_icon_name (GTK_WINDOW (dialog), "stock_volume");
  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
  gtk_window_set_title (GTK_WINDOW (dialog), _("Xfmpc"));

  gtk_dialog_add_buttons (GTK_DIALOG (dialog),
                          GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
                          NULL);

  PangoAttrList *attrs = pango_attr_list_new ();
  PangoAttribute *attr = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
  pango_attr_list_insert (attrs, attr);

  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
  gtk_widget_show (vbox);

  GtkWidget *vbox2 = gtk_vbox_new (FALSE, 0);
  GtkWidget *frame = xfce_create_framebox_with_content (NULL, vbox2);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);

  /* File */
  GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
  GtkWidget *label = gtk_label_new (_("File"));
  gtk_widget_set_size_request (GTK_WIDGET (label), 60, 10);
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  priv->label_file = gtk_label_new (NULL);
  gtk_box_pack_start (GTK_BOX (hbox), priv->label_file, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, FALSE, 6);

  /* Artist */
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new (_("Artist"));
  gtk_widget_set_size_request (GTK_WIDGET (label), 60, 10);
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  priv->label_artist = gtk_label_new (NULL);
  gtk_box_pack_start (GTK_BOX (hbox), priv->label_artist, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, FALSE, 6);

  /* Title */
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new (_("Title"));
  gtk_widget_set_size_request (GTK_WIDGET (label), 60, 10);
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  priv->label_title = gtk_label_new (NULL);
  gtk_box_pack_start (GTK_BOX (hbox), priv->label_title, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, FALSE, 6);

  /* Album */
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new (_("Album"));
  gtk_widget_set_size_request (GTK_WIDGET (label), 60, 10);
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  priv->label_album = gtk_label_new (NULL);
  gtk_box_pack_start (GTK_BOX (hbox), priv->label_album, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, FALSE, 6);

  /* Year and Track */
  hbox = gtk_hbox_new (TRUE, 0);
  label = gtk_label_new (_("Date"));
  gtk_widget_set_size_request (GTK_WIDGET (label), 60, 10);
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  priv->label_year = gtk_label_new (NULL);
  gtk_widget_set_size_request (GTK_WIDGET (priv->label_year), 60, 10);
  gtk_box_pack_start (GTK_BOX (hbox), priv->label_year, FALSE, FALSE, 0);
  label = gtk_label_new (_("Track"));
  gtk_widget_set_size_request (GTK_WIDGET (label), 60, 10);
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  priv->label_track = gtk_label_new (NULL);
  gtk_widget_set_size_request (GTK_WIDGET (label), 60, 10);
  gtk_box_pack_start (GTK_BOX (hbox), priv->label_track, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, FALSE, 6);

  /* Genre */
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new (_("Genre"));
  gtk_widget_set_size_request (GTK_WIDGET (label), 60, 10);
  gtk_label_set_attributes (GTK_LABEL (label), attrs);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  priv->label_genre = gtk_label_new (NULL);
  gtk_box_pack_start (GTK_BOX (hbox), priv->label_genre, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox, FALSE, FALSE, 6);

  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show_all (GTK_DIALOG (dialog)->vbox);
}

static void
xfmpc_song_dialog_finalize (GObject *object)
{
  G_OBJECT_CLASS (parent_class)->finalize (object);
}



static void
xfmpc_song_dialog_response (GtkDialog *dialog,
                            gint response)
{
  gtk_widget_destroy (GTK_WIDGET (dialog));
}



GtkWidget *
xfmpc_song_dialog_new (gint id)
{
  XfmpcMpdclient *mpdclient = xfmpc_mpdclient_get ();

  XfmpcSongInfo *song_info = xfmpc_mpdclient_get_song_info (mpdclient, id);

  GtkWidget *dialog = g_object_new (XFMPC_TYPE_SONG_DIALOG, NULL);
  xfmpc_song_dialog_set_song_info (XFMPC_SONG_DIALOG (dialog), song_info);

  return dialog;
}

static void
xfmpc_song_dialog_set_song_info (XfmpcSongDialog *dialog,
                              	 XfmpcSongInfo *song_info)
{
  XfmpcSongDialogPrivate *priv = dialog->priv = GET_PRIVATE (dialog);

  gtk_label_set_text (GTK_LABEL (priv->label_file), g_path_get_basename (song_info->filename));
  gtk_label_set_text (GTK_LABEL (priv->label_artist), song_info->artist);
  gtk_label_set_text (GTK_LABEL (priv->label_album), song_info->album);
  gtk_label_set_text (GTK_LABEL (priv->label_title), song_info->title);
  gtk_label_set_text (GTK_LABEL (priv->label_year), song_info->date);
  gtk_label_set_text (GTK_LABEL (priv->label_track), song_info->track);
  gtk_label_set_text (GTK_LABEL (priv->label_genre), song_info->genre);

  gtk_window_set_title (GTK_WINDOW (dialog), song_info->title);
}

