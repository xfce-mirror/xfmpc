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

	public class PanedInterface : VBox {

		private unowned Xfmpc.Preferences preferences;
		private unowned Xfmpc.PluginLoader plugin_loader;
		private List<string> plugins;

		private Gtk.Notebook notebook;

		public PanedInterface () {
			preferences = Xfmpc.Preferences.get ();
			plugin_loader = Xfmpc.PluginLoader.get_default ();

			notebook = new Gtk.Notebook ();
			notebook.show_border = false;
			//notebook.show_tabs = false;
			base.add (notebook);

			plugins = plugin_loader.list_plugins (typeof (Xfmpc.PanedPlugin));
			foreach (string path in plugins) {
				Xfmpc.PanedPlugin plugin = (Xfmpc.PanedPlugin)plugin_loader.new_object (path);
				debug ("Load plugin %s", plugin.name);
				var viewport = new Gtk.Viewport (null, null);
				plugin.create_paned_widget (viewport);
				add (viewport);
			}
		}

		public new void add (Gtk.Widget widget) {
			var scrolled_window = new Gtk.ScrolledWindow (null, null);
			scrolled_window.hscrollbar_policy = Gtk.PolicyType.AUTOMATIC;
			scrolled_window.vscrollbar_policy = Gtk.PolicyType.AUTOMATIC;
			scrolled_window.shadow_type = Gtk.ShadowType.NONE;
			scrolled_window.add (widget);
			notebook.add (scrolled_window);
			scrolled_window.show_all ();
		}

		public new void remove (Gtk.Widget widget) {
			var scrolled_window = widget.get_parent ();
			notebook.remove (scrolled_window);
			scrolled_window.destroy ();
		}

	}

}
