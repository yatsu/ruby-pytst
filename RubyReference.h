#ifndef __RUBYREFERENCE_H__
#define __RUBYREFERENCE_H__

#include "ruby.h"

class RubyReference {
public:
    explicit RubyReference() : ref(Qnil) {};
    
    explicit RubyReference(VALUE object, int borrow=1) : ref(object) {
        if(ref==0) {
            throw TSTException("Cannot reference NULL");
        }
    }
    
    RubyReference(const RubyReference& that) : ref(that.ref) {}
    
    RubyReference& operator= (const RubyReference& that) {
        this->ref = that.ref;
        return *this;
    }
    
    ~RubyReference() {}

    int operator==(const RubyReference& that) {
        return (ref==that.ref);
    }
    
    int operator!=(const RubyReference& that) {
        return (ref!=that.ref);
    }

    RubyReference getattr(char* name) {
        return RubyReference(rb_ivar_get(get(), rb_intern(name)));
    }
    
    VALUE get() {
        return ref;
    }
    
    VALUE lend() {
        return ref;
    }

private:
    VALUE ref;
};

#endif
