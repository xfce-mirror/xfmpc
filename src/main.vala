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

	static void transform_string_to_int (GLib.Value src, out GLib.Value dst) {
		dst.set_int ((int) (src.get_string ()).to_ulong ());
	}

	static void transform_string_to_boolean (GLib.Value src, out GLib.Value dst) {
		dst.set_boolean ((src.get_string ()).collate ("FALSE") != 0);
	}

	static void transform_string_to_enum (GLib.Value src, out GLib.Value dst) {
		GLib.EnumClass klass = (GLib.EnumClass) (dst.type ()).class_ref ();
		int i = 0;
		unowned EnumValue enum_value;

		while ((enum_value = klass.get_value (i)) != null)
		{
			if (GLib.strcmp ((enum_value.value_name).casefold (), (src.get_string ()).casefold ()) == 0)
				break;
			i ++;
		}

		dst.set_enum (enum_value.value);
  	}

	public static int main (string[] args) {
		Xfce.textdomain (Config.GETTEXT_PACKAGE, Config.PACKAGE_LOCALE_DIR, "UTF-8");

		Gtk.init (ref args);

  		GLib.Value.register_transform_func (typeof (string),
						    typeof (int),
						    (GLib.ValueTransform) transform_string_to_int);
  		GLib.Value.register_transform_func (typeof (string),
						    typeof (bool),
						    (GLib.ValueTransform) transform_string_to_boolean);
  		GLib.Value.register_transform_func (typeof (string),
						    typeof (Xfmpc.Preferences.SongFormat),
						    (GLib.ValueTransform) transform_string_to_enum);

		Xfmpc.MainWindow window = new Xfmpc.MainWindow ();
		window.show_all ();

		Gtk.main ();

		return 0;
	}
}

/* vi:set ts=8 sw=8: */
