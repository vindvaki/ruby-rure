require 'rure/version'

require 'ffi'

module Rure
  class Regex
    def initialize(pattern)
      @pattern = pattern
      @pointer = CRure.rure_compile_must(pattern)
      ObjectSpace.define_finalizer(self, proc { CRure.rure_free(@pointer)})
    end
  end

  module CRure
    extend FFI::Library
    # FIXME: Bundle lib as part of gem
    ffi_lib "/usr/local/lib64/librure.so"

    class RureMatch < FFI::Struct
      layout \
        :start, :size_t,
        :end, :size_t
    end

    attach_function :rure_compile_must, [:string], :pointer
    attach_function :rure_compile, [:pointer, :size_t, :size_t, :uint32, :pointer, :pointer], :pointer
    attach_function :rure_free, [:pointer], :void

    attach_function :rure_is_match, [:pointer, :pointer, :size_t, :size_t]
    attach_function :rure_find, [:pointer, :pointer, :size_t, :size_t, :pointer]
  end
end
