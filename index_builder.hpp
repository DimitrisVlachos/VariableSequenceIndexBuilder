/*
 Variable sequence string to index builder 
 * (c) 2018 Dimitris Vlachos
https://github.com/DimitrisVlachos?tab=repositories
 */

#ifndef __index_builder_hpp__
#define __index_builder_hpp__
#include <stdint.h>
#include <unordered_map>
#include <map>
template <class obj_t,typename index_t>
class index_builder_if {
    
private:
    
public:
    
    virtual ~index_builder_if() {}
    
    virtual void cleanup() {}
    virtual bool add(obj_t& str) { return false; }
    virtual bool get_index(obj_t& str,index_t& result) { return false; }
    virtual bool get_string(const index_t index,obj_t*& res) { return false; }
    virtual bool store(const std::string& path) { return false; }
    virtual bool load(const std::string& path) { return false; }
     
};

template <class obj_t,typename index_t>
class index_builder_big_data_impl : public index_builder_if<obj_t,index_t> {
    /*TODO BIG DATA VERSION*/
};

template <class obj_t,typename index_t>
class index_builder_basic_impl : public index_builder_if<obj_t,index_t> {
private:
    struct cmp_obj {
         inline bool operator()(  obj_t* a,  obj_t* b)const {
            if (a->length() < b->length())
                return true;
            else if(a->length() > b->length()) 
                return false;
            for (std::size_t i = 0, l = a->length();i < l;++i) {
                auto x = a->at(i);
                auto y = b->at(i);
                if (x==y)
                    continue;
                return (bool)(x < y);
            }
            return false;
        }
    };
    std::map<obj_t*,index_t,cmp_obj > m_indexes;
    std::map<index_t,obj_t*> m_obj;
    index_t m_index;
   
     const std::string m_header = "ibuilder_basic";
    
     bool add_indernal(obj_t& str,index_t idx) { 
        obj_t* ptr = new obj_t();
        if (!ptr)
            return false;
        *ptr = str;
        m_indexes.insert(std::pair<obj_t*,index_t>(ptr,idx));
        m_obj.insert(std::pair<index_t,obj_t*>(idx,ptr));
        if (m_index <= idx) {
            m_index = idx+1;
        }
        return true;
    }
public:
    
    index_builder_basic_impl() : m_index(0) {
        
    }
    
    ~index_builder_basic_impl() {
        cleanup();
    }
    
     void cleanup() {
        for (auto i = m_indexes.begin();i != m_indexes.end();++i) 
            delete i->first;
        
        m_indexes.clear();
        m_obj.clear();
        m_index=0;
     }
     
     bool add(obj_t& str) { 
        obj_t* ptr = new obj_t();
        if (!ptr)
            return false;
        *ptr = str;
        m_indexes.insert(std::pair<obj_t*,index_t>(ptr,m_index));
        m_obj.insert(std::pair<index_t,obj_t*>(m_index++,ptr));
        return true;
    }
     
    bool get_index(  obj_t& str,index_t& result) { 
        auto it = m_indexes.find((&str));
        if (it == m_indexes.end()) {
        obj_t* ptr = new obj_t();
        *ptr = str;
            m_indexes.insert(std::pair<obj_t*,index_t>(ptr,m_index));
            m_obj.insert(std::pair<index_t,obj_t*>(m_index,ptr));
            result = m_index++;
            return false;
        }  
        result = it->second;
        return true;
    }
    
    bool get_string(const index_t index,obj_t*& res) {
        auto it = m_obj.find(index);
        if (it != m_obj.end()) {
            res = it->second;
            return true;
        }
        return false; 
    }
    
    //Serial IO operations are slow.Can be heavily improved.
    bool store(const std::string& path) { 
        FILE* fp = fopen(path.c_str(),"wb");
        if (!fp)
            return false;
        
        //WARNING ENDIANESS IS ARCH-DEPENDED. Implement custom version for generic one
        fwrite((void*)&m_header[0],m_header.length(),1,fp);
        std::size_t tt = m_indexes.size();
        fwrite((void*)&tt,sizeof(tt) , 1,fp);
        for (auto i = m_indexes.begin();i != m_indexes.end();++i) {
            
            fwrite((void*)&i->second,sizeof(index_t) , 1,fp);
            
            const obj_t& fetch = *i->first;
            for (auto x = 0;x < fetch.length();++x)
                fputc(fetch[x],fp);
            
            fputc(0,fp);
        }
        fclose(fp);
        return false; 
    }
    
    bool load(const std::string& path) { 
        FILE* fp = fopen(path.c_str(),"rb");
        if (!fp)
            return false;
        
        uint8_t* hdr = new uint8_t[m_header.length()];
        fread(hdr,m_header.length(),1,fp);
        
        if (memcmp(hdr,&m_header[0],m_header.length()) != 0) {
            fclose(fp);
            return false;
        }
        
        cleanup();
        std::size_t i;
        fread((void*)&i,sizeof(i),1,fp);
        
        obj_t tmp;
        
        //slow
        for (auto x = 0;x < i;++x) {
            index_t idx;
            fread((void*)&idx,sizeof(idx),1,fp);
            
            int t;
            
            tmp.clear();
            
            for (;;) {
                t = fgetc(fp);
                if (t==EOF)
                    break;
                else if (t == 0) {
                    add_indernal(tmp,idx);
                    break;
                } else tmp += t;
            }
            if (feof(fp))
                break;
        }
        fclose(fp);
        return true; 
    }
};
#endif
