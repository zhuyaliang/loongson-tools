project('loongson-daemon', 'c',
  version : '1.0.3',
  license : 'GPLv3',
  meson_version : '>=0.46.1',
  default_options : [ 'warning_level=1', 'buildtype=debugoptimized', 'c_std=gnu11'],
  )

ls_name = meson.project_name()
ls_api_version = '1.0'
ls_api_name = '@0@-@1@'.format(ls_name, ls_api_version)
libversion = meson.project_version()
dbus_dep = dependency('dbus-1')
ls_namespace = 'cn.loongson.daemon'
ls_prefix = get_option('prefix')
data_dir = join_paths(meson.current_source_dir(), 'data')
ls_sysconfdir = join_paths(ls_prefix, get_option('sysconfdir'))
ls_datadir = join_paths(ls_prefix, get_option('datadir'))
dbus_ifaces_dir = dbus_dep.get_pkgconfig_variable('interfaces_dir', define_variable: ['datadir', ls_datadir])
dbus_conf_dir = join_paths(dbus_dep.get_pkgconfig_variable('sysconfdir', define_variable: ['sysconfdir', ls_sysconfdir]), 'dbus-1', 'system.d')
install_systemd_unit_dir = join_paths(ls_prefix, 'lib', 'systemd', 'system')
ls_includedir = join_paths(ls_prefix, get_option('includedir'))
ls_pkgincludedir = join_paths(ls_includedir, ls_api_name)

# Compiler & Project arguments
add_project_arguments([
  '-DHAVE_CONFIG_H',
  '-I' + meson.build_root(),
  ], language: 'c')

# get suported warning flags
test_args = [
  '-fstack-protector-strong',
  '-g',
  '-mloongson-csr',
  '-Waggregate-return',
  '-Warray-bounds',
  '-Wcast-align',
  '-Wclobbered',
  '-Wdeclaration-after-statement',
  '-Wempty-body',
  '-Wextra',
  '-Wformat=2',
  '-Wformat-nonliteral',
  '-Wformat-security',
  '-Wformat-signedness',
  '-Wignored-qualifiers',
  '-Wimplicit-function-declaration',
  '-Winit-self',
  '-Winline',
  '-Wmissing-declarations',
  '-Wmissing-format-attribute',
  '-Wmissing-include-dirs',
  '-Wmissing-noreturn',
  '-Wmissing-parameter-type',
  '-Wmissing-prototypes',
  '-Wnested-externs',
  '-Wno-discarded-qualifiers',
  '-Wno-missing-field-initializers',
  '-Wno-strict-aliasing',
  '-Wno-suggest-attribute=format',
  '-Wno-unused-parameter',
  '-Wold-style-definition',
  '-Woverride-init',
  '-Wpacked',
  '-Wpointer-arith',
  '-Wredundant-decls',
  '-Wreturn-type',
  '-Wshadow',
  '-Wsign-compare',
  '-Wstrict-aliasing',
  '-Wstrict-prototypes',
  '-Wswitch-default',
  '-Wtype-limits',
  '-Wundef',
  '-Wuninitialized',
  '-Wunused-but-set-variable',
  '-Wwrite-strings'
  ]

cc = meson.get_compiler('c')

foreach arg: test_args
  if cc.has_argument(arg)
    add_project_arguments(arg, language : 'c')
  endif
endforeach

# enable full RELRO where possible
# FIXME: until https://github.com/mesonbuild/meson/issues/1140 is fixed
global_link_args = []
test_link_args = [
  '-Wl,-z,relro',
  '-Wl,-z,now',
  ]
foreach arg: test_link_args
  if cc.has_argument(arg)
    global_link_args += arg
  endif
endforeach
add_global_link_arguments(
  global_link_args,
  language: 'c'
  )

# Dependencies
gtk_req_version = '>= 3.0.0'
glib_req_version = '>= 2.32.0'
gtk_deps = dependency('gtk+-3.0', version: gtk_req_version)
polkit_deps = dependency('polkit-gobject-1')
gio_dep = dependency('gio-2.0', method: 'pkg-config', version: glib_req_version)
gio_os_dep = dependency('gio-unix-2.0', version: glib_req_version)

# Configure data
conf = configuration_data()
conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('PACKAGE_NAME', meson.project_name())
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())
conf.set_quoted('LOONGSON_INFO_LOCALEDIR', join_paths(get_option('prefix'), get_option('localedir')))
conf.set_quoted('DATADIR', join_paths(get_option('prefix'), get_option('datadir')))
conf.set_quoted('LIBDIR', join_paths(get_option('prefix'), get_option('libdir')))
conf.set_quoted('BINDIR', join_paths(get_option('prefix'), get_option('bindir')))
conf.set_quoted('SYSCONFDIR', join_paths(get_option('prefix'), get_option('sysconfdir')))
conf.set_quoted('LOCALSTATEDIR', join_paths(get_option('prefix'), get_option('localstatedir')))
conf.set_quoted('LIBEXECDIR', join_paths(get_option('prefix'), get_option('libexecdir')))
conf.set_quoted('ICONSDIR', join_paths(get_option('prefix'), get_option('datadir'), meson.project_name(), 'icons/'))
conf.set10('TIME_WITH_SYS_TIME', cc.has_header('sys/time.h'))

configure_file(
  output : 'config.h',
  configuration : conf
  )

top_srcdir = include_directories('.')

subdir('po')
subdir('src')
subdir('data')
meson.add_install_script('meson_post_install.sh')
