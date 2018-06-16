#include <ruby.h>
#include "extconf.h"
#include "regex-capi/include/rure.h"

static VALUE cRureRegex;
static VALUE cRureMatch;
static VALUE cRureCaptures;
static VALUE cRureIter;

typedef struct rure_regex_data_t {
    rure* ptr;
} rure_regex_data_t;

typedef struct rure_captures_data_t {
    rure_captures* ptr;
} rure_captures_data_t;

typedef struct rure_iter_data_t {
    rure_iter* ptr;
} rure_iter_data_t;

void rure_regex_data_free(void *data_void) {
    rure_regex_data_t *data = (rure_regex_data_t *) data_void;
    rure* ptr = data->ptr;
    free(data);
    rure_free(ptr);
}

VALUE rure_regex_data_alloc(VALUE self) {
    rure_regex_data_t *data = malloc(sizeof(rure_regex_data_t));
    return Data_Wrap_Struct(self, 0, rure_regex_data_free, data);
}

VALUE rure_match_data_alloc(VALUE self) {
    rure_match *data = malloc(sizeof(rure_match));
    return Data_Wrap_Struct(self, 0, free, data);
}

void rure_iter_data_free(void *data_void) {
    rure_iter_data_t *data = (rure_iter_data_t *) data_void;
    rure_iter *ptr = data->ptr;
    free(data);
    rure_iter_free(ptr);
}

VALUE rure_iter_data_alloc(VALUE self) {
    rure_iter *data = malloc(sizeof(rure_iter_data_t));
    return Data_Wrap_Struct(self, 0, rure_iter_data_free, data);
}

void rure_captures_data_free(void *data_void) {
    rure_captures_data_t *data = (rure_captures_data_t *) data_void;
    rure_captures* ptr = data->ptr;
    free(data);
    rure_captures_free(ptr);
}

VALUE rure_captures_data_alloc(VALUE self) {
    rure_captures_data_t *data = malloc(sizeof(rure_captures_data_t));
    return Data_Wrap_Struct(self, 0, rure_captures_data_free, data);
}

VALUE rb_rure_captures_initialize(VALUE self, VALUE regex) {
    rure_captures_data_t *data;
    Data_Get_Struct(self, rure_captures_data_t, data);
    rure_regex_data_t *regex_data;
    Data_Get_Struct(regex, rure_regex_data_t, regex_data);
    data->ptr = rure_captures_new(regex_data->ptr);
    rb_iv_set(self, "@regex", regex);
    return self;
}

VALUE rb_rure_regex_initialize(VALUE self, VALUE pattern) {
    rure_regex_data_t *data;
    Data_Get_Struct(self, rure_regex_data_t, data);
    data->ptr = rure_compile_must(StringValueCStr(pattern));
    return self;
}

VALUE rb_rure_iter_initialize(VALUE self, VALUE regex, VALUE haystack) {
    rure_iter_data_t *data;
    Data_Get_Struct(self, rure_iter_data_t, data);
    rure_regex_data_t *regex_data;
    Data_Get_Struct(regex, rure_regex_data_t, regex_data);
    data->ptr = rure_iter_new(regex_data->ptr);
    rb_iv_set(self, "@regex", regex);
    rb_iv_set(self, "@haystack", haystack);
    return self;
}

