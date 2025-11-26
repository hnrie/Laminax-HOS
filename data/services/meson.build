service_files = [
  'org.Laminax.CalendarServer.service',
  'org.Laminax.HotplugSniffer.service',
  'org.Laminax.Melange.service',
  'org.Laminax.Slideshow.service',
]

foreach service_file : service_files
    configure_file(
        input: service_file + '.in',
        output: service_file,
        configuration: desktop_conf,
        install_dir: servicedir
    )
endforeach
