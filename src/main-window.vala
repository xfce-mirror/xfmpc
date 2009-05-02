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

		private string gettext_package = Config.GETTEXT_PACKAGE;
		private string localedir = Config.PACKAGE_LOCALE_DIR;

		private VBox vbox;
		private ActionGroup action_group;
		private Widget statusbar;
		private UIManager ui_manager;

		private const ToggleActionEntry[] toggle_action_entries = {
			{ "view-statusbar", null, "", null, null, action_statusbar, false }
		};

		private const ActionEntry[] action_entries = {
			{ "quit", null, "", "<control>q", null, action_close },
			{ "previous", null, "", "<control>b", null, action_previous },
			{ "pp", null, "", "<control>p", null, action_pp },
			{ "stop", null, "", "<control>s", null, action_stop },
			{ "next", null, "", "<control>f", null, action_next },
			{ "volume", null, "", "<control>v", null, action_volume }
		};

		private const string main_ui = "<ui><accelerator action=\"quit\" /><accelerator action=\"previous\" /><accelerator action=\"pp\" /><accelerator action=\"stop\" /><accelerator action=\"next\" /><accelerator action=\"volume\" /></ui>";
		private const int main_ui_length = 187u;

		construct {
			Xfce.textdomain (gettext_package, localedir, "UTF-8");

			mpdclient = Xfmpc.Mpdclient.get ();
			preferences = Xfmpc.Preferences.get ();

  	  	  	/* Window */
			set_default_icon_name ("xfmpc");
			set_icon_name ("stock_volume");
			set_title (Config.PACKAGE_NAME);
			set_default_size (330, 330);
			this.delete_event += cb_window_closed;
			this.window_state_event += cb_window_state_event;

			vbox = new VBox (false, 0);
			add (vbox);

			if (preferences.last_window_posx != -1 && preferences.last_window_posy != -1)
				move (preferences.last_window_posx, preferences.last_window_posy);
			if (preferences.last_window_width != -1 && preferences.last_window_height != -1)
				set_default_size (preferences.last_window_width, preferences.last_window_height);
			if (preferences.last_window_state_sticky == true)
				stick ();

  	  	  	/* Interface */
			var interface = new Interface ();
			set_data ("XfmpcInterface", interface);
			vbox.pack_start (interface, false, false, 4);

  	  	  	/* Separator */
			var separator = new HSeparator ();
			vbox.pack_start (separator, false, false, 0);

  	  	  	/* ExtendedInterface */
			var extended_interface = new ExtendedInterface ();
			vbox.pack_start (extended_interface, true, true, 0);

  	  	  	/* Accelerators */
			ui_manager = new UIManager ();

  	  	  	/* Action group */
			action_group = new ActionGroup ("XfmpcMainWindow");
			action_group.add_actions (action_entries, this);
			action_group.add_toggle_actions (toggle_action_entries, this);
			ui_manager.insert_action_group (action_group, 0);
			try {
				ui_manager.add_ui_from_string (main_ui, main_ui_length);
			} catch (Error e) {
				warning (e.message);
			}

  	  	  	/* Accel group */
			var accel_group = ui_manager.get_accel_group ();
			add_accel_group (accel_group);

  	  	  	/* show-statusbar action */
			((ToggleAction )(action_group.get_action ("view-statusbar"))).set_active (preferences.show_statusbar);

  	  	  	/* === Signals === */
			mpdclient.playlist_changed += cb_playlist_changed;
			preferences.notify["show-statusbar"] += cb_show_statusbar_changed;
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

				preferences.last_window_state_sticky = sticky;
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

			preferences.last_window_posx = posx;
			preferences.last_window_posy = posy;
			preferences.last_window_width = width;
			preferences.last_window_height = height;

			main_quit ();
		}

		private void action_previous () {
			mpdclient.previous ();
		}

		private void action_pp () {
			Interface interface = (Interface) get_data ("XfmpcInterface");
			interface.pp_clicked ();
		}

		private void action_stop () {
			mpdclient.stop ();
		}

		private void action_next () {
			mpdclient.next ();
		}

		private void action_volume () {
			Interface interface = (Interface) get_data ("XfmpcInterface");
			interface.popup_volume ();
		}

		private void action_statusbar (Action action) {
			bool active = ((ToggleAction) action).get_active ();

			if (!active && statusbar != null) {
				statusbar.destroy ();
				statusbar = null;
			}
			else if (active && statusbar == null) {
				statusbar = new Statusbar ();
				statusbar.show ();
				vbox.pack_start (statusbar, false, false, 0);
			}
		}

		private void update_statusbar () {
			int seconds, length;

			if (statusbar == null)
				return;

			if (!mpdclient.is_connected ())
				return;

			length = mpdclient.playlist_get_length ();
			seconds = mpdclient.playlist_get_total_time ();

			StringBuilder text = new StringBuilder ();
			if (seconds / 3600 > 0)
				text.append_printf (_("%d songs, %d hours and %d minutes"), length, seconds / 3600, (seconds / 60) % 60);
			else
				text.append_printf (_("%d songs, %d minutes"), length, (seconds / 60) % 60);

			((Statusbar) statusbar).text = text.str;
		}

		private void cb_playlist_changed () {
			update_statusbar ();
		}

		private void cb_show_statusbar_changed (ParamSpec pspec) {
			var action = action_group.get_action ("view-statusbar");
			bool active = preferences.show_statusbar;

			((ToggleAction) action).set_active (active);
			update_statusbar ();
		}
	}
}

/* vi:set ts=8 sw=8: */
