#include<iostream>
#include<string>
#include<vector>

using namespace std;

class singleton{
public:
    static singleton *get_instance(){
        cout<<"get instance"<<endl;
        static singleton instance;
        return &instance;
    }
private:
    
    singleton(){
        cout<<"constructor"<<endl;
    }
};

//singleton singleton::instance;

int main(){
    auto p1 = singleton::get_instance();
    auto p2 = singleton::get_instance();

    cout<<(p1 == p2)<<endl;
    return 0;
}