require 'pytst'

t = Pytst::TST.new

t['1234'] = 'token 1'
t['123456'] = 'token 2'
t['45678'] = 'token 3'
t['5678910'] = 'token 4'

result = t.scan('1234561234567891012345', Pytst::TupleListAction.new)
p result
