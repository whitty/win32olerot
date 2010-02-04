#!/usr/bin/ruby -w
require 'test/unit'
require 'win32olerot'

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

  def test_invalid_monikers_arent_running
    assert_nothing_thrown do
      assert !@rot.is_running?("blah_garbage_data_here")
      assert !@rot.is_running?("blah_garbage_data_here", :raise_exception => false)
    end
  end

  def test_invalid_monikers_exception_with_raise_exception_option
    assert_raises(WIN32OLERuntimeError) { !@rot.is_running?("blah_garbage_data_here", :raise_exception => true) }
  end


end

