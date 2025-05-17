libxfmpc_sources = [
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

libxfmpc_deps = [
  glib,
  gtk,
  libxfce4ui,
  libxfce4util,
]

libxfmpc = static_library(
  'xfmpc',
  libxfmpc_sources,
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
  dependencies: libxfmpc_deps,
  install: false,
)

libxfmpc_dep = declare_dependency(
  include_directories: [
    include_directories('.'),
    include_directories('..'),
  ],
  dependencies: libxfmpc_deps,
  link_with: libxfmpc,
)

subdir('libc')

executable(
  'xfmpc',
  dependencies: [
    libxfmpc_dep,
    libcxfmpc_dep,
  ],
  install: true,
  install_dir: get_option('prefix') / get_option('bindir'),
)
