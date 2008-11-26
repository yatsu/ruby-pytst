/* 
 # Copyright (C) 2006 Masaki Yatsu <yatsu@yatsu.info>
 #
 # 2006-10-11 Masaki Yatsu
 #   * Support Ruby extension instead of Python's.
 #
 # Copyright (C) 2004-2005 Nicolas Lehuen <nicolas@lehuen.com>
 #
 # This library is free software; you can redistribute it and/or
 # modify it under the terms of the GNU Lesser General Public
 # License as published by the Free Software Foundation; either
 # version 2.1 of the License, or (at your option) any later version.
 #
 # This library is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 # Lesser General Public License for more details.
 #
 # You should have received a copy of the GNU Lesser General Public
 # License along with this library; if not, write to the Free Software
 # Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
%module pytst

%feature("autodoc", "1");

%typemap(in) RubyReference {
   $1 = RubyReference($input);
}

%typemap(out) RubyReference {
    $result = $1.lend();
}

%typemap(in) const string_type&, string_type& {
    char* str = StringValuePtr($input);
    $1 = new string_type(str, strlen(str));
}

%typemap(freearg) const string_type&, string_type& {
    delete $1;
}

%typemap(out) const string_type&, string_type& {
    $result = PyString_FromStringAndSize($1.data(),$1.size());
}

%typemap(ret) const string_type&, string_type& {
    delete $1;
}

%exception {
    try { $action }
    catch (std::exception e) { rb_raise(rb_eRuntimeError, "%s", e.what()); }
}

#define __RUBY__BUILD__
#define SCANNER
%include "tst.h"
%include "fundamentals.h"
%include "iterators.h"
%include "memory_storage.h"

%template(TST_) tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >;
%template(Action_) tst_action<char,RubyReference,string_type >;
%template(Filter_) tst_filter<char,RubyReference,string_type >;

%predicate has_key;

%rename("delete") __delitem__;

%markfunc TST "mark_TST";

%include "rubyTST.h"

%template(Lexical_iterator_type) lexical_iterator<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >;
%template(Close_match_iterator_type) match_iterator<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >;
%template(TSTLexicalIterator)    TSTIterator<lexical_iterator<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type > >;
%template(TSTCloseMatchIterator) TSTIterator<match_iterator<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type > >;

%{
#define SCANNER
#include "rubyTST.h"

class MarkAction : public tst_action<char,RubyReference,string_type > {
public:
    MarkAction() {}

    virtual ~MarkAction() {}

    virtual void perform(const string_type& string,int remaining_distance,RubyReference data) {
        rb_gc_mark(data.get());
    }

    virtual RubyReference result() {
        return RubyReference();
    }
};

static void mark_TST(void* ptr) {
    TST* tst = (TST*)ptr;
    MarkAction* action = new MarkAction();
    tst->walk(0,action);
    delete action;
}
%}

%wrapper %{
SWIGINTERN VALUE
_wrap_TST_marshal_load(int argc, VALUE *argv, VALUE self) {
  TST *result = 0 ;

  if ((argc < 1) || (argc > 1)) {
    rb_raise(rb_eArgError, "wrong # of arguments(%d for 1)",argc); SWIG_fail;
  }
  {
    try {
      result = (TST *)new TST();
      result->load(argv[0]);
      DATA_PTR(self) = result;
    }
    catch (std::exception e) {
      rb_raise(rb_eRuntimeError, "%s", e.what());
    }
  }
  return self;
fail:
  return Qnil;
}
%}

%init %{
rb_define_method(cTST.klass, "marshal_load", VALUEFUNC(_wrap_TST_marshal_load), -1);
%}
