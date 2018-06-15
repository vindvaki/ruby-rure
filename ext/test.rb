require './rure'

p Rure::Regex.new("ab").match?("abc", 0)
p Rure::Regex.new("ab").match?("bc", 0)

p Rure::Regex.new("a.*c").find("xxabcrr").to_s

p Rure::Regex.new("a.*c").find_captures("xxabcrr").at_index(0).to_s

p Rure::Regex.new("a(?P<thename>.*)c").find_captures("xxabcrr").at_name('thename').to_s

class Rure::Regex
  def each_match(haystack)
    Enumerator.new do |yielder|
      iter = Rure::Iter.new(self, haystack)
      loop do
        match = iter.next
        break if match.nil?
        yielder << match
      end
    end
  end
end

p Rure::Regex.new("abc").each_match("xxabcrabcr").map(&:to_s)

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