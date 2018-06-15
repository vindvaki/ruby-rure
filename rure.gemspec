
lib = File.expand_path("../lib", __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require "rure/version"

Gem::Specification.new do |spec|
  spec.name          = "rure"
  spec.version       = Rure::VERSION
  spec.authors       = ["Hordur Freyr Yngvason"]
  spec.email         = ["hordurf@gmail.com"]

  spec.summary       = %q{Ruby wrapper for Rust's Regex crate}
  spec.homepage      = "https://github.com/vindvaki/ruby-rure"
  spec.license       = "MIT"

  spec.files         = `git ls-files -z`.split("\x0").reject do |f|
    f.match(%r{^(test|spec|features)/})
  end
  spec.bindir        = "exe"
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ["lib", "ext"]
  spec.extensions    = ["ext/rure/extconf.rb"]

  spec.add_development_dependency "bundler", "~> 1.16"
  spec.add_development_dependency "rake", "~> 10.0"
  spec.add_development_dependency "rspec", "~> 3.0"
end
