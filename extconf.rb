#----------------------------------
# extconf.rb
# - Based on:
# $Revision: 1.5.2.2 $
# $Date: 2005/10/09 02:15:03 $
#----------------------------------
require 'mkmf'

dir_config("win32")

SRCFILES=<<SRC
win32ole_moniker.c
SRC

def create_docfile(src)
  open(File.expand_path($srcdir) + "/.document", "w") {|ofs|
    ofs.print src
  }
end

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


def create_win32ole_makefile
  if have_library("ole32") and
     have_library("oleaut32") and
     have_library("uuid") and 
     have_library("user32") and
     have_library("kernel32") and
     have_library("advapi32") and
     have_header("windows.h")
    create_makefile("win32ole_moniker")
    create_docfile(SRCFILES)
    append_manifest("win32ole_moniker")
  else
    create_docfile("")
  end
end

case RUBY_PLATFORM
when /mswin32/
  $CFLAGS += ' /W3'
when /cygwin/, /mingw/
  $defs << '-DNONAMELESSUNION'
end

$cleanfiles << "manifest-stamp $(DLLIB).manifest $(DEFFILE)" if CONFIG["target_os"] ==  "mswin32"

create_win32ole_makefile
