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

	public class MainWindow : Window {

		private Gtk.StatusIcon status_icon;
		private Gtk.Menu status_icon_menu;

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private Xfmpc.Interface @interface;
		private Xfmpc.ExtendedInterface extended_interface;

		private Gtk.Box vbox;
		private Gtk.ActionGroup action_group;
		private Gtk.UIManager ui_manager;
		private Xfmpc.Statusbar statusbar;

		private const Gtk.ToggleActionEntry[] toggle_action_entries = {
			{ "view-statusbar", null, "", null, null, action_statusbar, false }
		};

		private const Gtk.ActionEntry[] action_entries = {
			{ "quit", null, "", "<control>q", null, action_quit },
			{ "previous", null, "", "<control>b", null, action_previous },
			{ "pp", null, "", "<control>p", null, action_pp },
			{ "stop", null, "", "<control>s", null, action_stop },
			{ "next", null, "", "<control>f", null, action_next },
			{ "volume", null, "", "<control>v", null, action_volume }
		};

		private const string ui_string =
"""
<ui>
  <accelerator action="quit" />
  <accelerator action="previous" />
  <accelerator action="pp" />
  <accelerator action="stop" />
  <accelerator action="next" />
  <accelerator action="volume" />
</ui>
""";

		construct {
			mpdclient = Xfmpc.Mpdclient.get_default ();
			preferences = Xfmpc.Preferences.get_default ();

			MainWindow.load_css ();

			/* StatusIcon */
			this.status_icon = new Gtk.StatusIcon.from_icon_name ("multimedia-player");
			show_hide_status_icon ();
			this.status_icon.activate.connect (status_icon_activated);
			this.status_icon.popup_menu.connect (status_icon_popup_menu);
			this.preferences.notify["show-status-icon"].connect (show_hide_status_icon);

  	  	  	/* Window */
			set_default_icon_name ("xfmpc");
			set_icon_name ("multimedia-player");
			set_title (Config.PACKAGE_NAME);
			set_default_size (330, 330);
			this.delete_event.connect (cb_window_closed);
			this.window_state_event.connect (cb_window_state_event);

			this.vbox = new Gtk.Box (Gtk.Orientation.VERTICAL, 0);
			add (vbox);

			if (this.preferences.last_window_posx != -1 && this.preferences.last_window_posy != -1)
				move (this.preferences.last_window_posx, this.preferences.last_window_posy);
			if (this.preferences.last_window_width != -1 && this.preferences.last_window_height != -1)
				set_default_size (this.preferences.last_window_width, this.preferences.last_window_height);
			if (this.preferences.last_window_state_sticky == true)
				stick ();

  	  	  	/* Interface */
			this.@interface = new Xfmpc.Interface ();
			this.vbox.pack_start (this.@interface, false, false, 4);

  	  	  	/* Separator */
			var separator = new Gtk.Separator (Gtk.Orientation.HORIZONTAL);
			this.vbox.pack_start (separator, false, false, 0);

  	  	  	/* ExtendedInterface */
			this.extended_interface = new Xfmpc.ExtendedInterface ();
			this.vbox.pack_start (this.extended_interface, true, true, 0);

  	  	  	/* Accelerators */
			this.ui_manager = new Gtk.UIManager ();

  	  	  	/* Action group */
			this.action_group = new Gtk.ActionGroup ("XfmpcMainWindow");
			this.action_group.add_actions (this.action_entries, this);
			this.action_group.add_toggle_actions (this.toggle_action_entries, this);
			this.ui_manager.insert_action_group (this.action_group, 0);
			try {
				this.ui_manager.add_ui_from_string (this.ui_string, -1);
			} catch (Error e) {
				warning (e.message);
			}

  	  	  	/* Accel group */
			var accel_group = this.ui_manager.get_accel_group ();
			add_accel_group (accel_group);

  	  	  	/* show-statusbar action */
			((Gtk.ToggleAction )(this.action_group.get_action ("view-statusbar"))).set_active (this.preferences.show_statusbar);

  	  	  	/* === Signals === */
			this.mpdclient.pp_changed.connect (update_window_title);
			this.mpdclient.stopped.connect (update_window_title);
			this.mpdclient.song_changed.connect (update_window_title);
			this.mpdclient.playlist_changed.connect (cb_playlist_changed);
			this.preferences.notify["show-statusbar"].connect (cb_show_statusbar_changed);

  	  	  	/* === Timeout === */
			Gdk.threads_add_timeout (1000, refresh);
		}

		private static void load_css () {
			var provider = new Gtk.CssProvider ();
			provider.load_from_data (
				".primary-button { padding-left: 8px; padding-right: 8px; }" +
				".red { background-color: #FF6666; }",
				-1);
			Gtk.StyleContext.add_provider_for_screen (
				Gdk.Screen.get_default (),
				provider,
				Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
		}

		private void show_hide_status_icon () {
			this.status_icon.set_visible (this.preferences.show_status_icon);
		}

		private void status_icon_activated () {
			if (!this.visible) {
				this.show ();
				this.deiconify ();
			} else {
				this.close_window ();
			}
		}

		private void status_icon_popup_menu (uint button, uint activate_time) {
			if (this.status_icon_menu == null) {
				this.status_icon_menu = new Gtk.Menu ();
				var mi = new Gtk.MenuItem.with_mnemonic (_("Quit"));
				mi.activate.connect (Gtk.main_quit);
				this.status_icon_menu.add (mi);
				this.status_icon_menu.show_all ();
			}

			this.status_icon_menu.popup (null, null, this.status_icon.position_menu, button, activate_time);
		}

		private bool refresh () {
			if (this.mpdclient.is_connected ()) {
				this.mpdclient.update_status ();
			} else {
				this.@interface.reset ();
				this.mpdclient.reload ();
				if (this.statusbar != null)
					this.statusbar.text = "";
				this.mpdclient.connect ();
				if (this.mpdclient.is_connected ())
					this.@interface.update_title ();
			}

			return true;
		}

		private bool cb_window_state_event (Gdk.EventWindowState event) {
			if (event.type != Gdk.EventType.WINDOW_STATE)
				return false;

  	  	  	/**
   	   	   	 * Hiding the top level window will unstick it too, and send a
   	   	   	 * window-state-event signal, so here we take the value only if
   	   	   	 * the window is visible
   	   	   	 **/
			if ((bool) (event.changed_mask & Gdk.WindowState.STICKY) && this.visible) {
				bool sticky;
				if (((bool) (event.new_window_state & Gdk.WindowState.STICKY)) == false)
					sticky = false;
				else
					sticky = true;
				this.preferences.last_window_state_sticky = sticky;
			}

			return false;
		}

		private bool cb_window_closed (Gdk.Event event) {
			close_window ();

			/* Return true in case the window should not be destroyed,
			 * and there is currently one case when this should be done:
			 * the user choosed to "close into notification area," that
			 * is when the status icon is visible. */
			return this.status_icon.visible;
		}

		private void action_quit () {
			this.status_icon.set_visible (false);
			close_window ();
		}

		private void close_window () {
			int posx, posy;
			int width, height;

			get_position (out posx, out posy);
			get_size (out width, out height);

			this.preferences.last_window_posx = posx;
			this.preferences.last_window_posy = posy;
			this.preferences.last_window_width = width;
			this.preferences.last_window_height = height;

			if (this.status_icon.visible) {
				this.hide ();
				move (this.preferences.last_window_posx, this.preferences.last_window_posy);
				return;
			}

			main_quit ();
		}

		private void action_previous () {
			this.mpdclient.previous ();
		}

		private void action_pp () {
			this.@interface.pp_clicked ();
		}

		private void action_stop () {
			this.mpdclient.stop ();
		}

		private void action_next () {
			this.mpdclient.next ();
		}

		private void action_volume () {
			this.@interface.popup_volume ();
		}

		private void action_statusbar (Gtk.Action action) {
			bool active = ((Gtk.ToggleAction) action).get_active ();

			if (!active && this.statusbar != null) {
				this.statusbar.destroy ();
				this.statusbar = null;
			}
			else if (active && this.statusbar == null) {
				this.statusbar = new Statusbar ();
				this.statusbar.show ();
				this.vbox.pack_start (this.statusbar, false, false, 0);
			}
		}

		private void update_statusbar () {
			int seconds, length;

			if (this.statusbar == null)
				return;

			if (!this.mpdclient.is_connected ())
				return;

			length = this.mpdclient.playlist_get_length ();
			seconds = this.mpdclient.playlist_get_total_time ();

			GLib.StringBuilder text = new GLib.StringBuilder ();
			if (seconds / 3600 > 0)
				text.append_printf (_("%d songs, %d hours and %d minutes"), length, seconds / 3600, (seconds / 60) % 60);
			else
				text.append_printf (_("%d songs, %d minutes"), length, (seconds / 60) % 60);

			this.statusbar.text = text.str;
		}

		private void cb_playlist_changed () {
			update_statusbar ();
			update_window_title ();
		}

		private void update_window_title () {
			string _title;

			if (this.mpdclient.get_artist () != "n/a" && this.mpdclient.get_title () != "n/a")
				_title = ("%s - %s").printf (this.mpdclient.get_artist (),
							     this.mpdclient.get_title ());
			else if (this.mpdclient.get_artist () == "n/a")
				_title = ("%s").printf (this.mpdclient.get_title ());
			else if (this.mpdclient.get_title () == "n/a")
				_title = ("%s").printf (this.mpdclient.get_artist ());
			else
				_title = Config.PACKAGE_NAME;

			if (this.mpdclient.is_playing ())
				title = _title + " - " + Config.PACKAGE_NAME;
			else if (this.mpdclient.is_paused ())
				title = "[" + _title + "] - " + Config.PACKAGE_NAME;
			else
				title = Config.PACKAGE_NAME;

			this.status_icon.set_tooltip_text (title);
		}

		private void cb_show_statusbar_changed (ParamSpec pspec) {
			var action = this.action_group.get_action ("view-statusbar");
			bool active = this.preferences.show_statusbar;

			((Gtk.ToggleAction) action).set_active (active);
			update_statusbar ();
		}
	}
}
