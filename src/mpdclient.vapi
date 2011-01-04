/* mpdclient.vapi initially generated with:
   /usr/lib/vala/gen-introspect --namespace=Xfmpc mpdclient.h `pkg-config --cflags gobject-2.0` > mpdclient.gi
   vapigen --library=mpdclient mpdclient.gi */

[CCode (cprefix = "Xfmpc", lower_case_cprefix = "xfmpc_")]
namespace Xfmpc {
	[CCode (cheader_filename = "mpdclient.h")]
	public class Mpdclient {
		public bool connect ();
		public bool database_read (string dir, out string filename, out string basename, out bool is_dir);
		public bool database_refresh ();
		public bool database_search (string query, out string filename, out string basename);
		public void disconnect ();
		public static unowned Xfmpc.Mpdclient get_default ();
		public unowned string get_album ();
		public unowned string get_artist ();
		public unowned string get_date ();
		public int get_id ();
		public int get_pos ();
		public bool get_random ();
		public bool get_repeat ();
		public bool get_single ();
		public bool get_consume ();
		public unowned Xfmpc.SongInfo get_song_info (int id);
		public int get_time ();
		public unowned string get_title ();
		public int get_total_time ();
		public uchar get_volume ();
		public bool is_connected ();
		public bool is_playing ();
		public bool is_paused ();
		public bool is_stopped ();
		public bool next ();
		public bool pause ();
		public bool play ();
		public bool playlist_clear ();
		public int playlist_get_length ();
		public int playlist_get_total_time ();
		public bool playlist_read (out int id, out int pos, out string filename, out string song, out string length);
		public bool pp ();
		public bool previous ();
		public bool queue_add (string path);
		public bool queue_clear ();
		public bool queue_commit ();
		public bool queue_remove_id (int id);
		public bool set_id (int id);
		public bool set_random (bool random);
		public bool set_repeat (bool repeat);
		public bool set_single (bool single);
		public bool set_consume (bool consume);
		public bool set_song_time (uint time);
		public bool set_volume (uchar volume);
		public bool stop ();
		public void update_status ();
		public void reload ();
		public virtual signal void connected ();
		public virtual signal void song_changed ();
		public virtual signal void pp_changed (bool is_playing);
		public virtual signal void time_changed (int time);
		public virtual signal void total_time_changed (int total_time);
		public virtual signal void volume_changed (int volume);
		public virtual signal void stopped ();
		public virtual signal void database_changed ();
		public virtual signal void playlist_changed ();
		public virtual signal void repeat ();
		public virtual signal void random ();
		public virtual signal void single ();
		public virtual signal void consume ();
	}
	[Compact]
	[CCode (cheader_filename = "mpdclient.h")]
	public class SongInfo {
		public weak string album;
		public weak string artist;
		public weak string date;
		public weak string filename;
		public weak string genre;
		public weak string title;
		public weak string track;
	}
}
