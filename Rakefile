require 'bundler/gem_tasks'
require 'rspec/core/rake_task'
require 'rure'

RSpec::Core::RakeTask.new(:spec)

task :default => :spec

task :benchmark do
  require 'benchmark'

  Benchmark.bm do |x|
    text = <<-EOF
A Regexp holds a regular expression, used to match a pattern against strings.
Regexps are created using the /.../ and %r{...} literals, and by the
Regexp::new constructor.
EOF
    n = 10000
    pattern = 'Regexp.*literals'
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
end
