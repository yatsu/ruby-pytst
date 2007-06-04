/*
 # Copyright (C) 2006 Masaki Yatsu <yatsu@yatsu.info>
 # 
 # This file was derived from RubyTST.h.
 # RubyTST.h's license is the following.
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
#define __RUBY__BUILD__

#include "tst.h"
#include "ruby.h"
#include "RubyReference.h"
#include "qad_string.h"

#include <iostream>
#include <fstream>

#ifdef ZIPPED_TREE
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#endif

typedef qad_string<char> string_type;

class CallableAction : public tst_action<char,RubyReference,string_type > {
public:
    CallableAction(RubyReference perform,RubyReference result) : _perform(perform), _result(result) {
    }

    virtual ~CallableAction() {
    }

    virtual void perform(const string_type& string,int remaining_distance,RubyReference data) {
        if(_perform.get()==Qnil) {
            return;
        }
        rb_funcall(_perform.get(),rb_intern("call"),3,rb_str_new(string.data(),string.size()),INT2NUM(remaining_distance),data.get());
    }

    virtual RubyReference result() {
        if(_result.get()==Qnil) {
            return RubyReference();
        }
        else {
            return RubyReference(rb_funcall(_result.get(),rb_intern("call"),0));
        }
    }

private:
    RubyReference _perform,_result;
};

class CallableFilter : public tst_filter<char,RubyReference,string_type > {
public:
    CallableFilter(RubyReference _callable) : callable(_callable) {
    }

    virtual ~CallableFilter() {
    }

    virtual RubyReference perform(const string_type& string,int remaining_distance,RubyReference data) {
        return RubyReference(rb_funcall(callable.get(),rb_intern("call"),3,rb_str_new(string.data(),string.size()),INT2NUM(remaining_distance),data.get()));
    }

private:
    RubyReference callable;
};

class DictAction : public tst_action<char,RubyReference,string_type > {
public:
    DictAction() : dict(rb_hash_new(),0) {
    }

    virtual ~DictAction() {
    }

    virtual void perform(const string_type& string,int remaining_distance,RubyReference data) {
        RubyReference key(rb_str_new(string.data(),string.size()));
        
        VALUE old_tuple=rb_hash_aref(dict.get(),key.get());
        if(!NIL_P(old_tuple)) {
            long value=FIX2INT(rb_ary_entry(old_tuple,0));
            if(value<remaining_distance) {
                return;
            }
        }

        RubyReference tuple(rb_ary_new3(2,INT2NUM(remaining_distance),data.get()));
        rb_hash_aset(dict.get(),key.get(),tuple.get());
    };

    virtual RubyReference result() {
        return dict;
    }

private:
    RubyReference dict;
};

class ListAction : public tst_action<char,RubyReference,string_type > {
public:
    ListAction() : list(rb_ary_new(),0) {
    }

    virtual ~ListAction() {
    }

    virtual void perform(const string_type& string,int remaining_distance,RubyReference data) {
        rb_ary_push(list.get(),data.get());
    }

    virtual RubyReference result() {
        return list;
    }

private:
    RubyReference list;
};

class TupleListAction : public tst_action<char,RubyReference,string_type > {
public:
    TupleListAction() : list(rb_ary_new(),0){
    }

    virtual ~TupleListAction() {
    }

    virtual void perform(const string_type& string,int remaining_distance,RubyReference data) {
        rb_ary_push(list.get(),rb_ary_new3(3, rb_str_new(string.data(),string.size()), INT2NUM(remaining_distance), data.get()));
    }

    virtual RubyReference result() {
        return list;
    }

private:
    RubyReference list;
};

class ObjectSerializer {
public:
    ObjectSerializer();
    
    void write(std::ostream& file,RubyReference data);
    RubyReference read(std::istream& file);

private:
    RubyReference dumps,loads;
};

ObjectSerializer::ObjectSerializer() {
    /*
    RubyReference cPickle(PyImport_Import(name.get()),0);
    dumps = RubyReference(PyObject_GetAttrString(cPickle.get(),"dumps"),0);
    loads = RubyReference(PyObject_GetAttrString(cPickle.get(),"loads"),0);
    */
}


void ObjectSerializer::write(std::ostream& file,RubyReference data) {
    /*
    RubyReference call(Py_BuildValue("Oi",data.get(),2),0);
    RubyReference result(PyObject_CallObject(dumps.get(),call.get()),0);
    char *string;
    int length;
    PyString_AsStringAndSize(result.get(),&string,&length);
    file.write((char*)(&length),sizeof(int));
    file.write(string,length);
    */
}

