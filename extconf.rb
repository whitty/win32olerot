# Lovingly ripped off from win32ole extconf.rb

require 'mkmf'

dir_config("win32")

def append_manifest(extension_name)
  case CONFIG["target_os"] 
  when "mswin32"

    File.open("Makefile", "a") << "

# Add manifest for windows targets
site-install: manifest-stamp
install: manifest-stamp
all: manifest-stamp

manifest-stamp: $(DLLIB)
        mt -manifest $(DLLIB).manifest -outputresource:$(DLLIB);2
\ttouch $@
"
  else
    # no need for manifest step
  end

end


def create_win32ole_rot_makefile
  if have_library("ole32") and
     have_library("oleaut32") and
     have_library("uuid") and 
     have_library("user32") and
     have_library("kernel32") and
     have_library("advapi32") and
     have_header("windows.h")
    create_makefile("win32ole_rot")
    append_manifest("win32ole_rot")
  else
    ## TODO
  end
end

case RUBY_PLATFORM
when /mswin32/
  $CFLAGS += ' /W3'
end

$cleanfiles << "manifest-stamp $(DLLIB).manifest $(DEFFILE)" if CONFIG["target_os"] ==  "mswin32"

create_win32ole_rot_makefile
