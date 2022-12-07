actions_icons = [
  ['16x16', 'loongson-info.png'],
  ['22x22', 'loongson-info.png'],
  ['24x24', 'loongson-info.png'],
  ['32x32', 'loongson-info.png'],
  ['48x48', 'loongson-info.png']
]

foreach icon: actions_icons
  install_data(
    join_paths(icon[0], 'apps', icon[1]),
    install_dir: join_paths(get_option('prefix'), 'share/icons', 'hicolor', icon[0], 'apps')
  )
endforeach
