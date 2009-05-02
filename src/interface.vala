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

		private string gettext_package = Config.GETTEXT_PACKAGE;
		private string localedir = Config.PACKAGE_LOCALE_DIR;

		private Button button_prev;
		private Button button_pp;
		private Button button_next;
		private VolumeButton button_volume;
		private ProgressBar progress_bar;
		private Label title;
		private Label subtitle;
		private bool refresh_title;

		construct {
			Xfce.textdomain (gettext_package, localedir, "UTF-8");

			mpdclient = Xfmpc.Mpdclient.get ();
			preferences = Xfmpc.Preferences.get ();

			set_border_width (4);

			var image = new Image.from_stock (STOCK_MEDIA_PREVIOUS, IconSize.BUTTON);
			button_prev = new Button ();
			button_prev.set_relief (ReliefStyle.NONE);
			button_prev.add (image);

			image = new Image.from_stock (STOCK_MEDIA_PLAY, IconSize.BUTTON);
			button_pp = new Button ();
			button_pp.set_relief (ReliefStyle.NONE);
			button_pp.add (image);

			image = new Image.from_stock (STOCK_MEDIA_NEXT, IconSize.BUTTON);
			button_next = new Button ();
			button_next.set_relief (ReliefStyle.NONE);
			button_next.add (image);

			button_volume = new VolumeButton ();
			button_volume.set_relief (ReliefStyle.NONE);
			var adjustment = button_volume.get_adjustment ();
			adjustment.upper *= 100;
			adjustment.step_increment *= 100;
			adjustment.page_increment *= 100;

			var progress_box = new EventBox ();
			progress_bar = new ProgressBar ();
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

			title = new Label (_("Not connected"));
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

			subtitle = new Label (Config.PACKAGE_STRING);
			subtitle.set_attributes (attrs);
			subtitle.set_selectable (true);
			subtitle.set_ellipsize (Pango.EllipsizeMode.END);
			subtitle.set_alignment (0, (float) 0.5);

  	  	  	/* === Containers === */
			var box = new HBox (false, 0);
			pack_start (box, false, false, 0);
			box.pack_start (button_prev, false, false, 0);
			box.pack_start (button_pp, false, false, 0);
			box.pack_start (button_next, false, false, 0);
			box.pack_start (progress_box, true, true, 4);
			box.pack_start (button_volume, false, false, 0);

			var vbox = new VBox (false, 0);
			pack_start (vbox, false, true, 0);
			vbox.add (title);
			vbox.add (subtitle);

  	  	  	/* === Signals === */
			button_prev.clicked += cb_mpdclient_previous;
			button_pp.clicked += pp_clicked;
			button_next.clicked += cb_mpdclient_next;
			button_volume.value_changed += volume_changed;
			progress_box.button_release_event += cb_progress_box_release_event;

			mpdclient.connected += reconnect;
			mpdclient.song_changed += cb_song_changed;
			mpdclient.pp_changed += cb_pp_changed;
			mpdclient.time_changed += cb_time_changed;
			mpdclient.volume_changed += cb_volume_changed;
			mpdclient.stopped += cb_stopped;

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
			if (!mpdclient.pp ())
				return;
			set_pp (mpdclient.is_playing ());
		}

		public void set_pp (bool play) {
			var image = (Image) button_pp.get_child ();

			if (play == true)
				image.set_from_stock (STOCK_MEDIA_PAUSE, IconSize.BUTTON);
			else
				image.set_from_stock (STOCK_MEDIA_PLAY, IconSize.BUTTON);
		}

		private bool cb_progress_box_release_event (Gdk.EventButton event) {
			if (event.type != Gdk.EventType.BUTTON_RELEASE || event.button != 1)
				return false;

			int time_total = mpdclient.get_total_time ();
			if (time_total < 0)
				return false;

			double time = event.x / progress_bar.allocation.width;
			time *= time_total;

			mpdclient.set_song_time ((int) time);

			return true;
		}

		public void volume_changed (double value) {
			mpdclient.set_volume ((char) value);
		}

		public void set_volume (int volume) {
			button_volume.set_value (volume);
		}

		public void popup_volume () {
			Signal.emit_by_name (button_volume, "popup", null);
		}

		public void set_time (int time, int time_total) {
			int min, sec, min_total, sec_total;
			double fraction = 1.0;

			min = time / 60;
			sec = time % 60;

  	  	  	min_total = time_total / 60;
  	  	  	sec_total = time_total % 60;

			StringBuilder text = new StringBuilder ();
			text.append_printf ("%d:%02d / %d:%02d", min, sec, min_total, sec_total);
			progress_bar.set_text (text.str);

			if (time_total > 0)
				fraction = (float)time / (float)time_total;

			progress_bar.set_fraction ((fraction <= 1.0) ? fraction : 1.0);
		}

		private bool refresh () {
			if (mpdclient.connect () == false) {
				warning ("Failed to connect to MPD");
				mpdclient.disconnect ();
				set_pp (false);
				set_time (0, 0);
				set_volume (0);
				set_title (_("Not connected"));
				set_subtitle (Config.PACKAGE_STRING);

				Timeout.add (15000, (SourceFunc) reconnect);
				return false;
			}

			mpdclient.update_status ();
			return true;
		}

		private void reconnect () {
			if (mpdclient.connect () == false)
				return;

  	  	  	/* Refresh title/subtitle (bug #4975) */
			refresh_title = true;
			if (mpdclient.is_playing ())
				cb_song_changed ();
			else
				cb_stopped ();

  	  	  	/* Return FALSE to kill the reconnection timeout and start a refresh timeout */
			Timeout.add (1000, refresh);
		}

		private void cb_song_changed () {
  	  	  	/* title */
			set_title (mpdclient.get_title ());

  	  	  	/* subtitle "by \"artist\" from \"album\" (year)" */
			StringBuilder text = new StringBuilder ();
			text.append_printf (_("by \"%s\" from \"%s\" (%s)"),
				     	    mpdclient.get_artist (),
				     	    mpdclient.get_album (),
				     	    mpdclient.get_date ());

  	  	  	/* text = xfmpc_interface_get_subtitle (interface); to avoid "n/a" values, so far I don't care */
			set_subtitle (text.str);
		}

		private void cb_pp_changed (bool is_playing) {
			set_pp (is_playing);

			if (refresh_title) {
				cb_song_changed ();
				refresh_title = false;
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

			refresh_title = true;
		}

		private void cb_mpdclient_previous () {
			mpdclient.previous ();
		}

		private void cb_mpdclient_next () {
			mpdclient.next ();
		}
	}
}

/* vi:set ts=8 sw=8: */
