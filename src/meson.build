subdir('libc')

xfmpc_sources = [
  'dbbrowser.vala',
  'extended-interface.vala',
  'interface.vala',
  'main-window.vala',
  'main.vala',
  'playlist.vala',
  'preferences-dialog.vala',
  'preferences.vala',
  'progress-bar.vala',
  'shortcuts-dialog.vala',
  'song-dialog.vala',
  'statusbar.vala',
  xfce_revision_h,
]

xfmpc = executable(
  'xfmpc',
  xfmpc_sources,
  c_args: ['-w'],
  vala_args: [
    '--vapidir', meson.current_source_dir(),
    '--vapidir', meson.current_source_dir() / 'libc',
    '--vapidir', meson.current_source_dir() / '..' / 'third-party',
    '--pkg', 'config',
    '--pkg', 'mpdclient',
  ],
  include_directories: [
    include_directories('..'),
  ],
  vala_header: 'xfmpc.h',
  dependencies: [
    glib,
    gtk,
    libcxfmpc_dep,
    libxfce4ui,
    libxfce4util,
  ],
  install: true,
  install_dir: get_option('prefix') / get_option('bindir'),
)
