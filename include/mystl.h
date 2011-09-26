#ifndef MY_STL_H 
#define MY_STL_H

#include <ext/hash_map>
namespace Sgi = ::__gnu_cxx;

namespace __gnu_cxx {
    using namespace std;
    template<> struct hash<string> {
        size_t operator()(const string& s) const {
            const collate<char>& c = use_facet<collate<char> >(locale::classic());
            return c.hash(s.c_str(), s.c_str() + s.size());
        }
    };

} // namespace __gnu_cxx

#endif //MY_STL_H
