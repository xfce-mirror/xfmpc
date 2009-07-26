using Gtk;

namespace Xfmpc {

        class ArtworkPlugin : PanedPlugin {
	        construct {
		        name = _("Artwork");
			icon_name = "unknown";
		        description = _("Displays artist/album artwork from the current playing song");
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
	        return typeof (ArtworkPlugin);
        }
}
