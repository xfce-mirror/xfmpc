libcxfmpc_sources = [
  'mpdclient.c',
  'mpdclient.h',
]

libcxfmpc_deps = [
  glib,
  gthread,
  gtk,
  libmpd,
  libxfce4ui,
  libxfce4util,
  libxfmpc_dep,
]

libcxfmpc = static_library(
  'cxfmpc',
  libcxfmpc_sources,
  dependencies: libcxfmpc_deps,
  install: false,
)

libcxfmpc_dep = declare_dependency(
  include_directories: include_directories('.'),
  dependencies: libcxfmpc_deps,
  link_with: libcxfmpc
)
