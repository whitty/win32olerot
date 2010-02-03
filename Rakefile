require 'rubygems'
require 'rake/extensiontask'
require 'rake/gempackagetask'

# TODO - remove the old gem-spec
EXTENSION_SOURCE = FileList['ext/**/*.c', 'ext/**/*.cpp', 'ext/**/*.h']

# compilation gemspec
spec = Gem::Specification.new do |s|
  s.name = 'win32olerot'
  s.version = "0.0.1"
  s.date = %q{2010-02-02}
  s.authors = ["G Whiteley"]
  # TODO - fix email address
#  s.email = %q{whitty@rubyforge.org}
  s.rubyforge_project = 'win32olerot'
  s.summary = %q{win32olerot provides access to the win32 RunningObjectTable (ROT).}
  s.homepage = %q{http://win32olerot.rubyforge.org/}
  s.description = %q{win32olerot provides an adjunct to the standard WIN32OLE Ruby library for access to the win32 RunningObjectTable (ROT). The RunningObjectTable contains the list of registered monikers that can be connected to using WIN32OLE.connect().}

  s.add_dependency('win32ole')
  s.add_development_dependency('rake-compiler')

  # consider Changelog, docs etc,...
  s.files = [
    "README",
    "LICENSE.txt",
  ].concat(EXTENSION_SOURCE)

  s.platform = if RUBY_PLATFORM =~ /mswin32$/
                 # allow native compiles through rake-compiler
                 Gem::Platform::RUBY
               else
                 # otherwise require cross-compilation or no binary compile
                 'mswin32'
               end
  s.extensions = FileList['ext/**/extconf.rb']
end

Rake::GemPackageTask.new(spec) do |pkg|
end

Rake::ExtensionTask.new('win32olerot', spec) do |ext|
end
