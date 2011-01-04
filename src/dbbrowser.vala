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

	public class Dbbrowser : VBox {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private Gtk.ListStore store;
		private Gtk.TreeView treeview;
		private Gtk.Menu menu;
		private Gtk.Entry search_entry;
		private Gtk.ImageMenuItem mi_browse;

		private string wdir;
		private string last_wdir;

		private bool is_searching;
		private uint search_timeout;

		private enum Columns {
  	  	  COLUMN_ID,
  	  	  COLUMN_PIXBUF,
  	  	  COLUMN_FILENAME,
  	  	  COLUMN_BASENAME,
  	  	  COLUMN_IS_DIR,
  	  	  COLUMN_WEIGHT,
  	  	  N_COLUMNS,
		}

		construct {
			this.mpdclient = Xfmpc.Mpdclient.get_default ();
			this.preferences = Xfmpc.Preferences.get_default ();

			this.wdir = preferences.dbbrowser_last_path;
			if (this.wdir != null)
				this.last_wdir = this.wdir.ndup (this.wdir.length);
			else
				this.last_wdir = "";

			this.store = new Gtk.ListStore (Columns.N_COLUMNS,
					       	    	typeof (int),
					       	    	typeof (Gdk.Pixbuf),
					       	    	typeof (string),
					       	    	typeof (string),
					       	    	typeof (bool),
					       	    	typeof (int));

			this.treeview = new Gtk.TreeView ();
			(this.treeview.get_selection ()).set_mode (Gtk.SelectionMode.MULTIPLE);
			this.treeview.set_rubber_banding (true);
  			this.treeview.set_enable_search (true);
			this.treeview.set_search_column (Columns.COLUMN_BASENAME);
			this.treeview.set_headers_visible (false);
			this.treeview.set_rules_hint (true);
			this.treeview.set_model (this.store);

			var cell_pixbuf = new Gtk.CellRendererPixbuf ();
			this.treeview.insert_column_with_attributes (-1, "", cell_pixbuf,
							             "pixbuf", Columns.COLUMN_PIXBUF,
								     null);

			var cell_text = new Gtk.CellRendererText ();
			cell_text.ellipsize = Pango.EllipsizeMode.END;
			this.treeview.insert_column_with_attributes (-1, "Filename", cell_text,
			                                             "text", Columns.COLUMN_BASENAME,
							             "weight", Columns.COLUMN_WEIGHT,
							             null);

			var scrolled = new Gtk.ScrolledWindow (null, null);
			scrolled.set_policy (Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.ALWAYS);

			this.menu = new Gtk.Menu ();

			var mi = new Gtk.ImageMenuItem.from_stock (Gtk.STOCK_ADD, null);
			this.menu.append (mi);
			mi.activate.connect (add_selected_rows);
			mi = new Gtk.ImageMenuItem.with_mnemonic (_("Replace"));
			var image = new Gtk.Image.from_stock (Gtk.STOCK_CUT, Gtk.IconSize.MENU);
			mi.set_image (image);
			this.menu.append (mi);
			mi.activate.connect (cb_replace_with_selected_rows);
			this.mi_browse = new Gtk.ImageMenuItem.with_mnemonic (_("Browse"));
			image = new Gtk.Image.from_stock (Gtk.STOCK_OPEN, Gtk.IconSize.MENU);
			this.mi_browse.set_image (image);
			this.menu.append (this.mi_browse);
			this.mi_browse.activate.connect (cb_browse);

			this.menu.show_all ();

			this.search_entry = new Entry ();

			scrolled.add (this.treeview);
			pack_start (scrolled, true, true, 0);
			pack_start (this.search_entry, false, false, 0);

			/* Signals */
			this.mpdclient.connected.connect (reload);
			this.mpdclient.database_changed.connect (reload);
			this.mpdclient.playlist_changed.connect (cb_playlist_changed);

			this.treeview.row_activated.connect (cb_row_activated);
			this.treeview.key_press_event.connect (cb_key_pressed);
			this.treeview.button_press_event.connect (cb_button_released);
			this.treeview.popup_menu.connect (cb_popup_menu);

			this.search_entry.activate.connect (cb_search_entry_activated);
			this.search_entry.key_release_event.connect (cb_search_entry_key_released);
			this.search_entry.changed.connect (cb_search_entry_changed);

			this.preferences.notify["song-format"].connect (reload);
			this.preferences.notify["song-format-custom"].connect (reload);
		}

		public void reload () {
			string filename = null, basename = null;
			bool is_dir = false, is_bold = false;
			int i = 0;
			Xfmpc.Playlist playlist = get_data ("XfmpcPlaylist");

			if (this.is_searching)
				return;

			clear ();
			if (!this.mpdclient.is_connected ())
				return;

			if (!this.wdir_is_root ()) {
				filename = get_parent_wdir ();
				append (filename, "..", true, false);
				i ++;
			}

			while (this.mpdclient.database_read (wdir, out filename, out basename, out is_dir)) {
				is_bold = playlist.has_filename (filename, is_dir);
				append (filename, basename, is_dir, is_bold);

				if (filename.collate (last_wdir) == 0) {
					var path = new TreePath.from_indices (i, -1);
					this.treeview.set_cursor (path, null, false);
					this.treeview.scroll_to_cell (path, null, true, (float) 0.10, 0);
					i = -1;
				}
				else
					i++;
			}
		}

		public bool wdir_is_root () {
			return (this.wdir == "");
		}

		public string get_parent_wdir () {
			string filename = wdir.str ("/");

			if (filename == null)
				return "";
			else
				return GLib.Path.get_dirname (wdir);
		}

		public void append (string filename, string basename, bool is_dir, bool is_bold) {
			Gtk.TreeIter iter;

			var pixbuf = this.treeview.render_icon (is_dir ? Gtk.STOCK_DIRECTORY : Gtk.STOCK_FILE,
					                        Gtk.IconSize.MENU, null);

			this.store.append (out iter);
			this.store.set (iter,
				   	Columns.COLUMN_PIXBUF, pixbuf,
				   	Columns.COLUMN_FILENAME, filename,
				   	Columns.COLUMN_BASENAME, basename,
				   	Columns.COLUMN_IS_DIR, is_dir,
				   	Columns.COLUMN_WEIGHT, is_bold ? Pango.Weight.BOLD : Pango.Weight.NORMAL,
				   	-1);
		}

		public void set_wdir (string dir) {
			this.last_wdir = this.wdir;
			this.wdir = dir;
		}

		public void add_selected_rows () {
			Gtk.TreeModel model = this.store;
			Gtk.TreeIter iter;
			string filename = "";

			var list = (this.treeview.get_selection ()).get_selected_rows (out model);

			foreach (Gtk.TreePath path in list) {
				if (model.get_iter (out iter, path)) {
					model.get (iter, Columns.COLUMN_FILENAME, out filename, -1);
					this.mpdclient.queue_add (filename);
				}
			}

			this.mpdclient.queue_commit ();
		}

		public void search (string query) {
			string filename = null, basename = null;
			bool is_bold;
			int i = 0;
			Xfmpc.Playlist playlist = get_data ("XfmpcPlaylist");

			if (!this.mpdclient.is_connected ())
				return;

			this.is_searching = true;
			clear ();

			while (this.mpdclient.database_search (query, out filename, out basename)) {
				is_bold = playlist.has_filename (filename, false);
				append (filename, basename, false, is_bold);
				i++;
			}

			bool no_result_buf = false, no_result = false;

			Gdk.Color color;
			Gdk.Color.parse ("white", out color);
			color.red = 0xFFFF;
			color.green = 0x6666;
			color.blue = 0x6666;

			if (i == 0)
				no_result = true;
			else if (no_result)
				no_result = false;

			if (no_result == no_result_buf && no_result) {
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
				this.search_entry.modify_base (Gtk.StateType.NORMAL, color);
#endif
				this.search_entry.modify_bg (Gtk.StateType.NORMAL, color);
				this.search_entry.modify_bg (Gtk.StateType.SELECTED, color);
			}
			else if (no_result == no_result_buf && !no_result) {
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
				this.search_entry.modify_base (Gtk.StateType.NORMAL, null);
#endif
				this.search_entry.modify_bg (Gtk.StateType.NORMAL, null);
				this.search_entry.modify_bg (Gtk.StateType.SELECTED, null);
			}

			if (i == 0) {
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
      				this.search_entry.modify_base (Gtk.StateType.NORMAL, color);
#endif
				this.search_entry.modify_bg (Gtk.StateType.NORMAL, color);
				this.search_entry.modify_bg (Gtk.StateType.SELECTED, color);
			}
			else if (no_result) {
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
      				this.search_entry.modify_base (Gtk.StateType.NORMAL, null);
#endif
				this.search_entry.modify_bg (Gtk.StateType.NORMAL, null);
				this.search_entry.modify_bg (Gtk.StateType.SELECTED, null);
			}
		}

		private void clear () {
  			this.store.clear ();
		}

		private void menu_popup () {
			var selection = this.treeview.get_selection ();
			int count = selection.count_selected_rows ();

			if (this.is_searching) {
				this.mi_browse.show ();
				this.mi_browse.set_sensitive (count == 1 ? true : false);
			}
			else
				this.mi_browse.hide ();

			this.menu.popup (null, null, null, 0, get_current_event_time ());
		}

		/*
		 * Signal callbacks
		 */

		private void cb_playlist_changed () {
			Gtk.TreeModel model = this.store;
			Gtk.TreeIter iter;
			string filename = "";
			bool is_bold = false, is_dir = false;
			Xfmpc.Playlist playlist = get_data ("XfmpcPlaylist");

			if (!model.get_iter_first (out iter))
				return;

			do {
				model.get (iter,
					   Columns.COLUMN_FILENAME, out filename,
					   Columns.COLUMN_IS_DIR, out is_dir,
					   -1);

				is_bold = playlist.has_filename (filename, is_dir);
				store.set (iter,
					   Columns.COLUMN_WEIGHT,
					   is_bold ? Pango.Weight.BOLD : Pango.Weight.NORMAL,
					   -1);
			} while (model.iter_next (ref iter));
		}

		private bool cb_popup_menu () {
			menu_popup ();
			return true;
		}

		private bool cb_key_pressed (Gdk.EventKey event) {
			if (event.type != Gdk.EventType.KEY_PRESS)
				return false;

			switch (event.keyval) {
				case 0xff0d:
					var selection = this.treeview.get_selection ();
					if (selection.count_selected_rows () > 1)
						add_selected_rows ();
					else
						return false;
					break;

				case 0xff08:
					string filename = get_parent_wdir ();
					set_wdir (filename);
					reload ();
					break;

				default:
					return false;
			}

			return true;
		}

		private bool cb_button_released (Gdk.EventButton event) {
			if (event.type != Gdk.EventType.BUTTON_PRESS || event.button != 3)
				return false;

			Gtk.TreePath path;
			var selection = this.treeview.get_selection ();
			if (selection.count_selected_rows () < 1)
				return true;

			if (this.treeview.get_path_at_pos ((int) event.x, (int) event.y,
						           out path, null, null, null))
			{
				if (!selection.path_is_selected (path)) {
					selection.unselect_all ();
					selection.select_path (path);
				}
			}

			menu_popup ();

			return true;
		}

		private void cb_row_activated (TreePath path, TreeViewColumn column) {
			Gtk.TreePath ppath = path;
			Gtk.TreeModel model = this.store;
			Gtk.TreeIter iter;
			string filename = "";
			bool is_dir = false;

			if (!model.get_iter (out iter, ppath))
				return;

			model.get (iter,
				   Columns.COLUMN_FILENAME, out filename,
				   Columns.COLUMN_IS_DIR, out is_dir,
				   -1);

			if (is_dir) {
				set_wdir (filename);
				reload ();
			} else {
				this.mpdclient.queue_add (filename);
				this.mpdclient.queue_commit ();
			}
		}

		private void cb_replace_with_selected_rows () {
			this.mpdclient.queue_clear ();
			add_selected_rows ();
		}

		private void cb_browse () {
			Gtk.TreeModel model = this.store;
			Gtk.TreeIter iter;

			var selection = this.treeview.get_selection ();
			if (selection.count_selected_rows () > 1)
				return;

			var list = selection.get_selected_rows (out model);
			var path = list.nth_data (0);
			if (model.get_iter (out iter, path)) {
				string filename;
				model.get (iter, Columns.COLUMN_FILENAME, out filename, -1);
				string dir = Path.get_dirname (filename);
				set_wdir (dir);
				reload ();
			}
		}

		private void cb_search_entry_activated () {
			string entry_text = this.search_entry.get_text ();

			if (entry_text == "") {
				this.is_searching = false;
				reload ();

      	      	      	      	/* revert possible previous applied color */
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
				this.search_entry.modify_base (Gtk.StateType.NORMAL, null);
#endif
				this.search_entry.modify_bg (Gtk.StateType.NORMAL, null);
				this.search_entry.modify_bg (Gtk.StateType.SELECTED, null);

				return;
			}

			search (entry_text);
		}

		private bool cb_search_entry_key_released (Gdk.EventKey event) {
			if (event.type != Gdk.EventType.KEY_RELEASE)
				return false;

			/* Escape */
			if (event.keyval == 0xff1b)
				search_entry.set_text ("");

			return true;
		}

		private void cb_search_entry_changed () {
			if (this.search_timeout > 0)
				GLib.Source.remove (search_timeout);

			this.search_timeout = GLib.Timeout.add_full (Priority.DEFAULT, 642, this.timeout_search);
		}

		private bool timeout_search () {
			cb_search_entry_activated ();
			return false;
		}
	}
}
