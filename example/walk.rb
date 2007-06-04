require 'pytst'

t = Pytst::TST.new

t['1234'] = '[1234]'
t['123456'] = '[123456]'
t['45678'] = '[45678]'
t['5678910'] = '[5678910]'

result = t.walk(nil, Pytst::ListAction.new)

p result