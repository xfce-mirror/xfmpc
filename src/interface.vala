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

	public class Interface : VBox {

		private unowned Xfmpc.Mpdclient mpdclient;
		private unowned Xfmpc.Preferences preferences;

		private Gtk.Button button_prev;
		private Gtk.Button button_pp;
		private Gtk.Button button_next;
		private Gtk.VolumeButton button_volume;
		private Gtk.ProgressBar progress_bar;
		private Gtk.Label title;
		private Gtk.Label subtitle;
		private bool refresh_title;

		construct {
			this.mpdclient = Xfmpc.Mpdclient.get ();
			this.preferences = Xfmpc.Preferences.get ();

			set_border_width (4);

			var image = new Gtk.Image.from_stock (Gtk.STOCK_MEDIA_PREVIOUS, Gtk.IconSize.BUTTON);
			this.button_prev = new Gtk.Button ();
			this.button_prev.set_relief (Gtk.ReliefStyle.NONE);
			this.button_prev.add (image);

			image = new Gtk.Image.from_stock (Gtk.STOCK_MEDIA_PLAY, Gtk.IconSize.BUTTON);
			this.button_pp = new Gtk.Button ();
			this.button_pp.set_relief (Gtk.ReliefStyle.NONE);
			this.button_pp.add (image);

			image = new Gtk.Image.from_stock (Gtk.STOCK_MEDIA_NEXT, Gtk.IconSize.BUTTON);
			this.button_next = new Gtk.Button ();
			this.button_next.set_relief (Gtk.ReliefStyle.NONE);
			this.button_next.add (image);

			this.button_volume = new Gtk.VolumeButton ();
			this.button_volume.set_relief (Gtk.ReliefStyle.NONE);
			var adjustment = button_volume.get_adjustment ();
			adjustment.upper *= 100;
			adjustment.step_increment *= 100;
			adjustment.page_increment *= 100;

			var progress_box = new Gtk.EventBox ();
			progress_bar = new Gtk.ProgressBar ();
			progress_bar.set_text ("0:00 / 0:00");
			progress_bar.set_fraction (1.0);
			progress_box.add (progress_bar);

  	  	  	/* Title */
			var attrs = new Pango.AttrList ();
			unowned Pango.Attribute attr = Pango.attr_weight_new (Pango.Weight.BOLD);
			attr.start_index = 0;
			attr.end_index = -1;
			attrs.insert (attr);

			attr = Pango.attr_scale_new ((double) Pango.Scale.X_LARGE);
			attr.start_index = 0;
			attr.end_index = -1;
			attrs.insert (attr);

			title = new Gtk.Label (_("Not connected"));
			title.set_attributes (attrs);
			title.set_selectable (true);
			title.set_ellipsize (Pango.EllipsizeMode.END);
			title.set_alignment (0, (float) 0.5);

  	  	  	/* Subtitle */
			attrs = new Pango.AttrList ();
			attr = Pango.attr_scale_new ((double) Pango.Scale.SMALL);
			attr.start_index = 0;
			attr.end_index = -1;
			attrs.insert (attr);

			this.subtitle = new Gtk.Label (Config.PACKAGE_STRING);
			this.subtitle.set_attributes (attrs);
			this.subtitle.set_selectable (true);
			this.subtitle.set_ellipsize (Pango.EllipsizeMode.END);
			this.subtitle.set_alignment (0, (float) 0.5);

  	  	  	/* === Containers === */
			var box = new Gtk.HBox (false, 0);
			pack_start (box, false, false, 0);
			box.pack_start (this.button_prev, false, false, 0);
			box.pack_start (this.button_pp, false, false, 0);
			box.pack_start (this.button_next, false, false, 0);
			box.pack_start (progress_box, true, true, 4);
			box.pack_start (this.button_volume, false, false, 0);

			var vbox = new Gtk.VBox (false, 0);
			pack_start (vbox, false, true, 0);
			vbox.add (this.title);
			vbox.add (this.subtitle);

  	  	  	/* === Signals === */
			this.button_prev.clicked += cb_mpdclient_previous;
			this.button_pp.clicked += pp_clicked;
			this.button_next.clicked += cb_mpdclient_next;
			this.button_volume.value_changed += volume_changed;
			progress_box.button_release_event += cb_progress_box_release_event;

			this.mpdclient.connected += reconnect;
			this.mpdclient.song_changed += cb_song_changed;
			this.mpdclient.pp_changed += cb_pp_changed;
			this.mpdclient.time_changed += cb_time_changed;
			this.mpdclient.volume_changed += cb_volume_changed;
			this.mpdclient.stopped += cb_stopped;

  	  	  	/* === Timeout === */
			Timeout.add (1000, refresh);
		}

		public void set_title (string title) {
			this.title.set_text (title);
		}

		public void set_subtitle (string subtitle) {
			this.subtitle.set_text (subtitle);
		}

		public void pp_clicked () {
			if (!this.mpdclient.pp ())
				return;
			set_pp (this.mpdclient.is_playing ());
		}

		public void set_pp (bool play) {
			var image = (Gtk.Image) this.button_pp.get_child ();

			if (play == true)
				image.set_from_stock (Gtk.STOCK_MEDIA_PAUSE, Gtk.IconSize.BUTTON);
			else
				image.set_from_stock (Gtk.STOCK_MEDIA_PLAY, Gtk.IconSize.BUTTON);
		}

		private bool cb_progress_box_release_event (Gdk.EventButton event) {
			if (event.type != Gdk.EventType.BUTTON_RELEASE || event.button != 1)
				return false;

			int time_total = this.mpdclient.get_total_time ();
			if (time_total < 0)
				return false;

			double time = event.x / this.progress_bar.allocation.width;
			time *= time_total;

			this.mpdclient.set_song_time ((int) time);

			return true;
		}

		public void volume_changed (double value) {
			this.mpdclient.set_volume ((char) value);
		}

		public void set_volume (int volume) {
			this.button_volume.set_value (volume);
		}

		public void popup_volume () {
			GLib.Signal.emit_by_name (this.button_volume, "popup", null);
		}

		public void set_time (int time, int time_total) {
			int min, sec, min_total, sec_total;
			double fraction = 1.0;

			min = time / 60;
			sec = time % 60;

  	  	  	min_total = time_total / 60;
  	  	  	sec_total = time_total % 60;

			GLib.StringBuilder text = new GLib.StringBuilder ();
			text.append_printf ("%d:%02d / %d:%02d", min, sec, min_total, sec_total);
			this.progress_bar.set_text (text.str);

			if (time_total > 0)
				fraction = (float)time / (float)time_total;

			this.progress_bar.set_fraction ((fraction <= 1.0) ? fraction : 1.0);
		}

		private bool refresh () {
			if (this.mpdclient.connect () == false) {
				warning ("Failed to connect to MPD");
				this.mpdclient.disconnect ();

				set_pp (false);
				set_time (0, 0);
				set_volume (0);
				set_title (_("Not connected"));
				set_subtitle (Config.PACKAGE_STRING);

				GLib.Timeout.add (15000, (GLib.SourceFunc) reconnect);
				return false;
			}

			this.mpdclient.update_status ();
			return true;
		}

		private void reconnect () {
			if (this.mpdclient.connect () == false)
				return;

  	  	  	/* Refresh title/subtitle (bug #4975) */
			this.refresh_title = true;
			if (this.mpdclient.is_playing ())
				cb_song_changed ();
			else
				cb_stopped ();

  	  	  	/* Return FALSE to kill the reconnection timeout and start a refresh timeout */
			GLib.Timeout.add (1000, refresh);
		}

		private void cb_song_changed () {
  	  	  	/* title */
			set_title (this.mpdclient.get_title ());

  	  	  	/* subtitle "by \"artist\" from \"album\" (year)" */
			GLib.StringBuilder text = new GLib.StringBuilder ();
			text.append_printf (_("by \"%s\" from \"%s\" (%s)"),
				     	    this.mpdclient.get_artist (),
				     	    this.mpdclient.get_album (),
				     	    this.mpdclient.get_date ());

  	  	  	/* text = xfmpc_interface_get_subtitle (interface); to avoid "n/a" values, so far I don't care */
			set_subtitle (text.str);
		}

		private void cb_pp_changed (bool is_playing) {
			set_pp (is_playing);

			if (this.refresh_title) {
				cb_song_changed ();
				this.refresh_title = false;
			}
		}

		private void cb_time_changed (int time, int total_time) {
			set_time (time, total_time);
		}

		private void cb_volume_changed (int volume) {
			set_volume (volume);
		}

		private void cb_stopped () {
			set_pp (false);
			set_time (0, 0);
			set_title (_("Stopped"));
			set_subtitle (Config.PACKAGE_STRING);

			this.refresh_title = true;
		}

		private void cb_mpdclient_previous () {
			this.mpdclient.previous ();
		}

		private void cb_mpdclient_next () {
			this.mpdclient.next ();
		}
	}
}

/* vi:set ts=8 sw=8: */
