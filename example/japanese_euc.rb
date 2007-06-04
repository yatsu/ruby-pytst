require 'pytst'

# example text:
# http://ema.fsr.jp/~munehisa/diary/20050927.html#p01

TEXT = "�̲��˹Ԥ���̵����̲���ʤ�ޤ���"

t = Pytst::TST.new

t["�̲�"] = "[�̲�]"
t["�Ԥ�"] = "[�Ԥ�]"

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
