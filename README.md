# VariableSequenceIndexBuilder
Variable sequence index builder (WIP : BIG DATA support)

    index_builder_if<std::string,uint32_t>* builder = new index_builder_basic_impl<std::string,uint32_t>();

# Simple example

    //add some data
    
    std::array<std::string,5> data  = { "Hello" , "Test" , "Word" , "Something" , "Whatever" };
    
    uint32_t index;
    
    std::string* res;
    
    
    //Generate unique indexes
    
    std::cout << "Generate:" << std::endl;
    
    for(auto i = 0;i < data.size();++i) {
    
        std::cout << "Adding " << data[i] << std::endl;
        
        builder->get_index(data[i],index);
        
        std::cout << "Index: " << index << std::endl;
        
    }
    
    //Validate
    
    std::cout << "Validate:" << std::endl;
    
    for(auto i = 0;i < data.size();++i) {
    
        if (builder->get_index(data[i],index))
        
            std::cout << "Index: " << index << " for " << data[i] << " already exists" << std::endl;
            
        else
        
            std::cout << "This should not happen!" << std::endl;
            
    }
    
    //Resolve
    
    std::cout << "Resolve:" << std::endl;
    
    for(auto i = 0;i < data.size();++i) {
    
        builder->get_index(data[i],index);
        
        builder->get_string(index,res);
        
        std::cout << index << " => " << *res << std::endl;
        
    }
    
    
    //Store
    
    std::cout << "Store:" << std::endl;
    
    builder->store("data");
    
    
    //Load
    
    std::cout << "Load:" << std::endl;
    
    std::cout << "Res: " << builder->load("data") << std::endl;
    
    for(auto i = 0;i < data.size();++i) {
    
        if (builder->get_index(data[i],index))
        
            std::cout << "Index: " << index << " for " << data[i] << " already exists" << std::endl;
            
        else
        
            std::cout << "This should not happen! " << index << std::endl;
            
    }
    
    //done
    
    delete builder;
    
