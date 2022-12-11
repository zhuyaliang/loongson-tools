gnome = import('gnome')

sources = [
'main.c',
'tools-window.c',
'loongson-fan.c',
'loongson-firmware.c',
'loongson-utils.c',
]

gdbus_src = gnome.gdbus_codegen(
    'daemon-dbus-generated',
    join_paths(data_dir, 'cn.loongson.daemon.xml'),
    interface_prefix : 'cn.loongson.',
    namespace : 'Loong'
    )

executable(
    'loongson-daemond',
    gdbus_src,
    sources : ['loongson-daemon.h', 'loongson-daemon.c', 'loongson-server.c', 'dmidecode.c'],
    dependencies : [gio_dep, gio_os_dep, gtop_dep],
    install : true,
    install_dir : join_paths(get_option('prefix'), get_option('libexecdir'), 'loongson-daemon')
)

executable('loongson-tools',
  sources : sources + gdbus_src,
  dependencies : [gtk_deps, gtop_dep],
  include_directories: top_srcdir,
  install : true,
  install_dir : get_option('bindir')
  )

