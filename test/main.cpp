#include <string>
#include "torrent/torrent.hpp"

int main()
{
    bittorrent::Torrent torrent;
    char filename[] = "/Users/sychen/OneDrive - mail.ustc.edu.cn/项目/BitTorrent/test/files/test.torrent";

    torrent.readFile(filename);
    return 0;
}