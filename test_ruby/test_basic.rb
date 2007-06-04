require 'test/unit'
require 'pytst'

class BasicTest < Test::Unit::TestCase
  def test_initialize
    tst = Pytst::TST.new
    assert_not_nil tst
  end
  
  def test_set
    tst = Pytst::TST.new
    assert_equal tst["abc"], nil
    
    tst["abc"] = "ABC"    
    assert_equal tst["abc"], "ABC"
  end
  
  def test_has_key
    tst = Pytst::TST.new
    assert_equal tst.has_key?("abc"), false
    
    tst["abc"] = "ABC"
    assert_equal tst.has_key?("abc"), true
  end
  
  def test_delete
    tst = Pytst::TST.new
    
    tst["abc"] = "ABC"
    assert_equal tst["abc"], "ABC"
    
    tst.delete("abc")
    assert_equal tst["abc"], nil
  end
end
