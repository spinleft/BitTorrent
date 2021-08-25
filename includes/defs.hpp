namespace util {
// util::

enum type_t { STR, INT, LIST, DICT, END, NONE };

class Bencoding_node;
class Dictionary_entry;
class Bencoding_dictionary;
class Bencoding_list;
class Bencoding_string;
class Bencoding_int;
using Bnode = Bencoding_node;
using de_t = Dictionary_entry;
using Bdict = Bencoding_dictionary;
using Blist = Bencoding_list;
using Bstr = Bencoding_string;
using Bint = Bencoding_int;

type_t readType(char);
Bdict *decode(const char *filename);
de_t *readDictEntry(std:: ifstream &, int &);
Bdict *readDict(std::ifstream& );
Blist *readList(std::ifstream &);
Bstr *readStr(std::ifstream &);
int readStr(std::ifstream &, std::string &);
Bint *readInt(std::ifstream &);

} // namespace util