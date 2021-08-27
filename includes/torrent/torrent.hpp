#ifndef TORRENT_TORRENT_HPP
#define TORRENT_TORRENT_HPP

#include <set>
#include "util/bencoding.hpp"

namespace bittorrent {
// bittorrent::

struct file
{
    long length;
    std::string path;
};

struct info
{
    std::vector<file> files;
    long length;
    std::string name;
    long piece_length;
    char *pieces;
};

class Metainfo
{
private:
    std::string announce;
    std::vector<std::string> announce_list;
    info info;
    std::string comment;
    std::string created_by;

public:
    Metainfo() { info.pieces = 0; }
    int readFile(const char *filename);
    ~Metainfo() {
        if (info.pieces)
            delete [] info.pieces;
    }
};

int Metainfo::readFile(const char *filename)
{
    util::Bdict *torrent;
    std::set<std::string> torrent_keys;

    torrent_keys.insert("announce");
    torrent_keys.insert("announce-list");
    torrent_keys.insert("comment");
    torrent_keys.insert("created by");
    torrent_keys.insert("info");

    if ((torrent = util::decode(filename)) == 0) {
        std::cout << "Torrent::readFile(): decode file failed." << std::endl;
        return -1;
    }
    util::dict_t &torrent_dict = torrent->getDict();
    if (torrent_dict.find("announce") != torrent_dict.end()) {
        // read announce
        announce = torrent_dict["announce"]->getString();
        std::cout << "announce: " << announce << std::endl;
    }
    if (torrent_dict.find("announce-list") != torrent_dict.end()) {
        // read announce-list
        std::vector<util::Bnode *> &list = torrent_dict["announce-list"]->getList();
        size_t size = list.size();
        std::cout << "announce-list:" << std::endl;
        for (int j = 0; j < size; ++j) {
            announce_list.push_back(list[j]->getEntry(0)->getString());
            std::cout << ".." << announce_list[j] << std::endl;
        }
    }
    if (torrent_dict.find("comment") != torrent_dict.end()) {
        // read comment
        comment = torrent_dict["comment"]->getString();
        std::cout << "comment: " << comment << std::endl;
    }
    if (torrent_dict.find("created by") != torrent_dict.end()) {
        // read created by
        created_by = torrent_dict["created by"]->getString();
        std::cout << "created by: " << created_by << std::endl;
    }
    if (torrent_dict.find("info") != torrent_dict.end()) {
        // read info
        std::cout << "info: " << std::endl;
        util::dict_t &info_dict = torrent_dict["info"]->getDict();
        if (info_dict.find("files") != info_dict.end()) {
            // read info.files[]
            std::cout << "..files:" << std::endl;
            util::list_t &files_list = info_dict["files"]->getList();
            for (int i = 0; i < files_list.size(); ++i) {
                file entry;
                util::dict_t &entry_dict = files_list[i]->getDict();
                if (entry_dict.find("length") != entry_dict.end()) {
                    entry.length = entry_dict["length"]->getInt();
                }
                if (entry_dict.find("path") != entry_dict.end()) {
                    util::list_t &path_list = entry_dict["path"]->getList();
                    for (int j = 0; j < path_list.size(); ++j) {
                        entry.path.append(path_list[j]->getString());
                        entry.path.push_back('/');
                    }
                    entry.path.pop_back();
                }
                info.files.push_back(entry);
                std::cout << ".. ..length: " << info.files[i].length << std::endl;
                std::cout << ".. ..path: " << info.files[i].path << std::endl;
            }
        }
        if (info_dict.find("length") != info_dict.end()) {
            // read info.length
            info.length = info_dict["length"]->getInt();
            std::cout << "..length: " << info.length << std::endl;
        }
        if (info_dict.find("name") != info_dict.end()) {
            // read info.name
            info.name = info_dict["name"]->getString();
            std::cout << "..name: " << info.name << std::endl;
        }
        if (info_dict.find("piece length") != info_dict.end()) {
            // read info.piece_length
            info.piece_length = info_dict["piece length"]->getInt();
            std::cout << "..piece length: " << info.piece_length << std::endl;
        }
        if (info_dict.find("pieces") != info_dict.end()) {
            // read info.pieces
            size_t size = info_dict["pieces"]->size();
            info.pieces = new char[size];
            info_dict["pieces"]->getString().copy(info.pieces, size, 0);
            std::cout << "..pieces: " << size << ": ";
            size = size < 100 ? size : 100;
            for (int k = 0; k < size; ++k) {
                if (info.pieces[k] > 31 && info.pieces[k] < 127)
                    std::cout << info.pieces[k];
            }
            std::cout << "..." << std::endl;
        }
    }
    for(util::dict_t::iterator iter = torrent_dict.begin(); iter != torrent_dict.end(); ++iter) {
        if (torrent_keys.find(iter->first) == torrent_keys.end())
            std::cout << "UNKOWN: " << iter->first << std::endl;
    }

    return 0;
}

} // namespace bittorrent

#endif // TORRENT_TORRENT_HPP