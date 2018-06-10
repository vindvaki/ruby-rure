require 'rure/version'

require 'ffi'

module Rure
  class Regex
    def initialize(pattern)
      @pattern = pattern
      @c_rure = CRure.rure_compile_must(pattern)
      ObjectSpace.define_finalizer(self, proc { CRure.rure_free(@c_rure)})
    end

    def match?(string)
      data = FFI::MemoryPointer.new(:uint8, string.bytesize)
      data.put_bytes(0, string)
      CRure.rure_is_match(@c_rure, data, data.size, 0)
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

    attach_function :rure_compile_must, [:string], :pointer
    attach_function :rure_compile, [:pointer, :size_t, :size_t, :uint32, :pointer, :pointer], :pointer
    attach_function :rure_free, [:pointer], :void

    attach_function :rure_is_match, [:pointer, :pointer, :size_t, :size_t], :bool
    attach_function :rure_find, [:pointer, :pointer, :size_t, :size_t, :pointer], :bool
  end
end
