require 'rure/version'
require 'rure/rure' # C extension

module Rure
  class Regex
    attr_reader :pattern

    def each_match(haystack)
      return to_enum(:each_match, haystack) unless block_given?
      iter = Rure::Iter.new(self, haystack)
      loop do
        match = iter.next
        break if match.nil?
        yield(match)
      end
    end
  end

  class Match
    attr_reader :haystack

    def to_a
      [haystack, start, self.end]
    end

    private_instance_methods :initialize
  end

  class Captures
    include Enumerable

    attr_reader :haystack

    def each
      return to_enum unless block_given?
      (0...length).each { |i| yield(at_index(i)) }
    end

    def [](id)
      return at_name(id) if id.is_a?(String)
      at_index(id)
    end

    private_instance_methods :initialize
  end
end