VALUE rb_rure_regex_is_match(int argc, VALUE *argv, VALUE self) {
    VALUE haystack;
    VALUE rb_start;
    rb_scan_args(argc, argv, "11", &haystack, &rb_start);
    VALUE haystack_rb_string_value = StringValue(haystack);
    uint8_t *haystack_ptr = RSTRING_PTR(haystack_rb_string_value);
    size_t haystack_len = RSTRING_LEN(haystack_rb_string_value);
    rure_regex_data_t *data;
    Data_Get_Struct(self, rure_regex_data_t, data);
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
    rure_regex_data_t *data;
    Data_Get_Struct(self, rure_regex_data_t, data);
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

VALUE rb_rure_regex_find_captures(int argc, VALUE *argv, VALUE self) {
    VALUE haystack;
    VALUE rb_start;
    rb_scan_args(argc, argv, "11", &haystack, &rb_start);
    VALUE haystack_rb_string_value = StringValue(haystack);
    uint8_t *haystack_ptr = RSTRING_PTR(haystack_rb_string_value);
    size_t haystack_len = RSTRING_LEN(haystack_rb_string_value);
    rure_regex_data_t *data;
    Data_Get_Struct(self, rure_regex_data_t, data);
    size_t start = NIL_P(rb_start) ? 0 : NUM2SSIZET(rb_start);
    VALUE captures = rb_funcall(cRureCaptures, rb_intern("new"), 1, self);
    rure_captures_data_t *captures_data;
    Data_Get_Struct(captures, rure_captures_data_t, captures_data);
    bool found = rure_find_captures(data->ptr, haystack_ptr, haystack_len, start, captures_data->ptr);
    if (found) {
        rb_iv_set(captures, "@haystack", haystack);
        return captures;
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

VALUE rb_rure_captures_len(VALUE self) {
    rure_captures_data_t *data;
    Data_Get_Struct(self, rure_captures_data_t, data);
    return SIZET2NUM(rure_captures_len(data->ptr));
}

VALUE rb_rure_captures_at_index(VALUE self, VALUE index) {
    rure_captures_data_t *data;
    Data_Get_Struct(self, rure_captures_data_t, data);
    VALUE match = rb_funcall(cRureMatch, rb_intern("new"), 0);
    rure_match *match_data;
    Data_Get_Struct(match, rure_match, match_data);
    bool found = rure_captures_at(data->ptr, NUM2SIZET(index), match_data);
    if (found) {
        rb_iv_set(match, "@haystack", rb_iv_get(self, "@haystack"));
        return match;
    }
    return Qnil;
}

VALUE rb_rure_captures_at_name(VALUE self, VALUE name) {
    VALUE pattern = rb_iv_get(self, "@regex");
    rure_regex_data_t *regex_data;
    Data_Get_Struct(pattern, rure_regex_data_t, regex_data);
    int32_t index = rure_capture_name_index(regex_data->ptr, StringValueCStr(name));
    if (index == -1) {
        return Qnil;
    }
    return rb_rure_captures_at_index(self, INT2NUM(index));
}

VALUE rb_rure_iter_next(VALUE self) {
    VALUE haystack = rb_iv_get(self, "@haystack");
    VALUE haystack_rb_string_value = StringValue(haystack);
    uint8_t *haystack_ptr = RSTRING_PTR(haystack_rb_string_value);
    size_t haystack_len = RSTRING_LEN(haystack_rb_string_value);

    rure_iter_data_t* data;
    Data_Get_Struct(self, rure_iter_data_t, data);
    VALUE match = rb_funcall(cRureMatch, rb_intern("new"), 0);
    rure_match *match_data;
    Data_Get_Struct(match, rure_match, match_data);
    bool found = rure_iter_next(data->ptr, haystack_ptr, haystack_len, match_data);
    if (found) {
        rb_iv_set(match, "@haystack", rb_iv_get(self, "@haystack"));
        return match;
    }
    return Qnil;
}

void Init_rure() {
    VALUE mRure = rb_define_module("Rure");
    cRureRegex = rb_define_class_under(mRure, "Regex", rb_cObject);
    rb_define_alloc_func(cRureRegex, rure_regex_data_alloc);
    rb_define_method(cRureRegex, "initialize", rb_rure_regex_initialize, 1);
    rb_define_method(cRureRegex, "match?", rb_rure_regex_is_match, -1);
    rb_define_method(cRureRegex, "find", rb_rure_regex_find, -1);
    rb_define_method(cRureRegex, "find_captures", rb_rure_regex_find_captures, -1);

    cRureMatch = rb_define_class_under(mRure, "Match", rb_cObject);
    rb_define_alloc_func(cRureMatch, rure_match_data_alloc);
    rb_define_method(cRureMatch, "start", rb_rure_match_start, 0);
    rb_define_method(cRureMatch, "end", rb_rure_match_end, 0);
    rb_define_method(cRureMatch, "to_s", rb_rure_match_to_s, 0);

    cRureCaptures = rb_define_class_under(mRure, "Captures", rb_cObject);
    rb_define_method(cRureCaptures, "initialize", rb_rure_captures_initialize, 1);
    rb_define_alloc_func(cRureCaptures, rure_captures_data_alloc);
    rb_define_method(cRureCaptures, "length", rb_rure_captures_len, 0);
    rb_define_method(cRureCaptures, "at_index", rb_rure_captures_at_index, 1);
    rb_define_method(cRureCaptures, "at_name", rb_rure_captures_at_name, 1);

    cRureIter = rb_define_class_under(mRure, "Iter", rb_cObject);
    rb_define_method(cRureIter, "initialize", rb_rure_iter_initialize, 2);
    rb_define_alloc_func(cRureIter, rure_iter_data_alloc);
    rb_define_method(cRureIter, "next", rb_rure_iter_next, 0);
}