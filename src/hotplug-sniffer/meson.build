sniffer_headers = [
    'hotplug-mimetypes.h',
    'Laminax-mime-sniffer.h',
]

sniffer_sources = [
    'Laminax-mime-sniffer.c',
    'hotplug-sniffer.c',
]

sniffer_deps = [
    glib,
    gio,
    gtk,
]

executable(
    'Laminax-hotplug-sniffer',
    sniffer_sources,
    include_directories: include_root,
    dependencies: sniffer_deps,
    install: true,
    install_dir: libexecdir,
)
