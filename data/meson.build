subdir('icons')
subdir('hicolor')

i18n = import('i18n')

install_data(
  ls_namespace + '.conf',
  install_dir: dbus_conf_dir,
)

install_data(
  'cn.loongson.daemon.xml',
  install_dir: dbus_ifaces_dir,
)

service = 'loongson-daemon.service'
service_conf = configuration_data()
service_conf.set('pkglibexecdir', join_paths(get_option('prefix'), get_option('libexecdir'), 'loongson-daemon'))

configure_file(
   input: service + '.in',
   output: service,
   configuration: service_conf,
   install: true,
   install_dir: install_systemd_unit_dir,
)
# replace @pkglibexecdir@
configure_file(
    input : 'cn.loongson.daemon.service.in',
    output : 'cn.loongson.daemon.service',
    install: true,
    install_dir: join_paths(get_option('datadir'), 'dbus-1/system-services'),
    configuration : service_conf
)

i18n.merge_file(
  input: 'loongson-daemon.desktop.in',
  output: 'loongson-daemon.desktop',
  install: true,
  po_dir: join_paths(meson.source_root(), 'po'),
  install_dir : join_paths(get_option('prefix'), 'share/applications'),
  type: 'desktop',
)

appdata = 'loongson-daemon.appdata.xml'
i18n.merge_file(
  input: appdata + '.in',
  output: appdata,
  po_dir: join_paths(meson.source_root(), 'po'),
  install: true,
  install_dir: join_paths(get_option('prefix'), 'share/metadaemon')
)
