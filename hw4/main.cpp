#include <sstream>
#include <iostream>

int main()
{
    int a = 1;
    void * the_point = &a;
    std::stringstream ss;
    ss << the_point;
    std::string point_string = ss.str();
    unsigned long ul;
    const char* cstr = point_string.c_str();
    sscanf(cstr,"%lx",&ul);
    void * the_point2 = (void *)(uintptr_t) ul;
    const uint32_t &theval = *(static_cast<const u_int32_t *>(the_point));
    const uint32_t &theval2 = *(static_cast<const u_int32_t *>(the_point2));
    std::cout << the_point << "  "<< point_string << "  " << the_point2 << std::endl;
    std::cout << theval << "  " << theval2 << std::endl;
}