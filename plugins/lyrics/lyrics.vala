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

	class LyricsPlugin : PanedPlugin {
		construct {
			name = _("Lyrics");
			icon_name = "";
			description = _("Display lyrics from the current playing song");
			version = "0.1";
			author = "Joe";
		}

		public override void create_paned_widget (Gtk.Bin bin) {
			var vbox = new Gtk.VBox (false, 2);
			var label = new Gtk.Label ("1");
			vbox.pack_start (label, true, true, 0);
			var label2 = new Gtk.Label ("2");
			vbox.pack_start (label2, true, true, 0);
			var label3 = new Gtk.Label ("3");
			vbox.pack_start (label3, true, true, 0);
			var label4 = new Gtk.Label ("4");
			vbox.pack_start (label4, true, true, 0);
			bin.add (vbox);
		}
	}

	[ModuleInit]
	public Type register_plugin () {
		return typeof (LyricsPlugin);
	}

}