RubyReference ObjectSerializer::read(std::istream& file) {
    /*
    int length;
    file.read((char*)(&length),sizeof(int));
    char* string=(char*)tst_malloc(length);
    file.read(string,length);
    RubyReference dumped(PyString_FromStringAndSize(string,length),0);
    RubyReference call(Py_BuildValue("(O)",dumped.get()),0);
    RubyReference result(PyObject_CallObject(loads.get(),call.get()),0);
    tst_free(string);
    return result;
    */
    return RubyReference();
}

class TST;

template <typename iterator_type>
class TSTIterator {
    public:
        friend class TST;
    
        TSTIterator __iter__() {
            return *this;
        }
    
        VALUE next() {
            typename iterator_type::value_type v(iterator.next());
            if(v.second) {
                return rb_ary_new3(2, rb_str_new(v.first.data(),v.first.size()),v.second->get());
            }
            else {
                /*
                RubyReference exceptions(PyImport_ImportModule("exceptions"),0);
                RubyReference stop_iteration = exceptions.getattr("StopIteration");
                PyErr_SetNone(stop_iteration.get());
                */
                return 0;
            }
        }
    
    private:
        TSTIterator(iterator_type i) : iterator(i) {
        }
        
        iterator_type iterator;
};

typedef TSTIterator<lexical_iterator<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type > > TSTLexicalIterator;
typedef TSTIterator<match_iterator<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type > > TSTCloseMatchIterator;

class TST : private tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type > {
public:
    TST() : tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >() {
#ifdef SCANNER
        const char* s_kcode = rb_get_kcode();
        if(!strcmp(s_kcode,"EUC")) {
            kcode = EUC;
        }
        else if(!strcmp(s_kcode,"SJIS")) {
            kcode = SJIS;
        }
        else if(!strcmp(s_kcode,"UTF8")) {
            kcode = UTF8;
        }
        else {
            kcode = NONE;
        }
#endif
    }

    virtual ~TST() {
    }

    virtual RubyReference write_to_file(RubyReference file) {
        /*
        if(!PyString_CheckExact(file.get())) {
            throw TSTException("Argument of write_to_file() must be a string object");
        }
        std::ofstream out(PyString_AsString(file.get()),std::ofstream::binary|std::ofstream::out|std::ofstream::trunc);
        out.exceptions(std::ofstream::eofbit | std::ofstream::failbit | std::ofstream::badbit);

#ifdef ZIPPED_TREE
        boost::iostreams::filtering_ostream fout;
        fout.push(boost::iostreams::zlib_compressor());
        fout.push(out);
        this->write(fout);
        fout.strict_sync();
#else
        this->write(out);
#endif        
        
        out.close();
        */
        return RubyReference();
    }

    virtual RubyReference read_from_file(RubyReference file) {
        /*
        if(!PyString_CheckExact(file.get())) {
            throw TSTException("Argument of read_from_file() must be a string object");
        }
        std::ifstream in(PyString_AsString(file.get()),std::ifstream::binary|std::ifstream::in);
        in.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
        
#ifdef ZIPPED_TREE
        boost::iostreams::filtering_istream fin;
        fin.push(boost::iostreams::zlib_decompressor());
        fin.push(in);
        this->read(fin);
#else
        this->read(in);
#endif        

        in.close();
        */
        return RubyReference();
    }

    RubyReference __getitem__(const string_type& string) {
        return get(string);
    }

    RubyReference __setitem__(const string_type& string,RubyReference data) {
        return put(string,data);
    }

    void __delitem__(const string_type& string) {
        remove(string);
    }

    bool has_key(const string_type& string) {
        return contains(string);
    }
        
    TSTLexicalIterator __iter__() {
        return TSTLexicalIterator(tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::iterator());
    }

    TSTLexicalIterator iterator() {
        return TSTLexicalIterator(tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::iterator());
    }

    TSTLexicalIterator iterator(const string_type& string) {
        return TSTLexicalIterator(tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::iterator(string));
    }

    TSTCloseMatchIterator close_match_iterator(const string_type& string, int distance) {
        return TSTCloseMatchIterator(tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::close_match_iterator(string,distance));
    }

    using tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::put; 
    using tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::get; 
    using tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::pack; 
    using tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::walk; 
    using tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::close_match; 
    using tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::match; 
    using tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::scan; 
    using tst<char,RubyReference,memory_storage<char,RubyReference>,ObjectSerializer,string_type >::scan_with_stop_chars;
};
