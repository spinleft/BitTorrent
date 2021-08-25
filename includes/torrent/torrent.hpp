#ifndef TORRENT_TORRENT_HPP
#define TORRENT_TORRENT_HPP

#include <vector>
#include <string>
#include <iostream>
#include "util/bencoding.hpp"

namespace bittorrent
{
    struct file
    {
        long length;
        std::string path;
    };

    struct info
    {
        std::string name;
        long piece_length;
        char *pieces;
        long length;
        std::vector<file> files;
    };

    class Torrent
    {
    private:
        std::string announce;
        std::vector<std::string> announce_list;
        std::string comment;
        std::string created_by;
        info info;

    public:
        int readFile(const char *filename);
    };

    int Torrent::readFile(const char *filename)
    {
        util::Bdict *torrent;

        if ((torrent = util::decode(filename)) == 0) {
            std::cout << "Torrent::readFile(): decode file failed." << std::endl;
            return -1;
        }
        for (int i = 0; i < torrent->size(); ++i) {
            if (!torrent->getKey(i).compare("announce")) {
                // read announce
                announce = torrent->getValue(i)->getString();
                std::cout << "announce: " << announce << std::endl;
            } else if (!torrent->getKey(i).compare("announce-list")) {
                // read announce-list
                std::vector<util::Bnode *> &list = torrent->getValue(i)->getList();
                size_t size = list.size();
                for (int j = 0; j < size; ++j) {
                    announce_list.push_back(list[j]->getEntry(0)->getString());
                    std::cout << "announce-list: " << announce_list[j] << std::endl;
                }
            } else if (!torrent->getKey(i).compare("comment")) {
                // read comment
                comment = torrent->getValue(i)->getString();
                std::cout << "comment: " << comment << std::endl;
            } else if (!torrent->getKey(i).compare("created by")) {
                // read created by
                created_by = torrent->getValue(i)->getString();
                std::cout << "created by: " << created_by << std::endl;
            } else if (!torrent->getKey(i).compare("info")) {
                // read info
                std::cout << "info: " << std::endl;
                util::Bnode *info_dict = torrent->getValue(i);
                for (int j = 0; j < info_dict->size(); ++j) {
                    if (!info_dict->getKey(j).compare("name")) {
                        // read info.name
                        info.name = info_dict->getValue(j)->getString();
                        std::cout << "..name: " << info.name << std::endl;
                    } else if (!info_dict->getKey(j).compare("pieces")) {
                        // read info.pieces
                        size_t size = info_dict->getValue(j)->size();
                        info.pieces = new char[size];
                        info_dict->getValue(j)->getString().copy(info.pieces, size, 0);
                        std::cout << "..pieces: " << size << ": ";
                        size = size < 100 ? size : 100;
                        for (int k = 0; k < size; ++k) {
                            if (info.pieces[k] > 31 && info.pieces[k] < 127)
                                std::cout << info.pieces[k];
                        }
                        std::cout << "..." << std::endl;
                    } else if (!info_dict->getKey(j).compare("piece length")) {
                        // read info.piece_length
                        info.piece_length = info_dict->getValue(j)->getInt();
                        std::cout << "..piece length: " << info.piece_length << std::endl;
                    } else if (!info_dict->getKey(j).compare("length")) {
                        // read info.length
                        info.length = info_dict->getValue(j)->getInt();
                        std::cout << "..length: " << info.length << std::endl;
                    } else if (!info_dict->getKey(j).compare("files")) {
                        // read info.files[]
                        std::cout << "..files:" << std::endl;
                        util::Bnode *files_list = info_dict->getValue(j);
                        size_t size = files_list->size();
                        for (int k = 0; k < size; ++k) {
                            file entry;
                            util::Bnode *entry_dict = files_list->getEntry(k);
                            for (int l = 0; l < entry_dict->size(); ++l) {
                                if (!entry_dict->getKey(l).compare("length")) {
                                    entry.length = entry_dict->getValue(l)->getInt();
                                } else if (!entry_dict->getKey(l).compare("path")) {
                                    util::Bnode *path_list = entry_dict->getValue(l);
                                    for (int m = 0; m < path_list->size(); ++m) {
                                        entry.path.append(path_list->getEntry(m)->getString());
                                        entry.path.push_back('/');
                                    }
                                    entry.path.pop_back();
                                } else {
                                    std::cout << ".. ..UNKOWN: " << entry_dict->getKey(l) << std::endl;
                                }
                            }
                            info.files.push_back(entry);
                            std::cout << ".. ..length: " << info.files[k].length << std::endl;
                            std::cout << ".. ..path: " << info.files[k].path << std::endl;
                        }
                    } else {
                        std::cout << "..UNKOWN: " << info_dict->getKey(j) << std::endl;
                    }
                }
            } else {
                std::cout << "UNKOWN: " << torrent->getKey(i) << std::endl;
            }
        }
        return 0;
    }
} // namespace bittorrent

#endif // TORRENT_TORRENT_HPP