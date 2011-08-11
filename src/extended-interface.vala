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

	public class ExtendedInterface : VBox {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private static Xfce.ArrowButton context_button;
		private Gtk.ListStore list_store;
		private Gtk.ComboBox combobox;
		private Gtk.Notebook notebook;
		private Gtk.Menu context_menu;

		private Gtk.CheckMenuItem random;
		private Gtk.CheckMenuItem repeat;
		private Gtk.CheckMenuItem single;
		private Gtk.CheckMenuItem consume;

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
			this.mpdclient = Xfmpc.Mpdclient.get_default ();
			this.preferences = Xfmpc.Preferences.get_default ();

			var hbox = new Gtk.HBox (false, 2);
			pack_start (hbox, false, false, 2);

			var button = new Gtk.Button ();
			button.set_tooltip_text (_("Clear Playlist"));
			button.clicked.connect (cb_playlist_clear);
			hbox.pack_start (button, false, false, 0);

			var image = new Gtk.Image.from_stock (Gtk.STOCK_NEW, Gtk.IconSize.MENU);
			button.set_image (image);

			button = new Gtk.Button ();
			button.set_tooltip_text (_("Refresh Database"));
			button.clicked.connect (cb_database_refresh);
			hbox.pack_start (button, false, false, 0);

			image = new Gtk.Image.from_stock (Gtk.STOCK_REFRESH, Gtk.IconSize.MENU);
			button.set_image (image);

			this.context_button = new Xfce.ArrowButton (Gtk.ArrowType.DOWN);
			((Widget) this.context_button).set_tooltip_text (_("Context Menu"));
			((Button) this.context_button).pressed.connect (popup_context_menu);
			((Button) this.context_button).clicked.connect (cb_context_menu_clicked);
			hbox.pack_start (((Widget) this.context_button), false, false, 0);

			this.list_store = new Gtk.ListStore (Columns.N_COLUMNS,
						  	     typeof (string),
						             typeof (void *));

			this.combobox = new Gtk.ComboBox.with_model (this.list_store);
			hbox.pack_start (combobox, true, true, 0);
			this.combobox.changed.connect (cb_interface_changed);

			var cell = new Gtk.CellRendererText ();
			this.combobox.pack_start (cell, true);
			this.combobox.set_attributes (cell, "text", Columns.COLUMN_STRING, null);

			this.notebook = new Gtk.Notebook ();
			this.notebook.set_show_tabs (false);
			pack_start (this.notebook, true, true, 0);

			Gtk.Widget playlist = (Gtk.Widget) new Xfmpc.Playlist ();
			append_child (playlist, _("Current Playlist"));

			Gtk.Widget dbbrowser = (Gtk.Widget) new Xfmpc.Dbbrowser ();
			append_child (dbbrowser, _("Browse database"));

			playlist.set_data ("XfmpcDbbrowser", dbbrowser);
			dbbrowser.set_data ("XfmpcPlaylist", playlist);
			playlist.set_data ("XfmpcExtendedInterface", this);
		}

		public void set_active (ExtendedInterfaceWidget active_widget) {
			this.combobox.set_active (active_widget);
		}

		private void append_child (Gtk.Widget child, string title) {
			Gtk.TreeIter iter;

			this.list_store.append (out iter);
			this.list_store.set (iter,
				             Columns.COLUMN_STRING, title,
					     Columns.COLUMN_POINTER, child,
					     -1);

			if (this.combobox.get_active () == -1)
				this.combobox.set_active (0);

			this.notebook.append_page (child, null);
			this.notebook.set_tab_label_packing (child, true, true, Gtk.PackType.START);
		}

		private void popup_context_menu () {
			if (this.context_menu == null)
				this.context_menu_new ((Gtk.Widget) this.context_button);

			this.repeat.set_active (this.mpdclient.get_repeat ());
			this.random.set_active (this.mpdclient.get_random ());
			this.single.set_active (this.mpdclient.get_single ());
			this.consume.set_active (this.mpdclient.get_consume ());

			this.context_menu.popup (null, null,
					    (Gtk.MenuPositionFunc) this.position_context_menu,
					    0, get_current_event_time ());
		}

		private static void position_context_menu (Gtk.Menu menu, int x, int y, bool push_in) {
			Gtk.Requisition menu_req;
			int root_x;
			int root_y;

			menu.size_request (out menu_req);
			(((Gtk.Widget) context_button).window).get_origin (out root_x, out root_y);

			x = root_x + (((Gtk.Widget) context_button).allocation).x;
			y = root_y + (((Gtk.Widget) context_button).allocation).y;

			if (y > Gdk.Screen.height () - menu_req.height)
				y = Gdk.Screen.height () - menu_req.height;
			else if (y < 0)
				y = 0;

			push_in = false;
		}

		private void context_menu_new (Gtk.Widget attach_widget) {
			this.context_menu = new Gtk.Menu ();
			this.context_menu.set_screen (attach_widget.get_screen ());
			this.context_menu.attach_to_widget (attach_widget, (Gtk.MenuDetachFunc) menu_detach);
			this.context_menu.deactivate.connect (cb_context_menu_deactivate);

			this.repeat = new Gtk.CheckMenuItem.with_label (_("Repeat"));
			this.repeat.activate.connect (cb_repeat_switch);
			this.context_menu.append (this.repeat);

			this.random = new Gtk.CheckMenuItem.with_label (_("Random"));
			this.random.activate.connect (cb_random_switch);
			this.context_menu.append (this.random);

			this.single = new Gtk.CheckMenuItem.with_label (_("Single Mode"));
			this.single.activate.connect (cb_single_switch);
			this.context_menu.append (this.single);

			this.consume = new Gtk.CheckMenuItem.with_label (_("Consume Mode"));
			this.consume.toggled.connect (cb_consume_switch);
			this.context_menu.append (this.consume);

			var separator = new Gtk.SeparatorMenuItem ();
			this.context_menu.append (separator);

			var imi = new Gtk.ImageMenuItem.from_stock (Gtk.STOCK_PREFERENCES, null);
			imi.activate.connect (cb_preferences);
			this.context_menu.append (imi);

			imi = new Gtk.ImageMenuItem.from_stock (Gtk.STOCK_ABOUT, null);
			imi.activate.connect (cb_about);
			this.context_menu.append (imi);

			this.context_menu.show_all ();
		}

		private void menu_detach (Widget attach_widget, Menu menu) {
		}

		/*
		 * Signal callbacks
		 */

		private void cb_playlist_clear () {
			this.mpdclient.playlist_clear ();
		}

		private void cb_database_refresh () {
			this.mpdclient.database_refresh ();
		}

		private void cb_interface_changed () {
			void *child = null;
			Gtk.TreeIter iter;
			int i = 0;

			if (this.combobox.get_active_iter (out iter) == false)
				return;

			((Gtk.TreeModel) this.list_store).get (iter, Columns.COLUMN_POINTER, out child, -1);

			i = this.notebook.page_num ((Gtk.Widget) child);
			this.notebook.set_current_page (i);
		}

		private void cb_context_menu_clicked () {
			if (!((Gtk.ToggleButton) this.context_button).get_active ())
				return;

			popup_context_menu ();
		}

		private void cb_context_menu_deactivate () {
			((Gtk.ToggleButton) this.context_button).set_active (false);
		}

		private void cb_repeat_switch () {
			this.mpdclient.set_repeat (this.repeat.get_active ());
		}

		private void cb_random_switch () {
			this.mpdclient.set_random (this.random.get_active ());
		}

		private void cb_single_switch () {
			this.mpdclient.set_single (this.single.get_active ());
		}

		private void cb_consume_switch () {
			this.mpdclient.set_consume (this.consume.get_active ());
		}

		private void cb_preferences () {
			var dialog = new Xfmpc.PreferencesDialog ();
			dialog.show ();
		}

		private void cb_about () {
			string[] authors = {"Mike Massonnet <mmassonnet@xfce.org>",
					   "Vincent Legout <vincent@xfce.org>"};

  	  	  	Gtk.show_about_dialog (((Gtk.Window) ((Gtk.Widget) this).get_toplevel ()),
					       "artists", null,
					       "authors", authors,
					       "comments", _("MPD client written in GTK+ for Xfce"),
					       "copyright", "Copyright \xc2\xa9 2008-2011 Mike Massonnet, Vincent Legout",
                         	 	       "documenters", null,
                         	 	       "license", Xfce.get_license_text (Xfce.LicenseTextType.GPL),
                         	 	       "translator-credits", _("translator-credits"),
                         	 	       "version", Config.PACKAGE_VERSION,
                         	 	       "website", "http://goodies.xfce.org/projects/applications/xfmpc",
					       null);
		}
	}
}
