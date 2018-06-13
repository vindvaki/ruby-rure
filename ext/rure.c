#include <ruby.h>
#include "extconf.h"
#include "regex-crate/regex-capi/include/rure.h"

static VALUE cRureRegex;
static VALUE cRureMatch;

typedef struct rure_wrapper_t {
    rure* ptr;
} rure_wrapper_t;

void rure_wrapper_free(void *data_void) {
    rure_wrapper_t *data = (rure_wrapper_t *) data_void;
    rure* ptr = data->ptr;
    free(data);
    free(ptr);
}

VALUE rure_wrapper_alloc(VALUE self) {
    rure_wrapper_t *data = malloc(sizeof(rure_wrapper_t));
    return Data_Wrap_Struct(self, 0, rure_wrapper_free, data);
}

VALUE rure_match_wrapper_alloc(VALUE self) {
    rure_match *data = malloc(sizeof(rure_match));
    return Data_Wrap_Struct(self, 0, free, data);
}

VALUE rb_rure_regex_initialize(VALUE self, VALUE pattern) {
    rure_wrapper_t *data;
    Data_Get_Struct(self, rure_wrapper_t, data);
    data->ptr = rure_compile_must(StringValueCStr(pattern));
    return self;
}

VALUE rb_rure_regex_is_match(int argc, VALUE *argv, VALUE self) {
    VALUE haystack;
    VALUE rb_start;
    rb_scan_args(argc, argv, "11", &haystack, &rb_start);
    VALUE haystack_rb_string_value = StringValue(haystack);
    uint8_t *haystack_ptr = RSTRING_PTR(haystack_rb_string_value);
    size_t haystack_len = RSTRING_LEN(haystack_rb_string_value);
    rure_wrapper_t *data;
    Data_Get_Struct(self, rure_wrapper_t, data);
    size_t start = NIL_P(rb_start) ? 0 : NUM2SSIZET(rb_start);
    bool is_match = rure_is_match(data->ptr, haystack_ptr, haystack_len, start);
    if (is_match) {
        return Qtrue;
    }
    return Qfalse;
}

VALUE rb_rure_regex_find(int argc, VALUE *argv, VALUE self) {
    VALUE haystack;
    VALUE rb_start;
    rb_scan_args(argc, argv, "11", &haystack, &rb_start); 
    VALUE haystack_rb_string_value = StringValue(haystack);
    uint8_t *haystack_ptr = RSTRING_PTR(haystack_rb_string_value);
    size_t haystack_len = RSTRING_LEN(haystack_rb_string_value);
    rure_wrapper_t *data;
    Data_Get_Struct(self, rure_wrapper_t, data);
    size_t start = NIL_P(rb_start) ? 0 : NUM2SSIZET(rb_start);
    VALUE match = rb_funcall(cRureMatch, rb_intern("new"), 0);
    rure_match *match_ptr;
    Data_Get_Struct(match, rure_match, match_ptr);
    bool found = rure_find(data->ptr, haystack_ptr, haystack_len, start, match_ptr);
    if (found) {
        rb_iv_set(match, "@haystack", haystack);
        return match;
    }
    return Qnil;
}

VALUE rb_rure_match_start(VALUE self) {
    rure_match *match_ptr;
    Data_Get_Struct(self, rure_match, match_ptr);
    return SIZET2NUM(match_ptr->start);
}

VALUE rb_rure_match_end(VALUE self) {
    rure_match *match_ptr;
    Data_Get_Struct(self, rure_match, match_ptr);
    return SIZET2NUM(match_ptr->end);
}

VALUE rb_rure_match_to_s(VALUE self) {
    rure_match *match_ptr;
    Data_Get_Struct(self, rure_match, match_ptr);
    VALUE haystack = rb_iv_get(self, "@haystack");
    return rb_str_substr(haystack, match_ptr->start, match_ptr->end - match_ptr->start);
}

void Init_rure() {
    VALUE mRure = rb_define_module("Rure");
    cRureRegex = rb_define_class_under(mRure, "Regex", rb_cObject);
    rb_define_alloc_func(cRureRegex, rure_wrapper_alloc);
    rb_define_method(cRureRegex, "initialize", rb_rure_regex_initialize, 1);
    rb_define_method(cRureRegex, "match?", rb_rure_regex_is_match, -1);
    rb_define_method(cRureRegex, "find", rb_rure_regex_find, -1);

    cRureMatch = rb_define_class_under(mRure, "Match", rb_cObject);
    rb_define_alloc_func(cRureMatch, rure_match_wrapper_alloc);
    rb_define_method(cRureMatch, "start", rb_rure_match_start, 0);
    rb_define_method(cRureMatch, "end", rb_rure_match_end, 0);
    rb_define_method(cRureMatch, "to_s", rb_rure_match_to_s, 0);
}