/*
 *  Copyright (c) 2009-2010 Mike Massonnet <mmassonnet@xfce.org>
 *  Copyright (c) 2009-2010 Vincent Legout <vincent@xfce.org>
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

using Gtk;

namespace Xfmpc {

	public class SongDialog : Xfce.TitledDialog {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.SongInfo song;

		public SongDialog (int song_id) {
			this.skip_taskbar_hint = true;
			this.icon_name = "multimedia-player";
			this.resizable = false;

			this.mpdclient = Xfmpc.Mpdclient.get_default ();
			this.song = mpdclient.get_song_info (song_id);

			this.title = this.song.title;

			this.get_content_area ().set_spacing (0);

			var vbox2 = new Gtk.Box (Gtk.Orientation.VERTICAL, 0);
			var frame = Xfce.gtk_frame_box_new_with_content ("", vbox2);

			var attrs = new Pango.AttrList ();
			attrs.insert (Pango.attr_weight_new (Pango.Weight.BOLD));

			var hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
			var label = new Gtk.Label (_("File"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Gtk.Label (GLib.Path.get_basename (song.filename));
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
			label = new Gtk.Label (_("Artist"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Gtk.Label (song.artist);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
			label = new Gtk.Label (_("Title"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Gtk.Label (song.title);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
			label = new Gtk.Label (_("Album"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Gtk.Label (song.album);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
			label = new Gtk.Label (_("Date"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Gtk.Label (song.date);
			hbox.pack_start (label, false, false, 5);
			label = new Gtk.Label (_("Track"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 15);
			label = new Gtk.Label (song.track);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 0);
			label = new Gtk.Label (_("Genre"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Gtk.Label (song.genre);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			get_content_area ().pack_start (frame, true, true, 0);

			Gtk.Dialog dialog = this;
			dialog.add_button (_("Close"), Gtk.ResponseType.CLOSE);

			show_all ();

			/* Signals */
			this.response.connect (cb_response);
		}

		/*
		 * Signal callbacks
		 */

		private void cb_response (int response) {
        		switch (response) {
        			case Gtk.ResponseType.CLOSE:
            	    	    	    destroy ();
            	    	    	    break;
			}
        	}
	}

}
