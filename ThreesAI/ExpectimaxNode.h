//
//  ExpectimaxNode.h
//  ThreesAI
//
//  Created by Drew Gross on 2/26/15.
//  Copyright (c) 2015 DrewGross. All rights reserved.
//

#ifndef __ThreesAI__ExpectimaxNode__
#define __ThreesAI__ExpectimaxNode__

#include <stdio.h>
#include <iostream>

#include <list>
#include <map>
#include <memory>
#include <fstream>
#include <cstdlib>

#include "BoardState.h"

class ExpectimaxNodeBase {
public:
    static int num_existing_nodes;
    ExpectimaxNodeBase(unsigned int depth);
    
    virtual void fillInChildren(std::list<std::weak_ptr<ExpectimaxNodeBase>> & unfilledList) = 0;
    virtual void outputDot() const = 0;
    virtual void outputDotEdges(std::ostream& os, float p) const = 0;
    virtual void pruneUnreachableChildren() = 0;
    
    const unsigned int depth;
};

template <typename edge_type>
class ExpectimaxNode : public ExpectimaxNodeBase {
public:
    ExpectimaxNode(unsigned int depth) : ExpectimaxNodeBase(depth) {};
    
    virtual std::shared_ptr<const ExpectimaxNodeBase> child(edge_type const& edge) const = 0;
    std::map<edge_type, std::shared_ptr<ExpectimaxNodeBase>> children;
    bool childrenAreFilledIn() const;
    
    void outputDot() const;
};

template<typename edge_type>
bool ExpectimaxNode<edge_type>::childrenAreFilledIn() const {
    return !this->children.empty();
}
#include <stdio.h>
#include <streambuf>
#include <ostream>

class stdiobuf
: public std::streambuf {
    enum { bufsize = 2048 };
    char buffer[bufsize];
    FILE* fp;
    int   (*close)(FILE*);
    int overflow(int c) {
        if (c != std::char_traits<char>::eof()) {
            *this->pptr() = std::char_traits<char>::to_char_type(c);
            this->pbump(1);
        }
        return this->sync()
        ? std::char_traits<char>::eof()
        : std::char_traits<char>::not_eof(c);
    }
    int sync() {
        std::streamsize size(this->pptr() - this->pbase());
        std::streamsize done(this->fp? fwrite(this->pbase(), 1, size, this->fp): 0);
        this->setp(this->pbase(), this->epptr());
        return size == done? 0: -1;
    }
public:
    stdiobuf(FILE* fp, int(*close)(FILE*) = fclose)
    : fp(fp)
    , close(close) {
        this->setp(this->buffer, this->buffer + (this->fp? bufsize - 1: 0));
    }
    ~stdiobuf() {
        this->sync();
        this->fp && this->close(this->fp);
    }
};
class opipestream
: private virtual stdiobuf
, public std::ostream {
public:
    opipestream(std::string const& pipe)
    : stdiobuf(popen(pipe.c_str(), "w"), pclose)
    , std::ios(static_cast<std::streambuf*>(this))
    , std::ostream(static_cast<std::streambuf*>(this)) {
    }
};

template<typename edge_type>
void ExpectimaxNode<edge_type>::outputDot() const {

    opipestream ofs("/usr/local/bin/dot -Tsvg | /Users/drewgross/.rvm/gems/ruby-1.9.3-p327/wrappers/bcat");

    ofs << "digraph {" << std::endl;
    ofs << "\tnode [fontname=Courier]" << std::endl;
    ofs << "\tedge [fontname=Courier]" << std::endl;
    this->outputDotEdges(ofs, NAN);
    ofs << "}" << std::endl;
}

#endif /* defined(__ThreesAI__ExpectimaxNode__) */
