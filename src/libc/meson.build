libcxfmpc_sources = [
  'mpdclient.c',
  'mpdclient.h',
]

libcxfmpc = static_library(
  'cxfmpc',
  libcxfmpc_sources,
  vala_args: [
    '--pkg', 'config',
  ],
  include_directories: [
    include_directories('..'/ '..'),
  ],
  dependencies: [
    glib,
    gthread,
    gtk,
    libmpd,
    libxfce4ui,
    libxfce4util,
  ],
  install: false,
)

libcxfmpc_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: libcxfmpc
)
