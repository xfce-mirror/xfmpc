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

	public class PreferencesDialog : Xfce.TitledDialog {

		private unowned Xfmpc.Preferences preferences;

		private CheckButton entry_use_defaults;
		private Entry entry_host;
		private Entry entry_passwd;
		private SpinButton entry_port;
		private CheckButton show_statusbar;
		private ComboBox combo_format;
		private Entry entry_custom;

		private uint format_timeout;

		private VBox mpd_vbox;

		construct {
			has_separator = true;
			skip_taskbar_hint = true;
			icon_name = "stock_volume";
			resizable = false;
			title = "Xfmpc Preferences";

			preferences = Xfmpc.Preferences.get ();

			var notebook = new Notebook ();
			notebook.set_border_width (6);
			vbox.pack_start (notebook, true, true, 0);

			/* Mpd Settings */
			var vbox = new VBox (false, 6);
			vbox.set_border_width (6);
			var label = new Label (_("MPD"));
			notebook.append_page (vbox, label);

			var vbox2 = new VBox (false, 6);
			var frame = Xfce.create_framebox_with_content (_("Connection"), vbox2);
			vbox.pack_start (frame, false, false, 0);

			entry_use_defaults = new CheckButton.with_mnemonic (_("Use _default system settings"));
			entry_use_defaults.set_tooltip_text (_("If checked, Xfmpc will try to read the environment variables MPD_HOST and MPD_PORT otherwise it will use localhost"));
			entry_use_defaults.set_active (preferences.mpd_use_defaults);

			vbox2.pack_start (entry_use_defaults, false, false, 0);

			mpd_vbox = new VBox (false, 6);
			vbox2.pack_start (mpd_vbox, false, false, 0);

			entry_use_defaults.toggled += cb_use_defaults_toggled;
			mpd_vbox.set_sensitive (!entry_use_defaults.get_active ());

			var hbox = new HBox (false, 2);
			mpd_vbox.pack_start (hbox, false, false, 0);

			label = new Label (_("Hostname:"));
			hbox.pack_start (label, false, false, 0);
			entry_host = new Entry ();
			entry_host.set_width_chars (15);
			entry_host.set_text (preferences.mpd_hostname);
			hbox.pack_start (entry_host, true, true, 0);

			label = new Label (_("Port:"));
			hbox.pack_start (label, false, false, 0);
			entry_port = new SpinButton.with_range (0, 65536, 1);
			entry_port.set_digits (0);
			entry_port.set_value (preferences.mpd_port);
			hbox.pack_start (entry_port, true, true, 0);

			hbox = new HBox (false, 2);
			mpd_vbox.pack_start (hbox, false, false, 0);

			label = new Label (_("Password:"));
			hbox.pack_start (label, false, false, 0);
			entry_passwd = new Entry ();
			entry_passwd.set_visibility (false);
			if (preferences.mpd_password != null)
				entry_passwd.set_text (preferences.mpd_password);
			hbox.pack_start (entry_passwd, true, true, 0);

			var button = new Button.from_stock (Gtk.STOCK_APPLY);
			button.clicked += cb_update_mpd;
			vbox2.pack_start (button, true, true, 0);

			/* Display */
			vbox = new VBox (false, 6);
			vbox.set_border_width (6);
			label = new Label (_("Appearance"));
			notebook.append_page (vbox, label);

			vbox2 = new VBox (false, 6);
			frame = Xfce.create_framebox_with_content (_("Statusbar"), vbox2);
			vbox.pack_start (frame, false, false, 0);

			show_statusbar = new CheckButton.with_mnemonic (_("Show _stastusbar"));
			show_statusbar.set_active (preferences.show_statusbar);
			show_statusbar.toggled += cb_show_statusbar_toggled;
			vbox2.pack_start (show_statusbar, false, false, 0);

			vbox2 = new VBox (false, 6);
			frame = Xfce.create_framebox_with_content (_("Song Format"), vbox2);
			vbox.pack_start (frame, false, false, 0);

			hbox = new HBox (false, 2);

			label = new Label (_("Song Format:"));
			hbox.pack_start (label, false, false, 0);

			combo_format = new ComboBox.text ();
			hbox.pack_start (combo_format, true, true, 0);

			combo_format.append_text (_("Title"));
			combo_format.append_text (_("Album - Title"));
			combo_format.append_text (_("Artist - Title"));
			combo_format.append_text (_("Artist - Title (Date)"));
			combo_format.append_text (_("Artist - Album - Title"));
			combo_format.append_text (_("Artist - Album - Track. Title"));
			combo_format.append_text (_("Custom..."));

			combo_format.set_active (preferences.song_format);

			vbox2.pack_start (hbox, true, true, 0);

			hbox = new HBox (false, 2);

			label = new Label (_("Custom format:"));
			hbox.pack_start (label, false, false, 0);

			entry_custom = new Entry ();
			entry_custom.set_width_chars (15);
			entry_custom.set_max_length (30);
			entry_custom.set_text (preferences.song_format_custom);
			entry_custom.set_sensitive (combo_format.get_active () == 6);
			hbox.pack_start (entry_custom, true, true, 0);

			combo_format.changed += cb_combo_format_changed;
			entry_custom.changed += cb_entry_custom_changed;

			vbox2.pack_start (hbox, true, true, 0);

			label = new Label (_("Available parameters:"));
			vbox2.pack_start (label, true, true, 0);

			var table = new Table (4, 6, true);

			var attrs = new Pango.AttrList ();
			attrs.insert (Pango.attr_scale_new ((double) Pango.Scale.SMALL));

			label = new Label (_("%a: Artist"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 1, 3, 0, 1);
			label = new Label (_("%A: Album"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 4, 6, 0, 1);

			label = new Label (_("%d: Date"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 1, 3, 1, 2);
			label = new Label (_("%D: Disc"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 4, 6, 1, 2);

			label = new Label (_("%f: File"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 1, 3, 2, 3);
			label = new Label (_("%g: Genre"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 4, 6, 2, 3);

			label = new Label (_("%t: Title"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 1, 3, 3, 4);
			label = new Label (_("%T: Track"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 4, 6, 3, 4);

			vbox2.pack_start (table, true, true, 0);

        		add_button (STOCK_CLOSE, ResponseType.CLOSE);

			show_all ();

			/* Signals */
			response += cb_response;
		}

		/*
		 * Signal callbacks
		 */

		private void cb_response (PreferencesDialog source, int response) {
        		switch (response) {
        			case ResponseType.CLOSE:
            	    	    	    destroy ();
            	    	    	    break;
			}
        	}

		private void cb_use_defaults_toggled (CheckButton source) {
			mpd_vbox.set_sensitive (!entry_use_defaults.get_active ());
        	}

		private void cb_update_mpd (Button source) {
			unowned Xfmpc.Mpdclient mpdclient = Xfmpc.Mpdclient.get ();
			string mpd_hostname = entry_host.get_text ();
			string mpd_password = entry_passwd.get_text ();

			preferences.mpd_hostname = mpd_hostname;
			preferences.mpd_port = entry_port.get_value_as_int ();
			preferences.mpd_password = mpd_password;
			preferences.mpd_use_defaults = entry_use_defaults.get_active ();

			mpdclient.disconnect ();
			mpdclient.connect ();
		}

		private void cb_show_statusbar_toggled (CheckButton source) {
			preferences.show_statusbar = show_statusbar.get_active ();
		}

		private void cb_combo_format_changed (ComboBox source) {

			switch (combo_format.get_active ())
			{
				case 0: preferences.song_format = Xfmpc.SongFormat.TITLE; break;
				case 1: preferences.song_format = Xfmpc.SongFormat.ALBUM_TITLE; break;
				case 2: preferences.song_format = Xfmpc.SongFormat.ARTIST_TITLE; break;
				case 3: preferences.song_format = Xfmpc.SongFormat.ARTIST_TITLE_DATE; break;
				case 4: preferences.song_format = Xfmpc.SongFormat.ARTIST_ALBUM_TITLE; break;
				case 5: preferences.song_format = Xfmpc.SongFormat.ARTIST_ALBUM_TRACK_TITLE; break;
				case 6: preferences.song_format = Xfmpc.SongFormat.CUSTOM; break;
				default: preferences.song_format = Xfmpc.SongFormat.TITLE; break;
			}

			entry_custom.set_sensitive (combo_format.get_active () == 6);
		}

		private void cb_entry_custom_changed (Entry source) {
			if (format_timeout > 0)
				Source.remove (format_timeout);

			format_timeout = Timeout.add_seconds_full (Priority.DEFAULT, 1, timeout_format);
		}

		private bool timeout_format () {
			string custom_format = entry_custom.get_text ();
			preferences.song_format_custom = custom_format;
			return false;
		}
	}
}

/* vi:set ts=8 sw=8: */
