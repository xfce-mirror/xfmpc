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

	public abstract class BasePlugin : GLib.Object {
		public unowned string name { get; set; }
		public unowned string icon_name { get; set; }
		public unowned string description { get; set; }
		public unowned string version { get; set; }
		public unowned string author { get; set; }
		public unowned string title { get; set; }
	}

	public abstract class PanedPlugin : BasePlugin {
		public abstract void create_paned_widget (Gtk.Bin bin);
	}

}
