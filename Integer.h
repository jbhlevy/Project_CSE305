#ifndef integer_h
#define integer_h
#include <string>

class Integer{
    public:
        int integer;

        Integer(int i){
            integer = i;
        };


        Integer(){};

        bool operator==(const Integer &i){
            return ((integer == i.integer));// && (html == a.html));
	        }

        int hash(){
            return integer;
        }

        void print(){
            std::cout << integer;// << std::endl;
        }
};

#endif 