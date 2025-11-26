if get_option('docs')
    version_conf = configuration_data()
    version_conf.set('VERSION', version)

    configure_file(
        input: 'version.xml.in',
        output: 'version.xml',
        configuration: version_conf,
    )

    subdir('Laminax')
    subdir('Laminax-js')
    subdir('Laminax-tutorials')
    subdir('st')
endif
