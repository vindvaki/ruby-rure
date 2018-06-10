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
      c_match = match_ptr[:value]
      Match.new(c_match[:start], c_match[:end])
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
    end

    class RureMatchPtr < FFI::Struct
      layout :value, RureMatch
    end

    attach_function :rure_compile_must, [:string], :pointer
    attach_function :rure_compile, [:pointer, :size_t, :size_t, :uint32, :pointer, :pointer], :pointer
    attach_function :rure_free, [:pointer], :void

    attach_function :rure_is_match, [:pointer, :pointer, :size_t, :size_t], :bool
    attach_function :rure_find, [:pointer, :pointer, :size_t, :size_t, RureMatchPtr], :bool

    attach_function :rure_shortest_match, [:pointer, :pointer, :size_t, :size_t, :size_t], :bool
  end
end
