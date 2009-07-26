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

	public class MainWindow : Window {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private Xfmpc.Interface @interface;
		private Xfmpc.ExtendedInterface extended_interface;

		private Gtk.VBox vbox;
		private Gtk.ActionGroup action_group;
		private Gtk.UIManager ui_manager;
		private Xfmpc.Statusbar statusbar;

		private const Gtk.ToggleActionEntry[] toggle_action_entries = {
			{ "view-statusbar", null, "", null, null, action_statusbar, false }
		};

		private const Gtk.ActionEntry[] action_entries = {
			{ "quit", null, "", "<control>q", null, action_close },
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

  	  	  	/* Window */
			set_default_icon_name ("xfmpc");
			set_icon_name ("stock_volume");
			set_title (Config.PACKAGE_NAME);
			set_default_size (330, 330);
			this.delete_event.connect (cb_window_closed);
			this.window_state_event.connect (cb_window_state_event);

			this.vbox = new Gtk.VBox (false, 0);
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
			var separator = new Gtk.HSeparator ();
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
			this.mpdclient.playlist_changed.connect (cb_playlist_changed);
			this.preferences.notify["show-statusbar"].connect (cb_show_statusbar_changed);

  	  	  	/* === Timeout === */
			Timeout.add (1000, refresh);
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
			if ((bool) event.changed_mask & Gdk.WindowState.STICKY && this.visible){
				bool sticky;
				if (((bool) event.new_window_state & Gdk.WindowState.STICKY) == false)
					sticky = false;
				else
					sticky = true;

				this.preferences.last_window_state_sticky = sticky;
			}

			return false;
		}

		private bool cb_window_closed (Gdk.Event event) {
			close_window ();

			return false;
		}

		private void action_close () {
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

		private void action_statusbar (Action action) {
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
		}

		private void cb_show_statusbar_changed (ParamSpec pspec) {
			var action = this.action_group.get_action ("view-statusbar");
			bool active = this.preferences.show_statusbar;

			((Gtk.ToggleAction) action).set_active (active);
			update_statusbar ();
		}
	}
}

/* vi:set ts=8 sw=8: */
