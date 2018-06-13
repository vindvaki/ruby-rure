require './rure'

p Rure::Regex.new("ab").match?("abc", 0)
p Rure::Regex.new("ab").match?("bc", 0)

require 'benchmark'

Benchmark.bm do |x|
text = <<-EOF
A Regexp holds a regular expression, used to match a pattern against strings.
Regexps are created using the /.../ and %r{...} literals, and by the
Regexp::new constructor.
EOF
n = 10000
pattern = 'Regexp.*(literals)'
x.report('ruby') do
    re = Regexp.new(pattern)
    n.times do
        re.match?(text)
    end
end

x.report('rure') do
    re = Rure::Regex.new(pattern)
    n.times do
        re.match?(text)
    end
end
end