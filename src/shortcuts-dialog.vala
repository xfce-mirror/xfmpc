/*
 *  Copyright (c) 2011 Mike Massonnet <mmassonnet@xfce.org>
 *  Copyright (c) 2011 Vincent Legout <vincent@xfce.org>
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

	public class ShortcutsDialog : Xfce.TitledDialog {

		construct {
			this.skip_taskbar_hint = true;
			this.icon_name = "multimedia-player";
			this.resizable = false;
			this.title = _("Xfmpc Shortcuts");
			this.subtitle = _("Control your MPD client with your keyboard");

			Gtk.Dialog dialog = this;
			dialog.add_button (_("Close"), Gtk.ResponseType.CLOSE);

			this.response.connect ((response) => {
				switch (response) {
					case Gtk.ResponseType.CLOSE:
						destroy ();
						break;
				}
			});

			var content_area = (Gtk.Box)get_content_area ();

			var vbox = new Gtk.Box (Gtk.Orientation.VERTICAL, 6);
			vbox.set_homogeneous (true);
			vbox.set_border_width (6);
			content_area.add (vbox);

			/* Shortcuts labels */
			var label = new Gtk.Label (_("Quit: CTRL+q"));
			label.xalign = 0.0f;
			label.yalign = 0.5f;
			vbox.add (label);

			label = new Gtk.Label (_("Previous: CTRL+b"));
			label.xalign = 0.0f;
			label.yalign = 0.5f;
			vbox.add (label);

			label = new Gtk.Label (_("Play/Pause: CTRL+p"));
			label.xalign = 0.0f;
			label.yalign = 0.5f;
			vbox.add (label);

			label = new Gtk.Label (_("Stop: CTRL+s"));
			label.xalign = 0.0f;
			label.yalign = 0.5f;
			vbox.add (label);

			label = new Gtk.Label (_("Next: CTRL+f"));
			label.xalign = 0.0f;
			label.yalign = 0.5f;
			vbox.add (label);

			label = new Gtk.Label (_("Volume: CTRL+v"));
			label.xalign = 0.0f;
			label.yalign = 0.5f;
			vbox.add (label);

			show_all ();
		}
	}
}
