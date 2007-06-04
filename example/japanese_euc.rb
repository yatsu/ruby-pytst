require 'pytst'

# example text:
# http://ema.fsr.jp/~munehisa/diary/20050927.html#p01

TEXT = "北欧に行くと無性に眠くなります。"

t = Pytst::TST.new

t["北欧"] = "[北欧]"
t["行く"] = "[行く]"

mycallback = Proc.new {|key, length, obj|
  if length > 0
    print obj
  else
    print key
  end
}

myresult = Proc.new {
  puts
}

puts TEXT

result = t.scan(TEXT, Pytst::CallableAction.new(mycallback, myresult))
