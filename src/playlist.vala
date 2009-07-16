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

		private Gtk.ListStore store;
		private Gtk.TreeModelFilter filter;
		private Gtk.TreeView treeview;
		private Gtk.Menu menu;
		private static Gtk.Entry filter_entry;

		private Gtk.ImageMenuItem mi_browse;
		private Gtk.ImageMenuItem mi_information;

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
			this.mpdclient = Xfmpc.Mpdclient.get ();
			this.preferences = Xfmpc.Preferences.get ();

			this.autocenter = this.preferences.playlist_autocenter;

			this.store = new Gtk.ListStore (Columns.N_COLUMNS,
					                typeof (int),
					                typeof (string),
					                typeof (int),
					                typeof (string),
					                typeof (string),
					                typeof (int));

			this.filter = new Gtk.TreeModelFilter (this.store, null);
			this.filter.set_visible_func ((Gtk.TreeModelFilterVisibleFunc) visible_func_filter_tree);

			this.treeview = new Gtk.TreeView ();
			(this.treeview.get_selection ()).set_mode (Gtk.SelectionMode.MULTIPLE);
			this.treeview.set_rubber_banding (true);
  			this.treeview.set_enable_search (false);
			this.treeview.set_headers_visible (false);
			this.treeview.set_rules_hint (true);
			this.treeview.set_model (this.filter);

			var cell = new Gtk.CellRendererText ();
			cell.xalign = 1;
			this.treeview.insert_column_with_attributes (-1, "Length", cell,
							             "text", Columns.COLUMN_POSITION,
							             "weight", Columns.COLUMN_WEIGHT,
							             null);
			cell = new Gtk.CellRendererText ();
			cell.ellipsize = Pango.EllipsizeMode.END;
			var column = new Gtk.TreeViewColumn.with_attributes ("Song", cell,
								             "text", Columns.COLUMN_SONG,
								             "weight", Columns.COLUMN_WEIGHT,
			        				             null);
			column.expand = true;
			this.treeview.append_column (column);
			cell = new Gtk.CellRendererText ();
			cell.xalign = 1;
			this.treeview.insert_column_with_attributes (-1, "Length", cell,
								     "text", Columns.COLUMN_LENGTH,
								     "weight", Columns.COLUMN_WEIGHT,
			        				     null);

			var scrolled = new Gtk.ScrolledWindow (new Gtk.Adjustment (0, 0, 0, 0, 0, 0),
							       new Gtk.Adjustment (0, 0, 0, 0, 0, 0));
			scrolled.set_policy (Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.ALWAYS);

			this.menu = new Menu ();

			var mi = new Gtk.ImageMenuItem.from_stock (Gtk.STOCK_REMOVE, null);
			this.menu.append (mi);
			mi.activate.connect (delete_selection);
			this.mi_browse = new Gtk.ImageMenuItem.with_mnemonic (_("Browse"));
			var image = new Gtk.Image.from_stock (Gtk.STOCK_OPEN, Gtk.IconSize.MENU);
			this.mi_browse.set_image (image);
			this.menu.append (this.mi_browse);
			this.mi_browse.activate.connect (cb_browse_selection);
			this.mi_information = new Gtk.ImageMenuItem.from_stock (Gtk.STOCK_INFO, null);
			this.menu.append (mi_information);
			this.mi_information.activate.connect (cb_info_selection);

			this.menu.show_all ();

			this.filter_entry = new Entry ();

			scrolled.add (this.treeview);
			pack_start (scrolled, true, true, 0);
			pack_start (this.filter_entry, false, false, 0);

			/* Signals */
			this.mpdclient.song_changed.connect (cb_song_changed);
			this.mpdclient.playlist_changed.connect (cb_playlist_changed);

			this.treeview.row_activated.connect (cb_row_activated);
			this.treeview.key_release_event.connect (cb_key_released);
			this.treeview.button_press_event.connect (cb_button_released);
			this.treeview.popup_menu.connect (cb_popup_menu);

			this.filter_entry.activate.connect (cb_filter_entry_activated);
			this.filter_entry.key_release_event.connect (cb_filter_entry_key_released);
			this.filter_entry.changed.connect (cb_filter_entry_changed);

			this.preferences.notify["song-format"].connect (cb_playlist_changed);
			this.preferences.notify["song-format-custom"].connect (cb_playlist_changed);
		}

		private static bool visible_func_filter_tree (Gtk.TreeModel model, out Gtk.TreeIter iter) {
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

			var selection = this.treeview.get_selection ();
			if (selection.count_selected_rows () > 1)
				return;

			Gtk.TreeModel model = this.store;
			var list = selection.get_selected_rows (out model);
			if (list.length () == 0)
				return;

			Gtk.TreeIter iter;
			var path = list.nth_data (0);
			if (this.store.get_iter (out iter, path)) {
				string filename = "", dir;
				store.get (iter, Columns.COLUMN_FILENAME, out filename, -1);
				dir = GLib.Path.get_dirname (filename);
				dbbrowser.set_wdir (dir);
				dbbrowser.reload ();
				extended_interface.set_active (Xfmpc.ExtendedInterface.ExtendedInterfaceWidget.DBBROWSER);
			}
		}

		private void cb_info_selection () {
			Gtk.TreeIter iter;
			int id = 0;

			var selection = this.treeview.get_selection ();
			if (selection.count_selected_rows () > 1)
				return;

			Gtk.TreeModel model = this.store;
			var list = selection.get_selected_rows (out model);
			if (list.length () == 0)
				return;

			var path = list.nth_data (0);

			if (this.store.get_iter (out iter, path)) {
				this.store.get (iter, Columns.COLUMN_ID, out id, -1);
				var dialog = new Xfmpc.SongDialog (id);
				dialog.show_all ();
			}
		}

		private bool cb_popup_menu () {
			menu_popup ();
			return true;
		}

		private void cb_song_changed () {
			refresh_current_song ();

			if (filter_entry.get_text () == "" && this.autocenter)
				select_row (this.current);
		}

		private void cb_playlist_changed () {
			string filename = "", song = "", length = "";
			int id = 0, pos = 0;

			this.current = this.mpdclient.get_id ();

			this.store.clear ();

			while (this.mpdclient.playlist_read (&id, &pos, &filename, &song, &length)) {
				append (id, pos, filename, song, length);
			}

			refresh_current_song ();

			if (filter_entry.get_text () != "" && this.autocenter)
				select_row (this.current);
		}

		private void cb_row_activated (Gtk.TreePath path, Gtk.TreeViewColumn column) {
			Gtk.TreeIter iter;
			Gtk.TreePath new_path = path;
			int id = 0;

			if (!this.filter.get_iter (out iter, new_path))
				return;

			this.filter.get (iter, Columns.COLUMN_ID, out id, -1);
			this.mpdclient.set_id (id);
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
			Gtk.TreePath path;
			Gtk.TreeSelection selection;

			if (event.type != Gdk.EventType.BUTTON_PRESS || event.button != 3)
				return false;

			selection = this.treeview.get_selection ();
			if (selection.count_selected_rows () < 1)
				return true;

			bool sensitive = selection.count_selected_rows () == 1;
			this.mi_browse.set_sensitive (sensitive);
			this.mi_information.set_sensitive (sensitive);

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

		private void menu_popup () {
			menu.popup (null, null, null, 0, get_current_event_time ());
		}

		private void cb_filter_entry_activated () {
			Gtk.TreeModel model = filter;
			var list = (this.treeview.get_selection ()).get_selected_rows (out model);
			var path = list.nth_data (0);

			if (list.length () > 0) {
				this.treeview.row_activated (path, this.treeview.get_column (0));
				filter_entry.set_text ("");
				select_row (current);
				this.treeview.grab_focus ();
			}
		}

		private bool cb_filter_entry_key_released (Gdk.EventKey event) {
			if (event.type != Gdk.EventType.KEY_RELEASE)
				return false;

			/* Escape */
			if (event.keyval == 0xff1b) {
				filter_entry.set_text ("");
				select_row (current);
				this.treeview.grab_focus ();
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
  			this.filter.refilter ();
		}

		/*
		 * Public
		 */

		public void append (int id, int pos, string filename, string song, string length) {
			Gtk.TreeIter iter;

			this.store.append (out iter);
			this.store.set (iter,
				        Columns.COLUMN_ID, id,
				        Columns.COLUMN_FILENAME, filename,
				        Columns.COLUMN_POSITION, pos + 1,
				        Columns.COLUMN_SONG, song,
				        Columns.COLUMN_LENGTH, length,
				        Columns.COLUMN_WEIGHT, Pango.Weight.NORMAL,
				        -1);
		}

		public void refresh_current_song () {
			Gtk.TreeIter iter;
			Gtk.TreePath path;

			if (this.current < 0)
				this.current = 0;

			path = new Gtk.TreePath.from_indices (this.current, -1);

			if (this.store.get_iter (out iter, path))
				this.store.set (iter, Columns.COLUMN_WEIGHT, Pango.Weight.NORMAL, -1);

			this.current = this.mpdclient.get_pos ();
			path = new Gtk.TreePath.from_indices (this.current, -1);
			if (this.store.get_iter (out iter, path))
				this.store.set (iter, Columns.COLUMN_WEIGHT, Pango.Weight.BOLD, -1);
		}

		public void select_row (int i) {
			if (this.current < 0)
				return;
			if (this.filter.iter_n_children (null) == 0)
				return;

			Gtk.TreePath path = new Gtk.TreePath.from_indices (i, -1);
			this.treeview.set_cursor (path, null, false);
			this.treeview.scroll_to_cell (path, null, true, (float) 0.42, 0);
		}

		public void delete_selection () {
			int id = 0;
			Gtk.TreeIter iter;
			Gtk.TreeModel model = this.store;

			var list = (this.treeview.get_selection ()).get_selected_rows (out model);

			foreach (Gtk.TreePath path in list) {
				if (this.store.get_iter (out iter, path)) {
					this.store.get (iter, Columns.COLUMN_ID, out id, -1);
					this.mpdclient.queue_remove_id (id);
				}
			}

			this.mpdclient.queue_commit ();
		}

		public bool has_filename (string filename, bool is_dir) {
			Gtk.TreeIter iter;
			Gtk.TreePath path;
			string name = "";

			path = new Gtk.TreePath.from_indices (0, -1);
			while (this.store.get_iter (out iter, path)) {
				this.store.get (iter, Columns.COLUMN_FILENAME, out name, -1);

				if (is_dir) {
					if (name.has_prefix (filename))
						return true;
				} else {
					if (name == filename)
						return true;
				}

				path.next ();
			}

			return false;
		}
	}
}

/* vi:set ts=8 sw=8: */
