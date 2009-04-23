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

	public class ExtendedInterface : VBox {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private string gettext_package = Config.GETTEXT_PACKAGE;
		private string localedir = Config.PACKAGE_LOCALE_DIR;

		private static Xfce.ArrowButton context_button;
		private ListStore list_store;
		private ComboBox combobox;
		private Notebook notebook;
		private Menu context_menu;

		private enum Columns {
  	  	  COLUMN_STRING,
  	  	  COLUMN_POINTER,
  	  	  N_COLUMNS,
		}

		public enum ExtendedInterfaceWidget {
  	  	  PLAYLIST,
  	  	  DBBROWSER,
		}

		construct {
			Xfce.textdomain (gettext_package, localedir, "UTF-8");

			mpdclient = Xfmpc.Mpdclient.get ();
			preferences = Xfmpc.Preferences.get ();

			var hbox = new HBox (false, 2);
			pack_start (hbox, false, false, 2);

			var button = new Button ();
			button.set_tooltip_text (_("Clear Playlist"));
			button.clicked += cb_playlist_clear;
			hbox.pack_start (button, false, false, 0);

			var image = new Image.from_stock (STOCK_NEW, IconSize.MENU);
			button.set_image (image);

			button = new Button ();
			button.set_tooltip_text (_("Refresh Database"));
			button.clicked += cb_database_refresh;
			hbox.pack_start (button, false, false, 0);

			image = new Image.from_stock (STOCK_REFRESH, IconSize.MENU);
			button.set_image (image);

			context_button = new Xfce.ArrowButton (ArrowType.DOWN);
			((Widget) context_button).set_tooltip_text (_("Context Menu"));
			((Button) context_button).pressed += popup_context_menu;
			((Button) context_button).clicked += cb_context_menu_clicked;
			hbox.pack_start (((Widget) context_button), false, false, 0);

			list_store = new ListStore (Columns.N_COLUMNS,
						    typeof (string),
						    typeof (void *));

			combobox = new ComboBox.with_model (list_store);
			hbox.pack_start (combobox, true, true, 0);
			combobox.changed += cb_interface_changed;

			var cell = new CellRendererText ();
			combobox.pack_start (cell, true);
			combobox.set_attributes (cell, "text", Columns.COLUMN_STRING, null);

			notebook = new Notebook ();
			notebook.set_show_tabs (false);
			pack_start (notebook, true, true, 0);

			Widget playlist = (Widget) new Playlist ();
			append_child (playlist, _("Current Playlist"));

			Widget dbbrowser = (Widget) new Dbbrowser ();
			append_child (dbbrowser, _("Browse database"));

			playlist.set_data ("XfmpcDbbrowser", dbbrowser);
			playlist.set_data ("XfmpcExtendedInterface", this);
		}

		public void set_active (ExtendedInterfaceWidget active_widget) {
			combobox.set_active (active_widget);
		}

		private void append_child (Widget child, string title) {
			TreeIter iter;

			list_store.append (out iter);
			list_store.set (iter,
					Columns.COLUMN_STRING, title,
					Columns.COLUMN_POINTER, child,
					-1);

			if (combobox.get_active () == -1)
				combobox.set_active (0);

			notebook.append_page (child, null);
			notebook.set_tab_label_packing (child, true, true, PackType.START);
		}

		private void popup_context_menu () {
			if (context_menu == null)
				context_menu_new ((Widget) context_button);

			context_menu.popup (null, null,
					    (MenuPositionFunc) position_context_menu,
					    0, get_current_event_time ());
		}

		private static void position_context_menu (Menu menu, int x, int y, bool push_in) {
			Requisition menu_req;
			int root_x;
			int root_y;

			menu.size_request (out menu_req);
			(((Widget) context_button).window).get_origin (out root_x, out root_y);

			x = root_x + (((Widget) context_button).allocation).x;
			y = root_y + (((Widget) context_button).allocation).y;

			if (y > Gdk.Screen.height () - menu_req.height)
				y = Gdk.Screen.height () - menu_req.height;
			else if (y < 0)
				y = 0;

			push_in = false;
		}

		private void context_menu_new (Widget attach_widget) {
			context_menu = new Menu ();
			context_menu.set_screen (attach_widget.get_screen ());
			context_menu.attach_to_widget (attach_widget, (MenuDetachFunc) menu_detach);
			context_menu.deactivate += cb_context_menu_deactivate;

			var mi = new CheckMenuItem.with_label (_("Repeat"));
			mi.set_active (mpdclient.get_repeat ());
			mi.activate += cb_repeat_switch;
			context_menu.append (mi);

			mi = new CheckMenuItem.with_label (_("Random"));
			mi.set_active (mpdclient.get_random ());
			mi.activate += cb_random_switch;
			context_menu.append (mi);

			var separator = new SeparatorMenuItem ();
			context_menu.append (separator);

			var imi = new ImageMenuItem.from_stock (STOCK_PREFERENCES, null);
			imi.activate += cb_preferences;
			context_menu.append (imi);

			imi = new ImageMenuItem.from_stock (STOCK_ABOUT, null);
			imi.activate += cb_about;
			context_menu.append (imi);

			context_menu.show_all ();
		}

		private void menu_detach (Widget attach_widget, Menu menu) {
		}

		/*
		 * Signal callbacks
		 */

		private void cb_playlist_clear () {
			mpdclient.playlist_clear ();
		}

		private void cb_database_refresh () {
			mpdclient.database_refresh ();
		}

		private void cb_interface_changed () {
			void *child = null;
			TreeIter iter;
			int i = 0;

			if (combobox.get_active_iter (out iter) == false)
				return;

			((TreeModel) list_store).get (iter, Columns.COLUMN_POINTER, out child, -1);

			i = notebook.page_num ((Widget) child);
			notebook.set_current_page (i);
		}

		private void cb_context_menu_clicked () {
			if (!((ToggleButton) context_button).get_active ())
				return;

			popup_context_menu ();
		}

		private void cb_context_menu_deactivate () {
			((ToggleButton) context_button).set_active (false);
		}

		private void cb_repeat_switch () {
			mpdclient.set_repeat (!mpdclient.get_repeat ());
		}

		private void cb_random_switch () {
			mpdclient.set_random (!mpdclient.get_random ());
		}

		private void cb_preferences () {
			var dialog = new PreferencesDialog ();
			dialog.show ();
		}

		private void cb_about () {
			string[] artists = { null };
			string[] authors = {"Mike Massonnet <mmassonnet@xfce.org>",
					   "Vincent Legout <vincent@xfce.org>"};
			string[] documenters = { null };

  	  	  	show_about_dialog (((Window) ((Widget) this).get_toplevel ()),
					   "artists", artists,
					   "authors", authors,
					   "comments", _("MPD client written in GTK+ for Xfce"),
					   "copyright", "Copyright \302\251 2008-2009 Mike Massonnet, Vincent Legout",
                         	 	   "documenters", documenters,
                         	 	   "license", Xfce.get_license_text (Xfce.LicenseTextType.GPL),
                         	 	   "translator-credits", _("translator-credits"),
                         	 	   "version", Config.PACKAGE_VERSION,
                         	 	   "website", "http://goodies.xfce.org/projects/applications/xfmpc",
					   null);
		}
	}
}

/* vi:set ts=8 sw=8: */
