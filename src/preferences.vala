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

	public class Preferences : GLib.Object {

		static Preferences preferences;

		public enum SongFormat
		{
  	  	  TITLE,
  	  	  ALBUM_TITLE,
  	  	  ARTIST_TITLE,
  	  	  ARTIST_TITLE_DATE,
  	  	  ARTIST_ALBUM_TITLE,
  	  	  ARTIST_ALBUM_TRACK_TITLE,
  	  	  CUSTOM_FORMAT,
		}

		private int _last_window_posx;
		public int last_window_posx {
			get { return _last_window_posx; }
			set { _last_window_posx = value; store (); }
		}
		private int _last_window_posy;
		public int last_window_posy {
			get { return _last_window_posy; }
			set { _last_window_posy = value; store (); }
		}
		private int _last_window_width;
		public int last_window_width {
			get { return _last_window_width; }
			set { _last_window_width = value; store (); }
		}
		private int _last_window_height;
		public int last_window_height {
			get { return _last_window_height; }
			set { _last_window_height = value; store (); }
		}
		private bool _last_window_state_sticky;
		public bool last_window_state_sticky {
			get { return _last_window_state_sticky; }
			set { _last_window_state_sticky = value; store (); }
		}
		private bool _playlist_autocenter;
		public bool playlist_autocenter {
			get { return _playlist_autocenter; }
			set { _playlist_autocenter = value; store (); }
		}
		private string _dbbrowser_last_path;
		public string dbbrowser_last_path {
			get { return _dbbrowser_last_path; }
			set { _dbbrowser_last_path = value; store (); }
		}
		private string _mpd_hostname;
		public string mpd_hostname {
			get { return _mpd_hostname; }
			set { _mpd_hostname = value; store (); }
		}
		private int _mpd_port;
		public int mpd_port {
			get { return _mpd_port; }
			set { _mpd_port = value; store (); }
		}
		private string _mpd_password;
		public string mpd_password {
			get { return _mpd_password; }
			set { _mpd_password = value; store (); }
		}
		private bool _mpd_use_defaults;
		public bool mpd_use_defaults {
			get { return _mpd_use_defaults; }
			set { _mpd_use_defaults = value; store (); }
		}
		private bool _show_statusbar;
		public bool show_statusbar {
			get { return _show_statusbar; }
			set { _show_statusbar = value; store (); }
		}
		private SongFormat _song_format;
		public SongFormat song_format {
			get { return _song_format; }
			set { _song_format = value; store (); }
		}
		private string _song_format_custom;
		public string song_format_custom {
			get { return _song_format_custom; }
			set { _song_format_custom = value; store (); }
		}

		construct {
			_last_window_posx = 200;
			_last_window_posy = 100;
			_last_window_width = 330;
			_last_window_height = 330;
			_last_window_state_sticky = true;
			_playlist_autocenter = true;
			_dbbrowser_last_path = "";
			_mpd_hostname = "localhost";
			_mpd_port = 6600;
			_mpd_password = "";
			_mpd_use_defaults = true;
			_show_statusbar = true;
			_song_format = SongFormat.ARTIST_TITLE;
			_song_format_custom = "%a - %t";
		}

		public Preferences () {
			load ();
		}

		public static unowned Preferences get_default () {
			if (preferences == null) {
				preferences = new Preferences ();
				preferences.add_weak_pointer (&preferences);
			}
			else
				preferences.ref ();

			return preferences;
		}

		private void load () {
			Xfce.Rc rc = new Xfce.Rc.from_resource (Xfce.ResourceType.CONFIG, "xfce4/xfmpcrc", false);

			if (rc == null) {
				GLib.warning ("Failed to load the preferences");
				return;
			}

			freeze_notify ();

			rc.set_group ("Configuration");

			string entry;
			GLib.Value src = Value (typeof (string));

			foreach (var spec in ((ObjectClass)(get_type ().class_peek ())).list_properties ())
			{
				entry = rc.read_entry (spec.get_nick (), "");
				if (entry == "")
					continue;

				src.set_static_string (entry);
				set_property (spec.name, src);
			}

			thaw_notify ();
		}

		private void store () {
			Xfce.Rc rc = new Xfce.Rc.from_resource (Xfce.ResourceType.CONFIG, "xfce4/xfmpcrc", false);

			if (rc == null) {
				GLib.warning ("Failed to save the preferences");
				return;
			}

			rc.set_group ("Configuration");

			string entry;
			GLib.Value dst = Value (typeof (string));

			foreach (var spec in ((ObjectClass)(get_type ().class_peek ())).list_properties ())
			{
				if (spec.value_type == typeof (string))
					get_property (spec.name, ref dst);
				else
				{
					GLib.Value src = Value (spec.value_type);
					get_property (spec.name, ref src);
					src.transform (ref dst);
				}

				entry = dst.get_string ();
				if (entry != null)
					rc.write_entry (spec.get_nick (), entry);
			}
		}
	}
}
