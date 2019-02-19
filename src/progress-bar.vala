/*
 *  Copyright (c) 2019 Johannes Marbach <n0-0ne+xfce@mailbox.org>
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

using Gdk;
using Gtk;

namespace Xfmpc {

	public class ProgressBar : Box {

		private Gtk.TreeView treeview;
		private Gtk.ListStore store;
		private Gtk.TreeIter iter;

		private enum Columns {
		  COLUMN_PERCENTAGE,
		  COLUMN_TEXT,
		  COLUMN_BACKGROUND,
		  N_COLUMNS
		}

		construct {
			set_orientation (Gtk.Orientation.VERTICAL);

			store = new Gtk.ListStore (Columns.N_COLUMNS,
						   typeof (double),
						   typeof (string),
						   typeof (Gdk.RGBA));

			store.append (out iter);

			treeview = new Gtk.TreeView ();
			treeview.set_headers_visible (false);
			treeview.set_model (store);
			treeview.get_selection ().set_mode (Gtk.SelectionMode.NONE);

			treeview.hierarchy_changed.connect (cb_hierarchy_changed);
			treeview.style_updated.connect (cb_style_updated);

			var renderer = new Gtk.CellRendererProgress ();
			renderer.height = 32;
			treeview.insert_column_with_attributes (-1, null, renderer,
								"value", Columns.COLUMN_PERCENTAGE,
								"text", Columns.COLUMN_TEXT,
								"cell-background-rgba", Columns.COLUMN_BACKGROUND, null);

			pack_start (treeview, true, true, 0);
		}

		public void set_text (string text) {
			store.set (iter,
				   Columns.COLUMN_TEXT, text,
				   -1);
		}

		public void set_fraction (double fraction) {
			store.set (iter,
				   Columns.COLUMN_PERCENTAGE, fraction * 100.0,
				   -1);
		}

		private void cb_hierarchy_changed (Widget widget, Widget? previous_toplevel) {
			update_background_color ();
		}

		private void cb_style_updated (Widget widget) {
			update_background_color ();
		}

		private void update_background_color () {
			var toplevel = get_toplevel ();
			if (!toplevel.get_type ().is_a (typeof (Gtk.Window))) {
				return;
			}

			var context = toplevel.get_style_context ();

			var value = context.get_property (Gtk.STYLE_PROPERTY_BACKGROUND_COLOR, Gtk.StateFlags.NORMAL);
			if (!value.holds (typeof (Gdk.RGBA))) {
				return;
			}

			store.set (iter,
				Columns.COLUMN_BACKGROUND, (Gdk.RGBA *) value.get_boxed (),
				-1);
		}

	}

}
