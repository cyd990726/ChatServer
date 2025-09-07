#ifndef USER_H
#define USER_H
#include <string>

using namespace std;

class User{

public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline" )
    :_id(id), _name(name), _password(pwd), _state(state) {}
    
    void setId(int id){
        this->_id = id;
    }
    void setName(string name){
        this->_name = name;
    }
    void setPassword(string password){
        this->_password = password;
    }
    void setState(string state){
        this->_state = state;
    }

    int getId() const{
        return this->_id;
    }
    string getName() const{
        return this->_name;
    }

    string getPassword() const{
        return this->_password;
    }
    string getState() const{
        return this->_state;
    }
private:
    int _id;
    string _name;
    string _password;
    string _state;

};




#endif