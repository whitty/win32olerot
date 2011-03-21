require 'rubygems'
require 'rake/extensiontask'
require 'rake/gempackagetask'
require 'rake/testtask'

EXTCONF_FILES = FileList['ext/**/extconf.rb']
EXTENSION_SOURCE = FileList['ext/**/*.c', 'ext/**/*.cpp', 'ext/**/*.h']
TEST_FILES = FileList['tests/tc_*.rb']

task :default => [:test]

Rake::TestTask.new :test => [:compile] do |t|
  t.test_files = TEST_FILES
  t.warning = true
end

# compilation gemspec
spec = Gem::Specification.new do |s|
  s.name = 'win32olerot'
  s.version = "0.0.5"
  s.date = %q{2011-03-21}
  s.authors = ["G Whiteley"]
  s.email = %q{whitty@users.sourceforge.net}
  s.rubyforge_project = 'win32olerot'
  s.summary = %q{win32olerot provides access to the win32 RunningObjectTable (ROT).}
  s.homepage = %q{http://win32olerot.rubyforge.org/}
  s.description = %q{win32olerot provides an adjunct to the standard WIN32OLE Ruby library for access to the win32 RunningObjectTable (ROT). The RunningObjectTable contains the list of registered monikers that can be connected to using WIN32OLE.connect().}

  s.add_development_dependency('rake-compiler')

  # consider Changelog, docs etc,...
  s.files = [
    "README",
    "LICENSE.txt",
  ].concat(EXTENSION_SOURCE)

  s.platform = Gem::Platform::RUBY

  s.extensions = EXTCONF_FILES
  s.test_files = TEST_FILES
end

Rake::GemPackageTask.new(spec) do |pkg|
end

Rake::ExtensionTask.new('win32olerot', spec) do |ext|
  ext.cross_compile = true
  ext.cross_platform = 'i386-mingw32'
end
