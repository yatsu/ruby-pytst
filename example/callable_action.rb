require 'pytst'

t = Pytst::TST.new

t['1234'] = '[1234]'
t['123456'] = '[123456]'
t['45678'] = '[45678]'
t['5678910'] = '[5678910]'

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

result = t.scan('1234561234567891012345', Pytst::CallableAction.new(mycallback, myresult))
