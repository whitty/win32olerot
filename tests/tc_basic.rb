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
    # not really a test since we don't know what could be running
    @rot.each do |x| 
      assert_nothing_thrown { WIN32OLE.connect(x) }
    end
  end

  def test_is_running
    @rot.each do |x|
      assert @rot.is_running?(x)
    end
  end
  
end

