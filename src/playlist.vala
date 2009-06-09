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

	public class Playlist : VBox {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private ListStore store;
		private TreeModelFilter filter;
		private TreeView treeview;
		private Menu menu;
		private static Entry filter_entry;

		private ImageMenuItem mi_browse;
		private ImageMenuItem mi_information;

		private int current = 0;
		private bool autocenter;

		enum Columns {
  	  	  COLUMN_ID,
  	  	  COLUMN_FILENAME,
  	  	  COLUMN_POSITION,
  	  	  COLUMN_SONG,
  	  	  COLUMN_LENGTH,
  	  	  COLUMN_WEIGHT,
  	  	  N_COLUMNS,
		}

		construct {
			mpdclient = Xfmpc.Mpdclient.get ();
			preferences = Xfmpc.Preferences.get ();

			autocenter = preferences.playlist_autocenter;

			store = new ListStore (Columns.N_COLUMNS,
					       typeof (int),
					       typeof (string),
					       typeof (int),
					       typeof (string),
					       typeof (string),
					       typeof (int));

			filter = new TreeModelFilter (store, null);
			filter.set_visible_func ((TreeModelFilterVisibleFunc) visible_func_filter_tree);

			treeview = new TreeView ();
			(treeview.get_selection ()).set_mode (SelectionMode.MULTIPLE);
			treeview.set_rubber_banding (true);
  			treeview.set_enable_search (false);
			treeview.set_headers_visible (false);
			treeview.set_rules_hint (true);
			treeview.set_model (filter);

			var cell = new CellRendererText ();
			cell.xalign = 1;
			treeview.insert_column_with_attributes (-1, "Length", cell,
							        "text", Columns.COLUMN_POSITION,
							        "weight", Columns.COLUMN_WEIGHT,
							        null);
			cell = new CellRendererText ();
			cell.ellipsize = Pango.EllipsizeMode.END;
			var column = new TreeViewColumn.with_attributes ("Song", cell,
								"text", Columns.COLUMN_SONG,
								"weight", Columns.COLUMN_WEIGHT,
			        				null);
			column.expand = true;
			treeview.append_column (column);
			cell = new CellRendererText ();
			cell.xalign = 1;
			treeview.insert_column_with_attributes (-1, "Length", cell,
								"text", Columns.COLUMN_LENGTH,
								"weight", Columns.COLUMN_WEIGHT,
			        				null);

			var scrolled = new ScrolledWindow (new Adjustment (0, 0, 0, 0, 0, 0),
							   new Adjustment (0, 0, 0, 0, 0, 0));
			scrolled.set_policy (PolicyType.AUTOMATIC, PolicyType.ALWAYS);

			menu = new Menu ();

			var mi = new ImageMenuItem.from_stock (STOCK_REMOVE, null);
			menu.append (mi);
			mi.activate += delete_selection;
			mi_browse = new ImageMenuItem.with_mnemonic (_("Browse"));
			var image = new Image.from_stock (STOCK_OPEN, IconSize.MENU);
			mi_browse.set_image (image);
			menu.append (mi_browse);
			mi_browse.activate += cb_browse_selection;
			mi_information = new ImageMenuItem.from_stock (STOCK_INFO, null);
			menu.append (mi_information);
			mi_information.activate += cb_info_selection;

			menu.show_all ();

			filter_entry = new Entry ();

			scrolled.add (treeview);
			pack_start (scrolled, true, true, 0);
			pack_start (filter_entry, false, false, 0);

			/* Signals */
			mpdclient.song_changed += cb_song_changed;
			mpdclient.playlist_changed += cb_playlist_changed;

			treeview.row_activated += cb_row_activated;
			treeview.key_release_event += cb_key_released;
			treeview.button_press_event += cb_button_released;
			treeview.popup_menu += cb_popup_menu;

			filter_entry.activate += cb_filter_entry_activated;
			filter_entry.key_release_event += cb_filter_entry_key_released;
			filter_entry.changed += cb_filter_entry_changed;

			preferences.notify["song-format"] += cb_playlist_changed;
			preferences.notify["song-format-custom"] += cb_playlist_changed;
		}

		private static bool visible_func_filter_tree (TreeModel model, out TreeIter iter) {
  			string song = "", search = "";
			bool result = true;

			model.get (iter, Columns.COLUMN_SONG, &song, -1);
			if (song == "")
				return true;

			search = filter_entry.get_text ();
			search = search.casefold (-1);

			if (search != "") {
				song = song.casefold (-1);

				if (song.str (search) == null)
					result = false;
			}

			return result;
		}

		/*
		 * Signal callbacks
		 */

		private void cb_browse_selection () {
			unowned Xfmpc.Dbbrowser dbbrowser;
			unowned Xfmpc.ExtendedInterface extended_interface;

			dbbrowser = (Xfmpc.Dbbrowser) get_data ("XfmpcDbbrowser");
			extended_interface = (Xfmpc.ExtendedInterface) get_data ("XfmpcExtendedInterface");

			var selection = treeview.get_selection ();
			if (selection.count_selected_rows () > 1)
				return;

			var model = store;
			var list = selection.get_selected_rows (out model);
			if (list.length () == 0)
				return;

			TreeIter iter;
			var path = list.nth_data (0);
			if (store.get_iter (out iter, path)) {
				string filename = "", dir;
				store.get (iter, Columns.COLUMN_FILENAME, out filename, -1);
				dir = Path.get_dirname (filename);
				dbbrowser.set_wdir (dir);
				dbbrowser.reload ();
				extended_interface.set_active (ExtendedInterface.ExtendedInterfaceWidget.DBBROWSER);
			}
		}

		private void cb_info_selection () {
			TreeIter iter;
			int id = 0;

			var selection = treeview.get_selection ();
			if (selection.count_selected_rows () > 1)
				return;

			var model = store;
			var list = selection.get_selected_rows (out model);
			if (list.length () == 0)
				return;

			var path = list.nth_data (0);

			if (store.get_iter (out iter, path)) {
				store.get (iter, Columns.COLUMN_ID, out id, -1);
				var dialog = new SongDialog (id);
				dialog.show_all ();
			}
		}

		private bool cb_popup_menu () {
			menu_popup ();
			return true;
		}

		private void cb_song_changed () {
			refresh_current_song ();

			if (filter_entry.get_text () == "" && autocenter)
				select_row (current);
		}

		private void cb_playlist_changed () {
			string filename = "", song = "", length = "";
			int id = 0, pos = 0;

			current = mpdclient.get_id ();

			store.clear ();

			while (mpdclient.playlist_read (&id, &pos, &filename, &song, &length)) {
				append (id, pos, filename, song, length);
			}

			refresh_current_song ();

			if (filter_entry.get_text () != "" && autocenter)
				select_row (current);
		}

		private void cb_row_activated (TreePath path, TreeViewColumn column) {
			TreeIter iter;
			TreePath new_path = path;
			int id = 0;

			if (!filter.get_iter (out iter, new_path))
				return;

			filter.get (iter, Columns.COLUMN_ID, out id, -1);
			mpdclient.set_id (id);
		}

		private bool cb_key_released (Gdk.EventKey event) {
			if (event.type != Gdk.EventType.KEY_RELEASE)
				return false;

			/* Suppr key */
			switch (event.keyval)
			{
				case 0xffff:
					delete_selection ();
					break;
				default:
					return false;
			}

			return true;
		}

		private bool cb_button_released (Gdk.EventButton event) {
			TreePath path;
			TreeSelection selection;

			if (event.type != Gdk.EventType.BUTTON_PRESS || event.button != 3)
				return false;

			selection = treeview.get_selection ();
			if (selection.count_selected_rows () < 1)
				return true;

			bool sensitive = selection.count_selected_rows () == 1;
			mi_browse.set_sensitive (sensitive);
			mi_information.set_sensitive (sensitive);

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

		private void menu_popup () {
			menu.popup (null, null, null, 0, get_current_event_time ());
		}

		private void cb_filter_entry_activated () {
			var list = (treeview.get_selection ()).get_selected_rows (out filter);
			var path = list.nth_data (0);

			if (list.length () > 0) {
				treeview.row_activated (path, treeview.get_column (0));
				filter_entry.set_text ("");
				select_row (current);
				treeview.grab_focus ();
			}
		}

		private bool cb_filter_entry_key_released (Gdk.EventKey event) {
			if (event.type != Gdk.EventType.KEY_RELEASE)
				return false;

			/* Escape */
			if (event.keyval == 0xff1b) {
				filter_entry.set_text ("");
				select_row (current);
				treeview.grab_focus ();
			}
			else {
				if (filter_entry.get_text () != "")
					select_row (0);
				else
					select_row (current);
			}

			return true;
		}

		private void cb_filter_entry_changed () {
  			filter.refilter ();
		}

		/*
		 * Public
		 */

		public void append (int id, int pos, string filename, string song, string length) {
			TreeIter iter;

			store.append (out iter);
			store.set (iter,
				   Columns.COLUMN_ID, id,
				   Columns.COLUMN_FILENAME, filename,
				   Columns.COLUMN_POSITION, pos + 1,
				   Columns.COLUMN_SONG, song,
				   Columns.COLUMN_LENGTH, length,
				   Columns.COLUMN_WEIGHT, Pango.Weight.NORMAL,
				   -1);
		}

		public void refresh_current_song () {
			TreeIter iter;
			TreePath path;

			if (current < 0)
				current = 0;

			path = new TreePath.from_indices (current, -1);

			if (store.get_iter (out iter, path))
				store.set (iter, Columns.COLUMN_WEIGHT, Pango.Weight.NORMAL, -1);

			current = mpdclient.get_pos ();
			path = new TreePath.from_indices (current, -1);
			if (store.get_iter (out iter, path))
				store.set (iter, Columns.COLUMN_WEIGHT, Pango.Weight.BOLD, -1);
		}

		public void select_row (int i) {
			if (current < 0)
				return;
			if (filter.iter_n_children (null) == 0)
				return;

			TreePath path = new TreePath.from_indices (i, -1);
			treeview.set_cursor (path, null, false);
			treeview.scroll_to_cell (path, null, true, (float) 0.42, 0);
		}

		public void delete_selection () {
			int id = 0;
			TreeIter iter;
			var model = store;

			var list = (treeview.get_selection ()).get_selected_rows (out model);

			foreach (Gtk.TreePath path in list) {
				if (store.get_iter (out iter, path)) {
					store.get (iter, Columns.COLUMN_ID, out id, -1);
					mpdclient.queue_remove_id (id);
				}
			}

			mpdclient.queue_commit ();
		}
	}
}

/* vi:set ts=8 sw=8: */
