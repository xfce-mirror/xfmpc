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

namespace Xfmpc {

	public class PluginLoader : GLib.Object {

		private List<string> plugin_paths;
		private HashTable<string, Type> registered_types;

		private delegate Type RegisterPluginFunction ();

		construct {
			assert (Module.supported ());

			registered_types = new HashTable<string, Type> (GLib.str_hash, GLib.str_equal);

			try {
				string name;
				var dir = Dir.open (Config.PLUGINDIR, 0);
				while ((name = dir.read_name ()) != null) {
					if (!name.has_suffix (Module.SUFFIX))
						continue;

					string path = "%s/%s".printf (Config.PLUGINDIR, name);
					Module module = Module.open (path, ModuleFlags.BIND_LAZY);
					if (module == null)
						continue;

					void* function;
					module.symbol ("xfmpc_register_plugin", out function);
					if (function == null) {
						warning ("%s: Unable to lookup symbol xfmpc_register_function", path);
						continue;
					}

					plugin_paths.append (path);
				}
			}
			catch (Error e) {
			}
		}

		private static PluginLoader singleton;
		public static unowned PluginLoader get_default () {
			if (singleton == null) {
				singleton = new PluginLoader ();
				singleton.add_weak_pointer (&singleton);
			}
			else
				singleton.ref ();

			return singleton;
		}

		public List<string> list_plugins (GLib.Type type) {
			List<string> list = null;
			foreach (string path in plugin_paths) {
				Type plugin_type = get_plugin_type (path);
				if (plugin_type.parent () == type) {
					list.append (path);
				}
			}
			return list;
		}

		public Type get_plugin_type (string path) {
			Type type = registered_types.lookup (path);
			if (type != Type.INVALID)
				return type;

			Module module = Module.open (path, ModuleFlags.BIND_LAZY);
			if (module == null)
				return Type.INVALID;

			void* function;
			module.symbol ("xfmpc_register_plugin", out function);
			if (function == null)
				return Type.INVALID;

			RegisterPluginFunction register_plugin = (RegisterPluginFunction)function;
			type = register_plugin ();

			if (type == Type.INVALID)
				return Type.INVALID;

			module.make_resident (); // Avoid module from getting closed
			registered_types.insert (path, type);

			return type;
		}

		public Xfmpc.BasePlugin? new_object (string path) {
			Type type = get_plugin_type (path);
			if (type == Type.INVALID)
				return null;
			return (Xfmpc.BasePlugin)GLib.Object.new (type);
		}
	}
}
