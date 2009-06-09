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

	public class Dbbrowser : VBox {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private ListStore store;
		private TreeView treeview;
		private Menu menu;
		private Entry search_entry;
		private ImageMenuItem mi_browse;

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
			mpdclient = Xfmpc.Mpdclient.get ();
			preferences = Xfmpc.Preferences.get ();

			wdir = preferences.dbbrowser_last_path;
			last_wdir = wdir.ndup (wdir.len ());

			store = new ListStore (Columns.N_COLUMNS,
					       typeof (int),
					       typeof (Gdk.Pixbuf),
					       typeof (string),
					       typeof (string),
					       typeof (bool),
					       typeof (int));

			treeview = new TreeView ();
			(treeview.get_selection ()).set_mode (SelectionMode.MULTIPLE);
			treeview.set_rubber_banding (true);
  			treeview.set_enable_search (true);
			treeview.set_search_column (Columns.COLUMN_BASENAME);
			treeview.set_headers_visible (false);
			treeview.set_rules_hint (true);
			treeview.set_model (store);

			var cell_pixbuf = new CellRendererPixbuf ();
			treeview.insert_column_with_attributes (-1, "", cell_pixbuf,
								"pixbuf", Columns.COLUMN_PIXBUF,
								null);

			var cell_text = new CellRendererText ();
			cell_text.ellipsize = Pango.EllipsizeMode.END;
			treeview.insert_column_with_attributes (-1, "Filename", cell_text,
							        "text", Columns.COLUMN_BASENAME,
							        "weight", Columns.COLUMN_WEIGHT,
							        null);

			var scrolled = new ScrolledWindow (null, null);
			scrolled.set_policy (PolicyType.AUTOMATIC, PolicyType.ALWAYS);

			menu = new Menu ();

			var mi = new ImageMenuItem.from_stock (STOCK_ADD, null);
			menu.append (mi);
			mi.activate += add_selected_rows;
			mi = new ImageMenuItem.with_mnemonic (_("Replace"));
			var image = new Image.from_stock (STOCK_CUT, IconSize.MENU);
			mi.set_image (image);
			menu.append (mi);
			mi.activate += cb_replace_with_selected_rows;
			mi_browse = new ImageMenuItem.with_mnemonic (_("Browse"));
			image = new Image.from_stock (STOCK_OPEN, IconSize.MENU);
			mi_browse.set_image (image);
			menu.append (mi_browse);
			mi_browse.activate += cb_browse;

			menu.show_all ();

			search_entry = new Entry ();

			scrolled.add (treeview);
			pack_start (scrolled, true, true, 0);
			pack_start (search_entry, false, false, 0);

			/* Signals */
			mpdclient.connected += reload;
			mpdclient.database_changed += reload;
			mpdclient.playlist_changed += cb_playlist_changed;

			treeview.row_activated += cb_row_activated;
			treeview.key_press_event += cb_key_pressed;
			treeview.button_press_event += cb_button_released;
			treeview.popup_menu += cb_popup_menu;

			search_entry.activate += cb_search_entry_activated;
			search_entry.key_release_event += cb_search_entry_key_released;
			search_entry.changed += cb_search_entry_changed;

			preferences.notify["song-format"] += reload;
			preferences.notify["song-format-custom"] += reload;
		}

		public void free () {
		}

		public void reload () {
			string filename = "", basename = "";
			bool is_dir = false, is_bold = false;
			int i = 0;

			if (!mpdclient.is_connected ())
				return;
			if (is_searching)
				return;

			clear ();

			if (!wdir_is_root ()) {
				filename = get_parent_wdir ();
				append (filename, "..", true, false);
				i ++;
			}

			while (mpdclient.database_read (wdir, &filename, &basename, &is_dir)) {
				is_bold = mpdclient.playlist_has_filename (filename, is_dir);
				append (filename, basename, is_dir, is_bold);

				if (filename.collate (last_wdir) == 0) {
					var path = new TreePath.from_indices (i, -1);
					treeview.set_cursor (path, null, false);
					treeview.scroll_to_cell (path, null, true, (float) 0.10, 0);
					i = -1;
				}
				else
					i++;
			}
		}

		public bool wdir_is_root () {
			return (wdir == "");
		}

		public string get_parent_wdir () {
			string filename = wdir.str ("/");

			if (filename == null)
				return "";
			else
				return Path.get_dirname (wdir);
		}

		public void append (string filename, string basename, bool is_dir, bool is_bold) {
			TreeIter iter;

			var pixbuf = treeview.render_icon (is_dir ? STOCK_DIRECTORY : STOCK_FILE,
							   IconSize.MENU, null);

			store.append (out iter);
			store.set (iter,
				   Columns.COLUMN_PIXBUF, pixbuf,
				   Columns.COLUMN_FILENAME, filename,
				   Columns.COLUMN_BASENAME, basename,
				   Columns.COLUMN_IS_DIR, is_dir,
				   Columns.COLUMN_WEIGHT, is_bold ? Pango.Weight.BOLD : Pango.Weight.NORMAL,
				   -1);
		}

		public void set_wdir (string dir) {
			last_wdir = wdir;
			wdir = dir;
		}

		public void add_selected_rows () {
			TreeModel model = store;
			TreeIter iter;
			string filename = "";

			var list = (treeview.get_selection ()).get_selected_rows (out model);

			foreach (TreePath path in list) {
				if (model.get_iter (out iter, path)) {
					model.get (iter, Columns.COLUMN_FILENAME, out filename, -1);
					mpdclient.queue_add (filename);
				}
			}

			mpdclient.queue_commit ();
		}

		public void search (string query) {
			string filename = "", basename = "";
			bool is_bold;
			int i = 0;

			if (!mpdclient.is_connected ())
				return;

			is_searching = true;
			clear ();

			while (mpdclient.database_search (query, &filename, &basename)) {
				is_bold = mpdclient.playlist_has_filename (filename, false);
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
				search_entry.modify_base (StateType.NORMAL, color);
#endif
				search_entry.modify_bg (StateType.NORMAL, color);
				search_entry.modify_bg (StateType.SELECTED, color);
			}
			else if (no_result == no_result_buf && !no_result) {
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
				search_entry.modify_base (StateType.NORMAL, null);
#endif
				search_entry.modify_bg (StateType.NORMAL, null);
				search_entry.modify_bg (StateType.SELECTED, null);
			}

			if (i == 0) {
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
      				search_entry.modify_base (StateType.NORMAL, color);
#endif
				search_entry.modify_bg (StateType.NORMAL, color);
				search_entry.modify_bg (StateType.SELECTED, color);
			}
			else if (no_result) {
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
      				search_entry.modify_base (StateType.NORMAL, null);
#endif
				search_entry.modify_bg (StateType.NORMAL, null);
				search_entry.modify_bg (StateType.SELECTED, null);
			}
		}

		private void clear () {
  			store.clear ();
		}

		private void menu_popup () {
			var selection = treeview.get_selection ();
			int count = selection.count_selected_rows ();

			if (is_searching) {
				mi_browse.show ();
				mi_browse.set_sensitive (count == 1 ? true : false);
			}
			else
				mi_browse.hide ();

			menu.popup (null, null, null, 0, get_current_event_time ());
		}

		/*
		 * Signal callbacks
		 */

		private void cb_playlist_changed () {
			TreeModel model = store;
			TreeIter iter;
			string filename = "";
			bool is_bold = false, is_dir = false;

			if (!model.get_iter_first (out iter))
				return;

			do {
				model.get (iter,
					   Columns.COLUMN_FILENAME, out filename,
					   Columns.COLUMN_IS_DIR, out is_dir,
					   -1);

				is_bold = mpdclient.playlist_has_filename (filename, is_dir);
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
					var selection = treeview.get_selection ();
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

			TreePath path;
			var selection = treeview.get_selection ();
			if (selection.count_selected_rows () < 1)
				return true;

			if (treeview.get_path_at_pos ((int) event.x, (int) event.y,
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
			TreePath ppath = path;
			TreeModel model = store;
			TreeIter iter;
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
				mpdclient.queue_add (filename);
				mpdclient.queue_commit ();
			}
		}

		private void cb_replace_with_selected_rows () {
			mpdclient.queue_clear ();
			add_selected_rows ();
		}

		private void cb_browse () {
			TreeModel model = store;
			TreeIter iter;

			var selection = treeview.get_selection ();
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
			string entry_text = search_entry.get_text ();

			if (entry_text == "") {
				is_searching = false;
				reload ();

      	      	      	      	/* revert possible previous applied color */
#if MORE_FUNKY_COLOR_ON_SEARCH_ENTRY
				search_entry.modify_base (StateType.NORMAL, null);
#endif
				search_entry.modify_bg (StateType.NORMAL, null);
				search_entry.modify_bg (StateType.SELECTED, null);

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
			if (search_timeout > 0)
				Source.remove (search_timeout);

			search_timeout = Timeout.add_full (Priority.DEFAULT, 642, timeout_search);
		}

		private bool timeout_search () {
			cb_search_entry_activated ();
			return false;
		}
	}
}

/* vi:set ts=8 sw=8: */
