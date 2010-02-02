Gem::Specification.new do |s|
  s.name = %q{win32olerot}
  s.version = "0.0.1"
  s.date = %q{2010-02-02}
  s.authors = ["G Whiteley"]
#  s.email = %q{whitty@rubyforge.org}
  s.summary = %q{win32olerot provides access to the win32 RunningObjectTable (ROT).}
  s.homepage = %q{http://win32olerot.rubyforge.org/}
  s.description = %q{win32olerot provides an adjunct to the standard WIN32OLE Ruby library for access to the win32 RunningObjectTable (ROT). The RunningObjectTable contains the list of registered monikers that can be connected to using WIN32OLE.connect().}

  # consider Changelog, docs etc,...
  s.files = [
    "README",
    "LICENSE.txt",
    "win32olerot.cpp"
  ]


  s.platform = Gem::Platform::WIN32
  s.extensions = ['extconf.rb']
end
