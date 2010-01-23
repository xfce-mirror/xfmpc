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

	public class PreferencesDialog : Xfce.TitledDialog {

		private unowned Xfmpc.Preferences preferences;

		private Gtk.CheckButton entry_use_defaults;
		private Gtk.Entry entry_host;
		private Gtk.Entry entry_passwd;
		private Gtk.SpinButton entry_port;
		private Gtk.CheckButton show_statusbar;
		private Gtk.ComboBox combo_format;
		private Gtk.Entry entry_custom;

		private uint format_timeout;

		private Gtk.VBox mpd_vbox;

		construct {
			this.has_separator = true;
			this.skip_taskbar_hint = true;
			this.icon_name = "stock_volume";
			this.resizable = false;
			this.title = "Xfmpc Preferences";

			this.preferences = Xfmpc.Preferences.get_default ();

			var notebook = new Gtk.Notebook ();
			notebook.set_border_width (6);
			this.vbox.pack_start (notebook, true, true, 0);

			/* Mpd Settings */
			var vbox = new Gtk.VBox (false, 6);
			vbox.set_border_width (6);
			var label = new Gtk.Label (_("MPD"));
			notebook.append_page (vbox, label);

			var vbox2 = new Gtk.VBox (false, 6);
			var frame = Xfce.WidgetHelpers.create_framebox_with_content (_("Connection"), vbox2);
			vbox.pack_start (frame, false, false, 0);

			this.entry_use_defaults = new Gtk.CheckButton.with_mnemonic (_("Use _default system settings"));
			this.entry_use_defaults.set_tooltip_text (_("If checked, Xfmpc will try to read the environment variables MPD_HOST and MPD_PORT otherwise it will use localhost"));
			this.entry_use_defaults.set_active (this.preferences.mpd_use_defaults);

			vbox2.pack_start (this.entry_use_defaults, false, false, 0);

			this.mpd_vbox = new Gtk.VBox (false, 6);
			vbox2.pack_start (this.mpd_vbox, false, false, 0);

			this.entry_use_defaults.toggled.connect (cb_use_defaults_toggled);
			this.mpd_vbox.set_sensitive (!this.entry_use_defaults.get_active ());

			var hbox = new Gtk.HBox (false, 2);
			this.mpd_vbox.pack_start (hbox, false, false, 0);

			label = new Gtk.Label (_("Hostname:"));
			hbox.pack_start (label, false, false, 0);
			this.entry_host = new Gtk.Entry ();
			this.entry_host.set_width_chars (15);
			this.entry_host.set_text (this.preferences.mpd_hostname);
			hbox.pack_start (this.entry_host, true, true, 0);

			label = new Gtk.Label (_("Port:"));
			hbox.pack_start (label, false, false, 0);
			this.entry_port = new Gtk.SpinButton.with_range (0, 65536, 1);
			this.entry_port.set_digits (0);
			this.entry_port.set_value (this.preferences.mpd_port);
			hbox.pack_start (this.entry_port, true, true, 0);

			hbox = new Gtk.HBox (false, 2);
			this.mpd_vbox.pack_start (hbox, false, false, 0);

			label = new Gtk.Label (_("Password:"));
			hbox.pack_start (label, false, false, 0);
			this.entry_passwd = new Gtk.Entry ();
			this.entry_passwd.set_visibility (false);
			if (this.preferences.mpd_password != null)
				this.entry_passwd.set_text (this.preferences.mpd_password);
			hbox.pack_start (this.entry_passwd, true, true, 0);

			var button = new Gtk.Button.from_stock (Gtk.STOCK_APPLY);
			button.clicked.connect (cb_update_mpd);
			vbox2.pack_start (button, true, true, 0);

			/* Display */
			vbox = new Gtk.VBox (false, 6);
			vbox.set_border_width (6);
			label = new Gtk.Label (_("Appearance"));
			notebook.append_page (vbox, label);

			vbox2 = new Gtk.VBox (false, 6);
			frame = Xfce.WidgetHelpers.create_framebox_with_content (_("Statusbar"), vbox2);
			vbox.pack_start (frame, false, false, 0);

			this.show_statusbar = new Gtk.CheckButton.with_mnemonic (_("Show _stastusbar"));
			this.show_statusbar.set_active (this.preferences.show_statusbar);
			this.show_statusbar.toggled.connect (cb_show_statusbar_toggled);
			vbox2.pack_start (this.show_statusbar, false, false, 0);

			vbox2 = new Gtk.VBox (false, 6);
			frame = Xfce.WidgetHelpers.create_framebox_with_content (_("Song Format"), vbox2);
			vbox.pack_start (frame, false, false, 0);

			hbox = new Gtk.HBox (false, 2);

			label = new Gtk.Label (_("Song Format:"));
			hbox.pack_start (label, false, false, 0);

			this.combo_format = new Gtk.ComboBox.text ();
			hbox.pack_start (this.combo_format, true, true, 0);

			this.combo_format.append_text (_("Title"));
			this.combo_format.append_text (_("Album - Title"));
			this.combo_format.append_text (_("Artist - Title"));
			this.combo_format.append_text (_("Artist - Title (Date)"));
			this.combo_format.append_text (_("Artist - Album - Title"));
			this.combo_format.append_text (_("Artist - Album - Track. Title"));
			this.combo_format.append_text (_("Custom..."));

			this.combo_format.set_active (this.preferences.song_format);

			vbox2.pack_start (hbox, true, true, 0);

			hbox = new Gtk.HBox (false, 2);

			label = new Gtk.Label (_("Custom format:"));
			hbox.pack_start (label, false, false, 0);

			this.entry_custom = new Gtk.Entry ();
			this.entry_custom.set_width_chars (15);
			this.entry_custom.set_max_length (30);
			this.entry_custom.set_text (this.preferences.song_format_custom);
			this.entry_custom.set_sensitive (this.combo_format.get_active () == 6);
			hbox.pack_start (this.entry_custom, true, true, 0);

			this.combo_format.changed.connect (cb_combo_format_changed);
			this.entry_custom.changed.connect (cb_entry_custom_changed);

			vbox2.pack_start (hbox, true, true, 0);

			label = new Gtk.Label (_("Available parameters:"));
			vbox2.pack_start (label, true, true, 0);

			var table = new Gtk.Table (4, 6, true);

			var attrs = new Pango.AttrList ();
			attrs.insert (Pango.attr_scale_new ((double) Pango.Scale.SMALL));

			label = new Gtk.Label (_("%a: Artist"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 1, 3, 0, 1);
			label = new Gtk.Label (_("%A: Album"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 4, 6, 0, 1);

			label = new Gtk.Label (_("%d: Date"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 1, 3, 1, 2);
			label = new Gtk.Label (_("%D: Disc"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 4, 6, 1, 2);

			label = new Gtk.Label (_("%f: File"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 1, 3, 2, 3);
			label = new Gtk.Label (_("%g: Genre"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 4, 6, 2, 3);

			label = new Gtk.Label (_("%t: Title"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 1, 3, 3, 4);
			label = new Gtk.Label (_("%T: Track"));
			label.set_attributes (attrs);
			label.set_alignment (0, (float) 0.5);
			table.attach_defaults (label, 4, 6, 3, 4);

			vbox2.pack_start (table, true, true, 0);

        		add_button (Gtk.STOCK_CLOSE, Gtk.ResponseType.CLOSE);

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

		private void cb_use_defaults_toggled () {
			this.mpd_vbox.set_sensitive (!this.entry_use_defaults.get_active ());
        	}

		private void cb_update_mpd (Gtk.Button source) {
			unowned Xfmpc.Mpdclient mpdclient = Xfmpc.Mpdclient.get_default ();

			this.preferences.mpd_hostname = this.entry_host.get_text ();
			this.preferences.mpd_port = this.entry_port.get_value_as_int ();
			this.preferences.mpd_password = this.entry_passwd.get_text ();
			this.preferences.mpd_use_defaults = this.entry_use_defaults.get_active ();

			mpdclient.disconnect ();
			mpdclient.connect ();
		}

		private void cb_show_statusbar_toggled () {
			this.preferences.show_statusbar = this.show_statusbar.get_active ();
		}

		private void cb_combo_format_changed (Gtk.ComboBox source) {

			switch (combo_format.get_active ())
			{
				case 0: this.preferences.song_format = Xfmpc.Preferences.SongFormat.TITLE; break;
				case 1: this.preferences.song_format = Xfmpc.Preferences.SongFormat.ALBUM_TITLE; break;
				case 2: this.preferences.song_format = Xfmpc.Preferences.SongFormat.ARTIST_TITLE; break;
				case 3: this.preferences.song_format = Xfmpc.Preferences.SongFormat.ARTIST_TITLE_DATE; break;
				case 4: this.preferences.song_format = Xfmpc.Preferences.SongFormat.ARTIST_ALBUM_TITLE; break;
				case 5: this.preferences.song_format = Xfmpc.Preferences.SongFormat.ARTIST_ALBUM_TRACK_TITLE; break;
				case 6: this.preferences.song_format = Xfmpc.Preferences.SongFormat.CUSTOM_FORMAT; break;
				default: this.preferences.song_format = Xfmpc.Preferences.SongFormat.TITLE; break;
			}

			this.entry_custom.set_sensitive (this.combo_format.get_active () == 6);
		}

		private void cb_entry_custom_changed () {
			if (this.format_timeout > 0)
				GLib.Source.remove (this.format_timeout);

			this.format_timeout = GLib.Timeout.add_seconds_full (GLib.Priority.DEFAULT, 1, timeout_format);
		}

		private bool timeout_format () {
			string custom_format = this.entry_custom.get_text ();
			this.preferences.song_format_custom = custom_format;
			return false;
		}
	}
}
