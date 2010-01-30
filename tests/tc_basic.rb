#!/usr/bin/ruby -w
require 'test/unit'
require 'win32ole_rot'

class TC_Create < Test::Unit::TestCase

  def setup
    @rot = WIN32OLE::RunningObjectTable.new
  end

  def test_create
    assert_nothing_thrown { WIN32OLE::RunningObjectTable.new }
  end

  def test_each
    # not really a test
    @rot.each {|x| p x}
  end

  def test_is_running
    @rot.each do |x|
      if x !~ /^!/ then
        assert @rot.is_running?(x)
      end
    end
  end
  
end

