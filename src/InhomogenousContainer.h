#ifndef _INHOMOCONTAINER_
#define _INHOMOCONTAINER_

#include <boost\any.hpp>
#include <map>
#include <memory>
#include <string>
#include <stdexcept>

typedef std::map<std::string, boost::any> t_map;

class InhomogenousContainer{
private:
	std::shared_ptr<t_map>	_nodes;

	boost::any getElement(const std::string& key){
		if(_nodes->count(key) == 0){
			throw std::logic_error((key + " not found").c_str());
		}

		return (*_nodes)[key];
	}

public:
	InhomogenousContainer() : _nodes(new t_map){
	}

	~InhomogenousContainer(){
	}

	void add(std::string key, boost::any value){
		_nodes->insert( std::pair<std::string, boost::any>(key, value));
	}

	template<typename T>
	T get(std::string key){
		try{
			return boost::any_cast<T>(getElement(key));
		} catch(boost::bad_any_cast& e){
			throw std::logic_error(std::string("Key: ").append(key.c_str()).append(" ").append(e.what()).c_str());
		} catch(std::exception& e){
			throw e;
		} catch(...){
			throw std::logic_error("An error occured while casting an element in \"InhomogenousContainer\"!");
		}
	}
	
	void change(const std::string& key, boost::any newValue){
		_nodes->erase(key);
		add(key, newValue);
	}

	boost::any get(const std::string& key){
		try{
			return getElement(key);
		} catch(std::logic_error& e){
			throw e;
		} 
	}

	const std::type_info* type(const std::string& key){
		try{
			return &(*_nodes)[key].type();
		} catch(std::exception& e){
			throw e;
		} catch(...){
			throw std::logic_error("An error occured while receiving the type_info of an element in \"InhomogenousContainer\"!");
		}
	}

	t_map::iterator begin(){
		return _nodes->begin();
	}

	t_map::iterator end(){
		return _nodes->end();
	}

};


#endif