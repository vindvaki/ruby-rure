require 'rure/version'

require 'ffi'

module Rure
  Match = Struct.new(:start, :end)

  class Regex
    def initialize(pattern)
      @pattern = pattern
      @c_rure = CRure.rure_compile_must(pattern)
      ObjectSpace.define_finalizer(self, proc { CRure.rure_free(@c_rure)})
    end

    def match?(haystack, start = 0)
      data = haystack_ptr(haystack)
      CRure.rure_is_match(@c_rure, data, data.size, start)
    end

    def find(haystack, start = 0)
      data = haystack_ptr(haystack)
      match_ptr = CRure::RureMatchPtr.new
      found = CRure.rure_find(@c_rure, data, data.size, start, match_ptr)
      return nil unless found
      match_ptr.to_ruby
    end

    def each_match(haystack)
      c_iter = CRure.rure_iter_new(@c_rure)
      enum = Enumerator.new do |yielder|
        data = haystack_ptr(haystack)
        match_ptr = CRure::RureMatchPtr.new
        loop do
          found = CRure.rure_iter_next(c_iter, data, data.size, match_ptr)
          break unless found
          yielder << match_ptr.to_ruby
        end
      end
      ObjectSpace.define_finalizer(enum, proc { CRure.rure_iter_free(c_iter) })
      return enum unless block_given?
      enum.each { |match| yield(match) }
    end

    private

    def haystack_ptr(haystack)
      data = FFI::MemoryPointer.new(:uint8, haystack.bytesize)
      data.put_bytes(0, haystack)
      data
    end
  end

  module CRure
    extend FFI::Library
    # FIXME: Bundle lib as part of gem
    ffi_lib "librure"

    class RureMatch < FFI::Struct
      layout \
        :start, :size_t,
        :end, :size_t

      def to_ruby
        Match.new(self[:start], self[:end])
      end
    end

    class RureMatchPtr < FFI::Struct
      layout :value, RureMatch

      def to_ruby
        self[:value].to_ruby
      end
    end

    attach_function :rure_compile_must, [:string], :pointer
    attach_function :rure_compile, [:pointer, :size_t, :size_t, :uint32, :pointer, :pointer], :pointer
    attach_function :rure_free, [:pointer], :void

    attach_function :rure_is_match, [:pointer, :pointer, :size_t, :size_t], :bool
    attach_function :rure_find, [:pointer, :pointer, :size_t, :size_t, RureMatchPtr], :bool

    attach_function :rure_shortest_match, [:pointer, :pointer, :size_t, :size_t, :size_t], :bool

    attach_function :rure_iter_new, [:pointer], :pointer
    attach_function :rure_iter_free, [:pointer], :pointer
    attach_function :rure_iter_next, [:pointer, :pointer, :size_t, RureMatchPtr], :bool
  end
end
