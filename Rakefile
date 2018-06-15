require 'bundler/gem_tasks'
require 'rspec/core/rake_task'
require 'rure'

RSpec::Core::RakeTask.new(:spec)

task :default => :spec

task :benchmark do
  require 'benchmark'
  n = 1_000

  puts '======================='
  puts 'short haystack bechmark'
  puts '======================='
  short_haystack = <<-EOF
A Regexp holds a regular expression, used to match a pattern against strings.
Regexps are created using the /.../ and %r{...} literals, and by the
Regexp::new constructor.
  EOF
  run_benchmark(n, short_haystack)

  puts
  puts '======================'
  puts 'long haystack bechmark'
  puts '======================'
  long_haystack = 'x' * 10_000 + short_haystack
  run_benchmark(n, long_haystack)

end

def run_benchmark(n, haystack)
  Benchmark.bm(21) do |x|
    pattern = 'Regexp.*(literals)'
    ruby_re = Regexp.new(pattern)
    rure_re = Rure::Regex.new(pattern)

    puts '-- existence check'
    x.report('ruby #match?') { n.times { ruby_re.match?(haystack) } }
    x.report('rure #match?') { n.times { rure_re.match?(haystack) } }

    puts
    puts '-- match data'
    x.report('ruby #match') { n.times { ruby_re.match(haystack) } }
    x.report('rure #find') { n.times { rure_re.find(haystack) } }

    puts
    puts '-- second capture'
    x.report('ruby #match[1]') { n.times { ruby_re.match(haystack)[1] } }
    x.report('rure #find_captures[1]') { n.times { rure_re.find_captures(haystack)[1] } }
  end
end