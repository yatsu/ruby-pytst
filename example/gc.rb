require 'pytst'

def store(tst)
  tst["aaa"] = "AAA"
  tst["bbb"] = "BBB"
end

t = Pytst::TST.new

store(t)

p t["aaa"]

puts "GC"
GC.start

# if stored data is not marked, this causes a segmentation fault.
p t["aaa"]
