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

using Gtk;

namespace Xfmpc {

	public class SongDialog : Xfce.TitledDialog {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.SongInfo song;

		public SongDialog (int song_id) {
			has_separator = true;
			skip_taskbar_hint = true;
			icon_name = "stock_volume";
			resizable = false;

			mpdclient = Xfmpc.Mpdclient.get ();
			song = mpdclient.get_song_info (song_id);

			title = song.title;

			vbox.set_spacing (0);

			var vbox2 = new VBox (false, 0);
			var frame = Xfce.create_framebox_with_content ("", vbox2);

			var attrs = new Pango.AttrList ();
			attrs.insert (Pango.attr_weight_new (Pango.Weight.BOLD));

			var hbox = new HBox (false, 0);
			var label = new Label (_("File"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Label (Path.get_basename (song.filename));
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new HBox (false, 0);
			label = new Label (_("Artist"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Label (song.artist);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new HBox (false, 0);
			label = new Label (_("Title"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Label (song.title);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new HBox (false, 0);
			label = new Label (_("Album"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Label (song.album);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new HBox (false, 0);
			label = new Label (_("Date"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Label (song.date);
			hbox.pack_start (label, false, false, 5);
			label = new Label (_("Track"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 15);
			label = new Label (song.track);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			hbox = new HBox (false, 0);
			label = new Label (_("Genre"));
			label.set_attributes (attrs);
			hbox.pack_start (label, false, false, 5);
			label = new Label (song.genre);
			hbox.pack_start (label, false, false, 5);
			vbox2.pack_start (hbox, false, false, 6);

			vbox.pack_start (frame, true, true, 0);

        		add_button (STOCK_CLOSE, ResponseType.CLOSE);

			show_all ();

			/* Signals */
			response += cb_response;
		}

		/*
		 * Signal callbacks
		 */

		private void cb_response (SongDialog source, int response) {
        		switch (response) {
        			case ResponseType.CLOSE:
            	    	    	    destroy ();
            	    	    	    break;
			}
        	}
	}

}

/* vi:set ts=8 sw=8: */
